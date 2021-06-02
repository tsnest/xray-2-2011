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

#include "BallHill.h"

WM4_WINDOW_APPLICATION(BallHill);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
BallHill::BallHill ()
    :
    WindowApplication3("BallHill",0,0,640,480,
        ColorRGBA(0.839215f,0.894117f,0.972549f,1.0f))
{
}
//----------------------------------------------------------------------------
bool BallHill::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up the physics module
    m_kModule.Gravity = 1.0;
    m_kModule.A1 = 2.0;
    m_kModule.A2 = 1.0;
    m_kModule.A3 = 1.0;
    m_kModule.Radius = 0.1;

    double dTime = 0.0;
    double dDeltaTime = 0.01;
    double dY1 = 0.0;
    double dY2 = 0.0;
    double dY1Dot = 0.1;
    double dY2Dot = 0.1;
    m_kModule.Initialize(dTime,dDeltaTime,dY1,dY2,dY1Dot,dY2Dot);

    // set up the scene graph
    m_spkScene = WM4_NEW Node;

    // wireframe mode
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    m_spkScene->AttachChild(CreateGround());
    m_spkScene->AttachChild(CreateHill());
    m_spkScene->AttachChild(CreateBall());
    m_spkScene->AttachChild(CreatePath());

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
void BallHill::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkBall = 0;
    m_spkPath = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BallHill::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS();
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
bool BallHill::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
TriMesh* BallHill::CreateGround ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    TriMesh* pkGround = StandardMesh(kAttr).Rectangle(2,2,32.0f,32.0f);

    // change the texture repeat
    VertexBuffer* pkVB = pkGround->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        pkVB->TCoord2(0,i) *= 8.0f;
    }

    TextureEffect* pkEffect = WM4_NEW TextureEffect("Grass");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkGround->AttachEffect(pkEffect);

    return pkGround;
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateHill ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    TriMesh* pkHill = StandardMesh(kAttr).Disk(32,32,2.0f);

    // change the texture repeat
    VertexBuffer* pkVB = pkHill->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB->TCoord2(0,i) *= 8.0f;
    }

    TextureEffect* pkEffect = WM4_NEW TextureEffect("Gravel");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkHill->AttachEffect(pkEffect);

    // adjust disk vertices to form elliptical paraboloid for the hill
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB->Position3(i).Z() = m_kModule.GetHeight(pkVB->Position3(i).X(),
            pkVB->Position3(i).Y());
    }
    pkHill->UpdateMS(false);

    return pkHill;
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateBall ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    m_fRadius = (float)m_kModule.Radius;
    m_spkBall = StandardMesh(kAttr).Sphere(16,16,m_fRadius);

    Vector3f kTrn = m_spkBall->Local.GetTranslate();
    kTrn.Z() = (float)m_kModule.A3 + m_fRadius;
    m_spkBall->Local.SetTranslate(kTrn);

    TextureEffect* pkEffect = WM4_NEW TextureEffect("BallTexture");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    m_spkBall->AttachEffect(pkEffect);

    UpdateBall();

    return m_spkBall;
}
//----------------------------------------------------------------------------
Polyline* BallHill::CreatePath ()
{
    // points used to display path of ball
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    const int iPQuantity = 1024;
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iPQuantity);
    int i;
    for (i = 0; i < iPQuantity; i++)
    {
        pkVBuffer->Position3(i) = Vector3f::ZERO;
        pkVBuffer->Color3(0,i) = ColorRGB::WHITE;
    }

    m_spkPath = WM4_NEW Polyline(pkVBuffer,false,true);
    m_spkPath->SetActiveQuantity(0);
    m_iNextPoint = 0;

    m_spkPath->AttachEffect(WM4_NEW VertexColor3Effect);

    return m_spkPath;
}
//----------------------------------------------------------------------------
void BallHill::DoPhysical ()
{
    // allow motion only while ball is above the ground level
    if (m_spkBall->Local.GetTranslate().Z() <= m_fRadius)
    {
        return;
    }

    // move the ball
    m_kModule.Update();
    Vector3f kCenter = UpdateBall();

    // Draw only the active quantity of pendulum points for the initial
    // portion of the simulation.  Once all points are activated, then all
    // are drawn.
    int iVQuantity = m_spkPath->VBuffer->GetVertexQuantity();
    int iAQuantity = m_spkPath->GetActiveQuantity();
    if (iAQuantity < iVQuantity)
    {
        m_spkPath->SetActiveQuantity(++iAQuantity);
    }

    // Update the path that the ball has followed.  The points are stored in
    // a circular queue, so the oldest points eventually disappear and are
    // reused for the new points.
    m_spkPath->VBuffer->Position3(m_iNextPoint) = kCenter;
    if (++m_iNextPoint == iVQuantity)
    {
        m_iNextPoint = 0;
    }

    m_pkRenderer->ReleaseResources(m_spkPath);
}
//----------------------------------------------------------------------------
Vector3f BallHill::UpdateBall ()
{
    // Compute the location of the center of the ball and the incremental
    // rotation implied by its motion.
    Vector3f kCenter;
    Matrix3f kIncrRot;
    m_kModule.GetData(kCenter,kIncrRot);

    // update the ball position and orientation
    m_spkBall->Local.SetTranslate(kCenter);
    m_spkBall->UpdateGS();
    m_spkBall->Local.SetRotate(kIncrRot*m_spkBall->Local.GetRotate());

    // return the new ball center for further use by application
    return kCenter;
}
//----------------------------------------------------------------------------
