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
// Version: 4.0.3 (2007/09/24)

#include "Collisions.h"

WM4_WINDOW_APPLICATION(Collisions);

Vector2f Collisions::ms_kBlueUV(0.25f,0.25f);
Vector2f Collisions::ms_kCyanUV(0.75f,0.25f);
Vector2f Collisions::ms_kRedUV(0.25f,0.75f);
Vector2f Collisions::ms_kYellowUV(0.75f,0.75f);

//----------------------------------------------------------------------------
Collisions::Collisions ()
    :
    WindowApplication3("Collisions",0,0,640,480,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool Collisions::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(4.0f,0.0f,0.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Collisions::OnTerminate ()
{
    WM4_DELETE m_pkGroup;
    m_spkScene = 0;
    m_spkCyln0 = 0;
    m_spkCyln1 = 0;
    m_spkCylinderImage = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Collisions::OnIdle ()
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
bool Collisions::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    // move the tall/thin cylinder and activate the collision system
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
        TestStreaming(m_spkScene,128,128,640,480,"Collisions.wmof");
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void Collisions::CreateScene ()
{
    // The root of the scene will have two cylinders as children.
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Create a texture image to be used by both cylinders.
    unsigned int* auiData = WM4_NEW unsigned int[4];
    auiData[0] = System::MakeRGB(0,0,255);    // blue
    auiData[1] = System::MakeRGB(0,255,255);  // cyan
    auiData[2] = System::MakeRGB(255,0,0);    // red
    auiData[3] = System::MakeRGB(255,255,0);  // yellow
    m_spkCylinderImage = WM4_NEW Image(Image::IT_RGBA8888,2,2,
        (unsigned char*)auiData,"CylinderImage");
    Effect* pkEffect = WM4_NEW TextureEffect("CylinderImage");

    // Create two cylinders, one short and thick, one tall and thin.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);

    m_spkCyln0 = kSM.Cylinder(8,16,1.0f,2.0f,false);
    int i;
    for (i = 0; i < m_spkCyln0->VBuffer->GetVertexQuantity(); i++)
    {
        m_spkCyln0->VBuffer->TCoord2(0,i) = ms_kBlueUV;
    }
    m_spkCyln0->AttachEffect(pkEffect);
    m_spkScene->AttachChild(m_spkCyln0);

    m_spkCyln1 = kSM.Cylinder(16,8,0.25,4.0,false);
    for (i = 0; i < m_spkCyln0->VBuffer->GetVertexQuantity(); i++)
    {
        m_spkCyln1->VBuffer->TCoord2(0,i) = ms_kRedUV;
    }
    m_spkCyln1->AttachEffect(pkEffect);
    m_spkScene->AttachChild(m_spkCyln1);

    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // Set up collision stuff.  Record0 handles the collision response.
    // Record1 is not given a callback so that 'double processing' of the
    // events does not occur.

    CollisionRecord* pkRec0 = WM4_NEW CollisionRecord(m_spkCyln0,
        WM4_NEW BoxBVTree(m_spkCyln0,1,false),0,Response,this);

    CollisionRecord* pkRec1 = WM4_NEW CollisionRecord(m_spkCyln1,
        WM4_NEW BoxBVTree(m_spkCyln1,1,false),0,0,0);

    m_pkGroup = WM4_NEW CollisionGroup;
    m_pkGroup->Add(pkRec0);
    m_pkGroup->Add(pkRec1);

    ResetColors();
    m_pkGroup->TestIntersection();
}
//----------------------------------------------------------------------------
bool Collisions::Transform (unsigned char ucKey)
{
    // Move the tall/thin cylinder.  After each motion, reset the texture
    // coordinates to the "no intersection" state, then let the collision
    // system test for intersection.  Any intersecting triangles have their
    // texture coordinates changed to the "intersection" state.

    float fTrnSpeed = 0.1f;
    float fRotSpeed = 0.1f;

    Matrix3f kRot, kIncr;
    Vector3f kTrn;

    switch (ucKey)
    {
        case 'x':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.X() -= fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'X':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.X() += fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'y':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Y() -= fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'Y':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Y() += fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'z':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Z() -= fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'Z':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Z() += fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'r':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'R':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,-fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'a':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'A':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,-fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'p':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'P':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,-fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        default:
            return false;
    }

    // activate the collision system
    m_spkCyln1->UpdateGS(0.0f);
    ResetColors();
    m_pkGroup->TestIntersection();
    return true;
}
//----------------------------------------------------------------------------
void Collisions::ResetColors ()
{
    int i, iVQuantity = m_spkCyln0->VBuffer->GetVertexQuantity();
    for (i = 0; i < iVQuantity; i++)
    {
        m_spkCyln0->VBuffer->TCoord2(0,i) = ms_kBlueUV;
    }
    m_spkCyln0->VBuffer->Release();

    iVQuantity = m_spkCyln1->VBuffer->GetVertexQuantity();
    for (i = 0; i < iVQuantity; i++)
    {
        m_spkCyln1->VBuffer->TCoord2(0,i) = ms_kRedUV;
    }
    m_spkCyln1->VBuffer->Release();
}
//----------------------------------------------------------------------------
void Collisions::Response (CollisionRecord& rkRecord0, int iT0,
    CollisionRecord& rkRecord1, int iT1, Intersector<float,Vector3f>*)
{
    // Mesh0 triangles that are intersecting change from blue to cyan.
    TriMesh* pkMesh = rkRecord0.GetMesh();
    const int* aiIndex = pkMesh->IBuffer->GetData();
    int i0 = aiIndex[3*iT0];
    int i1 = aiIndex[3*iT0+1];
    int i2 = aiIndex[3*iT0+2];
    pkMesh->VBuffer->TCoord2(0,i0) = ms_kCyanUV;
    pkMesh->VBuffer->TCoord2(0,i1) = ms_kCyanUV;
    pkMesh->VBuffer->TCoord2(0,i2) = ms_kCyanUV;
    pkMesh->VBuffer->Release();

    // Mesh1 triangles that are intersecting change from red to yellow.
    pkMesh = rkRecord1.GetMesh();
    aiIndex = pkMesh->IBuffer->GetData();
    i0 = aiIndex[3*iT1];
    i1 = aiIndex[3*iT1+1];
    i2 = aiIndex[3*iT1+2];
    pkMesh->VBuffer->TCoord2(0,i0) = ms_kYellowUV;
    pkMesh->VBuffer->TCoord2(0,i1) = ms_kYellowUV;
    pkMesh->VBuffer->TCoord2(0,i2) = ms_kYellowUV;
    pkMesh->VBuffer->Release();
}
//----------------------------------------------------------------------------
