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

#include "MaterialTextures.h"
#include "Wm4MaterialTextureEffect.h"

WM4_WINDOW_APPLICATION(MaterialTextures);

//----------------------------------------------------------------------------
MaterialTextures::MaterialTextures ()
    :
    WindowApplication3("MaterialTextures",0,0,640,480,ColorRGBA::WHITE)
{
}
//----------------------------------------------------------------------------
bool MaterialTextures::OnInitialize ()
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
    Vector3f kCLoc = -2.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
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
void MaterialTextures::OnTerminate ()
{
    m_spkScene = 0;
    m_spkMaterial = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void MaterialTextures::OnIdle ()
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
bool MaterialTextures::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    case '+':
    case '=':
        m_spkMaterial->Alpha += 0.1f;
        if (m_spkMaterial->Alpha > 1.0f)
        {
            m_spkMaterial->Alpha = 1.0f;
        }
        return true;
    case '-':
    case '_':
        m_spkMaterial->Alpha -= 0.1f;
        if (m_spkMaterial->Alpha < 0.0f)
        {
            m_spkMaterial->Alpha = 0.0f;
        }
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"MaterialTextures.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void MaterialTextures::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Node* pkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(pkTrnNode);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(-1.0f, 1.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f( 1.0f, 1.0f,0.0f);
    pkVBuffer->Position3(3) = Vector3f( 1.0f,-1.0f,0.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1; aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1; aiIndex[5] = 2;

    // square with door texture
    TriMesh* pkDoor = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkTrnNode->AttachChild(pkDoor);
    pkDoor->AttachEffect(WM4_NEW TextureEffect("Door"));

    // Create a square with
    //   1. material that has diffuse alpha less than one
    //   2. alpha state to enable blending
    //   3. texture
    // The texture is combined with the material to produce a semitransparent
    // effect on the Sand.  You should be able to see Door through it.
    TriMesh* pkSand = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkTrnNode->AttachChild(pkSand);
    pkSand->Local.SetTranslate(Vector3f(0.25f,0.25f,-0.25f));
    pkSand->AttachEffect(WM4_NEW MaterialTextureEffect("Sand"));
    m_spkMaterial = WM4_NEW MaterialState;
    m_spkMaterial->Diffuse = ColorRGB(1.0f,0.0f,0.0f);
    m_spkMaterial->Alpha = 0.5f;
    pkSand->AttachGlobalState(m_spkMaterial);

    // Create another square with material alpha less than one and with
    // a texture.
    TriMesh* pkWater = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkTrnNode->AttachChild(pkWater);
    pkWater->Local.SetTranslate(Vector3f(0.5f,0.5f,-0.5f));
    pkWater->AttachEffect(WM4_NEW MaterialTextureEffect("Water"));
    MaterialState* pkWMaterial = WM4_NEW MaterialState;
    pkWMaterial->Diffuse = ColorRGB(0.0f,0.0f,1.0f);
    pkWMaterial->Alpha = 0.5f;
    pkWater->AttachGlobalState(pkWMaterial);

    // Blending needs to be enabled for material alpha.  It is possible to
    // allow the MaterialTextureEffect object to enable blending when it
    // detects that the material alpha value is smaller than one, but then
    // you are forced into blending.  You might prefer to have a situation
    // where certain events must occur in order to trigger the transparency,
    // in which case you would toggle the BlendEnabled flag yourself.
    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    pkSand->AttachGlobalState(pkAS);
    pkWater->AttachGlobalState(pkAS);
}
//----------------------------------------------------------------------------
