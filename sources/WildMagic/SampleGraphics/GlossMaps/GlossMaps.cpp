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

#include "GlossMaps.h"
#include "GlossMapEffect.h"

WM4_WINDOW_APPLICATION(GlossMaps);

//----------------------------------------------------------------------------
GlossMaps::GlossMaps ()
    :
    WindowApplication3("GlossMaps",0,0,640,480,
        ColorRGBA(0.85f,0.85f,0.85f,1.0f))
{
    System::InsertDirectory("Data");
}
//----------------------------------------------------------------------------
bool GlossMaps::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // center-and-fit
    m_spkScene->UpdateGS();
    m_spkScene->GetChild(0)->Local.SetTranslate(
        -m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
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
void GlossMaps::OnTerminate ()
{
    m_spkScene = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GlossMaps::OnIdle ()
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
bool GlossMaps::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"GlossMaps.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void GlossMaps::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    Node* pkModel = WM4_NEW Node;
    m_spkScene->AttachChild(pkModel);

    // Create a material and light for use by a gloss-mapped object and a
    // non-gloss-mapped object.
    MaterialState* pkMS = WM4_NEW MaterialState;
    pkMS->Ambient = ColorRGB(0.2f,0.2f,0.2f);
    pkMS->Diffuse = ColorRGB(0.7f,0.7f,0.7f);
    pkMS->Specular = ColorRGB::WHITE;
    pkMS->Shininess = 25.0f;
    pkModel->AttachGlobalState(pkMS);

    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB(0.1f,0.1f,0.1f);
    pkLight->Diffuse = ColorRGB(0.6f,0.6f,0.6f);
    pkLight->Specular = ColorRGB::WHITE;
    pkLight->DVector = Vector3f(0.0f,-1.0f,0.0f);

    // Create vertex and index buffers to be shared by two meshes.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(kAttr,4);
    pkVB->SetName("vb0");
    pkVB->Position3(0) = Vector3f(-0.5f,0.0f,-0.5f);
    pkVB->Position3(1) = Vector3f(-0.5f,0.0f, 0.5f);
    pkVB->Position3(2) = Vector3f( 0.5f,0.0f, 0.5f);
    pkVB->Position3(3) = Vector3f( 0.5f,0.0f,-0.5f);
    pkVB->Normal3(0) = Vector3f::UNIT_Y;
    pkVB->Normal3(1) = Vector3f::UNIT_Y;
    pkVB->Normal3(2) = Vector3f::UNIT_Y;
    pkVB->Normal3(3) = Vector3f::UNIT_Y;
    pkVB->TCoord2(0,0) = Vector2f(1.0f,0.0f);
    pkVB->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVB->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVB->TCoord2(0,3) = Vector2f(0.0f,0.0f);

    VertexBuffer* pkVB1 = WM4_NEW VertexBuffer(kAttr,4);
    pkVB1->SetName("vb1");
    pkVB1->Position3(0) = Vector3f(-0.5f,0.0f,-0.5f);
    pkVB1->Position3(1) = Vector3f(-0.5f,0.0f, 0.5f);
    pkVB1->Position3(2) = Vector3f( 0.5f,0.0f, 0.5f);
    pkVB1->Position3(3) = Vector3f( 0.5f,0.0f,-0.5f);
    pkVB1->Normal3(0) = Vector3f::UNIT_Y;
    pkVB1->Normal3(1) = Vector3f::UNIT_Y;
    pkVB1->Normal3(2) = Vector3f::UNIT_Y;
    pkVB1->Normal3(3) = Vector3f::UNIT_Y;
    pkVB1->TCoord2(0,0) = Vector2f(1.0f,0.0f);
    pkVB1->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVB1->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVB1->TCoord2(0,3) = Vector2f(0.0f,0.0f);

    IndexBuffer* pkIB = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIB->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1;  aiIndex[5] = 2;

    IndexBuffer* pkIB1 = WM4_NEW IndexBuffer(6);
    aiIndex = pkIB1->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1;  aiIndex[5] = 2;

    // Create a non-gloss-mapped square.
    TriMesh* pkNSquare = WM4_NEW TriMesh(pkVB,pkIB);
    pkNSquare->SetName("nsquare");
    pkNSquare->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_X,
        -0.25f*Mathf::PI));
    pkNSquare->Local.SetTranslate(Vector3f(1.0f,-1.0f,0.0f));
    pkNSquare->AttachLight(pkLight);
    pkModel->AttachChild(pkNSquare);

    // Create a gloss-mapped square.
    TriMesh* pkGSquare = WM4_NEW TriMesh(pkVB1,pkIB1);
    pkGSquare->SetName("gsquare");
    pkGSquare->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_X,
        -0.25f*Mathf::PI));
    pkGSquare->Local.SetTranslate(Vector3f(-1.0f,-1.0f,0.0f));
    pkModel->AttachChild(pkGSquare);

    GlossMapEffect* pkGEffect = WM4_NEW GlossMapEffect("Magic",pkLight);
    pkGSquare->AttachEffect(pkGEffect);
}
//----------------------------------------------------------------------------
