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

#include "PlanarShadows.h"

WM4_WINDOW_APPLICATION(PlanarShadows);

//----------------------------------------------------------------------------
PlanarShadows::PlanarShadows ()
    :
    WindowApplication3("PlanarShadows",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
    m_dUpdateTime = 0.0;
}
//----------------------------------------------------------------------------
bool PlanarShadows::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // scene graph:
    // Root
    //     BipedNode (attach PlanarShadowEffect)
    //     PlaneMesh0 (attach TextureEffect)
    //     PlaneMesh1 (attach TextureEffect)
    //     ProjectorLight

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(80.0f,0.0f,23.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(m_dUpdateTime);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.1f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void PlanarShadows::OnTerminate ()
{
    m_spkScene = 0;
    m_spkBiped = 0;
    m_spkPlane0 = 0;
    m_spkPlane1 = 0;
    m_spkProjectorNode = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void PlanarShadows::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    if (MoveObject())
    {
        m_spkScene->UpdateGS(m_dUpdateTime);
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
bool PlanarShadows::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 'g':
        m_dUpdateTime += 0.01;
        m_spkScene->UpdateGS(m_dUpdateTime);
        return true;
    case 'G':
        m_dUpdateTime = 0.0;
        m_spkScene->UpdateGS(m_dUpdateTime);
        return true;
    }

    const float fDelta = 1.0f;
    Vector3f kTrn;

    switch (ucKey)
    {
    case 'x':
        kTrn = m_spkProjectorNode->Local.GetTranslate();
        kTrn.X() -= fDelta;
        m_spkProjectorNode->Local.SetTranslate(kTrn);
        m_spkProjectorNode->UpdateGS();
        break;
    case 'X':
        kTrn = m_spkProjectorNode->Local.GetTranslate();
        kTrn.X() += fDelta;
        m_spkProjectorNode->Local.SetTranslate(kTrn);
        m_spkProjectorNode->UpdateGS();
        break;
    case 'y':
        kTrn = m_spkProjectorNode->Local.GetTranslate();
        kTrn.Y() -= fDelta;
        m_spkProjectorNode->Local.SetTranslate(kTrn);
        m_spkProjectorNode->UpdateGS();
        break;
    case 'Y':
        kTrn = m_spkProjectorNode->Local.GetTranslate();
        kTrn.Y() += fDelta;
        m_spkProjectorNode->Local.SetTranslate(kTrn);
        m_spkProjectorNode->UpdateGS();
        break;
    case 'z':
        kTrn = m_spkProjectorNode->Local.GetTranslate();
        kTrn.Z() -= fDelta;
        m_spkProjectorNode->Local.SetTranslate(kTrn);
        m_spkProjectorNode->UpdateGS();
        break;
    case 'Z':
        kTrn = m_spkProjectorNode->Local.GetTranslate();
        kTrn.Z() += fDelta;
        m_spkProjectorNode->Local.SetTranslate(kTrn);
        m_spkProjectorNode->UpdateGS();
        break;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"PlanarShadows.wmof");
        return true;
    default:
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
void PlanarShadows::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    LoadBiped();
    CreatePlanes();
    CreatePlanarShadow();
}
//----------------------------------------------------------------------------
void PlanarShadows::LoadBiped ()
{
    const char* acPath = System::GetPath("SkinnedBipedPN.wmof",
        System::SM_READ);
    Stream kIStream;
    kIStream.Load(acPath);
    m_spkBiped = (Node*)kIStream.GetObjectAt(0);
    m_spkScene->AttachChild(m_spkBiped);

    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    pkLight->Diffuse = pkLight->Ambient;
    pkLight->Specular = ColorRGB::BLACK;
    pkLight->DVector = -Vector3f::UNIT_X;
    m_spkBiped->AttachLight(pkLight);
}
//----------------------------------------------------------------------------
void PlanarShadows::CreatePlanes ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);

    // create the floor mesh
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    float fXValue = 128.0f;
    float fYValue = 256.0f;
    float fZValue = 0.0f;
    pkVBuffer->Position3(0) = Vector3f(-fXValue,-fYValue,fZValue);
    pkVBuffer->Position3(1) = Vector3f(+fXValue,-fYValue,fZValue);
    pkVBuffer->Position3(2) = Vector3f(+fXValue,+fYValue,fZValue);
    pkVBuffer->Position3(3) = Vector3f(-fXValue,+fYValue,fZValue);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    m_spkPlane0 = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkPlane0->AttachEffect(WM4_NEW TextureEffect("Sand"));
    m_spkScene->AttachChild(m_spkPlane0);

    // create the wall mesh
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    fXValue = -128.0f;
    fYValue = 256.0f;
    fZValue = 128.0f;
    pkVBuffer->Position3(0) = Vector3f(fXValue,-fYValue,0.0f);
    pkVBuffer->Position3(1) = Vector3f(fXValue,+fYValue,0.0f);
    pkVBuffer->Position3(2) = Vector3f(fXValue,+fYValue,fZValue);
    pkVBuffer->Position3(3) = Vector3f(fXValue,-fYValue,fZValue);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    m_spkPlane1 = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkPlane1->AttachEffect(WM4_NEW TextureEffect("Stone"));
    m_spkScene->AttachChild(m_spkPlane1);
}
//----------------------------------------------------------------------------
void PlanarShadows::CreatePlanarShadow ()
{
#if 1
    Light* pkProjector = WM4_NEW Light(Light::LT_POINT);
    pkProjector->Position = Vector3f(152.0f,-55.0f,53.0f);
#else
    pkProjector = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkProjector->DVector = Vector3f(0.25f,0.25f,-1.0f);
    pkProjector->DVector.Normalize();
#endif

    PlanarShadowEffect* pkEffect = WM4_NEW PlanarShadowEffect(2);
    ColorRGBA kShadowColor(0.0f,0.0f,0.0f,0.5f);
    pkEffect->SetPlane(0,m_spkPlane0);
    pkEffect->SetProjector(0,pkProjector);
    pkEffect->SetShadowColor(0,kShadowColor);
    pkEffect->SetPlane(1,m_spkPlane1);
    pkEffect->SetProjector(1,pkProjector);
    pkEffect->SetShadowColor(1,kShadowColor);
    m_spkBiped->AttachEffect(pkEffect);

    // This guarantees that the shadow is drawn even if the biped is not
    // visible to the camera.
    m_spkBiped->Culling = Spatial::CULL_NEVER;

    m_spkProjectorNode = WM4_NEW LightNode(pkProjector);
    m_spkScene->AttachChild(m_spkProjectorNode);
}
//----------------------------------------------------------------------------
