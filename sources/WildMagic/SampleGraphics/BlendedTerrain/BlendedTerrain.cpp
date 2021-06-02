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
// Version: 4.0.3 (2007/09/24)

#include "BlendedTerrain.h"

WM4_WINDOW_APPLICATION(BlendedTerrain);

//#define SCREEN_CAPTURE

//----------------------------------------------------------------------------
BlendedTerrain::BlendedTerrain ()
    :
    WindowApplication3("BlendedTerrain",0,0,640,480,
        ColorRGBA(0.0f,0.5f,0.75f,1.0f))
{
    System::InsertDirectory("Data");

    m_fZAngle = 0.0f;
#ifdef SCREEN_CAPTURE
    m_fFlowDelta = 0.1f;
    m_fZDeltaAngle = 0.1f;
#else
    m_fFlowDelta = 0.0001f;
    m_fZDeltaAngle = 0.0001f;
#endif

#ifndef USE_BTEFFECT
    m_afFlowDirection[0] = 0.0f;
    m_afFlowDirection[1] = 0.0f;
    m_afPowerFactor[0] = 0.5f;
#endif
}
//----------------------------------------------------------------------------
bool BlendedTerrain::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,m_iWidth/(float)m_iHeight,0.01f,100.0f);
    Vector3f kCLoc(0.0f,-7.0f,1.5f);
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
void BlendedTerrain::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkHeightField = 0;
    m_spkSkyDome = 0;
    m_spkHeight = 0;
    m_spkBlend = 0;
#ifdef USE_BTEFFECT
    m_spkEffect = 0;
#endif
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BlendedTerrain::OnIdle ()
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

#ifndef SCREEN_CAPTURE
    UpdateClouds();
#endif

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->SetCamera(m_spkCamera);
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BlendedTerrain::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    {
#ifdef USE_BTEFFECT
        float fPowerFactor = m_spkEffect->GetPowerFactor();
        fPowerFactor += 0.01f;
        m_spkEffect->SetPowerFactor(fPowerFactor);
#else
        m_afPowerFactor[0] += 0.01f;
#endif
        return true;
    }

    case '-':
    case '_':
    {
#ifdef USE_BTEFFECT
        float fPowerFactor = m_spkEffect->GetPowerFactor();
        fPowerFactor -= 0.01f;
        if (fPowerFactor < 0.01f)
        {
            fPowerFactor = 0.01f;
        }
        m_spkEffect->SetPowerFactor(fPowerFactor);
#else
        m_afPowerFactor[0] -= 0.01f;
        if (m_afPowerFactor[0] < 0.01f)
        {
            m_afPowerFactor[0] = 0.01f;
        }
#endif
        return true;
    }

#ifdef SCREEN_CAPTURE
    case 'u':  // 19 key presses to get BlendedTerrain1.png
    case 'U':
        UpdateClouds();
        return true;
#endif

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"BlendedTerrain.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void BlendedTerrain::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    CreateSkyDome();
    CreateHeightField();

    // Create a 1-dimensional texture whose intensities are proportional to
    // height.
    const int iHSize = 256;
    unsigned char* aucData = WM4_NEW unsigned char[3*iHSize];
    int i;
    for (i = 0; i < iHSize; i++)
    {
        aucData[3*i+0] = i;
        aucData[3*i+1] = i;
        aucData[3*i+2] = i;
    }
    m_spkBlend = WM4_NEW Image(Image::IT_RGB888,iHSize,aucData,"BTBlend");

#ifdef USE_BTEFFECT
    m_spkEffect = WM4_NEW BlendedTerrainEffect("BTGrass","BTStone","BTBlend",
        "BTCloud");
    m_spkHeightField->AttachEffect(m_spkEffect);
#else
    VertexShader* pkVShader = WM4_NEW VertexShader("BlendedTerrain");
    PixelShader* pkPShader = WM4_NEW PixelShader("BlendedTerrain");

    pkPShader->SetTexture(0,"BTGrass");
    pkPShader->SetTexture(1,"BTStone");
    pkPShader->SetTexture(2,"BTBlend");
    pkPShader->SetTexture(3,"BTCloud");
    pkPShader->GetTexture(0)->SetFilterType(Texture::LINEAR_LINEAR);
    pkPShader->GetTexture(0)->SetWrapType(0,Texture::REPEAT);
    pkPShader->GetTexture(0)->SetWrapType(1,Texture::REPEAT);
    pkPShader->GetTexture(1)->SetFilterType(Texture::LINEAR_LINEAR);
    pkPShader->GetTexture(1)->SetWrapType(0,Texture::REPEAT);
    pkPShader->GetTexture(1)->SetWrapType(1,Texture::REPEAT);
    pkPShader->GetTexture(2)->SetFilterType(Texture::LINEAR);
    pkPShader->GetTexture(2)->SetWrapType(0,Texture::CLAMP_EDGE);
    pkPShader->GetTexture(3)->SetFilterType(Texture::LINEAR_LINEAR);
    pkPShader->GetTexture(3)->SetWrapType(0,Texture::REPEAT);
    pkPShader->GetTexture(3)->SetWrapType(1,Texture::REPEAT);

    ShaderEffect* pkEffect = WM4_NEW ShaderEffect(1);
    pkEffect->SetVShader(0,pkVShader);
    pkEffect->SetPShader(0,pkPShader);
    pkEffect->ConnectVShaderConstant(0,"FlowDirection",m_afFlowDirection);
    pkEffect->ConnectPShaderConstant(0,"PowerFactor",m_afPowerFactor);

    m_spkHeightField->AttachEffect(pkEffect);
