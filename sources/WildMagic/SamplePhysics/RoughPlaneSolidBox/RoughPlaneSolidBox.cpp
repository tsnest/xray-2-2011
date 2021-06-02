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

#include "RoughPlaneSolidBox.h"
using namespace std;

WM4_WINDOW_APPLICATION(RoughPlaneSolidBox);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneSolidBox::RoughPlaneSolidBox ()
    :
    WindowApplication3("RoughPlaneSolidBox",0,0,640,480,
        ColorRGBA(0.839215f,0.894117f,0.972549f,1.0f))
{
    m_bDoUpdate = true;
}
//----------------------------------------------------------------------------
bool RoughPlaneSolidBox::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    InitializeModule();

    // set up the scene graph
    m_spkScene = WM4_NEW Node;

    // wireframe mode
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    m_spkScene->AttachChild(CreateGround());
    m_spkScene->AttachChild(CreateRamp());
    m_spkScene->AttachChild(CreateBox());

    m_spkScene->Local.SetRotate(
        Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,0.661917f));

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fAngle = 0.1f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(17.695415f,0.0f,6.4494629f);
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
void RoughPlaneSolidBox::OnTerminate ()
{
    m_spkScene = 0;
    m_spkBox = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS(0.0f);
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
bool RoughPlaneSolidBox::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    switch (ucKey)
    {
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 'r':  // restart
    case 'R':
        InitializeModule();
        MoveBox();
        return true;

#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        DoPhysical();
        return true;
#endif
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
Node* RoughPlaneSolidBox::CreateBox ()
{
    Node* pkBox = WM4_NEW Node;
    float fXExtent = (float)m_kModule.XLocExt;
    float fYExtent = (float)m_kModule.YLocExt;
    float fZExtent = (float)m_kModule.ZLocExt;

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    StandardMesh kSM(kAttr);
    VertexColor3Effect* pkEffect = WM4_NEW VertexColor3Effect;
    Transformation kXFrm;
    TriMesh* pkFace;
    int i;

    // +z face
    kXFrm.SetTranslate(Vector3f(0.0f,0.0f,fZExtent));
    kSM.SetTransformation(kXFrm);
    pkFace = kSM.Rectangle(2,2,fXExtent,fYExtent);
    for (i = 0; i < 4; i++)
    {
        pkFace->VBuffer->Color3(0,0) = ColorRGB(1.0f,0.0f,0.0f);
        pkFace->VBuffer->Color3(0,1) = ColorRGB(1.0f,0.0f,0.0f);
        pkFace->VBuffer->Color3(0,2) = ColorRGB(1.0f,0.0f,0.0f);
        pkFace->VBuffer->Color3(0,3) = ColorRGB(1.0f,0.0f,0.0f);
    }
    pkFace->AttachEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // -z face
    kXFrm.SetTranslate(Vector3f(0.0f,0.0f,-fZExtent));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Y,Vector3f::UNIT_X,
        -Vector3f::UNIT_Z,true));
    kSM.SetTransformation(kXFrm);
    pkFace = kSM.Rectangle(2,2,fYExtent,fXExtent);
    for (i = 0; i < 4; i++)
    {
        pkFace->VBuffer->Color3(0,0) = ColorRGB(0.5f,0.0f,0.0f);
        pkFace->VBuffer->Color3(0,1) = ColorRGB(0.5f,0.0f,0.0f);
        pkFace->VBuffer->Color3(0,2) = ColorRGB(0.5f,0.0f,0.0f);
        pkFace->VBuffer->Color3(0,3) = ColorRGB(0.5f,0.0f,0.0f);
    }
    pkFace->AttachEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // +y face
    kXFrm.SetTranslate(Vector3f(0.0f,fYExtent,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Z,Vector3f::UNIT_X,
        Vector3f::UNIT_Y,true));
    kSM.SetTransformation(kXFrm);
    pkFace = kSM.Rectangle(2,2,fZExtent,fXExtent);
    for (i = 0; i < 4; i++)
    {
        pkFace->VBuffer->Color3(0,0) = ColorRGB(0.0f,1.0f,0.0f);
        pkFace->VBuffer->Color3(0,1) = ColorRGB(0.0f,1.0f,0.0f);
        pkFace->VBuffer->Color3(0,2) = ColorRGB(0.0f,1.0f,0.0f);
        pkFace->VBuffer->Color3(0,3) = ColorRGB(0.0f,1.0f,0.0f);
    }
    pkFace->AttachEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // -y face
    kXFrm.SetTranslate(Vector3f(0.0f,-fYExtent,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_X,Vector3f::UNIT_Z,
        -Vector3f::UNIT_Y,true));
    kSM.SetTransformation(kXFrm);
    pkFace = kSM.Rectangle(2,2,fXExtent,fZExtent);
    for (i = 0; i < 4; i++)
    {
        pkFace->VBuffer->Color3(0,0) = ColorRGB(0.0f,0.5f,0.0f);
        pkFace->VBuffer->Color3(0,1) = ColorRGB(0.0f,0.5f,0.0f);
        pkFace->VBuffer->Color3(0,2) = ColorRGB(0.0f,0.5f,0.0f);
        pkFace->VBuffer->Color3(0,3) = ColorRGB(0.0f,0.5f,0.0f);
    }
    pkFace->AttachEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // +x face
    kXFrm.SetTranslate(Vector3f(fXExtent,0.0f,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Y,Vector3f::UNIT_Z,
        Vector3f::UNIT_X,true));
    kSM.SetTransformation(kXFrm);
    pkFace = kSM.Rectangle(2,2,fYExtent,fZExtent);
    for (i = 0; i < 4; i++)
    {
        pkFace->VBuffer->Color3(0,0) = ColorRGB(0.0f,0.0f,1.0f);
        pkFace->VBuffer->Color3(0,1) = ColorRGB(0.0f,0.0f,1.0f);
        pkFace->VBuffer->Color3(0,2) = ColorRGB(0.0f,0.0f,1.0f);
        pkFace->VBuffer->Color3(0,3) = ColorRGB(0.0f,0.0f,1.0f);
    }
    pkFace->AttachEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // -x face
    kXFrm.SetTranslate(Vector3f(-fXExtent,0.0f,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Z,Vector3f::UNIT_Y,
        -Vector3f::UNIT_X,true));
    kSM.SetTransformation(kXFrm);
    pkFace = kSM.Rectangle(2,2,fZExtent,fYExtent);
    for (i = 0; i < 4; i++)
    {
        pkFace->VBuffer->Color3(0,0) = ColorRGB(0.0f,0.0f,0.5f);
        pkFace->VBuffer->Color3(0,1) = ColorRGB(0.0f,0.0f,0.5f);
        pkFace->VBuffer->Color3(0,2) = ColorRGB(0.0f,0.0f,0.5f);
        pkFace->VBuffer->Color3(0,3) = ColorRGB(0.0f,0.0f,0.5f);
    }
    pkFace->AttachEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    m_spkBox = pkBox;
    MoveBox();

    return pkBox;
}
//----------------------------------------------------------------------------
TriMesh* RoughPlaneSolidBox::CreateRamp ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    float fX = 8.0f, fY = 8.0f, fZ = fY*Mathf::Tan((float)m_kModule.Angle);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,6);
    pkVBuffer->Position3(0) = Vector3f(-fX,0.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(+fX,0.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(-fX,fY,0.0f);
    pkVBuffer->Position3(3) = Vector3f(+fX,fY,0.0f);
    pkVBuffer->Position3(4) = Vector3f(-fX,fY,fZ);
    pkVBuffer->Position3(5) = Vector3f(+fX,fY,fZ);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.25f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(0.75f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,4) = Vector2f(0.25f,1.0f);
    pkVBuffer->TCoord2(0,5) = Vector2f(0.75f,1.0f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(18);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] = 0;  aiIndex[ 1] = 1;  aiIndex[ 2] = 4;
    aiIndex[ 3] = 1;  aiIndex[ 4] = 5;  aiIndex[ 5] = 4;
    aiIndex[ 6] = 0;  aiIndex[ 7] = 4;  aiIndex[ 8] = 2;
    aiIndex[ 9] = 1;  aiIndex[10] = 3;  aiIndex[11] = 5;
    aiIndex[12] = 3;  aiIndex[13] = 2;  aiIndex[14] = 4;
    aiIndex[15] = 3;  aiIndex[16] = 4;  aiIndex[17] = 5;

    TriMesh* pkRamp = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    TextureEffect* pkEffect = WM4_NEW TextureEffect("Metal");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkRamp->AttachEffect(pkEffect);

    return pkRamp;
}
//----------------------------------------------------------------------------
TriMesh* RoughPlaneSolidBox::CreateGround ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    TriMesh* pkGround = StandardMesh(kAttr).Rectangle(2,2,32.0f,32.0f);

    // change the texture repeat
    int iVQuantity = pkGround->VBuffer->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        pkGround->VBuffer->TCoord2(0,i) *= 8.0f;
    }

    TextureEffect* pkEffect = WM4_NEW TextureEffect("Gravel");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkGround->AttachEffect(pkEffect);

    return pkGround;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::InitializeModule ()
{
    // set up the physics module
    m_kModule.Mu = 0.01;
    m_kModule.Gravity = 10.0;
    m_kModule.Angle = 0.125*Mathd::PI;
    m_kModule.XLocExt = 0.8;
    m_kModule.YLocExt = 0.4;
    m_kModule.ZLocExt = 0.2;

    // initialize the differential equations
    double dTime = 0.0;
    double dDeltaTime = 0.0005;
    double dX = -6.0;
    double dW = 1.0;
    double dXDot = 4.0;
    double dWDot = 6.0;
    double dTheta = 0.25*Mathd::PI;
    double dThetaDot = 4.0;
    m_kModule.Initialize(dTime,dDeltaTime,dX,dW,dTheta,dXDot,dWDot,dThetaDot);

    m_bDoUpdate = true;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::MoveBox ()
{
    float fX = (float)m_kModule.GetX();
    float fW = (float)m_kModule.GetW();
    float fXExt = (float)m_kModule.XLocExt;
    float fYExt = (float)m_kModule.YLocExt;
    float fZExt = (float)m_kModule.ZLocExt;
    float fSinPhi = (float)m_kModule.SinAngle;
    float fCosPhi = (float)m_kModule.CosAngle;
    float fTheta = (float)m_kModule.GetTheta();
    float fSinTheta = Mathf::Sin(fTheta);
    float fCosTheta = Mathf::Cos(fTheta);

    // compute the box center
    Vector3f kCenter(fX,fW*fCosPhi-fZExt*fSinPhi,fW*fSinPhi+fZExt*fCosPhi);

    // compute the box orientation
    Vector3f kAxis1(fCosTheta,-fSinTheta*fCosPhi,-fSinTheta*fSinPhi);
    Vector3f kAxis2(fSinTheta,+fCosTheta*fCosPhi,+fCosTheta*fSinPhi);
    Vector3f kAxis3(0.0f,-fSinPhi,fCosPhi);

    // keep the box from sliding below the ground
    float fZRadius = fXExt*Mathf::FAbs(kAxis1.Z()) +
        fYExt*Mathf::FAbs(kAxis2.Z()) + fZExt*Mathf::FAbs(kAxis3.Z());
    if (kCenter.Z() >= fZRadius)
    {
        // update the box
        m_spkBox->Local.SetTranslate(kCenter);
        m_spkBox->Local.SetRotate(Matrix3f(kAxis1,kAxis2,kAxis3,true));
        m_spkBox->UpdateGS(0.0f);
    }
    else
    {
        m_bDoUpdate = false;
    }
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::DoPhysical ()
{
    if (m_bDoUpdate)
    {
        m_kModule.Update();
        MoveBox();
    }
}
//----------------------------------------------------------------------------
