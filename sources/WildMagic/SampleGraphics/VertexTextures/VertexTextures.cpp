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
// Version: 4.6.0 (2007/08/22)

#include "VertexTextures.h"

WM4_WINDOW_APPLICATION(VertexTextures);

#define USE_32BIT_TEXTURE

//----------------------------------------------------------------------------
VertexTextures::VertexTextures ()
    :
    WindowApplication3("VertexTextures",0,0,640,480,
        ColorRGBA(0.75f,0.75f,0.75f,1.0f))
{
    System::InsertDirectory("Data");
}
//----------------------------------------------------------------------------
bool VertexTextures::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(60.0f,4.0f/3.0f,1.0f,10000.0f);
    Vector3f kCLoc(0.0f,0.0f,4.0f);
    Vector3f kCDir(0.0f,0.0f,-1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

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
void VertexTextures::OnTerminate ()
{
    m_spkScene = 0;
    m_spkHeight = 0;
    m_spkHeight32 = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void VertexTextures::OnIdle ()
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
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool VertexTextures::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
TriMesh* VertexTextures::CreateRectangle (int iXSamples, int iYSamples,
    float fXExtent, float fYExtent)
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);

    int iVQuantity = iXSamples*iYSamples;
    int iTQuantity = 2*(iXSamples-1)*(iYSamples-1);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // generate geometry
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
            pkVB->TCoord2(0,i) = Vector2f(fU,fV);
            i++;
        }
    }

    // generate connectivity
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

    return WM4_NEW TriMesh(pkVB,pkIB);
}
//----------------------------------------------------------------------------
void VertexTextures::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    const int iBound = 32;
    TriMesh* pkMesh = CreateRectangle(iBound,iBound,1.0f,1.0f);

    m_spkHeight = Image::Load("HeightField");
    unsigned char* aucData = m_spkHeight->GetData();

#ifdef USE_32BIT_TEXTURE
    const int iQuantity = iBound*iBound;
    float* afData = WM4_NEW float[iQuantity];
    for (int i = 0; i < iQuantity; i++)
    {
        afData[i] = aucData[i];///255.0f;
    }

    m_spkHeight32 = WM4_NEW Image(Image::IT_INTENSITY32F,iBound,iBound,
        (unsigned char*)afData,"HeightField32");
#endif

    VertexShader* pkVShader = WM4_NEW VertexShader("HeightEffect");
#ifdef USE_32BIT_TEXTURE
    pkVShader->SetTexture(0,"HeightField32");
#else
    pkVShader->SetTexture(0,"HeightField");
#endif

    PixelShader* pkPShader = WM4_NEW PixelShader("HeightEffect");
    pkPShader->SetTexture(0,"HeightField");

    ShaderEffect* pkEffect = WM4_NEW ShaderEffect(1);
    pkEffect->SetVShader(0,pkVShader);
    pkEffect->SetPShader(0,pkPShader);

    pkMesh->AttachEffect(pkEffect);

    m_spkScene->AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
