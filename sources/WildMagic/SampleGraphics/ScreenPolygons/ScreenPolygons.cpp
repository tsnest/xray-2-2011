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

#include "ScreenPolygons.h"

WM4_WINDOW_APPLICATION(ScreenPolygons);

//----------------------------------------------------------------------------
ScreenPolygons::ScreenPolygons ()
    :
    WindowApplication3("ScreenPolygons",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
    m_dAnimTime = 0.0;
    m_dAnimTimeDelta = 0.01;
}
//----------------------------------------------------------------------------
bool ScreenPolygons::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // camera and view frustum
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(80.0f,0.0f,23.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkBackPoly->UpdateGS();
    m_spkBackPoly->UpdateRS();
    m_spkForePoly->UpdateGS();
    m_spkForePoly->UpdateRS();
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
void ScreenPolygons::OnTerminate ()
{
    m_spkScene = 0;
    m_spkScreenCamera = 0;
    m_spkBackPoly = 0;
    m_spkMidPoly = 0;
    m_spkForePoly = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ScreenPolygons::OnIdle ()
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

    m_pkRenderer->ClearZBuffer();
    if (m_pkRenderer->BeginScene())
    {
        // Draw the background and middle polygons.
        m_pkRenderer->SetCamera(m_spkScreenCamera);
        m_pkRenderer->Draw(m_spkBackPoly);
        m_pkRenderer->Draw(m_spkMidPoly);

        m_pkRenderer->SetCamera(m_spkCamera);
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());

        // Draw the foreground polygon last since it has transparency.
        m_pkRenderer->SetCamera(m_spkScreenCamera);
        m_pkRenderer->Draw(m_spkForePoly);

        // text goes on top of foreground polygon
        m_pkRenderer->SetCamera(m_spkCamera);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);

        Vector3f kTrn = m_spkMidPoly->World.GetTranslate();
        const size_t uiSize = 128;
        char acMsg[uiSize];
        System::Sprintf(acMsg,uiSize,"linear z = %f , perspective z = %f",
            m_fLinearZ,m_fDepthZ);
        m_pkRenderer->Draw(8,16,ColorRGBA::WHITE,acMsg);

        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ScreenPolygons::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    const float fDelta = 0.00001f;
    float fDMin, fDMax, fFrustumZ;
    Vector3f kTrn;

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 'g':
        m_dAnimTime += m_dAnimTimeDelta;
        m_spkScene->UpdateGS(m_dAnimTime);
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case 'G':
        m_dAnimTime = 0.0;
        m_spkScene->UpdateGS(m_dAnimTime);
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;

    case '+':
    case '=':
        m_fDepthZ += fDelta;
        if (m_fDepthZ > 1.0f)
        {
            m_fDepthZ = 1.0f;
        }
        fDMin = m_spkCamera->GetDMin();
        fDMax = m_spkCamera->GetDMax();
        fFrustumZ = fDMin*fDMax/(fDMax - (fDMax - fDMin)*m_fDepthZ);
        m_fLinearZ = (fFrustumZ - fDMin)/(fDMax - fDMin);
        m_spkMidPoly->Local.SetTranslate(Vector3f(0.0f,0.0f,m_fLinearZ));
        m_spkMidPoly->UpdateGS();
        return true;

    case '-':
    case '_':
        m_fDepthZ -= fDelta;
        if (m_fDepthZ < 0.0f)
        {
            m_fDepthZ = 0.0f;
        }
        fDMin = m_spkCamera->GetDMin();
        fDMax = m_spkCamera->GetDMax();
        fFrustumZ = fDMin*fDMax/(fDMax - (fDMax - fDMin)*m_fDepthZ);
        m_fLinearZ = (fFrustumZ - fDMin)/(fDMax - fDMin);
        m_spkMidPoly->Local.SetTranslate(Vector3f(0.0f,0.0f,m_fLinearZ));
        m_spkMidPoly->UpdateGS();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ScreenPolygons::CreateScene ()
{
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    m_spkScreenCamera = WM4_NEW Camera;
    m_spkScreenCamera->Perspective = false;
    m_spkScreenCamera->SetFrustum(0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    m_spkScreenCamera->SetFrame(Vector3f::ZERO,Vector3f::UNIT_Z,
        Vector3f::UNIT_Y,Vector3f::UNIT_X);

    // Load the biped just for some model to display.
    const char* acFilename =
        System::GetPath("SkinnedBipedPN.wmof",System::SM_READ);
    Stream kIStream;
    kIStream.Load(acFilename);
    m_spkScene = DynamicCast<Node>(kIStream.GetObjectAt(0));
    assert(m_spkScene);

    // The biped has only material colors.  We could use a MaterialEffect,
    // but let's add lighting.
    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    pkLight->Diffuse = pkLight->Ambient;
    pkLight->Specular = ColorRGB::BLACK;
    pkLight->DVector = Vector3f(-1.0f,0.0f,0.0f); // the camera direction
    m_spkScene->AttachLight(pkLight);

    // The background is a textured screen polygon (z = 1).
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,0.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,1.0f,1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    m_spkBackPoly = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkBackPoly->AttachEffect(WM4_NEW TextureEffect("RedSky"));
    m_spkBackPoly->UpdateGS();

    // The middle polygon, which may be translated via '+' or '-'.
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.3f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,0.3f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,0.7f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,0.7f,0.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.3f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.3f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,0.7f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,0.7f);
    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    m_spkMidPoly = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkMidPoly->AttachEffect(WM4_NEW TextureEffect("BallTexture"));
    m_fLinearZ = 1.0f;
    m_fDepthZ = 1.0f;
    m_spkMidPoly->Local.SetTranslate(Vector3f(0.0f,0.0f,m_fLinearZ));
    m_spkMidPoly->UpdateGS();

    // A portion of the foreground is a textured screen polygon (z = 0).
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,5);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(0.5f,0.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(0.75f,0.5f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(0.5f,0.75f,0.0f);
    pkVBuffer->Position3(4) = Vector3f(0.0f,0.5f,0.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(0.67f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,0.67f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.67f,1.0f);
    pkVBuffer->TCoord2(0,4) = Vector2f(0.0f,0.67f);
    pkIBuffer = WM4_NEW IndexBuffer(9);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    aiIndex[6] = 0;  aiIndex[7] = 3;  aiIndex[8] = 4;
    m_spkForePoly = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkForePoly->AttachEffect(WM4_NEW TextureEffect("Flower"));
    m_spkForePoly->UpdateGS();

    // Make the foreground semitransparent.
    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    m_spkForePoly->AttachGlobalState(pkAS);

    // Add wireframe to see where the screen polygons are positioned.
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkBackPoly->AttachGlobalState(m_spkWireframe);
    m_spkForePoly->AttachGlobalState(m_spkWireframe);
}
//----------------------------------------------------------------------------
