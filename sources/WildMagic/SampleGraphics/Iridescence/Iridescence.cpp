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

#include "Iridescence.h"

WM4_WINDOW_APPLICATION(Iridescence);

//----------------------------------------------------------------------------
Iridescence::Iridescence ()
    :
    WindowApplication3("Iridescence",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool Iridescence::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,-8.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

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
void Iridescence::OnTerminate ()
{
    m_spkScene = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Iridescence::OnIdle ()
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
bool Iridescence::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    float fInterpolateFactor;

    switch (ucKey)
    {
    case '+':
    case '=':
        fInterpolateFactor = m_spkEffect->GetInterpolateFactor();
        fInterpolateFactor += 0.1f;
        if (fInterpolateFactor > 1.0f)
        {
            fInterpolateFactor = 1.0f;
        }

        m_spkEffect->SetInterpolateFactor(fInterpolateFactor);
        return true;

    case '-':
    case '_':
        fInterpolateFactor = m_spkEffect->GetInterpolateFactor();
        fInterpolateFactor -= 0.1f;
        if (fInterpolateFactor < 0.0f)
        {
            fInterpolateFactor = 0.0f;
        }

        m_spkEffect->SetInterpolateFactor(fInterpolateFactor);
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"Iridescence.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Iridescence::CreateScene ()
{
    m_spkScene = WM4_NEW Node;

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    TriMesh* pkMesh = kSM.Torus(20,20,2.0f,1.0f);
    m_spkScene->AttachChild(pkMesh);

    m_spkEffect = WM4_NEW IridescenceEffect("Leaf","Gradient");
    m_spkEffect->SetInterpolateFactor(0.5f);
    pkMesh->AttachEffect(m_spkEffect);
}
//----------------------------------------------------------------------------
