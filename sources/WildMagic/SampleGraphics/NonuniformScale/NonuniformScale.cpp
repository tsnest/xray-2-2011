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

#include "NonuniformScale.h"

WM4_WINDOW_APPLICATION(NonuniformScale);

//----------------------------------------------------------------------------
NonuniformScale::NonuniformScale ()
    :
    WindowApplication3("NonuniformScale",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool NonuniformScale::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // set camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fCos = 0.866025f, fSin = 0.5f;
    Vector3f kCLoc(0.0f,-4.0f,2.0f);
    Vector3f kCDir(0.0f,fCos,-fSin);
    Vector3f kCUp(0.0f,fSin,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void NonuniformScale::OnTerminate ()
{
    m_spkScene = 0;
    m_spkMesh = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void NonuniformScale::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }
        
    if (MoveObject())
    {
        m_spkScene->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

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
bool NonuniformScale::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    Vector3f kScale;

    switch (ucKey)
    {
    case 'x':
        kScale = m_spkMesh->Local.GetScale();
        kScale.X() /= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'X':
        kScale = m_spkMesh->Local.GetScale();
        kScale.X() *= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'y':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Y() /= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'Y':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Y() *= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'z':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Z() /= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'Z':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Z() *= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"NonuniformScale.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void NonuniformScale::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    m_spkMesh = kSM.Dodecahedron();
    m_spkMesh->AttachEffect(WM4_NEW TextureEffect("Flower"));
    m_spkScene->AttachChild(m_spkMesh);
}
//----------------------------------------------------------------------------
