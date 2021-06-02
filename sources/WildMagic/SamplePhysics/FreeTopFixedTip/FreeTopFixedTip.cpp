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

#include "FreeTopFixedTip.h"

WM4_WINDOW_APPLICATION(FreeTopFixedTip);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
FreeTopFixedTip::FreeTopFixedTip ()
    :
    WindowApplication3("FreeTopFixedTip",0,0,640,480,
        ColorRGBA(0.839215f,0.894117f,0.972549f,1.0f))
{
    m_fLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool FreeTopFixedTip::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // initialize the simulation (TO DO.  Set up mass based on top geometry.)
    m_kModule.Gravity = 10.0;
    m_kModule.Mass = 1.0;
    m_kModule.Length = 8.0;
    m_kModule.Inertia1 = 1.0;
    m_kModule.Inertia3 = 2.0;

    double dTime = 0.0;
    double dDeltaTime = 0.01;
    double dTheta = 0.0;
    double dPhi = 0.001;
    double dPsi = 0.0;
    double dAngVel1 = 1.0;
    double dAngVel2 = 0.0;
    double dAngVel3 = 10.0;
    m_kModule.Initialize(dTime,dDeltaTime,dTheta,dPhi,dPsi,dAngVel1,dAngVel2,
        dAngVel3);

    m_fMaxPhi = Mathf::HALF_PI - Mathf::ATan(2.0f/3.0f);

    // set up the scene graph
    CreateScene();
    m_spkTopRoot->Local.SetRotate(m_kModule.GetBodyAxes());

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fAngle = 0.1f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(4.0f,0.0f,2.0f);
    Vector3f kCDir(-fCos,0.0f,-fSin);
    Vector3f kCUp(-fSin,0.0f,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkTopRoot = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS(0.0f);
    }

    float fCurrIdle = (float)System::GetTime();
    float fDiff = fCurrIdle - m_fLastIdle;
    if (fDiff >= 1.0f/30.0f)
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
bool FreeTopFixedTip::OnKeyDown (unsigned char ucKey, int iX, int iY)
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

#ifdef CONTROL_FRAME_RATE
    case 'p':
    case 'P':
        m_bDoPhysics = !m_bDoPhysics;
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::CreateScene ()
{
    // scene -+--- floor
    //        |
    //        +--- vertical axis
    //        |
    //        +--- top root ---+--- top
    //                         |
    //                         +--- top axis

    m_spkScene = WM4_NEW Node;
    m_spkTopRoot = WM4_NEW Node;

    // wireframe mode
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    m_spkScene->AttachChild(CreateFloor());
    m_spkScene->AttachChild(CreateAxisVertical());
    m_spkScene->AttachChild(m_spkTopRoot);
    m_spkTopRoot->AttachChild(CreateTop());
    m_spkTopRoot->AttachChild(CreateAxisTop());
}
//----------------------------------------------------------------------------
TriMesh* FreeTopFixedTip::CreateFloor ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    TriMesh* pkFloor = StandardMesh(kAttr).Rectangle(2,2,32.0f,32.0f);
    TextureEffect* pkEffect = WM4_NEW TextureEffect("Wood");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkFloor->AttachEffect(pkEffect);
    return pkFloor;
}
//----------------------------------------------------------------------------
TriMesh* FreeTopFixedTip::CreateTop ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    TriMesh* pkTop = StandardMesh(kAttr).Cylinder(32,32,1.0f,2.0f,true);

    // adjust shape
    VertexBuffer* pkVB = pkTop->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        float fZ = pkVB->Position3(i).Z() + 1.0f;
        float fR = 0.75f*( fZ >= 1.5f ? 4.0f-2.0f*fZ : fZ/1.5f );
        float fInvLength = Mathf::InvSqrt(
            pkVB->Position3(i).X()*pkVB->Position3(i).X() +
            pkVB->Position3(i).Y()*pkVB->Position3(i).Y());
        pkVB->Position3(i).X() *= fR*fInvLength;
        pkVB->Position3(i).Y() *= fR*fInvLength;
        pkVB->TCoord2(0,i) *= 4.0f;
    }
    pkTop->UpdateMS(false);
    pkTop->Local.SetTranslate(Vector3f::UNIT_Z);

    TextureEffect* pkEffect = WM4_NEW TextureEffect("TopTexture");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkTop->AttachEffect(pkEffect);

    return pkTop;
}
//----------------------------------------------------------------------------
Polyline* FreeTopFixedTip::CreateAxisTop ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,2);
    pkVBuffer->Position3(0) = Vector3f::ZERO;
    pkVBuffer->Position3(1) = 4.0f*Vector3f::UNIT_Z;
    pkVBuffer->Color3(0,0) = ColorRGB::WHITE;
    pkVBuffer->Color3(0,1) = ColorRGB::WHITE;

    Polyline* pkAxisTop = WM4_NEW Polyline(pkVBuffer,false,true);
    pkAxisTop->AttachEffect(WM4_NEW VertexColor3Effect);

    return pkAxisTop;
}
//----------------------------------------------------------------------------
Polyline* FreeTopFixedTip::CreateAxisVertical ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,2);
    pkVBuffer->Position3(0) = Vector3f::ZERO;
    pkVBuffer->Position3(1) = 4.0f*Vector3f::UNIT_Z;
    pkVBuffer->Color3(0,0) = ColorRGB::BLACK;
    pkVBuffer->Color3(0,1) = ColorRGB::BLACK;

    Polyline* pkAxisVertical = WM4_NEW Polyline(pkVBuffer,false,true);
    pkAxisVertical->AttachEffect(WM4_NEW VertexColor3Effect);

    return pkAxisVertical;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::DoPhysical ()
{
    // Stop the simulation when the top edge reaches the ground.
    if (m_kModule.GetPhi() >= m_fMaxPhi)
    {
        // EXERCISE.  Instead of stopping the top, maintain its phi value
        // at m_fMaxPhi so that the top continues to roll on the ground.
        // In addition, arrange for the top to slow down while rolling on
        // the ground, eventually coming to a stop.
        return;
    }

    // move the top
    m_kModule.Update();
    m_spkTopRoot->Local.SetRotate(m_kModule.GetBodyAxes());
    m_spkTopRoot->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
