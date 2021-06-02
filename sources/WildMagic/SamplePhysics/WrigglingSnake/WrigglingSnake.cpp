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

#include "WrigglingSnake.h"

WM4_WINDOW_APPLICATION(WrigglingSnake);

float WrigglingSnake::ms_fRadius = 0.0625f;

//#define SINGLE_STEP

//----------------------------------------------------------------------------
WrigglingSnake::WrigglingSnake ()
    :
    WindowApplication3("WrigglingSnake",0,0,640,480,
        ColorRGBA(1.0f,0.823529f,0.607843f,1.0f))
{
    m_iNumCtrl = 32;
    m_iDegree = 3;
    m_pkCenter = 0;
    m_afAmplitude = WM4_NEW float[m_iNumCtrl];
    m_afPhase = WM4_NEW float[m_iNumCtrl];
    m_iShellQuantity = 4;
}
//----------------------------------------------------------------------------
WrigglingSnake::~WrigglingSnake ()
{
    WM4_DELETE[] m_afAmplitude;
    WM4_DELETE[] m_afPhase;
}
//----------------------------------------------------------------------------
bool WrigglingSnake::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    CreateSnake();

    // center-and-fit for camera viewing
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,-1.0f,0.0f);
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

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void WrigglingSnake::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkSnakeRoot = 0;
    m_spkSnakeBody = 0;
    m_spkSnakeHead = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void WrigglingSnake::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS(0.0f);
    }

#ifndef SINGLE_STEP
    ModifyCurve();
