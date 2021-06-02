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

#include "Multieffects.h"

WM4_WINDOW_APPLICATION(Multieffects);

//----------------------------------------------------------------------------
Multieffects::Multieffects ()
    :
    WindowApplication3("Multieffects",0,0,512,512,ColorRGBA::WHITE)
{
}
//----------------------------------------------------------------------------
bool Multieffects::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,1.0f,0.1f,100.0f);
    Vector3f kCLoc = Vector3f(0.0f,0.0f,2.0f);
    Vector3f kCDir = -Vector3f::UNIT_Z;
    Vector3f kCUp = Vector3f::UNIT_Y;
    Vector3f kCRight = Vector3f::UNIT_X;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Multieffects::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Multieffects::OnIdle ()
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
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Multieffects::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    AlphaState* pkAState;

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 'n':
    case 'N':
        if (m_iActive == 0)
        {
            // switch to hard additive
            pkAState = m_spkEffect->GetBlending(0);
            pkAState->SrcBlend = AlphaState::SBF_ONE;
            pkAState->DstBlend = AlphaState::DBF_ONE;
            m_iActive = 1;
        }
        else if (m_iActive == 1)
        {
            // soft additive
            pkAState = m_spkEffect->GetBlending(0);
            pkAState->SrcBlend = AlphaState::SBF_ONE_MINUS_DST_COLOR;
            pkAState->DstBlend = AlphaState::DBF_ONE;
            m_iActive = 2;
        }
        else
        {
            // multiplicative
            pkAState = m_spkEffect->GetBlending(0);
            pkAState->SrcBlend = AlphaState::SBF_DST_COLOR;
            pkAState->DstBlend = AlphaState::DBF_ZERO;
            m_iActive = 0;
        }
        return true;

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"Multieffects.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Multieffects::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    kAttr.SetTCoordChannels(1,2);
    StandardMesh kSM(kAttr);
    TriMesh* pkPlane = kSM.Rectangle(2,2,1.0f,1.0f);

    // The primary texture is the first effect to be drawn.
    pkPlane->AttachEffect(WM4_NEW TextureEffect("Horizontal"));

    // The secondary texture is drawn as a second effect.
    m_spkEffect = WM4_NEW TextureEffect("Magician");
    pkPlane->AttachEffect(m_spkEffect);

    // Set the blending mode (multiplicative) for the secondary effect when
    // it is applied to the primary effect.
    AlphaState* pkAState = m_spkEffect->GetBlending(0);
    pkAState->SrcBlend = AlphaState::SBF_DST_COLOR;
    pkAState->DstBlend = AlphaState::DBF_ZERO;

    m_spkScene->AttachChild(pkPlane);

    m_iActive = 0;
}
//----------------------------------------------------------------------------
