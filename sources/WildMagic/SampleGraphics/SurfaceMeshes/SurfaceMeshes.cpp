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

#include "SurfaceMeshes.h"

WM4_WINDOW_APPLICATION(SurfaceMeshes);

//----------------------------------------------------------------------------
SurfaceMeshes::SurfaceMeshes ()
    :
    WindowApplication3("SurfaceMeshes",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool SurfaceMeshes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,12.0f);
    Vector3f kCDir(0.0f,0.0f,-1.0f);
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

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void SurfaceMeshes::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkSegment = 0;
    m_spkCurve = 0;
    m_spkPatch = 0;
    m_spkSurface = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SurfaceMeshes::OnIdle ()
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
bool SurfaceMeshes::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        if (m_iLevel < 5)
        {
            m_iLevel++;
            m_spkCurve->SetLevel(m_iLevel);
            m_spkSurface->SetLevel(m_iLevel);
            m_spkSurface->UpdateGS();
            m_kCuller.ComputeVisibleSet(m_spkScene);
        }
        return true;
    case '-':
    case '_':
        if (m_iLevel > 0)
        {
            m_iLevel--;
            m_spkCurve->SetLevel(m_iLevel);
            m_spkSurface->SetLevel(m_iLevel);
            m_spkSurface->UpdateGS();
            m_kCuller.ComputeVisibleSet(m_spkScene);
        }
        return true;
    case 'l':
    case 'L':
        m_spkCurve->Lock();
        m_spkSurface->Lock();
        return true;
    case 'a':
        m_spkPatch->Amplitude -= 0.1f;
        m_spkSurface->OnDynamicChange();
        m_spkSurface->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case 'A':
        m_spkPatch->Amplitude += 0.1f;
        m_spkSurface->OnDynamicChange();
        m_spkSurface->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"SurfaceMeshes.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SurfaceMeshes::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    CullState* pkCS = WM4_NEW CullState;
    pkCS->Enabled = false;
    m_spkScene->AttachGlobalState(pkCS);

    CreateSimpleSegment();
    CreateSimplePatch();

    // start with level zero on both segment and patch
    m_iLevel = 0;
}
//----------------------------------------------------------------------------
void SurfaceMeshes::CreateSimpleSegment ()
{
    m_spkSegment = WM4_NEW SimpleSegment;

    // parameters for the initial vertex construction
    const int iVQuantity = 4;
    float afVParam[iVQuantity];
    afVParam[0] = 0.0f;
    afVParam[1] = 1.0f/3.0f;
    afVParam[2] = 2.0f/3.0f;
    afVParam[3] = 1.0f;

    // initial vertex construction
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i) = m_spkSegment->P(afVParam[i]);
    }
    pkVBuffer->Color3(0,0) = ColorRGB(0.0f,0.0f,0.0f);
    pkVBuffer->Color3(0,1) = ColorRGB(1.0f,0.0f,0.0f);
    pkVBuffer->Color3(0,2) = ColorRGB(0.0f,1.0f,0.0f);
    pkVBuffer->Color3(0,3) = ColorRGB(0.0f,0.0f,1.0f);

    // Each edge has its own parameters associated with its vertices.
    // A vertex can have different parameters for the edges sharing it.
    float* afParam = WM4_NEW float[2*(iVQuantity-1)];
    afParam[0] = afVParam[0];
    afParam[1] = afVParam[1];
    afParam[2] = afVParam[1];
    afParam[3] = afVParam[2];
    afParam[4] = afVParam[2];
    afParam[5] = afVParam[3];
    FloatArray* pkParams = WM4_NEW FloatArray(2*(iVQuantity-1),afParam);

    // The simple segment is used for all curve segments.
    CurveSegmentPtr* aspkSegment = WM4_NEW CurveSegmentPtr[iVQuantity-1];
    for (i = 0; i < iVQuantity-1; i++)
    {
        aspkSegment[i] = m_spkSegment;
    }

    m_spkCurve = WM4_NEW CurveMesh(pkVBuffer,aspkSegment,pkParams,true);
    m_spkCurve->AttachEffect(WM4_NEW VertexColor3Effect);
    m_spkScene->AttachChild(m_spkCurve);
}
//----------------------------------------------------------------------------
void SurfaceMeshes::CreateSimplePatch ()
{
    m_spkPatch = WM4_NEW SimplePatch;

    // parameters for the initial vertex construction
    const int iVQuantity = 5;
    Vector2f akVParam[iVQuantity];
    akVParam[0] = Vector2f(-1.0f,-1.0f);
    akVParam[1] = Vector2f(1.0f,-1.0f);
    akVParam[2] = Vector2f(1.0f,1.0f);
    akVParam[3] = Vector2f(-1.0f,1.0f);
    akVParam[4] = Vector2f(2.0f,0.0f);

    // initial vertex construction
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i) = m_spkPatch->P(akVParam[i].X(),
            akVParam[i].Y());
    }
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(0.5f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.5f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,4) = Vector2f(1.0f,0.5f);

    // initial index construction
    int iTQuantity = 3, iIQuantity = 3*iTQuantity;
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    aiIndex[6] = 1;  aiIndex[7] = 4;  aiIndex[8] = 2;

    // Each triangle has its own parameters associated with its vertices.
    // A vertex can have different parameters for the triangles sharing it.
    Vector2f* akParam = WM4_NEW Vector2f[iIQuantity];
    for (i = 0; i < iIQuantity; i++)
    {
        akParam[i] = akVParam[aiIndex[i]];
    }
    Vector2fArray* pkParams = WM4_NEW Vector2fArray(iIQuantity,akParam);

    // The simple patch is used for all triangles.
    SurfacePatchPtr* aspkPatch = WM4_NEW SurfacePatchPtr[iTQuantity];
    for (i = 0; i < iTQuantity; i++)
    {
        aspkPatch[i] = m_spkPatch;
    }

    m_spkSurface = WM4_NEW SurfaceMesh(pkVBuffer,pkIBuffer,pkParams,
        aspkPatch,true);
    m_spkSurface->AttachEffect(WM4_NEW TextureEffect("Magician"));
    m_spkScene->AttachChild(m_spkSurface);

    // start with level 0 (original triangle mesh)
    m_iLevel = 0;
}
//----------------------------------------------------------------------------
