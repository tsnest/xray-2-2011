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
// Version: 4.0.2 (2008/10/13)

#include "ShadowMaps.h"

WM4_WINDOW_APPLICATION(ShadowMaps);

const char* ShadowMaps::ms_aacCaptions[3] =
{
    "mouse moves scene and light together (press 'm' to change)",
    "mouse moves scene (press 'm' to change)",
    "mouse moves light (press 'm' to change)"
};

//----------------------------------------------------------------------------
ShadowMaps::ShadowMaps ()
    :
    WindowApplication3("ShadowMaps",0,0,512,512,
        ColorRGBA(0.0f,0.25f,0.75f,1.0f))
{
    m_acCaption = 0;
}
//----------------------------------------------------------------------------
bool ShadowMaps::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,1.0f,1.0f,100.0f);
    Vector3f kCLoc = 4.0f*Vector3f(2.0f,0.0f,1.0f);
    Vector3f kCDir = -kCLoc;  // lookat origin
    kCDir.Normalize();
    Vector3f kCUp(kCDir.Z(),0.0f,-kCDir.X());
    Vector3f kCRight(0.0f,1.0f,0.0f);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    m_acCaption = ms_aacCaptions[0];
    return true;
}
//----------------------------------------------------------------------------
void ShadowMaps::OnTerminate ()
{
    m_spkScene = 0;
    m_spkShadowMapNode = 0;
    m_spkFlashlight = 0;
    m_spkGround = 0;
    m_spkTorus = 0;
    m_spkGlobe = 0;
    m_spkLightSphere = 0;
    m_spkWireframe = 0;
    m_spkSMEffect = 0;
    m_spkLight = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ShadowMaps::OnIdle ()
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
        m_pkRenderer->Draw(8,16,ColorRGBA::WHITE,m_acCaption);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ShadowMaps::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    const float fTrnDelta = 0.1f;
    Vector3f kTrn;

    switch (ucKey)
    {
    // Toggle wireframe for the entire scene.
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    // Move the spotlight within its plane of constant z.
    case 'x':
        kTrn = m_spkFlashlight->Local.GetTranslate();
        kTrn.X() -= fTrnDelta;
        m_spkFlashlight->Local.SetTranslate(kTrn);
        m_spkFlashlight->UpdateGS();
        return true;
    case 'X':
        kTrn = m_spkFlashlight->Local.GetTranslate();
        kTrn.X() += fTrnDelta;
        m_spkFlashlight->Local.SetTranslate(kTrn);
        m_spkFlashlight->UpdateGS();
        return true;
    case 'y':
        kTrn = m_spkFlashlight->Local.GetTranslate();
        kTrn.Y() -= fTrnDelta;
        m_spkFlashlight->Local.SetTranslate(kTrn);
        m_spkFlashlight->UpdateGS();
        return true;
    case 'Y':
        kTrn = m_spkFlashlight->Local.GetTranslate();
        kTrn.Y() += fTrnDelta;
        m_spkFlashlight->Local.SetTranslate(kTrn);
        m_spkFlashlight->UpdateGS();
        return true;
    case 'z':
        kTrn = m_spkFlashlight->Local.GetTranslate();
        kTrn.Z() -= fTrnDelta;
        m_spkFlashlight->Local.SetTranslate(kTrn);
        m_spkFlashlight->UpdateGS();
        return true;
    case 'Z':
        kTrn = m_spkFlashlight->Local.GetTranslate();
        kTrn.Z() += fTrnDelta;
        m_spkFlashlight->Local.SetTranslate(kTrn);
        m_spkFlashlight->UpdateGS();
        return true;

    // Change the object affected by the virtual trackball.
    case 'm':
    case 'M':
        if (m_spkMotionObject == m_spkScene)
        {
            m_spkMotionObject = m_spkShadowMapNode;
            m_acCaption = ms_aacCaptions[1];
        }
        else if (m_spkMotionObject == m_spkShadowMapNode)
        {
            m_spkMotionObject = m_spkFlashlight;
            m_acCaption = ms_aacCaptions[2];
        }
        else if (m_spkMotionObject == m_spkFlashlight)
        {
            m_spkMotionObject = m_spkScene;
            m_acCaption = ms_aacCaptions[0];
        }
        return true;

    // Adjust the polygon offset scale.
    case '+':
    case '=':
        m_spkSMEffect->SetDepthBias(m_spkSMEffect->GetDepthBias()+0.001f);
        return true;

    case '-':
    case '_':
        m_spkSMEffect->SetDepthBias(m_spkSMEffect->GetDepthBias()-0.001f);
        return true;

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"ShadowMaps.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateScene ()
{
    // The scene graph is
    //
    // scene -+---> shadowmapnode -+---> ground
    //        |                    |
    //        |                    +---> torus
    //        |                    |
    //        |                    +---> globe
    //        |
    //        +---> flashlight -+---> lightsphere (visualize light source)
    //                          |
    //                          +---> spotlight
    //                          |
    //                          +---> projector camera
    //
    // The ground is the plane z = 0 with a normal vector (0,0,1).  The torus
    // has center at z = 1 and the dodecahedron has center at z = 2.  The
    // flashlight center is at z = 4.  The torus and dodecahedron may be
    // rotated about their centers.  The flashlight may be translated in x or
    // y and rotated about its center.

    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);
    m_spkShadowMapNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkShadowMapNode);

    CreateFlashlight();
    CreateGround();
    CreateTorus();
    CreateGlobe();
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateGround ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    m_spkGround = StandardMesh(kAttr).Rectangle(16,16,8.0f,8.0f);
    m_spkShadowMapNode->AttachChild(m_spkGround);

    TextureEffect* pkEffect = WM4_NEW TextureEffect("Stone");
    AlphaState* pkAState = pkEffect->GetBlending(0);
    pkAState->SrcBlend = AlphaState::SBF_DST_COLOR;
    pkAState->DstBlend = AlphaState::DBF_ZERO;

    m_spkGround->AttachEffect(pkEffect);
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateTorus ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    m_spkTorus = StandardMesh(kAttr).Torus(64,64,1.0f,0.2f);
    m_spkShadowMapNode->AttachChild(m_spkTorus);
    m_spkTorus->Local.SetTranslate(Vector3f(0.0f,0.0f,1.0f));

    MaterialState* pkMaterial = WM4_NEW MaterialState;
    pkMaterial->Emissive = ColorRGB::BLACK;
    pkMaterial->Ambient = ColorRGB(0.2295f,0.08825f,0.0275f);
    pkMaterial->Diffuse = ColorRGB(0.5508f,0.2118f,0.066f);
    pkMaterial->Specular = ColorRGB(0.580594f,0.223257f,0.0695701f);
    pkMaterial->Alpha = 1.0f;
    pkMaterial->Shininess = 51.2f;
    m_spkTorus->AttachGlobalState(pkMaterial);
    m_spkTorus->AttachLight(m_spkLight);
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateGlobe ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    m_spkGlobe = StandardMesh(kAttr).Sphere(64,64,0.5f);
    m_spkShadowMapNode->AttachChild(m_spkGlobe);
    m_spkGlobe->Local.SetTranslate(Vector3f(0.0f,0.0f,2.0f));
    m_spkGlobe->Local.SetUniformScale(0.5f);

    MaterialState* pkMaterial = WM4_NEW MaterialState;
    pkMaterial->Emissive = ColorRGB::BLACK;
    pkMaterial->Ambient = ColorRGB(0.24725f,0.4245f,0.0645f);
    pkMaterial->Diffuse = ColorRGB(0.34615f,0.4143f,0.0903f);
    pkMaterial->Specular = ColorRGB(0.797357f,0.523991f,0.208006f);
    pkMaterial->Alpha = 1.0f;
    pkMaterial->Shininess = 83.2f;
    m_spkGlobe->AttachGlobalState(pkMaterial);
    m_spkGlobe->AttachLight(m_spkLight);
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateFlashlight ()
{
    m_spkFlashlight = WM4_NEW Node;
    m_spkFlashlight->Local.SetTranslate(Vector3f(0.0f,0.0f,4.0f));
    m_spkScene->AttachChild(m_spkFlashlight);

    // Create a sphere to represent the light source.  The sphere has a
    // gradient of yellow color, bright at the bottom (facing the objects to
    // be lit) and dark at the top.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    float fRadius = 0.1f;
    m_spkLightSphere = StandardMesh(kAttr).Sphere(32,32,fRadius);
    m_spkFlashlight->AttachChild(m_spkLightSphere);
    VertexBuffer* pkVBuffer = m_spkLightSphere->VBuffer;
    int iVQuantity = pkVBuffer->GetVertexQuantity();
    ColorRGB kYellow(1.0f,1.0f,0.0f);
    for (int i = 0; i < iVQuantity; i++)
    {
        float fAmpl = 0.5f*(fRadius - pkVBuffer->Position3(i).Z())/fRadius;
        pkVBuffer->Color3(0,i) = fAmpl*kYellow;
    }
    m_spkLightSphere->AttachEffect(WM4_NEW VertexColor3Effect);

    // Create the spotlight.
    m_spkLight = WM4_NEW Light(Light::LT_SPOT);
    m_spkLight->Ambient = ColorRGB::WHITE;
    m_spkLight->Diffuse = ColorRGB::WHITE;
    m_spkLight->Specular = ColorRGB::WHITE;
    m_spkLight->Exponent = 1.0f;
    m_spkLight->SetAngle(Mathf::HALF_PI);
    m_spkFlashlight->AttachChild(WM4_NEW LightNode(m_spkLight));

    // Create a camera to project the texture and shadows.  The default
    // coordinate frame is the same as the light's, so no need to set these.
    // The frustum values were chosen by empirical means.  That is, I modified
    // them until the projection looked right.
    Camera* pkProjector = WM4_NEW Camera;
    pkProjector->SetFrustum(120.0f,1.0f,1.0f,5.0f);
    m_spkFlashlight->AttachChild(WM4_NEW CameraNode(pkProjector));

    // The depth bias parameter was chosen by empirical means; that is, I
    // modified it until the scene looked right.
    float fDepthBias = 0.02f;

    m_spkSMEffect = WM4_NEW ShadowMapEffect(pkProjector,"Magician",512,512,
        fDepthBias);

    m_spkShadowMapNode->AttachEffect(m_spkSMEffect);
}
//----------------------------------------------------------------------------
