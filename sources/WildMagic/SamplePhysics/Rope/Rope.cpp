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
// Version: 4.0.2 (2007/06/16)

#include "Rope.h"

WM4_WINDOW_APPLICATION(Rope);

//----------------------------------------------------------------------------
Rope::Rope ()
    :
    WindowApplication3("Rope",0,0,640,480,ColorRGBA(0.75f,0.85f,0.95f,1.0f))
{
    m_pkSpline = 0;
    m_pkModule = 0;
    m_fLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool Rope::OnInitialize ()
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
    Vector3f kCDir(0.0f,-1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir
        - 0.5f*kCUp;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Rope::OnTerminate ()
{
    WM4_DELETE m_pkModule;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkWireframe = 0;
    m_spkRope = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Rope::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS(0.0f);
    }

    float fCurrIdle = (float)System::GetTime();
    float fDiff = fCurrIdle - m_fLastIdle;
    if (fDiff >= 0.001f)
    {
        m_fLastIdle = fCurrIdle;

        DoPhysical();
        m_kCuller.ComputeVisibleSet(m_spkScene);

        m_pkRenderer->ClearBuffers();
        if (m_pkRenderer->BeginScene())
        {
            m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
            m_pkRenderer->EndScene();
        }
        m_pkRenderer->DisplayBackBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Rope::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    int i;

    switch (ucKey)
    {
    case 'g':
        DoPhysical();
        return true;
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 'm':  // decrease mass
        if (m_pkModule->GetMass(1) > 0.05f)
        {
            for (i = 1; i < m_pkModule->GetNumParticles()-1; i++)
            {
                m_pkModule->SetMass(i,m_pkModule->GetMass(i)-0.01f);
            }
        }
        return true;
    case 'M':  // increase mass
        for (i = 1; i < m_pkModule->GetNumParticles()-1; i++)
        {
            m_pkModule->SetMass(i,m_pkModule->GetMass(i)+0.01f);
        }
        return true;
    case 'c':  // decrease spring constant
        if (m_pkModule->Constant(0) > 0.05f)
        {
            for (i = 0; i < m_pkModule->GetNumSprings(); i++)
            {
                m_pkModule->Constant(i) -= 0.01f;
            }
        }
        return true;
    case 'C':  // increase spring constant
        for (i = 0; i < m_pkModule->GetNumSprings(); i++)
        {
            m_pkModule->Constant(i) += 0.01f;
        }
        return true;
    case 'l':  // decrease spring resting length
        if (m_pkModule->Length(0) > 0.05f)
        {
            for (i = 0; i < m_pkModule->GetNumSprings(); i++)
            {
                m_pkModule->Length(i) -= 0.01f;
            }
        }
        return true;
    case 'L':  // increase spring resting length
        for (i = 0; i < m_pkModule->GetNumSprings(); i++)
        {
            m_pkModule->Length(i) += 0.01f;
        }
        return true;
    case 'f':  // toggle wind force on/off
    case 'F':
        m_pkModule->EnableWind() = !m_pkModule->EnableWind();
        return true;
    case 'r':  // toggle random wind direction change on/off
    case 'R':
        m_pkModule->EnableWindChange() = !m_pkModule->EnableWindChange();
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void Rope::CreateSprings ()
{
    int iNumParticles = 8;
    float fStep = 0.1f;
    Vector3f kGravity(0.0f,0.0f,-1.0f);
    Vector3f kWind(0.0f,-0.25f,0.0f);
    float fWindChangeAmplitude = 0.01f;
    float fViscosity = 10.0f;
    m_pkModule = WM4_NEW PhysicsModule(iNumParticles,fStep,kGravity,kWind,
        fWindChangeAmplitude,fViscosity);

    // constant mass at interior points (end points are immovable)
    m_pkModule->SetMass(0,Mathf::MAX_REAL);
    m_pkModule->SetMass(iNumParticles-1,Mathf::MAX_REAL);
    int i;
    for (i = 1; i < iNumParticles-1; i++)
    {
        m_pkModule->SetMass(i,1.0f);
    }

    // initial position on a horizontal line segment
    float fFactor = 1.0f/(float)(iNumParticles-1);
    for (i = 0; i < iNumParticles; i++)
    {
        m_pkModule->Position(i) = Vector3f(i*fFactor,0.0f,1.0f);
    }

    // initial velocities are all zero
    for (i = 0; i < iNumParticles; i++)
    {
        m_pkModule->Velocity(i) = Vector3f::ZERO;
    }

    // springs are at rest in the initial horizontal configuration
    int iNumSprings = iNumParticles - 1;
    float fRestLength = 1.0f/(float)iNumSprings;
    for (i = 0; i < iNumSprings; i++)
    {
        m_pkModule->Constant(i) = 10.0f;
        m_pkModule->Length(i) = fRestLength;
    }
}
//----------------------------------------------------------------------------
void Rope::CreateRope ()
{
    // create quadratic spline using particles as control points
    int iNumCtrlPoints = m_pkModule->GetNumParticles();
    Vector3f* akCtrlPoint = m_pkModule->Positions();
    int iDegree = 2;
    m_pkSpline = WM4_NEW BSplineCurve3f(iNumCtrlPoints,akCtrlPoint,iDegree,
        false,true);

    // generate a tube surface whose medial axis is the spline
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    Vector2f kUVMin(0.0f,0.0f), kUVMax(1.0f,1.0f);
    m_spkRope = WM4_NEW TubeSurface(m_pkSpline,Radial,false,Vector3f::UNIT_Z,
        64,8,kAttr,false,false,&kUVMin,&kUVMax);

    // attach a texture for the rope
    TextureEffect* pkEffect = WM4_NEW TextureEffect("Rope");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetFilterType(Texture::LINEAR_LINEAR);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    m_spkRope->AttachEffect(pkEffect);

    m_spkTrnNode->AttachChild(m_spkRope);
}
//----------------------------------------------------------------------------
void Rope::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    CreateSprings();
    CreateRope();
}
//----------------------------------------------------------------------------
void Rope::DoPhysical ()
{
    // forces are independent of time, just pass in t=0
    m_pkModule->Update(0.0f);

    // Update spline curve.  Remember that the spline maintains its own
    // copy of the control points, so this update is necessary.
    int iNumCtrlPoints = m_pkModule->GetNumParticles();
    Vector3f* akCtrlPoint = m_pkModule->Positions();
    for (int i = 0; i < iNumCtrlPoints; i++)
    {
        m_pkSpline->SetControlPoint(i,akCtrlPoint[i]);
    }

    m_spkRope->UpdateSurface();
    m_spkRope->VBuffer->Release();
}
//----------------------------------------------------------------------------
