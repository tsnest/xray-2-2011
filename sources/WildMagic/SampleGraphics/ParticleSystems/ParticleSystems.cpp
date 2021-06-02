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

#include "ParticleSystems.h"
#include "BloodCellController.h"

WM4_WINDOW_APPLICATION(ParticleSystems);

//----------------------------------------------------------------------------
ParticleSystems::ParticleSystems ()
    :
    WindowApplication3("ParticleSystems",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool ParticleSystems::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(4.0f,0.0f,0.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // set up scene
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);
    m_spkScene->AttachChild(CreateParticles());

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ParticleSystems::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkBloodCell = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ParticleSystems::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    MoveObject();
    m_spkScene->UpdateGS(System::GetTime());
    m_kCuller.ComputeVisibleSet(m_spkScene);

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
bool ParticleSystems::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"ParticleSystems.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ParticleSystems::CreateBloodCellImage ()
{
    // create an image with transparency
    int iWidth = 32, iHeight = 32;
    float fFactor = 1.0f/(iWidth*iWidth + iHeight*iHeight);
    unsigned char* aucData = WM4_NEW unsigned char[4*iWidth*iHeight];
    int i = 0;
    for (int iY = 0; iY < iHeight; iY++)
    {
        for (int iX = 0; iX < iWidth; iX++)
        {
            aucData[i++] = 255;
            aucData[i++] = 0;
            aucData[i++] = 0;

            int iDX = 2*iX - iWidth;
            int iDY = 2*iY - iHeight;
            float fValue = fFactor*(iDX*iDX + iDY*iDY);
            assert(0.0f <= fValue && fValue <= 1.0f);
            if (fValue < 0.125f)
            {
                fValue = Mathf::Cos(4.0f*Mathf::PI*fValue);
            }
            else
            {
                fValue = 0.0f;
            }
            aucData[i++] = (unsigned char)(255.0f*fValue);
        }
    }

    m_spkBloodCell = WM4_NEW Image(Image::IT_RGBA8888,iWidth,iHeight,aucData,
        "BloodCell");
}
//----------------------------------------------------------------------------
Particles* ParticleSystems::CreateParticles ()
{
    const int iVQuantity = 32;
    Vector3f* akVertex = WM4_NEW Vector3f[iVQuantity];
    float* afSize = WM4_NEW float[iVQuantity];
    for (int i = 0; i < iVQuantity; i++)
    {
        akVertex[i].X() = Mathf::SymmetricRandom();
        akVertex[i].Y() = Mathf::SymmetricRandom();
        akVertex[i].Z() = Mathf::SymmetricRandom();
        afSize[i] = 0.25f*Mathf::UnitRandom();
    }
    Vector3fArray* pkVertices = WM4_NEW Vector3fArray(iVQuantity,akVertex);
    FloatArray* pkSizes = WM4_NEW FloatArray(iVQuantity,afSize);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    Particles* pkParticles = WM4_NEW Particles(kAttr,pkVertices,pkSizes);

    CreateBloodCellImage();
    Effect* pkEffect = WM4_NEW TextureEffect("BloodCell");
    pkParticles->AttachEffect(pkEffect);

    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    pkParticles->AttachGlobalState(pkAS);

    ZBufferState* pkZS = WM4_NEW ZBufferState;
    pkZS->Enabled = false;
    pkParticles->AttachGlobalState(pkZS);

    pkParticles->AttachController(WM4_NEW BloodCellController);

    return pkParticles;
}
//----------------------------------------------------------------------------
