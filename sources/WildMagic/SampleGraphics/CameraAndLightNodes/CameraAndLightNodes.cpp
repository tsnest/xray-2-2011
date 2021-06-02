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
// Version: 4.0.4 (2007/09/16)

#include "CameraAndLightNodes.h"

WM4_WINDOW_APPLICATION(CameraAndLightNodes);

//----------------------------------------------------------------------------
CameraAndLightNodes::CameraAndLightNodes ()
    :
    WindowApplication3("CameraAndLightNodes",0,0,640,480,
        ColorRGBA(0.5f,0.5f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool CameraAndLightNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // scene -+--> groundPoly
    //        |
    //        +--> cameraNode --+--> lightFixture0 +--> lightNode0
    //                          |                  |
    //                          |                  +--> lightTarget0
    //                          |
    //                          +--> lightFixture1 +--> lightNode1
    //                                             |
    //                                             +--> lightTarget0

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,-100.0f,5.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
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

    InitializeCameraMotion(0.01f,0.001f);
    return true;
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframeState = 0;
    m_spkCNode = 0;
    m_spkScreenCamera = 0;
    m_spkSky = 0;
    m_spkAdjustableLight0 = 0;
    m_spkAdjustableLight1 = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->SetCamera(m_spkScreenCamera);
        m_pkRenderer->Draw(m_spkSky);

        m_pkRenderer->SetCamera(m_spkCamera);
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());

        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool CameraAndLightNodes::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframeState->Enabled = !m_spkWireframeState->Enabled;
        return true;

    case '+':  // increase light intensity
    case '=':
        m_spkAdjustableLight0->Intensity += 0.1f;
        m_spkAdjustableLight1->Intensity += 0.1f;
        return true;

    case '-':  // decrease light intensity
    case '_':
        if (m_spkAdjustableLight0->Intensity >= 0.1f)
        {
            m_spkAdjustableLight0->Intensity -= 0.1f;
        }
        if (m_spkAdjustableLight1->Intensity >= 0.1f)
        {
            m_spkAdjustableLight1->Intensity -= 0.1f;
        }
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"CameraAndLightNodes.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::CreateScene ()
{
    CreateScreenPolygon();

    m_spkScene = WM4_NEW Node;
    m_spkWireframeState = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframeState);

    TriMesh* pkGround = CreateGround();
    m_spkScene->AttachChild(pkGround);
    m_spkCNode = WM4_NEW CameraNode(m_spkCamera);
    m_spkScene->AttachChild(m_spkCNode);

    Node* pkLFixture0 = CreateLightFixture(m_spkAdjustableLight0);
    pkLFixture0->Local.SetTranslate(Vector3f(25.0f,-5.75f,6.0f));
    pkLFixture0->Local.SetRotate(Matrix3f(Vector3f::UNIT_X,-Mathf::HALF_PI));
    m_spkCNode->AttachChild(pkLFixture0);

    Node* pkLFixture1 = CreateLightFixture(m_spkAdjustableLight1);
    pkLFixture1->Local.SetTranslate(Vector3f(25.0f,-5.75f,-6.0f));
    pkLFixture1->Local.SetRotate(Matrix3f(Vector3f::UNIT_X,-Mathf::HALF_PI));
    m_spkCNode->AttachChild(pkLFixture1);
}
//----------------------------------------------------------------------------
TriMesh* CameraAndLightNodes::CreateGround ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);

    float fMax = 100.0f;
    pkVBuffer->Position3(0) = Vector3f(-fMax,-fMax,0.0f);
    pkVBuffer->Position3(1) = Vector3f(+fMax,-fMax,0.0f);
    pkVBuffer->Position3(2) = Vector3f(+fMax,+fMax,0.0f);
    pkVBuffer->Position3(3) = Vector3f(-fMax,+fMax,0.0f);
    pkVBuffer->Normal3(0) = Vector3f::UNIT_Z;
    pkVBuffer->Normal3(1) = Vector3f::UNIT_Z;
    pkVBuffer->Normal3(2) = Vector3f::UNIT_Z;
    pkVBuffer->Normal3(3) = Vector3f::UNIT_Z;
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(8.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(8.0f,8.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,8.0f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    Light* pkLight = WM4_NEW Light(Light::LT_AMBIENT);
    pkLight->Ambient = ColorRGB::WHITE;
    pkMesh->AttachLight(pkLight);

    TextureEffect* pkEffect = WM4_NEW TextureEffect("Gravel");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetFilterType(Texture::LINEAR_LINEAR);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkMesh->AttachEffect(pkEffect);

    AlphaState* pkAState = pkEffect->GetBlending(0);
    pkAState->SrcBlend = AlphaState::SBF_DST_COLOR;
    pkAState->DstBlend = AlphaState::DBF_ZERO;

    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* CameraAndLightNodes::CreateLightTarget (Light* pkLight)
{
    // Create a parabolic rectangle patch that is illuminated by the light.
    // To hide the artifacts of vertex normal lighting on a grid, the patch
    // is slightly bent so that the intersection with a plane is nearly
    // circular.  The patch is translated slightly below the plane of the
    // ground to hide the corners and the jaggies.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    TriMesh* pkMesh = StandardMesh(kAttr).Rectangle(64,64,8.0f,8.0f);
    VertexBuffer* pkVBuffer = pkMesh->VBuffer;
    int iVQuantity = pkVBuffer->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        Vector3f kPos = pkVBuffer->Position3(i);
        kPos.Z() = 1.0f - (kPos.X()*kPos.X()+kPos.Y()*kPos.Y())/128.0f;
        pkVBuffer->Position3(i) = kPos;
    }
    pkMesh->UpdateMS();

    AlphaState* pkAState = new AlphaState;
    pkAState->BlendEnabled = true;
    pkMesh->AttachGlobalState(pkAState);

    MaterialState* pkMState = new MaterialState;
    pkMState->Emissive = ColorRGB(0.0f,0.0f,0.0f);
    pkMState->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    pkMState->Diffuse = ColorRGB(1.0f,0.85f,0.75f);
    pkMState->Specular = ColorRGB(0.8f,0.8f,0.8f);
    pkMState->Shininess = 1.0f;
    pkMState->Alpha = 0.5f;
    pkMesh->AttachGlobalState(pkMState);
    pkMesh->AttachLight(pkLight);

    return pkMesh;
}
//----------------------------------------------------------------------------
Node* CameraAndLightNodes::CreateLightFixture (LightPtr& rspkAdjustableLight)
{
    Node* pkLFixture = WM4_NEW Node;

    // point light illuminates the target
    Light* pkPLight = WM4_NEW Light(Light::LT_POINT);
    pkPLight->Ambient = ColorRGB(1.0f,1.0f,0.5f);
    pkPLight->Diffuse = ColorRGB(1.0f,1.0f,0.5f);
    pkPLight->Specular = ColorRGB(1.0f,1.0f,0.5f);
    rspkAdjustableLight = pkPLight;

    // the target itself
    TriMesh* pkLTarget = CreateLightTarget(pkPLight);

    // Encapsulate the light in a light node.  Rotate the light node so the
    // light points down.
    LightNode* pkLNode = WM4_NEW LightNode(pkPLight);

    pkLFixture->AttachChild(pkLNode);
    pkLFixture->AttachChild(pkLTarget);

    return pkLFixture;
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::CreateScreenPolygon ()
{
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    m_spkScreenCamera = WM4_NEW Camera;
    m_spkScreenCamera->Perspective = false;
    m_spkScreenCamera->SetFrustum(0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    m_spkScreenCamera->SetFrame(Vector3f::ZERO,Vector3f::UNIT_Z,
        Vector3f::UNIT_Y,Vector3f::UNIT_X);

    // Create a background screen polygon.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,0.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,1.0f,1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,0.0f);
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    m_spkSky = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkSky->AttachEffect(WM4_NEW TextureEffect("RedSky"));
    m_spkSky->UpdateGS();
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveForward ()
{
    Vector3f kLocation = m_spkCNode->Local.GetTranslate();
    Vector3f kDirection = m_spkCNode->Local.GetRotate().GetColumn(0);
    kLocation += m_fTrnSpeed*kDirection;
    m_spkCNode->Local.SetTranslate(kLocation);
    m_spkCNode->UpdateGS();
    m_kCuller.ComputeVisibleSet(m_spkScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveBackward ()
{
    Vector3f kLocation = m_spkCNode->Local.GetTranslate();
    Vector3f kDirection = m_spkCNode->Local.GetRotate().GetColumn(0);
    kLocation -= m_fTrnSpeed*kDirection;
    m_spkCNode->Local.SetTranslate(kLocation);
    m_spkCNode->UpdateGS();
    m_kCuller.ComputeVisibleSet(m_spkScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::TurnLeft ()
{
    Vector3f kUp = m_spkCNode->Local.GetRotate().GetColumn(1);
    m_spkCNode->Local.SetRotate(
        Matrix3f(kUp,m_fRotSpeed)*m_spkCNode->Local.GetRotate());
    m_spkCNode->UpdateGS();
    m_kCuller.ComputeVisibleSet(m_spkScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::TurnRight ()
{
    Vector3f kUp = m_spkCNode->Local.GetRotate().GetColumn(1);
    m_spkCNode->Local.SetRotate(
        Matrix3f(kUp,-m_fRotSpeed)*m_spkCNode->Local.GetRotate());
    m_spkCNode->UpdateGS();
    m_kCuller.ComputeVisibleSet(m_spkScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveUp ()
{
    // disabled
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveDown ()
{
    // disabled
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::LookUp ()
{
    // disabled
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::LookDown ()
{
    // disabled
}
//----------------------------------------------------------------------------