#endif
    m_kCuller.ComputeVisibleSet(m_spkScene);

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool WrigglingSnake::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        ModifyCurve();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
float WrigglingSnake::Radial (float fT)
{
    return ms_fRadius*(2.0f*fT)/(1.0f+fT);
}
//----------------------------------------------------------------------------
void WrigglingSnake::CreateSnake ()
{
    m_spkSnakeRoot = WM4_NEW Node;
    m_spkTrnNode->AttachChild(m_spkSnakeRoot);
    CreateSnakeBody();
    CreateSnakeHead();
}
//----------------------------------------------------------------------------
void WrigglingSnake::CreateSnakeBody ()
{
    // create the B-spline curve for the snake body
    Vector3f* akCtrl = WM4_NEW Vector3f[m_iNumCtrl];
    int i;
    for (i = 0; i < m_iNumCtrl; i++)
    {
        // control points for a snake
        float fRatio = ((float)i)/(float)(m_iNumCtrl-1);
        float fX = -1.0f + 2.0f*fRatio;
        float fXMod = 10.0f*fX - 4.0f;
        akCtrl[i].X() = fX;
        akCtrl[i].Y() = ms_fRadius*(1.5f + Mathf::ATan(fXMod)/Mathf::PI);
        akCtrl[i].Z() = 0.0f;

        // sinusoidal motion for snake
        m_afAmplitude[i] = 0.1f+fRatio*Mathf::Exp(-fRatio);
        m_afPhase[i] = 1.5f*fRatio*Mathf::TWO_PI;
    }

    // the control points are copied by the curve objects
    m_pkCenter = WM4_NEW BSplineCurve3f(m_iNumCtrl,akCtrl,m_iDegree,false,
        true);
    WM4_DELETE[] akCtrl;

    // generate a tube surface
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    Vector2f kUVMin(0.0f,0.0f), kUVMax(1.0f,16.0f);
    m_spkSnakeBody = WM4_NEW TubeSurface(m_pkCenter,Radial,false,
        Vector3f::UNIT_Y,128,32,kAttr,false,false,&kUVMin,&kUVMax);

    // attach a texture to the snake body
    TextureEffect* pkEffect = WM4_NEW TextureEffect("Snake");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetFilterType(Texture::LINEAR_LINEAR);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    m_spkSnakeBody->AttachEffect(pkEffect);

    m_spkSnakeRoot->AttachChild(m_spkSnakeBody);
}
//----------------------------------------------------------------------------
void WrigglingSnake::CreateSnakeHead ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);

    // Create the snake head as a paraboloid that is attached to the last
    // ring of vertices on the snake body.  These vertices are generated
    // for t = 1.
    int iSliceSamples = m_spkSnakeBody->GetSliceSamples();

    // number of rays (determined by slice samples of tube surface)
    int iRQ = iSliceSamples - 1;

    // number of shells (your choice, specified in application constructor)
    int iSQ = m_iShellQuantity, iSQm1 = iSQ-1;

    // generate vertices (to be filled in by UpdateSnakeHead)
    int iVQuantity = 1 + iRQ*iSQm1;
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);

    // generate triangles
    int iTQuantity = iRQ*(2*iSQm1-1);
    int iIQuantity = 3*iTQuantity;
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* piIndex = pkIBuffer->GetData();
    int iT = 0;
    for (int iR0 = iRQ-1, iR1 = 0; iR1 < iRQ; iR0 = iR1++)
    {
        *piIndex++ = 0;
        *piIndex++ = 1+iSQm1*iR0;
        *piIndex++ = 1+iSQm1*iR1;
        iT++;
        for (int iS = 1; iS < iSQm1 ; iS++)
        {
            int i00 = iS+iSQm1*iR0;
            int i01 = iS+iSQm1*iR1;
            int i10 = i00+1;
            int i11 = i01+1;
            *piIndex++ = i00;
            *piIndex++ = i10;
            *piIndex++ = i11;
            *piIndex++ = i00;
            *piIndex++ = i11;
            *piIndex++ = i01;
            iT += 2;
        }
    }
    assert( iT == iTQuantity );

    m_spkSnakeHead = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    // generate vertex colors coordinates
    for (int i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Color3(0,i) = ColorRGB(0.0f,0.25f,0.0f);
    }

    m_spkSnakeHead->AttachEffect(WM4_NEW VertexColor3Effect);

    m_spkSnakeRoot->AttachChild(m_spkSnakeHead);
    UpdateSnakeHead();
}
//----------------------------------------------------------------------------
void WrigglingSnake::UpdateSnake ()
{
    // The order of calls is important since the snake head uses the last
    // ring of vertices in the tube surface of the snake body.
    UpdateSnakeBody();
    UpdateSnakeHead();
    m_spkSnakeRoot->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
void WrigglingSnake::UpdateSnakeBody ()
{
    m_spkSnakeBody->UpdateSurface();
    m_spkSnakeBody->VBuffer->Release();
}
//----------------------------------------------------------------------------
void WrigglingSnake::UpdateSnakeHead ()
{
    // get the ring of vertices at the head-end of the tube
    int iSliceSamples = m_spkSnakeBody->GetSliceSamples();
    Vector3f* akSlice = WM4_NEW Vector3f[iSliceSamples+1];
    m_spkSnakeBody->GetTMaxSlice(akSlice);

    // compute the center of the slice vertices
    Vector3f kCenter = akSlice[0];
    int i;
    for (i = 1; i <= iSliceSamples; i++)
    {
        kCenter += akSlice[i];
    }
    kCenter /= (float)(iSliceSamples+1);

    // Compute a unit-length normal of the plane of the vertices.  The normal
    // points away from tube and is used to extrude the paraboloid surface
    // for the head.
    Vector3f kEdge1 = akSlice[1] - akSlice[0];
    Vector3f kEdge2 = akSlice[2] - akSlice[0];
    Vector3f kNormal = kEdge1.UnitCross(kEdge2);

    // Adjust the normal length to include the height of the ellipsoid vertex
    // above the plane of the slice.
    kNormal *= 3.0f*ms_fRadius;

    // origin
    m_spkSnakeHead->VBuffer->Position3(0) = kCenter + kNormal;

    // remaining shells
    const int iSQm1 = m_iShellQuantity - 1;
    float fFactor = 1.0f/iSQm1;
    for (int iR = 0; iR < iSliceSamples-1; iR++)
    {
        for (int iS = 1; iS < m_iShellQuantity; iS++)
        {
            float fT = fFactor*iS, fOmT = 1.0f - fT;
            i = iS + iSQm1*iR;
            m_spkSnakeHead->VBuffer->Position3(i) = fOmT*kCenter +
                fT*akSlice[iR] + Mathf::Pow(fOmT,0.25f)*kNormal;
        }
    }

    WM4_DELETE[] akSlice;

    m_spkSnakeHead->UpdateMS(false);
    m_spkSnakeHead->VBuffer->Release();
}
//----------------------------------------------------------------------------
void WrigglingSnake::ModifyCurve ()
{
    // perturb the snake medial curve
    float fTime = (float)System::GetTime();
    for (int i = 0; i < m_iNumCtrl; i++)
    {
        Vector3f kCtrl = m_pkCenter->GetControlPoint(i);
        kCtrl.Z() = m_afAmplitude[i]*Mathf::Sin(3.0f*fTime+m_afPhase[i]);
        m_pkCenter->SetControlPoint(i,kCtrl);
    }

    UpdateSnake();
}
//----------------------------------------------------------------------------
