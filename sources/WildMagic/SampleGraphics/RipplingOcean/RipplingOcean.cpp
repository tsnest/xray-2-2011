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

#include "RipplingOcean.h"

WM4_WINDOW_APPLICATION(RipplingOcean);

//----------------------------------------------------------------------------
RipplingOcean::RipplingOcean ()
    :
    WindowApplication3("RipplingOcean",0,0,640,480,
        ColorRGBA(0.635294f,0.917647f,1.0f,1.0f))
{
    System::InsertDirectory("Data");

    // Arbitrary constants to make particular looking waves.
    m_afWaveSpeed[0] = 0.2f;
    m_afWaveSpeed[1] = 0.15f;
    m_afWaveSpeed[2] = 0.4f;
    m_afWaveSpeed[3] = 0.4f;

    m_afWaveHeight[0] = -16.0f;
    m_afWaveHeight[1] = 10.0f;
    m_afWaveHeight[2] = 5.8f;
    m_afWaveHeight[3] = 8.5f;

    m_afBumpSpeed[0] = 0.031f;
    m_afBumpSpeed[1] = 0.04f;
    m_afBumpSpeed[2] = -0.03f;
    m_afBumpSpeed[3] = 0.02f;
}
//----------------------------------------------------------------------------
bool RipplingOcean::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10000.0f);
    Vector3f kCLoc(0.0f,-600.0f,-100.0f);
    Vector3f kCDir(0.0f,1.0f,0.5f);
    kCDir.Normalize();
    Vector3f kCUp(0.0f,kCDir.Z(),-kCDir.Y());
    Vector3f kCRight = -Vector3f::UNIT_X;
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
void RipplingOcean::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkNormalMap = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void RipplingOcean::OnIdle ()
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

    float fTime = (m_bStopped ? m_fStopTime : (float)System::GetTime());
    m_spkEffect->SetTime(fTime);

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
bool RipplingOcean::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    float fAmbient, fTextureRepeat, afValue[4];
    int i;

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 'h':
        m_fWaveHeightAmplitude -= 0.1f;
        if (m_fWaveHeightAmplitude < 0.0f)
        {
            m_fWaveHeightAmplitude = 0.0f;
        }
        for (i = 0; i < 4; i++)
        {
            afValue[i] = m_fWaveHeightAmplitude*m_afWaveHeight[i];
        }
        m_spkEffect->SetWaveHeight(afValue);
        return true;
    case 'H':
        m_fWaveHeightAmplitude += 0.1f;
        for (i = 0; i < 4; i++)
        {
            afValue[i] = m_fWaveHeightAmplitude*m_afWaveHeight[i];
        }
        m_spkEffect->SetWaveHeight(afValue);
        return true;
    case 'v':
        m_fWaveSpeedAmplitude -= 0.1f;
        if (m_fWaveSpeedAmplitude < 0.0f)
        {
            m_fWaveSpeedAmplitude = 0.0f;
        }
        for (i = 0; i < 4; i++)
        {
            afValue[i] = m_fWaveSpeedAmplitude*m_afWaveSpeed[i];
        }
        m_spkEffect->SetWaveSpeed(afValue);
        return true;
    case 'V':
        m_fWaveSpeedAmplitude += 0.1f;
        for (i = 0; i < 4; i++)
        {
            afValue[i] = m_fWaveSpeedAmplitude*m_afWaveSpeed[i];
        }
        m_spkEffect->SetWaveSpeed(afValue);
        return true;
    case 'a':
        fAmbient = m_spkEffect->GetAmbient();
        fAmbient -= 0.05f;
        if (fAmbient < 0.0f)
        {
            fAmbient = 0.0f;
        }
        m_spkEffect->SetAmbient(fAmbient);
        return true;
    case 'A':
        fAmbient = m_spkEffect->GetAmbient();
        fAmbient += 0.05f;
        if (fAmbient > 1.0f)
        {
            fAmbient = 1.0f;
        }
        m_spkEffect->SetAmbient(fAmbient);
        return true;
    case 'r':
        m_fBumpSpeedAmplitude -= 0.1f;
        if (m_fBumpSpeedAmplitude < 0.0f)
        {
            m_fBumpSpeedAmplitude = 0.0f;
        }
        for (i = 0; i < 4; i++)
        {
            afValue[i] = m_fBumpSpeedAmplitude*m_afBumpSpeed[i];
        }
        m_spkEffect->SetBumpSpeed(afValue);
        return true;
    case 'R':
        m_fBumpSpeedAmplitude += 0.1f;
        for (i = 0; i < 4; i++)
        {
            afValue[i] = m_fBumpSpeedAmplitude*m_afBumpSpeed[i];
        }
        m_spkEffect->SetBumpSpeed(afValue);
        return true;
    case 'T':
        fTextureRepeat = m_spkEffect->GetTextureRepeat();
        fTextureRepeat += 0.1f;
        m_spkEffect->SetTextureRepeat(fTextureRepeat);
        return true;
    case 't':
        fTextureRepeat = m_spkEffect->GetTextureRepeat();
        fTextureRepeat -= 0.1f;
        if (fTextureRepeat < 0.0f)
        {
            fTextureRepeat = 0.0f;
        }
        m_spkEffect->SetTextureRepeat(fTextureRepeat);
        return true;
    case ' ':
        m_bStopped = !m_bStopped;
        m_fStopTime = (float)System::GetTime();
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"RipplingOcean.wmof");
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void RipplingOcean::CreateScene ()
{
    m_bStopped = false;
    m_fStopTime = (float)System::GetTime();

    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // The height field has origin (0,0,0) and up-direction of (0,0,-1).
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    kAttr.SetColorChannels(0,3);
    kAttr.SetTCoordChannels(0,2);
    Transformation kXFrm;
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Y,Mathf::PI));
    StandardMesh kSM(kAttr,false,&kXFrm);
    TriMesh* pkMesh = kSM.Rectangle(50,50,1400.0f,1200.0f);
    m_spkScene->AttachChild(pkMesh);

    CreateNormalMapFromHeightImage("Plasma");

    m_spkEffect = WM4_NEW RipplingOceanEffect("NormalMap","WaterGradient",
        "RippleSky");
    pkMesh->AttachEffect(m_spkEffect);

    // Fix the light direction for the entire simulation.
    Vector3f kLightDir(0.0f,1.0f,1.0f);
    kLightDir.Normalize();
    m_spkEffect->SetLightDir(kLightDir);

    // Arbitrary constants to make particular looking waves.
    float afWaveDirX[4] = {0.25f,0.0f,-0.7f,-0.8f};
    float afWaveDirY[4] = {0.0f,0.15f,-0.7f,0.1f};
    float afWaveOffset[4] = {0.0f,0.2f,0.3f,-0.2f};
    m_spkEffect->SetWaveDirX(afWaveDirX);
    m_spkEffect->SetWaveDirY(afWaveDirY);
    m_spkEffect->SetWaveOffset(afWaveOffset);

    m_spkEffect->SetAverageDuDxDvDy(1.0f/24.0f);
    m_spkEffect->SetAmbient(0.3f);
    m_spkEffect->SetTextureRepeat(6.0f);

    m_fWaveSpeedAmplitude = 1.0f;
    m_fWaveHeightAmplitude = 1.0f;
    m_fBumpSpeedAmplitude = 1.0f;
    m_spkEffect->SetWaveSpeed(m_afWaveSpeed);
    m_spkEffect->SetWaveHeight(m_afWaveHeight);
    m_spkEffect->SetBumpSpeed(m_afBumpSpeed);
}
//----------------------------------------------------------------------------
void RipplingOcean::CreateNormalMapFromHeightImage (
    const char* acImageName)
{
    // Given a height field z = H(x,y), normal vectors to the graph of the
    // function are
    //   N(x,y) = (dH/dx,dH/dy,1)/sqrt(1+(dH/dx)^2+(dH/dy)^2)
    // The terms dH/dx and dH/dy are the partial derivatives of H with respect
    // to x and y.  These are approximated by centered differences
    //   dH/dx = (H(x+dx,y) - H(x-dx,y))/(2*dx)
    //   dH/dy = (H(x,y+dy) - H(x,y-dy))/(2*dy)
    // When the height field is stored in an image I(x,y), where 0 <= x < xmax
    // and 0 <= y < ymax (x and y are integers), choose dx = 1 and dy = 1.
    // The image is gray scale, so the red, green, and blue channels all have
    // the same value.  It is sufficient to use the red channel, R(x,y).
    // The approximations are
    //   dH/dx = (R(x+1,y) - R(x-1,y))/2
    //   dH/dy = (R(x,y+1) - R(x,y-1))/2
    // Special handling is required at the image boundaries x = 0, x = xmax-1,
    // y = 0, and y = ymax-1.  Wrap-around is used here, so define values
    //   R(-1,y) = R(xmax-1,y)
    //   R(xmax,y) = R(0,y)
    //   R(x,-1) = R(x,ymax-1)
    //   R(x,ymax) = R(x,0)
    //
    // Since the red channel has values in [0,255], the approximations for
    // dH/dx and dH/dy are in [-255/2,255/2].  In this particular application,
    // these are scaled to [-100,100] (WHY?).

    Image* pkPlasma = Image::Load(acImageName);
    assert(pkPlasma);

    int iXMax = pkPlasma->GetBound(0);
    int iYMax = pkPlasma->GetBound(1);
    int iXMaxM1 = iXMax -1, iYMaxM1 = iYMax - 1;
    int iBPP = pkPlasma->GetBytesPerPixel();
    unsigned char* aucHeight = pkPlasma->GetData();
    const float fScale = 100.0f/255.0f;
    unsigned char* aucNormal = WM4_NEW unsigned char[4*iXMax*iYMax];

    for (int iY = 0; iY < iYMax; iY++)
    {
        int iYm1 = (iY > 0 ? iY-1 : iYMaxM1);
        int iYp1 = (iY < iYMaxM1 ? iY+1 : 0);

        for (int iX = 0; iX < iXMax; iX++)
        {
            int iXm1 = (iX > 0 ? iX-1 : iXMaxM1);
            int iXp1 = (iX < iXMaxM1 ? iX+1 : 0);

            float fRXm1Y = (float)aucHeight[iBPP*(iXm1 + iY*iXMax)];
            float fRXp1Y = (float)aucHeight[iBPP*(iXp1 + iY*iXMax)];
            float fRXYm1 = (float)aucHeight[iBPP*(iX + iYm1*iXMax)];
            float fRXYp1 = (float)aucHeight[iBPP*(iX + iYp1*iXMax)];
            float fDHDX = 0.5f*(fRXp1Y - fRXm1Y);
            float fDHDY = 0.5f*(fRXYp1 - fRXYm1);

            Vector3f kNormal(fScale*fDHDX,fScale*fDHDY,1.0f);
            kNormal.Normalize();

            // Transform the normal vector from [-1,1]^3 to [0,255]^3 so it
            // can be stored as a color value.
            unsigned char* aucColor = &aucNormal[4*(iX+iY*iXMax)];
            aucColor[0] = (unsigned char)(127.5f*(kNormal[0]+1.0f));
            aucColor[1] = (unsigned char)(127.5f*(kNormal[1]+1.0f));
            aucColor[2] = (unsigned char)(127.5f*(kNormal[2]+1.0f));
            aucColor[3] = 0;
        }
    }

    WM4_DELETE pkPlasma;
    m_spkNormalMap = WM4_NEW Image(Image::IT_RGBA8888,iXMax,iYMax,aucNormal,
        "NormalMap");
}
//----------------------------------------------------------------------------
