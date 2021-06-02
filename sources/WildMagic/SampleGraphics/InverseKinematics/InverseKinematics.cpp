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

#include "InverseKinematics.h"

WM4_WINDOW_APPLICATION(InverseKinematics);

//----------------------------------------------------------------------------
InverseKinematics::InverseKinematics ()
    :
    WindowApplication3("InverseKinematics",0,0,640,480,
        ColorRGBA(0.9f,0.9f,0.9f,1.0f))
{
}
//----------------------------------------------------------------------------
bool InverseKinematics::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,-2.0f,0.5f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();
    UpdateRod();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.1f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void InverseKinematics::OnTerminate ()
{
    m_spkScene = 0;
    m_spkIKSystem = 0;
    m_spkGoal = 0;
    m_spkJoint0 = 0;
    m_spkJoint1 = 0;
    m_spkWireframe = 0;
    m_spkRod = 0;
    m_spkVCEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void InverseKinematics::OnIdle ()
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
bool InverseKinematics::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    if (Transform(ucKey))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"InverseKinematics.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool InverseKinematics::OnMotion (int iButton, int iX, int iY,
    unsigned int uiModifiers)
{
    bool bMoved = WindowApplication3::OnMotion(iButton,iX,iY,uiModifiers);
    if (bMoved)
    {
        UpdateRod();
    }
    return bMoved;
}
//----------------------------------------------------------------------------
void InverseKinematics::CreateScene ()
{
    // Scene
    //     GroundMesh
    //     IKSystem
    //         Goal
    //             GoalCube
    //         Joint0
    //             OriginCube
    //             Rod
    //             Joint1
    //                 EndCube

    // create objects
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    m_spkVCEffect = WM4_NEW VertexColor3Effect;

    m_spkIKSystem = WM4_NEW Node;
    m_spkGoal = WM4_NEW Node;
    m_spkJoint0 = WM4_NEW Node;
    m_spkJoint1 = WM4_NEW Node;
    m_spkRod = CreateRod();

    // transform objects
    m_spkGoal->Local.SetTranslate(2.0f*Vector3f::UNIT_Y);
    m_spkJoint1->Local.SetTranslate(Vector3f::UNIT_X);

    // set parent-child links
    m_spkScene->AttachChild(CreateGround());
    m_spkScene->AttachChild(m_spkIKSystem);
    m_spkIKSystem->AttachChild(m_spkGoal);
    m_spkGoal->AttachChild(CreateCube());
    m_spkIKSystem->AttachChild(m_spkJoint0);
    m_spkJoint0->AttachChild(CreateCube());
    m_spkJoint0->AttachChild(m_spkRod);
    m_spkJoint0->AttachChild(m_spkJoint1);
    m_spkJoint1->AttachChild(CreateCube());

    // create goal
    IKGoalPtr* aspkGoal = WM4_NEW IKGoalPtr[1];
    aspkGoal[0] = WM4_NEW IKGoal(m_spkGoal,m_spkJoint1,1.0f);

    // create joints
    IKJointPtr* aspkJoint = WM4_NEW IKJointPtr[2];

    IKGoalPtr* aspkJGoal0 = WM4_NEW IKGoalPtr[1];
    aspkJGoal0[0] = aspkGoal[0];
    aspkJoint[0] = WM4_NEW IKJoint(m_spkJoint0,1,aspkJGoal0);
    aspkJoint[0]->AllowRotation[2] = true;

    IKGoalPtr* aspkJGoal1 = WM4_NEW IKGoalPtr[1];
    aspkJGoal1[0] = aspkGoal[0];
    aspkJoint[1] = WM4_NEW IKJoint(m_spkJoint1,1,aspkJGoal1);
    aspkJoint[1]->AllowTranslation[2] = true;

    // create IK controller
    IKController* pkIKCtrl = WM4_NEW IKController(2,aspkJoint,1,aspkGoal);
    pkIKCtrl->Iterations = 1;
    m_spkJoint0->AttachController(pkIKCtrl);
}
//----------------------------------------------------------------------------
TriMesh* InverseKinematics::CreateCube ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,8);

    float fSize = 0.1f;
    pkVBuffer->Position3(0) = Vector3f(-fSize,-fSize,-fSize);
    pkVBuffer->Position3(1) = Vector3f(+fSize,-fSize,-fSize);
    pkVBuffer->Position3(2) = Vector3f(+fSize,+fSize,-fSize);
    pkVBuffer->Position3(3) = Vector3f(-fSize,+fSize,-fSize);
    pkVBuffer->Position3(4) = Vector3f(-fSize,-fSize,+fSize);
    pkVBuffer->Position3(5) = Vector3f(+fSize,-fSize,+fSize);
    pkVBuffer->Position3(6) = Vector3f(+fSize,+fSize,+fSize);
    pkVBuffer->Position3(7) = Vector3f(-fSize,+fSize,+fSize);
    pkVBuffer->Color3(0,0) = ColorRGB(0.0f,0.0f,1.0f);
    pkVBuffer->Color3(0,1) = ColorRGB(0.0f,1.0f,0.0f);
    pkVBuffer->Color3(0,2) = ColorRGB(1.0f,0.0f,0.0f);
    pkVBuffer->Color3(0,3) = ColorRGB(0.0f,0.0f,0.0f);
    pkVBuffer->Color3(0,4) = ColorRGB(0.0f,0.0f,1.0f);
    pkVBuffer->Color3(0,5) = ColorRGB(1.0f,0.0f,1.0f);
    pkVBuffer->Color3(0,6) = ColorRGB(1.0f,1.0f,0.0f);
    pkVBuffer->Color3(0,7) = ColorRGB(1.0f,1.0f,1.0f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(36);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] = 0; aiIndex[ 1] = 2; aiIndex[ 2] = 1;
    aiIndex[ 3] = 0; aiIndex[ 4] = 3; aiIndex[ 5] = 2;
    aiIndex[ 6] = 4; aiIndex[ 7] = 5; aiIndex[ 8] = 6;
    aiIndex[ 9] = 4; aiIndex[10] = 6; aiIndex[11] = 7;
    aiIndex[12] = 1; aiIndex[13] = 6; aiIndex[14] = 5;
    aiIndex[15] = 1; aiIndex[16] = 2; aiIndex[17] = 6;
    aiIndex[18] = 0; aiIndex[19] = 7; aiIndex[20] = 3;
    aiIndex[21] = 0; aiIndex[22] = 4; aiIndex[23] = 7;
    aiIndex[24] = 0; aiIndex[25] = 1; aiIndex[26] = 5;
    aiIndex[27] = 0; aiIndex[28] = 5; aiIndex[29] = 4;
    aiIndex[30] = 3; aiIndex[31] = 6; aiIndex[32] = 2;
    aiIndex[33] = 3; aiIndex[34] = 7; aiIndex[35] = 6;

    TriMesh* pkCube = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkCube->AttachEffect(m_spkVCEffect);

    return pkCube;
}
//----------------------------------------------------------------------------
Polyline* InverseKinematics::CreateRod ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,2);
    pkVBuffer->Position3(0) = Vector3f::ZERO;
    pkVBuffer->Position3(1) = Vector3f::UNIT_X;
    pkVBuffer->Color3(0,0) = ColorRGB::WHITE;
    pkVBuffer->Color3(0,1) = ColorRGB::WHITE;

    Polyline* pkLine = WM4_NEW Polyline(pkVBuffer,false,true);
    pkLine->AttachEffect(m_spkVCEffect);

    return pkLine;
}
//----------------------------------------------------------------------------
TriMesh* InverseKinematics::CreateGround ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    float fSize = 16.0f;
    pkVBuffer->Position3(0) = Vector3f(-fSize,-fSize,-0.1f);
    pkVBuffer->Position3(1) = Vector3f(+fSize,-fSize,-0.1f);
    pkVBuffer->Position3(2) = Vector3f(+fSize,+fSize,-0.1f);
    pkVBuffer->Position3(3) = Vector3f(-fSize,+fSize,-0.1f);
    pkVBuffer->Color3(0,0) = ColorRGB(0.5f,0.5f,0.70f);
    pkVBuffer->Color3(0,1) = ColorRGB(0.5f,0.5f,0.80f);
    pkVBuffer->Color3(0,2) = ColorRGB(0.5f,0.5f,0.90f);
    pkVBuffer->Color3(0,3) = ColorRGB(0.5f,0.5f,1.00f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0; aiIndex[1] = 1; aiIndex[2] = 2;
    aiIndex[3] = 0; aiIndex[4] = 2; aiIndex[5] = 3;

    TriMesh* pkPlane = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkPlane->AttachEffect(m_spkVCEffect);

    return pkPlane;
}
//----------------------------------------------------------------------------
void InverseKinematics::UpdateRod ()
{
    // The vertex[0] never moves.  The rod mesh is in the coordinate system
    // of joint0, so use the local translation of joint1 for the rod mesh's
    // moving end point.
    m_spkRod->VBuffer->Position3(1) = m_spkJoint1->Local.GetTranslate();
    m_spkRod->VBuffer->Release();
    m_spkRod->UpdateMS();
    m_spkRod->UpdateGS();
}
//----------------------------------------------------------------------------
bool InverseKinematics::Transform (unsigned char ucKey)
{
    Matrix3f kRot, kIncr;
    Vector3f kTrn;

    switch (ucKey)
    {
    case 'x':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.X() -= m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'X':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.X() += m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'y':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Y() -= m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'Y':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Y() += m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'z':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Z() -= m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'Z':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Z() += m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    default:
        return false;
    }

    m_spkIKSystem->UpdateGS();
    UpdateRod();
    return true;
}
//----------------------------------------------------------------------------
