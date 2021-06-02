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
// Version: 4.0.0 (2006/06/28)

#include "ProjectedTextures.h"
#include "ProjectedTextureEffect.h"

WM4_WINDOW_APPLICATION(ProjectedTextures);

//----------------------------------------------------------------------------
ProjectedTextures::ProjectedTextures ()
    :
    WindowApplication3("ProjectedTextures",0,0,640,480,ColorRGBA::WHITE)
{
}
//----------------------------------------------------------------------------
bool ProjectedTextures::OnInitialize ()
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
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ProjectedTextures::OnTerminate ()
{
    m_spkScene = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ProjectedTextures::OnIdle ()
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
bool ProjectedTextures::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"ProjectedTextures.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ProjectedTextures::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    Node* pkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(pkTrnNode);

    // load the face model
    const char* acPath = System::GetPath("FacePN.wmof",System::SM_READ);
    Stream kIStream;
    bool bLoaded = kIStream.Load(acPath);
    assert(bLoaded);
    TriMesh* pkMesh = StaticCast<TriMesh>(kIStream.GetObjectAt(0));

    // create a material for the face
    MaterialState* pkMaterial = WM4_NEW MaterialState;
    pkMaterial->Emissive = ColorRGB(0.0f,0.0f,0.0f);
    pkMaterial->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    pkMaterial->Diffuse = ColorRGB(0.99607f,0.83920f,0.67059f);
    pkMaterial->Specular = ColorRGB(0.8f,0.8f,0.8f);
    pkMaterial->Alpha = 1.0f;
    pkMaterial->Shininess = 0.0f;
    pkMesh->AttachGlobalState(pkMaterial);

    // create a camera to project the texture
    Camera* pkProjector = WM4_NEW Camera;
    pkProjector->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -303.0f*kCDir;
    pkProjector->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // create a directional light for the face
    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB(0.25f,0.25f,0.25f);
    pkLight->Diffuse = ColorRGB::WHITE;
    pkLight->Specular = ColorRGB::BLACK;
    pkLight->DVector = Vector3f::UNIT_Y;  // scene-camera direction

    // create the effect
    ProjectedTextureEffect* pkEffect = WM4_NEW ProjectedTextureEffect(
        pkProjector,"SunFire",pkLight);
    pkMesh->AttachEffect(pkEffect);

    pkTrnNode->AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
