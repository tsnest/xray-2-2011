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
// Version: 4.0.6 (2007/08/11)

#include "RenderToTexture.h"

WM4_WINDOW_APPLICATION(RenderToTexture);

//----------------------------------------------------------------------------
RenderToTexture::RenderToTexture ()
    :
    WindowApplication3("RenderToTexture",0,0,640,480,
        ColorRGBA(0.5f,0.5f,0.5f,1.0f))
{
    m_pkPBuffer = 0;
}
//----------------------------------------------------------------------------
bool RenderToTexture::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -300.0f*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

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
void RenderToTexture::OnTerminate ()
{
    WM4_DELETE m_pkPBuffer;

    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkScreenCamera = 0;
    m_spkScenePolygon = 0;
    m_spkSceneImage = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void RenderToTexture::OnIdle ()
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

    // Draw the scene to the back buffer/
    if (m_pkRenderer->BeginScene())
    {
        // Draw the scene to a color buffer.
        m_pkPBuffer->Enable();
        m_pkRenderer->SetClearColor(ColorRGBA::WHITE);
        m_pkRenderer->ClearBuffers();
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        m_pkPBuffer->Disable();

#if 0
        // TO DO. Dx9FrameBuffer::CopyToTexture currently works only for the
        // format Image::IT_RGBA8888.  It must be modified to read the render
        // target in the format of the render target.
        m_pkPBuffer->CopyToTexture(true);
        Image* pkImage = m_pkPBuffer->GetTarget()->GetImage();
        pkImage->Save("scene.wmif");
#endif

        // Draw the scene to the main window and also to a regular screen
        // polygon, placed in the lower-left corner of the main window.
        m_pkRenderer->SetClearColor(ColorRGBA(0.5f,0.5f,0.5f,1.0f));
        m_pkRenderer->ClearBuffers();
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        m_pkRenderer->SetCamera(m_spkScreenCamera);
        m_pkRenderer->Draw(m_spkScenePolygon);

        m_pkRenderer->SetCamera(m_spkCamera);
        DrawFrameRate(8,16,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool RenderToTexture::OnKeyDown (unsigned char ucKey, int iX, int iY)
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

    return false;
}
//----------------------------------------------------------------------------
void RenderToTexture::CreateScene ()
{
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    m_spkScreenCamera = WM4_NEW Camera;
    m_spkScreenCamera->Perspective = false;
    m_spkScreenCamera->SetFrustum(0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    m_spkScreenCamera->SetFrame(Vector3f::ZERO,Vector3f::UNIT_Z,
        Vector3f::UNIT_Y,Vector3f::UNIT_X);

    // Create a scene graph with the face model as the leaf node.
    m_spkScene = WM4_NEW Node;
    m_spkScene->AttachChild(CreateModel());
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Create a screen polygon to use as the RGBA render target.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(0.2f,0.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(0.2f,0.2f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,0.2f,0.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    m_spkScenePolygon = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    // Create a red image for the purposes of debugging.  The image itself
    // should not show up because the frame-buffer object writes to the
    // texture memory.  But if this fails, the red image should appear.
    int iWidth = 256, iHeight = 256;

    Image::FormatMode eFormat = Image::IT_RGBA8888;
    //Image::FormatMode eFormat = Image::IT_RGB888;
    //Image::FormatMode eFormat = Image::IT_RGBA32F;
    //Image::FormatMode eFormat = Image::IT_RGB32F;
    //Image::FormatMode eFormat = Image::IT_RGBA16F;

    // NOTE: Does not work on NVIDIA GeForce 7600GT with OpenGL.  The
    // rendering occurs, but the output image is half white and half black.
    // It does work with DX9.
    //Image::FormatMode eFormat = Image::IT_RGB16F;

    //Image::FormatMode eFormat = Image::IT_RGB16I;
    //Image::FormatMode eFormat = Image::IT_RGBA16I;

    m_spkSceneImage = WM4_NEW Image(eFormat,iWidth,iHeight,0,"SceneImage");

    // Create the texture effect for the scene polygon.  The resources are
    // loaded so that the scene target texture has everything needed for
    // FrameBuffer::Create to succeed.
    TextureEffect* pkEffect = WM4_NEW TextureEffect("SceneImage");
    m_pkSceneTarget = pkEffect->GetPTexture(0,0);
    m_pkSceneTarget->SetOffscreenTexture(true);
    m_spkScenePolygon->AttachEffect(pkEffect);
    m_spkScenePolygon->UpdateGS();
    m_spkScenePolygon->UpdateRS();
    m_pkRenderer->LoadResources(m_spkScenePolygon);

    // Create the RGBA frame-buffer object to be bound to the scene polygon.
    m_pkPBuffer = FrameBuffer::Create(m_eFormat,m_eDepth,m_eStencil,
        m_eBuffering,m_eMultisampling,m_pkRenderer,m_pkSceneTarget);
    assert(m_pkPBuffer);

    m_spkScene->UpdateGS();
    m_spkScene->GetChild(0)->Local.SetTranslate(
        -m_spkScene->WorldBound->GetCenter());
}
//----------------------------------------------------------------------------
Node* RenderToTexture::CreateModel ()
{
    const char* acPath = System::GetPath("FacePN.wmof",System::SM_READ);
    Stream kIStream;
    bool bLoaded = kIStream.Load(acPath);
    assert(bLoaded);
    (void)bLoaded;  // avoid warning in Release build
    TriMesh* pkMesh = StaticCast<TriMesh>(kIStream.GetObjectAt(0));

    // Create texture coordinates for the face.  Based on knowledge of the
    // mesh, the (x,z) values of the model-space vertices may be mapped to
    // (s,t) in [0,1]^2.
    VertexBuffer* pkVBuffer = pkMesh->VBuffer;
    int iVQuantity = pkVBuffer->GetVertexQuantity();

    float fXMin = pkVBuffer->Position3(0).X(), fXMax = fXMin;
    float fZMin = pkVBuffer->Position3(0).Z(), fZMax = fZMin;
    int i;
    for (i = 1; i < iVQuantity; i++)
    {
        float fX = pkVBuffer->Position3(i).X();
        if (fX < fXMin)
        {
            fXMin = fX;
        }
        else if (fX > fXMax)
        {
            fXMax = fX;
        }

        float fZ = pkVBuffer->Position3(i).Z();
        if (fZ < fZMin)
        {
            fZMin = fZ;
        }
        else if (fZ > fZMax)
        {
            fZMax = fZ;
        }
    }
    float fInvXRange = 1.0f/(fXMax - fXMin);
    float fInvZRange = 1.0f/(fZMax - fZMin);

    // Strip out the normal vectors since there is no lighting here.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    kAttr.SetTCoordChannels(1,2);
    VertexBuffer* pkNewVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        Vector3f kPosition = pkVBuffer->Position3(i);
        Vector2f kTCoord((kPosition.X() - fXMin)*fInvXRange,
            (kPosition.Z() - fZMin)*fInvZRange);

        pkNewVBuffer->Position3(i) = kPosition;
        pkNewVBuffer->TCoord2(0,i) = kTCoord;
        pkNewVBuffer->TCoord2(1,i) = kTCoord;
    }
    pkMesh->VBuffer = pkNewVBuffer;

    // Use a soft-additive multitexture.
    MultitextureEffect* pkEffect = WM4_NEW MultitextureEffect(2);
    pkEffect->SetTextureName(0,"Leaf");
    pkEffect->SetTextureName(1,"Water");
    AlphaState* pkAState = pkEffect->GetBlending(1);
    pkAState->SrcBlend = AlphaState::SBF_ONE_MINUS_DST_COLOR;
    pkAState->DstBlend = AlphaState::DBF_ONE;
    pkEffect->Configure();
    pkMesh->AttachEffect(pkEffect);

    Node* pkTrnNode = WM4_NEW Node;
    pkTrnNode->AttachChild(pkMesh);
    return pkTrnNode;
}
//----------------------------------------------------------------------------
