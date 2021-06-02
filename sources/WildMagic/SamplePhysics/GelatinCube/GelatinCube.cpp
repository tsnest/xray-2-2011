// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2009
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.1 (2007/06/16)

#include "GelatinCube.h"

WM4_WINDOW_APPLICATION(GelatinCube);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
GelatinCube::GelatinCube ()
    :
    WindowApplication3("GelatinCube",0,0,640,480,
        ColorRGBA(0.713725f,0.807843f,0.929411f,1.0f))
{
    m_pkSpline = 0;
    m_pkModule = 0;
}
//----------------------------------------------------------------------------
bool GelatinCube::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // center-and-fit for camera viewing
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    // sort the box faces based on current camera parameters
    m_spkBox->SortFaces(m_spkCamera->GetDVector());

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void GelatinCube::OnTerminate ()
{
    WM4_DELETE m_pkModule;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkWireframe = 0;
    m_spkBox = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GelatinCube::OnIdle ()
{
    MeasureTime();

    bool bNeedSort = MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS(0.0f);
        bNeedSort = true;
    }
    if (bNeedSort)
    {
        m_spkBox->SortFaces(m_spkCamera->GetDVector());
    }

#ifndef SINGLE_STEP
    DoPhysical();
#endif
    m_kCuller.ComputeVisibleSet(m_spkScene);

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool GelatinCube::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        DoPhysical();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void GelatinCube::CreateSprings ()
{
    // The inner 4-by-4-by-4 particles are used as the control points of a
    // B-spline volume.  The outer layer of particles are immovable to
    // prevent the cuboid from collapsing into itself.
    int iSlices = 6;
    int iRows = 6;
    int iCols = 6;

    // Viscous forces applied.  If you set viscosity to zero, the cuboid
    // wiggles indefinitely since there is no dissipation of energy.  If
    // the viscosity is set to a positive value, the oscillations eventually
    // stop.  The length of time to steady state is inversely proportional
    // to the viscosity.
#ifdef _DEBUG
    float fStep = 0.1f;
#else
    float fStep = 0.01f;  // simulation needs to run slower in release mode
#endif
    float fViscosity = 0.01f;
    m_pkModule = WM4_NEW PhysicsModule(iSlices,iRows,iCols,fStep,fViscosity);

    // The initial cuboid is axis-aligned.  The outer shell is immovable.
    // All other masses are constant.
    float fSFactor = 1.0f/(float)(iSlices-1);
    float fRFactor = 1.0f/(float)(iRows-1);
    float fCFactor = 1.0f/(float)(iCols-1);
    int iSlice, iRow, iCol;
    for (iSlice = 0; iSlice < iSlices; iSlice++)
    {
        for (iRow = 0; iRow < iRows; iRow++)
        {
            for (iCol = 0; iCol < iCols; iCol++)
            {
                m_pkModule->Position(iSlice,iRow,iCol) =
                    Vector3f(iCol*fCFactor,iRow*fRFactor,iSlice*fSFactor);

                if (1 <= iSlice && iSlice < iSlices-1
                &&  1 <= iRow && iRow < iRows-1
                &&  1 <= iCol && iCol < iCols-1)
                {
                    m_pkModule->SetMass(iSlice,iRow,iCol,1.0f);
                    m_pkModule->Velocity(iSlice,iRow,iCol) =
                        0.1f*Vector3f(Mathf::SymmetricRandom(),
                        Mathf::SymmetricRandom(),Mathf::SymmetricRandom());
                }
                else
                {
                    m_pkModule->SetMass(iSlice,iRow,iCol,Mathf::MAX_REAL);
                    m_pkModule->Velocity(iSlice,iRow,iCol) = Vector3f::ZERO;
                }
            }
        }
    }

    // springs are at rest in the initial configuration
    const float fConstant = 10.0f;
    Vector3f kDiff;

    for (iSlice = 0; iSlice < iSlices-1; iSlice++)
    {
        for (iRow = 0; iRow < iRows; iRow++)
        {
            for (iCol = 0; iCol < iCols; iCol++)
            {
                m_pkModule->ConstantS(iSlice,iRow,iCol) = fConstant;
                kDiff = m_pkModule->Position(iSlice+1,iRow,iCol) -
                    m_pkModule->Position(iSlice,iRow,iCol);
                m_pkModule->LengthS(iSlice,iRow,iCol) = kDiff.Length();
            }
        }
    }

    for (iSlice = 0; iSlice < iSlices; iSlice++)
    {
        for (iRow = 0; iRow < iRows-1; iRow++)
        {
            for (iCol = 0; iCol < iCols; iCol++)
            {
                m_pkModule->ConstantR(iSlice,iRow,iCol) = fConstant;
                kDiff = m_pkModule->Position(iSlice,iRow+1,iCol) -
                    m_pkModule->Position(iSlice,iRow,iCol);
                m_pkModule->LengthR(iSlice,iRow,iCol) = kDiff.Length();
            }
        }
    }

    for (iSlice = 0; iSlice < iSlices; iSlice++)
    {
        for (iRow = 0; iRow < iRows; iRow++)
        {
            for (iCol = 0; iCol < iCols-1; iCol++)
            {
                m_pkModule->ConstantC(iSlice,iRow,iCol) = fConstant;
                kDiff = m_pkModule->Position(iSlice,iRow,iCol+1) -
                    m_pkModule->Position(iSlice,iRow,iCol);
                m_pkModule->LengthC(iSlice,iRow,iCol) = kDiff.Length();
            }
        }
    }
}
//----------------------------------------------------------------------------
void GelatinCube::CreateBox ()
{
    // Create a quadratic spline using the interior particles as control
    // points.
    int iSlices = m_pkModule->GetSlices()-2;
    int iRows = m_pkModule->GetRows()-2;
    int iCols = m_pkModule->GetCols()-2;
    m_pkSpline = WM4_NEW BSplineVolumef(iCols,iRows,iSlices,2,2,2);

    for (int iSlice = 0; iSlice < iSlices; iSlice++)
    {
        for (int iRow = 0; iRow < iRows; iRow++)
        {
            for (int iCol = 0; iCol < iCols; iCol++)
            {
                m_pkSpline->SetControlPoint(iCol,iRow,iSlice,
                    m_pkModule->Position(iSlice+1,iRow+1,iCol+1));
            }
        }
    }

    // texture for the box faces
    TextureEffect* pkEffect = WM4_NEW TextureEffect("WaterA");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);

    // generate a rectangle surface
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    m_spkBox = WM4_NEW BoxSurface(m_pkSpline,8,8,8,kAttr,pkEffect);

    // the texture has an alpha channel of 1/2
    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    m_spkBox->AttachGlobalState(pkAS);

    m_spkBox->EnableSorting();

    m_spkTrnNode->AttachChild(m_spkBox);
}
//----------------------------------------------------------------------------
void GelatinCube::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    CreateSprings();
    CreateBox();
}
//----------------------------------------------------------------------------
void GelatinCube::DoPhysical ()
{
    m_pkModule->Update((float)System::GetTime());

    // Update spline surface.  Remember that the spline maintains its own
    // copy of the control points, so this update is necessary.
    int iSlices = m_pkModule->GetSlices()-2;
    int iRows = m_pkModule->GetRows()-2;
    int iCols = m_pkModule->GetCols()-2;

    for (int iSlice = 0; iSlice < iSlices; iSlice++)
    {
        for (int iRow = 0; iRow < iRows; iRow++)
        {
            for (int iCol = 0; iCol < iCols; iCol++)
            {
                m_pkSpline->SetControlPoint(iCol,iRow,iSlice,
                    m_pkModule->Position(iSlice+1,iRow+1,iCol+1));
            }
        }
    }

    m_spkBox->UpdateSurface();

    for (int i = 0; i < m_spkBox->GetQuantity(); i++)
    {
        Geometry* pkGeometry = StaticCast<Geometry>(m_spkBox->GetChild(i));
        pkGeometry->VBuffer->Release();
    }
}
//----------------------------------------------------------------------------
