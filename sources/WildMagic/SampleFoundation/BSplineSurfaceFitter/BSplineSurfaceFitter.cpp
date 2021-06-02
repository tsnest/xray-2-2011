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
// Version: 4.7.0 (2008/09/13)

#include "BSplineSurfaceFitter.h"

WM4_WINDOW_APPLICATION(BSplineSurfaceFitter);

//----------------------------------------------------------------------------
BSplineSurfaceFitter::BSplineSurfaceFitter ()
    :
    WindowApplication3("BSplineSurfaceFitter",0,0,640,480,
        ColorRGBA(0.0f,0.5f,0.75f,1.0f))
{
    System::InsertDirectory("Data");
}
//----------------------------------------------------------------------------
bool BSplineSurfaceFitter::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,m_iWidth/(float)m_iHeight,0.01f,100.0f);
    Vector3f kCLoc(0.0f,-9.0f,1.5f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    kCDir.Normalize();
    kCUp.Normalize();
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.005f,0.002f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void BSplineSurfaceFitter::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkHeightField = 0;
    m_spkFittedField = 0;
    m_spkHeightImage = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineSurfaceFitter::OnIdle ()
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
bool BSplineSurfaceFitter::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"BSplineSurfaceFitter.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void BSplineSurfaceFitter::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Begin with a flat 64x64 height field.
    const int iNumSamples = 64;
    const float fExtent = 8.0f;
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    m_spkHeightField = kSM.Rectangle(iNumSamples,iNumSamples,fExtent,fExtent);
    m_spkScene->AttachChild(m_spkHeightField);

    // Set the heights based on a precomputed height field.  Also create a
    // texture image to go with the height field.
    m_spkHeightImage = Image::Load("HeightField");
    unsigned char* pucData = m_spkHeightImage->GetData();
    VertexBuffer* pkVB = m_spkHeightField->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();

    Vector3f** aakSamplePoints;
    Allocate(iNumSamples,iNumSamples,aakSamplePoints);

    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        unsigned char ucValue = *pucData;
        float fHeight = 3.0f*((float)ucValue)/255.0f +
            0.05f*Mathf::SymmetricRandom();

        *pucData++ = (unsigned char)Mathf::IntervalRandom(32.0f,64.0f);
        *pucData++ = 3*(128 - ucValue/2)/4;
        *pucData++ = 0;

        pkVB->Position3(i).Z() = fHeight;
        aakSamplePoints[i % iNumSamples][i / iNumSamples] =
            pkVB->Position3(i);
    }

    m_spkHeightField->AttachEffect(WM4_NEW TextureEffect("HeightField"));

    // Compute a B-Spline surface with NxN control points, where N < 64.
    // This surface will be sampled to 64x64 and displayed together with the
    // original height field for comparison.
    const int iNumCtrlPoints = 32;
    const int iDegree = 3;
    BSplineSurfaceFitf kFitter(iDegree,iNumCtrlPoints,iNumSamples,iDegree,
        iNumCtrlPoints,iNumSamples,aakSamplePoints);

    Attributes kFAttr;
    kFAttr.SetPositionChannels(3);
    kFAttr.SetColorChannels(0,4);
    StandardMesh kFSM(kFAttr);
    m_spkFittedField = kFSM.Rectangle(iNumSamples,iNumSamples,fExtent,fExtent);
    m_spkScene->AttachChild(m_spkFittedField);

    pkVB = m_spkFittedField->VBuffer;
    for (i = 0; i < iVQuantity; i++)
    {
        float fU = 0.5f*(pkVB->Position3(i).X()/fExtent + 1.0f);
        float fV = 0.5f*(pkVB->Position3(i).Y()/fExtent + 1.0f);
        pkVB->Position3(i) = kFitter.GetPosition(fU,fV);
        pkVB->Color4(0,i) = ColorRGBA(1.0f,1.0f,1.0f,0.5f);
    }

    m_spkFittedField->AttachEffect(WM4_NEW VertexColor4Effect);

    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    m_spkFittedField->AttachGlobalState(pkAS);
}
//----------------------------------------------------------------------------