#endif
}
//----------------------------------------------------------------------------
void BlendedTerrain::CreateSkyDome ()
{
    const char* acPath = System::GetPath("SkyDome.wmof",System::SM_READ);
    Stream kStream;
    kStream.Load(acPath);
    m_spkSkyDome = DynamicCast<TriMesh>(kStream.GetObjectAt(0));
    assert(m_spkSkyDome);
    m_spkSkyDome->Local.SetTranslate(Vector3f::ZERO);
    m_spkSkyDome->Local.SetUniformScale(7.9f);
    m_spkScene->AttachChild(m_spkSkyDome);
}
//----------------------------------------------------------------------------
void BlendedTerrain::CreateHeightField ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    kAttr.SetTCoordChannels(1,1);
    kAttr.SetTCoordChannels(2,2);

    const int iXSamples = 64, iYSamples = 64;
    const float fXExtent = 8.0f, fYExtent = 8.0f;
    int iVQuantity = iXSamples*iYSamples;
    int iTQuantity = 2*(iXSamples-1)*(iYSamples-1);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // Generate the geometry for a flat height field.
    float fInv0 = 1.0f/(iXSamples - 1.0f);
    float fInv1 = 1.0f/(iYSamples - 1.0f);
    float fU, fV;
    int i, i0, i1;
    for (i1 = 0, i = 0; i1 < iYSamples; i1++)
    {
        fV = i1*fInv1;
        Vector3f kYTmp = ((2.0f*fV-1.0f)*fYExtent)*Vector3f::UNIT_Y;
        for (i0 = 0; i0 < iXSamples; i0++)
        {
            fU = i0*fInv0;
            Vector3f kXTmp = ((2.0f*fU-1.0f)*fXExtent)*Vector3f::UNIT_X;
            pkVB->Position3(i) = kXTmp + kYTmp;
            Vector2f kTCoord(fU,fV);
            pkVB->TCoord2(0,i) = kTCoord;
            pkVB->TCoord1(1,i) = 0.0f;
            pkVB->TCoord2(2,i) = kTCoord;
            i++;
        }
    }

    // Generate the index array for a regular grid of squares, each square a
    // pair of triangles.
    int* aiIndex = pkIB->GetData();
    for (i1 = 0, i = 0; i1 < iYSamples - 1; i1++)
    {
        for (i0 = 0; i0 < iXSamples - 1; i0++)
        {
            int iV0 = i0 + iXSamples * i1;
            int iV1 = iV0 + 1;
            int iV2 = iV1 + iXSamples;
            int iV3 = iV0 + iXSamples;
            aiIndex[i++] = iV0;
            aiIndex[i++] = iV1;
            aiIndex[i++] = iV2;
            aiIndex[i++] = iV0;
            aiIndex[i++] = iV2;
            aiIndex[i++] = iV3;
        }
    }

    // Set the heights based on a precomputed height field.
    m_spkHeight = Image::Load("BTHeightField");
    unsigned char* pucData = m_spkHeight->GetData();
    for (i = 0; i < iVQuantity; i++, pucData += 3)
    {
        unsigned char ucValue = *pucData;
        float fHeight = ((float)ucValue)/255.0f;
        float fPerturb = 0.05f*Mathf::SymmetricRandom();
        pkVB->Position3(i).Z() = 3.0f*fHeight + fPerturb;
        pkVB->TCoord2(0,i) *= 8.0f;
        pkVB->TCoord1(1,i) = fHeight;
    }

    m_spkHeightField = WM4_NEW TriMesh(pkVB,pkIB);
    m_spkScene->AttachChild(m_spkHeightField);
}
//----------------------------------------------------------------------------
void BlendedTerrain::UpdateClouds ()
{
    // Animate the cloud layer.
#ifdef USE_BTEFFECT
    BlendedTerrainEffect* pkEffect = (BlendedTerrainEffect*)
        m_spkHeightField->GetEffect(0);
    Vector2f kFlowDirection = pkEffect->GetFlowDirection();
    kFlowDirection[0] += m_fFlowDelta;
    if (kFlowDirection[0] < 0.0f)
    {
        kFlowDirection[0] += 1.0f;
    }
    else if (kFlowDirection[0] > 1.0f )
    {
        kFlowDirection[0] -= 1.0f;
    }
    pkEffect->SetFlowDirection(kFlowDirection);
#else
    m_afFlowDirection[0] += m_fFlowDelta;
    if (m_afFlowDirection[0] < 0.0f)
    {
        m_afFlowDirection[0] += 1.0f;
    }
    else if (m_afFlowDirection[0] > 1.0f )
    {
        m_afFlowDirection[0] -= 1.0f;
    }
#endif

    // Rotate the sky dome.
    m_fZAngle += m_fZDeltaAngle;
    if (m_fZAngle > Mathf::TWO_PI)
    {
        m_fZAngle -= Mathf::TWO_PI;
    }
    Matrix3f kRot(Vector3f::UNIT_Z,m_fZAngle);
    m_spkSkyDome->Local.SetRotate(kRot);
    m_spkSkyDome->UpdateGS();
}
//----------------------------------------------------------------------------
