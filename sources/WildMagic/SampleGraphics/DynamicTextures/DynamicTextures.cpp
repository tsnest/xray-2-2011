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

#include "DynamicTextures.h"

WM4_WINDOW_APPLICATION(DynamicTextures);

//----------------------------------------------------------------------------
DynamicTextures::DynamicTextures ()
    :
    WindowApplication3("DynamicTextures",0,0,512,512,ColorRGBA::WHITE)
{
    m_aiPermutation = 0;
    m_aiInversePermutation = 0;
}
//----------------------------------------------------------------------------
bool DynamicTextures::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,1.0f,0.1f,100.0f);
    Vector3f kCLoc = Vector3f(0.0f,0.0f,2.0f);
    Vector3f kCDir = -Vector3f::UNIT_Z;
    Vector3f kCUp = Vector3f::UNIT_Y;
    Vector3f kCRight = Vector3f::UNIT_X;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);

    m_bIsDirect3D = (strcmp(m_pkRenderer->GetExtension(),"dx9") == 0);
    return true;
}
//----------------------------------------------------------------------------
void DynamicTextures::OnTerminate ()
{
    WM4_DELETE[] m_aiPermutation;
    WM4_DELETE[] m_aiInversePermutation;
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkDTexture = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void DynamicTextures::OnIdle ()
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

    // This call occurs here to (1) allow the original image to be displayed
    // and (2) to allow the original texture to be loaded to VRAM.
    UpdateTexture();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool DynamicTextures::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"DynamicTextures.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void DynamicTextures::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    TriMesh* pkPlane = kSM.Rectangle(2,2,1.0f,1.0f);

    Image* pkImage = Image::Load("Magician");
    m_spkDTexture = WM4_NEW DynamicTexture("DynaMagician", pkImage);
    m_spkEffect = WM4_NEW TextureEffect("DynaMagician");

    pkPlane->AttachEffect(m_spkEffect);
    m_spkScene->AttachChild(pkPlane);

    // Create identity permutation (0,1,...,m_iQuantity-1).
    m_iQuantity = pkImage->GetQuantity();
    m_aiPermutation = WM4_NEW int[m_iQuantity];
    m_aiInversePermutation = WM4_NEW int[m_iQuantity];
    int i;
    for (i = 0; i < m_iQuantity; i++)
    {
        m_aiPermutation[i] = i;
    }

    // Generate random permutation.
    for (i = m_iQuantity - 1; i > 0; i--)
    {
        // Generate a random 32-bit integer.  Note that rand() might only
        // return a maximum of 2^{15}-1, so we build the 32-bit integer in
        // four parts.
        int iRnd0 = rand() % 256;
        int iRnd1 = rand() % 256;
        int iRnd2 = rand() % 256;
        int iRnd3 = rand() % 256;
        int j = iRnd0 | (iRnd1 << 8) | (iRnd2 << 16) | (iRnd3 << 24);
        if (j < 0)
        {
            j = -j;
        }
        j %= (i + 1);

        if (j != i)
        {
            int iSave = m_aiPermutation[i];
            m_aiPermutation[i] = m_aiPermutation[j];
            m_aiPermutation[j] = iSave;
        }
    }

    for (i = 0; i < m_iQuantity; i++)
    {
        m_aiInversePermutation[m_aiPermutation[i]] = i;
    }

    m_iCurrent = 0;
    m_iDelta = pkImage->GetBound(0);
    m_bReverse = false;
}
//----------------------------------------------------------------------------
void DynamicTextures::UpdateTexture ()
{
    const Image* pkImage = m_spkDTexture->GetImage();
    const unsigned char *aucSrc = pkImage->GetData();
    unsigned char* aucTrg = (unsigned char*)
        m_pkRenderer->Lock(m_spkDTexture, Renderer::DT_WRITE);

    const int iNumTrgChannels = (m_bIsDirect3D ? 4 : 3);
    const unsigned char* pucSrc;
    unsigned char* pucTrg;
    int iLast = m_iCurrent + m_iDelta;
    for (int i = m_iCurrent; i < iLast; i++)
    {
        if (m_bReverse)
        {
            pucSrc = &aucSrc[3*m_aiInversePermutation[i]];
            pucTrg = &aucTrg[iNumTrgChannels*m_aiInversePermutation[i]];
        }
        else
        {
            pucSrc = &aucSrc[3*i];
            pucTrg = &aucTrg[iNumTrgChannels*m_aiPermutation[i]];
        }

        // THIS IS A HACK.  The Direct3D renderer pads RGB888 to RGBA8888
        // and the red and blue channels are swapped.  WM5 will arrange to
        // do the right thing...
        if (m_bIsDirect3D)
        {
            pucTrg[0] = pucSrc[2];
            pucTrg[1] = pucSrc[1];
            pucTrg[2] = pucSrc[0];
            pucSrc += 3;
            pucTrg += 4;
        }
        else
        {
            *pucTrg++ = *pucSrc++;
            *pucTrg++ = *pucSrc++;
            *pucTrg++ = *pucSrc++;
        }
    }
    m_iCurrent = iLast;

    if (m_iCurrent == m_iQuantity)
    {
        m_iCurrent = 0;
        m_bReverse = !m_bReverse;
    }

    m_pkRenderer->Unlock(m_spkDTexture);
}
//----------------------------------------------------------------------------
