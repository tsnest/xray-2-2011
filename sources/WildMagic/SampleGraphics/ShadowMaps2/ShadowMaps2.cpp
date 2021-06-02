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
// Version: 4.5.4 (2007/08/11)

#include "ShadowMaps2.h"

WM4_WINDOW_APPLICATION(ShadowMaps2);

//----------------------------------------------------------------------------
ShadowMaps2::ShadowMaps2 ()
    :
    WindowApplication3("ShadowMaps2",0,0,640,480,
        ColorRGBA(0.75f,0.75f,0.75f,1.0f))
{
    m_pkShadowFBO = 0;
    m_pkUnlitFBO = 0;
    m_pkHBlurFBO = 0;
}
//----------------------------------------------------------------------------
bool ShadowMaps2::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,1.0f,0.1f,100.0f);
    Vector3f kCLoc = Vector3f(8.0f,0.0f,4.0f);
    Vector3f kCDir = -kCLoc;  // lookat origin
    kCDir.Normalize();
    Vector3f kCUp(kCDir.Z(),0,-kCDir.X());
    Vector3f kCRight = Vector3f::UNIT_Y;
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
    return true;
}
//----------------------------------------------------------------------------
void ShadowMaps2::OnTerminate ()
{
    WM4_DELETE m_pkShadowFBO;
    m_spkShadowImage = 0;
    m_spkShadowTarget = 0;
    m_spkShadowEffect = 0;

    WM4_DELETE m_pkUnlitFBO;
    m_spkUnlitImage = 0;
    m_spkUnlitTarget = 0;
    m_spkUnlitEffect = 0;

    WM4_DELETE m_pkHBlurFBO;
    m_spkHBlurImage = 0;
    m_spkHBlurTarget = 0;
    m_spkHBlurEffect = 0;
    m_spkVBlurEffect = 0;

    m_spkScene = 0;
    m_spkPlane = 0;
    m_spkSphere = 0;
    m_spkPlaneEffect = 0;
    m_spkSphereEffect = 0;
    m_spkWireframe = 0;
    m_spkScreenCamera = 0;
    m_spkScreenPolygon = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ShadowMaps2::OnIdle ()
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

//#define SAVE_RESULT

    if (m_pkRenderer->BeginScene())
    {
        // Draw the scene from the light's perspective, writing the
        // depths from the light to the pixels to the ShadowFBO.
        m_spkPlane->AttachEffect(m_spkShadowEffect);
        m_spkSphere->AttachEffect(m_spkShadowEffect);
        m_pkShadowFBO->Enable();
        m_pkRenderer->ClearBuffers();
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        m_pkShadowFBO->Disable();
        m_spkPlane->DetachEffect(m_spkShadowEffect);
        m_spkSphere->DetachEffect(m_spkShadowEffect);
#ifdef SAVE_RESULT
        m_pkShadowFBO->CopyToTexture(true);
        Image* pkImage = m_pkShadowFBO->GetTarget()->GetImage();
        pkImage->Save("shadowmap.wmif");
#endif

        // Draw the scene from the camera's perspective, using
        // projected texturing of the shadow map and determining
        // which pixels are lit and which are shadowed.  The results
        // are written to the UnlitFBO.
        m_spkPlane->AttachEffect(m_spkUnlitEffect);
        m_spkSphere->AttachEffect(m_spkUnlitEffect);
        m_pkUnlitFBO->Enable();
        m_pkRenderer->ClearBuffers();
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        m_pkUnlitFBO->Disable();
        m_spkPlane->DetachEffect(m_spkUnlitEffect);
        m_spkSphere->DetachEffect(m_spkUnlitEffect);
#ifdef SAVE_RESULT
        m_pkUnlitFBO->CopyToTexture(true);
        pkImage = m_pkUnlitFBO->GetTarget()->GetImage();
        pkImage->Save("unlit.wmif");
#endif

        // Do screen space drawing.
        m_pkRenderer->SetCamera(m_spkScreenCamera);

        // Horizontally blur the texture of UnlitFBO, writing it to HBlurFBO.
        m_spkScreenPolygon->AttachEffect(m_spkHBlurEffect);
        m_pkHBlurFBO->Enable();
        m_pkRenderer->ClearBuffers();
        m_pkRenderer->Draw(m_spkScreenPolygon);
        m_pkHBlurFBO->Disable();
        m_spkScreenPolygon->DetachEffect(m_spkHBlurEffect);
#ifdef SAVE_RESULT
        m_pkHBlurFBO->CopyToTexture(true);
        pkImage = m_pkHBlurFBO->GetTarget()->GetImage();
        pkImage->Save("hblur.wmif");
#endif

        // Vertically blur the texture of HBlurFBO, writing it to UnlitFBO.
        m_spkScreenPolygon->AttachEffect(m_spkVBlurEffect);
        m_pkUnlitFBO->Enable();
        m_pkRenderer->ClearBuffers();
        m_pkRenderer->Draw(m_spkScreenPolygon);
        m_pkUnlitFBO->Disable();
        m_spkScreenPolygon->DetachEffect(m_spkVBlurEffect);
#ifdef SAVE_RESULT
        m_pkUnlitFBO->CopyToTexture(true);
        pkImage = m_pkUnlitFBO->GetTarget()->GetImage();
        pkImage->Save("vblur.wmif");
#endif

        // Draw the scene using some base textures, combining the shadow
        // information with the scene.
        m_pkRenderer->SetCamera(m_spkCamera);
        m_spkPlane->AttachEffect(m_spkPlaneEffect);
        m_spkSphere->AttachEffect(m_spkSphereEffect);
        m_pkRenderer->ClearBuffers();
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        m_spkPlane->DetachEffect(m_spkPlaneEffect);
        m_spkSphere->DetachEffect(m_spkSphereEffect);

        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ShadowMaps2::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    }

    return true;
}
//----------------------------------------------------------------------------
void ShadowMaps2::CreateScene ()
{
    CreateGeometricObjects();
    CreateShaderConstants();
    CreateImagesTexturesFBOs();
    CreateShaders();
}
//----------------------------------------------------------------------------
void ShadowMaps2::CreateGeometricObjects ()
{
    // Create a scene graph containing a sphere and a plane.
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    m_spkPlane = kSM.Rectangle(128,128,16.0f,16.0f);
    m_spkScene->AttachChild(m_spkPlane);
    m_spkSphere = kSM.Sphere(64,64,1.0f);
    m_spkSphere->Local.SetTranslate(Vector3f(0.0f,0.0f,1.0f));
    m_spkScene->AttachChild(m_spkSphere);

    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    m_spkScreenCamera = WM4_NEW Camera;
    m_spkScreenCamera->Perspective = false;
    m_spkScreenCamera->SetFrustum(0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    m_spkScreenCamera->SetFrame(Vector3f::ZERO,Vector3f::UNIT_Z,
        Vector3f::UNIT_Y,Vector3f::UNIT_X);

    // Create a screen polygon to use with an RGBA render target.
    Attributes kSPAttr;
    kSPAttr.SetPositionChannels(3);
    kSPAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kSPAttr,4);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,0.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,1.0f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,1.0f,0.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    m_spkScreenPolygon = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkScreenPolygon->UpdateGS();
    m_spkScreenPolygon->UpdateRS();
}
//----------------------------------------------------------------------------
void ShadowMaps2::CreateShaderConstants ()
{
    // Choose the coordinate system for the light.
    m_kLightWorldPosition = Vector3f(4.0f,4.0f,4.0f);
    Vector3f kLDVector(-1.0f,-1.0f,-1.0f);
    kLDVector.Normalize();
    Vector3f kLUVector(-1.0f,-1.0f,2.0f);
    kLUVector.Normalize();
    Vector3f kLRVector = kLDVector.Cross(kLUVector);

    // Set up the view matrix for the light projector.
    float fRdE = kLRVector.Dot(m_kLightWorldPosition);
    float fUdE = kLUVector.Dot(m_kLightWorldPosition);
    float fDdE = kLDVector.Dot(m_kLightWorldPosition);
    Matrix4f kProjVMatrix(
        kLRVector[0], kLUVector[0], kLDVector[0], 0.0f,
        kLRVector[1], kLUVector[1], kLDVector[1], 0.0f,
        kLRVector[2], kLUVector[2], kLDVector[2], 0.0f,
        -fRdE,        -fUdE,        -fDdE,        1.0f);

    // Set up the projection matrix for the light projector.
    float fRMin, fRMax, fUMin, fUMax, fDMin, fDMax;
    m_spkCamera->GetFrustum(fRMin,fRMax,fUMin,fUMax,fDMin,fDMax);
    float fInvRDiff = 1.0f/(fRMax - fRMin);
    float fInvUDiff = 1.0f/(fUMax - fUMin);
    float fInvDDiff = 1.0f/(fDMax - fDMin);
    float fRTerm0 = fDMin*fInvRDiff;
    float fUTerm0 = fDMin*fInvUDiff;
    float fDTerm0 = fDMin*fInvDDiff;
    float fRTerm1 = -(fRMin+fRMax)*fInvRDiff;
    float fUTerm1 = -(fUMin+fUMax)*fInvUDiff;
    float fDTerm1 = fDMax*fInvDDiff;
    Matrix4f kProjPMatrix(
        2.0f*fRTerm0, 0.0f,         0.0f,           0.0f,
        0.0f,         2.0f*fUTerm0, 0.0f,           0.0f,
        fRTerm1,      fUTerm1,      fDTerm1,        1.0f,
        0.0f,         0.0f,         -fDMax*fDTerm0, 0.0f);

    m_kLightVPMatrix = kProjVMatrix * kProjPMatrix;

    // The bias and scale matrix for computing projected texture
    // coordinates.
    Matrix4f kBSMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    // The matrix to compute projected texture coordinates.
    m_kLightVPBSMatrix = m_kLightVPMatrix * kBSMatrix;

    m_kLightColor.R() = 1.0f;
    m_kLightColor.G() = 1.0f;
    m_kLightColor.B() = 1.0f;
    m_kLightColor.A() = 1.0f;  // for specular modulation

    m_fDepthBias = 0.1f;
    m_iShadowSize = 512;
    m_fTexelSize = 1.0f/(float)m_iShadowSize;

    // Compute the weights.
    const float fStdDev = 1.0f;
    const float fInvTwoVariance = 1.0f/(2.0f*fStdDev*fStdDev);
    float fTotalWeight = 0.0f;
    int i, j;
    for (i = 0, j = -NUM_WEIGHTS/2; i < NUM_WEIGHTS; i++, j++)
    {
        float fWeight = Mathf::Exp(-j*j*fInvTwoVariance);
        m_akWeights[i] = Vector3f(fWeight, fWeight, fWeight);
        fTotalWeight += fWeight;
    }

    // The weights must sum to 1.
    float fInvTotalWeight = 1.0f/fTotalWeight;
    for (i = 0; i < NUM_WEIGHTS; i++)
    {
        m_akWeights[i] *= fInvTotalWeight;
    }

    // Compute the offsets.
    float fUDelta = 1.0f/(float)m_iWidth;
    float fVDelta = 1.0f/(float)m_iHeight;
    for (i = 0, j = -NUM_WEIGHTS/2; i < NUM_WEIGHTS; i++, j++)
    {
        m_akHOffsets[i] = Vector2f(j*fUDelta, 0.0f);
        m_akVOffsets[i] = Vector2f(0.0f, j*fVDelta);
    }
}
//----------------------------------------------------------------------------
void ShadowMaps2::CreateImagesTexturesFBOs ()
{
    // TODO:  Modify Wild Magic so that the Image may have a null pointer
    // (no data necessary, FBOs can be uninitialized).

    // Create a floating-point format for the shadow map.  For the purpose of
    // debugging, the shadow map is read from VRAM to this image and saved
    // to disk.
    m_spkShadowImage = WM4_NEW Image(Image::IT_RGB32F,m_iShadowSize,
        m_iShadowSize,0,"ShadowMap");

    m_spkShadowTarget = WM4_NEW Texture("ShadowMap",m_spkShadowImage);
    m_spkShadowTarget->SetOffscreenTexture(true);
    m_spkShadowTarget->SetFilterType(Texture::NEAREST);
    m_spkShadowTarget->SetWrapType(0,Texture::CLAMP_EDGE);
    m_spkShadowTarget->SetWrapType(1,Texture::CLAMP_EDGE);
    m_pkRenderer->LoadTexture(m_spkShadowTarget);
    m_pkShadowFBO = FrameBuffer::Create(m_eFormat,m_eDepth,m_eStencil,
        m_eBuffering,m_eMultisampling,m_pkRenderer,m_spkShadowTarget);
    assert(m_pkShadowFBO);

    // Create a floating-point image for the unlit scene.  For the purpose
    // of debugging, the unlit image is read from VRAM and saved to disk.
    m_spkUnlitImage = WM4_NEW Image(Image::IT_RGBA8888,m_iWidth,m_iHeight,0,
        "UnlitImage");

    m_spkUnlitTarget = WM4_NEW Texture("UnlitImage",m_spkUnlitImage);
    m_spkUnlitTarget->SetOffscreenTexture(true);
    m_spkUnlitTarget->SetFilterType(Texture::LINEAR);
    m_spkUnlitTarget->SetWrapType(0,Texture::CLAMP_EDGE);
    m_spkUnlitTarget->SetWrapType(1,Texture::CLAMP_EDGE);
    m_pkRenderer->LoadTexture(m_spkUnlitTarget);
    m_pkUnlitFBO = FrameBuffer::Create(m_eFormat,m_eDepth,m_eStencil,
        m_eBuffering,m_eMultisampling,m_pkRenderer,m_spkUnlitTarget);
    assert(m_pkUnlitFBO);

    // Create a floating-point image for the horizontally blurred scene.  For
    // the purpose of debugging, the blurred image is read from VRAM and saved
    // to disk.
    m_spkHBlurImage = WM4_NEW Image(Image::IT_RGBA8888,m_iWidth,m_iHeight,0,
        "HBlurImage");

    m_spkHBlurTarget = WM4_NEW Texture("HBlurImage",m_spkHBlurImage);
    m_spkHBlurTarget->SetOffscreenTexture(true);
    m_spkHBlurTarget->SetFilterType(Texture::LINEAR);
    m_spkHBlurTarget->SetWrapType(0,Texture::CLAMP_EDGE);
    m_spkHBlurTarget->SetWrapType(1,Texture::CLAMP_EDGE);
    m_pkRenderer->LoadTexture(m_spkHBlurTarget);
    m_pkHBlurFBO = FrameBuffer::Create(m_eFormat,m_eDepth,m_eStencil,
        m_eBuffering,m_eMultisampling,m_pkRenderer,m_spkHBlurTarget);
    assert(m_pkHBlurFBO);

    // The UnlitFBO is used to vertically blur the image that was horizontally
    // blurred.  The source image is in HBlurFBO and the target is UnlitFBO.
}
//----------------------------------------------------------------------------
void ShadowMaps2::CreateShaders ()
{
    // Create the shader to compute the depth relative to the light projector.
    // The pixel program does not require textures.
    VertexShader* pkVShader = WM4_NEW VertexShader("Shadow");
    PixelShader* pkPShader = WM4_NEW PixelShader("Shadow");
    m_spkShadowEffect = WM4_NEW ShaderEffect(1);
    m_spkShadowEffect->SetVShader(0,pkVShader);
    m_spkShadowEffect->SetPShader(0,pkPShader);
    Program* pkProgram = m_spkShadowEffect->GetVProgram(0);
    pkProgram->GetUserConstant("LightVPMatrix")->SetDataSource(
        (float*)m_kLightVPMatrix);

    // Create the shader to draw the shadowed parts of the scene.  This uses
    // a shadow map drawn to the shadow target.
    pkVShader = WM4_NEW VertexShader("Unlit");
    pkPShader = WM4_NEW PixelShader("Unlit");
    pkPShader->SetTexture(0,m_spkShadowTarget);
    m_spkUnlitEffect = WM4_NEW ShaderEffect(1);
    m_spkUnlitEffect->SetVShader(0,pkVShader);
    m_spkUnlitEffect->SetPShader(0,pkPShader);
    m_pkRenderer->LoadResources(m_spkUnlitEffect);
    pkProgram = m_spkUnlitEffect->GetVProgram(0);
    pkProgram->GetUserConstant("LightVPMatrix")->SetDataSource(
        (float*)m_kLightVPMatrix);
    pkProgram->GetUserConstant("LightVPBSMatrix")->SetDataSource(
        (float*)m_kLightVPBSMatrix);
    pkProgram = m_spkUnlitEffect->GetPProgram(0);
    pkProgram->GetUserConstant("DepthBias")->SetDataSource(&m_fDepthBias);
    pkProgram->GetUserConstant("TexelSize")->SetDataSource(&m_fTexelSize);

    // Create the shader to perform a horizontal Gaussian blur on the
    // "UnlitImage" and stores the result in BlurFBO.
    pkVShader = WM4_NEW VertexShader("Blur");
    pkPShader = WM4_NEW PixelShader("HorizontalBlur");
    pkPShader->SetTexture(0,m_spkUnlitTarget);
    m_spkHBlurEffect = WM4_NEW ShaderEffect(1);
    m_spkHBlurEffect->SetVShader(0,pkVShader);
    m_spkHBlurEffect->SetPShader(0,pkPShader);
    m_pkRenderer->LoadResources(m_spkHBlurEffect);
    pkProgram = m_spkHBlurEffect->GetPProgram(0);
    int i;
    for (i = 0; i < NUM_WEIGHTS; i++)
    {
        char acName[16];

        System::Sprintf(acName,16,"Weights[%d]",i);
        pkProgram->GetUserConstant(acName)->SetDataSource(
            (float*)&m_akWeights[i]);

        System::Sprintf(acName,16,"Offsets[%d]",i);
        pkProgram->GetUserConstant(acName)->SetDataSource(
            (float*)&m_akHOffsets[i]);
    }

    // Create the shader to perform a vertical Gaussian blur on the
    // "BlurImage" and stores the result in UnlitFBO.
    pkVShader = WM4_NEW VertexShader("Blur");
    pkPShader = WM4_NEW PixelShader("VerticalBlur");
    pkPShader->SetTexture(0,m_spkHBlurTarget);
    m_spkVBlurEffect = WM4_NEW ShaderEffect(1);
    m_spkVBlurEffect->SetVShader(0,pkVShader);
    m_spkVBlurEffect->SetPShader(0,pkPShader);
    m_pkRenderer->LoadResources(m_spkVBlurEffect);
    pkProgram = m_spkVBlurEffect->GetPProgram(0);
    for (i = 0; i < NUM_WEIGHTS; i++)
    {
        char acName[16];

        System::Sprintf(acName,16,"Weights[%d]",i);
        pkProgram->GetUserConstant(acName)->SetDataSource(
            (float*)&m_akWeights[i]);

        System::Sprintf(acName,16,"Offsets[%d]",i);
        pkProgram->GetUserConstant(acName)->SetDataSource(
            (float*)&m_akVOffsets[i]);
    }

    // Create the final-pass shaders.

    // For the plane.
    pkVShader = WM4_NEW VertexShader("Scene");
    pkPShader = WM4_NEW PixelShader("Scene");
    pkPShader->SetTexture(0,"Stone");            // base texture
    pkPShader->SetTexture(1,m_spkUnlitTarget);   // blurred shadow texture
    pkPShader->SetTexture(2,"Magician");         // projected spot texture
    m_spkPlaneEffect = WM4_NEW ShaderEffect(1);
    m_spkPlaneEffect->SetVShader(0,pkVShader);
    m_spkPlaneEffect->SetPShader(0,pkPShader);
    m_pkRenderer->LoadResources(m_spkPlaneEffect);
    pkProgram = m_spkPlaneEffect->GetVProgram(0);
    pkProgram->GetUserConstant("LightVPMatrix")->SetDataSource(
        (float*)m_kLightVPMatrix);
    pkProgram->GetUserConstant("LightVPBSMatrix")->SetDataSource(
        (float*)m_kLightVPBSMatrix);
    pkProgram->GetUserConstant("LightWorldPosition")->SetDataSource(
        (float*)m_kLightWorldPosition);
    pkProgram = m_spkPlaneEffect->GetPProgram(0);
    pkProgram->GetUserConstant("LightColor")->SetDataSource(
        (float*)m_kLightColor);

    // For the sphere.
    pkVShader = WM4_NEW VertexShader("Scene");
    pkPShader = WM4_NEW PixelShader("Scene");
    pkPShader->SetTexture(0,"BallTexture");      // base texture
    pkPShader->SetTexture(1,m_spkUnlitTarget);   // blurred shadow texture
    pkPShader->SetTexture(2,"Magician");         // projected spot texture
    m_spkSphereEffect = WM4_NEW ShaderEffect(1);
    m_spkSphereEffect->SetVShader(0,pkVShader);
    m_spkSphereEffect->SetPShader(0,pkPShader);
    m_pkRenderer->LoadResources(m_spkSphereEffect);
    pkProgram = m_spkSphereEffect->GetVProgram(0);
    pkProgram->GetUserConstant("LightVPMatrix")->SetDataSource(
        (float*)m_kLightVPMatrix);
    pkProgram->GetUserConstant("LightVPBSMatrix")->SetDataSource(
        (float*)m_kLightVPBSMatrix);
    pkProgram->GetUserConstant("LightWorldPosition")->SetDataSource(
        (float*)m_kLightWorldPosition);
    pkProgram = m_spkSphereEffect->GetPProgram(0);
    pkProgram->GetUserConstant("LightColor")->SetDataSource(
        (float*)m_kLightColor);
}
//----------------------------------------------------------------------------
