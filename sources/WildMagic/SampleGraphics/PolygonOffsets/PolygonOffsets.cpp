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

#include "PolygonOffsets.h"

WM4_WINDOW_APPLICATION(PolygonOffsets);

//----------------------------------------------------------------------------
PolygonOffsets::PolygonOffsets ()
    :
    WindowApplication3("PolygonOffsets",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool PolygonOffsets::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,0.0f,-16.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
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
void PolygonOffsets::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void PolygonOffsets::OnIdle ()
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
        m_pkRenderer->Draw(8,16,ColorRGBA::WHITE,
            "Press UP/DOWN arrows. "
            "The left box flickers, the right does not");
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool PolygonOffsets::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"PolygonOffsets.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void PolygonOffsets::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);

    // Vertices to be shared by rectangles 1 and 3.
    VertexBuffer* pkVBuffer0 = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer0->Position3(0) = Vector3f(-1.0f,0.0f,-1.0f);
    pkVBuffer0->Position3(1) = Vector3f(-1.0f,0.0f,+1.0f);
    pkVBuffer0->Position3(2) = Vector3f(+1.0f,0.0f,+1.0f);
    pkVBuffer0->Position3(3) = Vector3f(+1.0f,0.0f,-1.0f);
    pkVBuffer0->Color3(0,0) = ColorRGB(1.0f,0.0f,0.0f);
    pkVBuffer0->Color3(0,1) = ColorRGB(1.0f,0.0f,0.0f);
    pkVBuffer0->Color3(0,2) = ColorRGB(1.0f,0.0f,0.0f);
    pkVBuffer0->Color3(0,3) = ColorRGB(1.0f,0.0f,0.0f);

    // Vertices to be shared by rectangles 2 and 4.
    VertexBuffer* pkVBuffer1 = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer1->Position3(0) = Vector3f(-1.0f,0.0f,-1.0f);
    pkVBuffer1->Position3(1) = Vector3f(-1.0f,0.0f,+1.0f);
    pkVBuffer1->Position3(2) = Vector3f(+1.0f,0.0f,+1.0f);
    pkVBuffer1->Position3(3) = Vector3f(+1.0f,0.0f,-1.0f);
    pkVBuffer1->Color3(0,0) = ColorRGB(0.0f,1.0f,0.0f);
    pkVBuffer1->Color3(0,1) = ColorRGB(0.0f,1.0f,0.0f);
    pkVBuffer1->Color3(0,2) = ColorRGB(0.0f,1.0f,0.0f);
    pkVBuffer1->Color3(0,3) = ColorRGB(0.0f,1.0f,0.0f);

    // Indices to be shared by all rectangles.
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1;  aiIndex[5] = 2;

    VertexColor3Effect* pkEffect = WM4_NEW VertexColor3Effect;

    // rectangle 1
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer0,pkIBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetTranslate(Vector3f(+2.0f,-4.0f,0.0f));
    m_spkScene->AttachChild(pkMesh);

    // rectangle 2
    pkMesh = WM4_NEW TriMesh(pkVBuffer1,pkIBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetTranslate(Vector3f(+2.0f,-4.0f,0.0f));
    pkMesh->Local.SetUniformScale(0.5f);
    m_spkScene->AttachChild(pkMesh);

    // rectangle 3
    pkMesh = WM4_NEW TriMesh(pkVBuffer0,pkIBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetTranslate(Vector3f(-2.0f,-4.0f,0.0f));
    m_spkScene->AttachChild(pkMesh);

    // rectangle 4
    pkMesh = WM4_NEW TriMesh(pkVBuffer1,pkIBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetTranslate(Vector3f(-2.0f,-4.0f,0.0f));
    pkMesh->Local.SetUniformScale(0.5f);
    m_spkScene->AttachChild(pkMesh);

    // Set up the polygon offset.  Attach it to the last TriMesh created.
    PolygonOffsetState* pkPolygonOffsetState = WM4_NEW PolygonOffsetState;
    pkPolygonOffsetState->FillEnabled = true;
    pkPolygonOffsetState->Scale = -1.0f;
    pkPolygonOffsetState->Bias = -2.0f;
    pkMesh->AttachGlobalState(pkPolygonOffsetState);
}
//----------------------------------------------------------------------------
