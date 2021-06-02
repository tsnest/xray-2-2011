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
// Version: 4.0.2 (2007/08/11)

#include "VolumeTextures.h"

WM4_WINDOW_APPLICATION(VolumeTextures);

//----------------------------------------------------------------------------
VolumeTextures::VolumeTextures ()
    :
    WindowApplication3("VolumeTextures",0,0,640,480,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool VolumeTextures::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,m_iWidth/(float)m_iHeight,0.01f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,4.0f);
    Vector3f kCDir(0.0f,0.0f,-1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
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
void VolumeTextures::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkMesh = 0;
    m_spkVolume = 0;
    m_spkVolumeTexture = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void VolumeTextures::OnIdle ()
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
bool VolumeTextures::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"VolumeTextures.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void VolumeTextures::CreateScene ()
{
    m_spkScene = WM4_NEW Node;

    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    AlphaState* pkAState = WM4_NEW AlphaState;
    pkAState->BlendEnabled = true;
    m_spkScene->AttachGlobalState(pkAState);

    CullState* pkCState = WM4_NEW CullState;
    pkCState->Enabled = false;
    m_spkScene->AttachGlobalState(pkCState);

    CreateVolumeTexture();
    CreateGridMesh();
}
//----------------------------------------------------------------------------
void VolumeTextures::CreateVolumeTexture ()
{
    const int iBound = 64;
    unsigned char* aucData = WM4_NEW unsigned char[3*iBound*iBound*iBound];

    // Create three Gaussian distributions to be used for the RGB color
    // channels.  The alpha channel is constant and is stored as a pixel
    // shader constant.
    const float fRParam = 0.01f;
    const float fGParam = 0.01f;
    const float fBParam = 0.01f;
    const float fExtreme = 8.0f;
    Vector3f kRCenter(0.5f*fExtreme,0.0f,0.0f);
    Vector3f kGCenter(-0.5f*fExtreme,-0.5f*fExtreme,0.0f);
    Vector3f kBCenter(-0.5f*fExtreme,+0.5f*fExtreme,0.0f);
    m_afCommonAlpha[0] = 0.05f;

    Vector3f kPoint;
    int i = 0;
    for (int iZ = 0; iZ < iBound; iZ++)
    {
        kPoint[2] = -fExtreme + 2.0f*fExtreme*iZ/(float)(iBound-1);
        for (int iY = 0; iY < iBound; iY++)
        {
            kPoint[1] = -fExtreme + 2.0f*fExtreme*iY/(float)(iBound-1);
            for (int iX = 0; iX < iBound; iX++)
            {
                kPoint[0] = -fExtreme + 2.0f*fExtreme*iX/(float)(iBound-1);

                Vector3f kDiff = kPoint - kRCenter;
                float fRSqr = kDiff.SquaredLength();
                float fRGauss = 1.0f - fRParam*fRSqr;
                if (fRGauss < 0.0f)
                {
                    fRGauss = 0.0f;
                }

                kDiff = kPoint - kGCenter;
                fRSqr = kDiff.SquaredLength();
                float fGGauss = 1.0f - fGParam*fRSqr;
                if (fGGauss < 0.0f)
                {
                    fGGauss = 0.0f;
                }

                kDiff = kPoint - kBCenter;
                fRSqr = kDiff.SquaredLength();
                float fBGauss = 1.0f - fBParam*fRSqr;
                if (fBGauss < 0.0f)
                {
                    fBGauss = 0.0f;
                }

                aucData[i++] = (unsigned char)(255.0f*fRGauss);
                aucData[i++] = (unsigned char)(255.0f*fGGauss);
                aucData[i++] = (unsigned char)(255.0f*fBGauss);
            }
        }
    }

    m_spkVolume = WM4_NEW Image(Image::IT_RGB888,64,64,64,aucData,
        "VolumeImage");

    VertexShader* pkVShader = WM4_NEW VertexShader("VolumeTextures");
    PixelShader* pkPShader = WM4_NEW PixelShader("VolumeTextures");

    pkPShader->SetTexture(0,"VolumeImage");
    pkPShader->GetTexture(0)->SetFilterType(Texture::NEAREST);
    pkPShader->GetTexture(0)->SetWrapType(0,Texture::REPEAT);
    pkPShader->GetTexture(0)->SetWrapType(1,Texture::REPEAT);

    m_spkVolumeTexture = WM4_NEW ShaderEffect(1);
    m_spkVolumeTexture->SetVShader(0,pkVShader);
    m_spkVolumeTexture->SetPShader(0,pkPShader);

    //m_pkRenderer->LoadResources(m_spkVolumeTexture);
    Program* pkProgram = m_spkVolumeTexture->GetPProgram(0);
    pkProgram->GetUserConstant("CommonAlpha")->SetDataSource(m_afCommonAlpha);
}
//----------------------------------------------------------------------------
void VolumeTextures::CreateGridMesh ()
{
    const int iSlices = 64;
    const int iDelta = 32;
    for (int i = 0; i < iSlices; i++)
    {
        float fW = i/(float)(iSlices-1);
        m_spkScene->AttachChild(Rectangle(iDelta,iDelta,fW));
    }
    m_spkScene->AttachEffect(m_spkVolumeTexture);
}
//----------------------------------------------------------------------------
TriMesh* VolumeTextures::Rectangle (int iXSamples, int iYSamples, float fW)
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,3);

    int iVQuantity = iXSamples*iYSamples;
    int iTQuantity = 2*(iXSamples-1)*(iYSamples-1);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // generate geometry
    float fInv0 = 1.0f/(iXSamples - 1.0f);
    float fInv1 = 1.0f/(iYSamples - 1.0f);
    Vector3f kZTmp = (2.0f*fW - 1.0f)*Vector3f::UNIT_Z;
    float fU, fV;
    int i, i0, i1;
    for (i1 = 0, i = 0; i1 < iYSamples; i1++)
    {
        fV = i1*fInv1;
        Vector3f kYTmp = (2.0f*fV - 1.0f)*Vector3f::UNIT_Y;
        for (i0 = 0; i0 < iXSamples; i0++)
        {
            fU = i0*fInv0;
            Vector3f kXTmp = (2.0f*fU - 1.0f)*Vector3f::UNIT_X;
            pkVB->Position3(i) = kXTmp + kYTmp + kZTmp;
            pkVB->TCoord3(0,i) = Vector3f(fU,fV,fW);
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
