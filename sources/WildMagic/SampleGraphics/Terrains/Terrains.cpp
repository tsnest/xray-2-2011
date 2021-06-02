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
// Version: 4.0.3 (2007/08/11)

#include "Terrains.h"
#include "TerrainEffect.h"

WM4_WINDOW_APPLICATION(Terrains);

//----------------------------------------------------------------------------
Terrains::Terrains ()
    :
    WindowApplication3("Terrains",0,0,640,480,ColorRGBA(0.5f,0.0f,1.0f,1.0f)),
        m_kFogColor(ColorRGB(0.5686f,0.7255f,0.8353f)),
        m_fFogDensity(0.0015f)
{
    // For lower-resolution data, change these paths to use Height64/Image64,
    // or Height32/Image32.
    System::InsertDirectory("../../Data/Terrain/Height128");
    System::InsertDirectory("../../Data/Terrain/Image128/");

    // The sky dome model, skyline image, and detail texture are found here.
    System::InsertDirectory("../../Data/Terrain");

    m_fUVBias = 0.0f;
    m_fHeightAboveTerrain = 20.0f;
}
//----------------------------------------------------------------------------
bool Terrains::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Position the camera in the middle of page[0][0].  Orient it to look
    // diagonally into the terrain.
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1500.0f);
    Vector3f kCLoc(64.0f,64.0f,m_fHeightAboveTerrain);
    Vector3f kCDir(1.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    kCDir.Normalize();
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // create scene graph
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);
    CreateSkyDome();
    CreateTerrain();

    // Preload all renderer resources to VRAM.  This is a large scene, so the
    // function call takes some time...
    m_pkRenderer->LoadAllResources(m_spkScene);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(1.0f,0.01f);
    MoveForward();
    return true;
}
//----------------------------------------------------------------------------
void Terrains::OnTerminate ()
{
    m_spkWireframe = 0;
    m_spkScene = 0;
    m_spkSkyDome = 0;
    m_spkTerrain = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Terrains::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        // The sky dome moves with the camera so that it is always in view.
        Vector3f kTrn = m_spkCamera->GetLocation();
        kTrn.Z() = 0.0f;
        m_spkSkyDome->Local.SetTranslate(kTrn);
        m_spkSkyDome->UpdateGS();

        // update the active terrain pages
        m_spkTerrain->OnCameraMotion();
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    Vector3f kNormal = m_spkTerrain->GetNormal(kLoc.X(),kLoc.Y());
    char acMsg[512];
    System::Sprintf(acMsg,512,"height = %f , normal = (%f,%f,%f)\n",
        fHeight,kNormal.X(),kNormal.Y(),kNormal.Z());

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->Draw(128,GetHeight()-8,ColorRGBA::WHITE,acMsg);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Terrains::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"Terrains.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Terrains::CreateSkyDome ()
{
    // Load hemispherical sky dome.  The bounding box is [-1,1]x[-1,1]x[0,1].
    // The dome is scaled to the proper size for this application.
    const char* acPath = System::GetPath("SkyDome.wmof",System::SM_READ);
    Stream kStream;
    kStream.Load(acPath);
    m_spkSkyDome = DynamicCast<TriMesh>(kStream.GetObjectAt(0));
    assert(m_spkSkyDome);
    m_spkScene->AttachChild(m_spkSkyDome);

    // scale the sky dome so that it just fits inside the far plane
    m_spkSkyDome->Local.SetUniformScale(m_spkCamera->GetDMax());

    // sky dome follows camera
    Vector3f kCLoc = m_spkCamera->GetLocation();
    m_spkSkyDome->Local.SetTranslate(Vector3f(kCLoc.X(),kCLoc.Y(),0.0f));
}
//----------------------------------------------------------------------------
void Terrains::CreateTerrain ()
{
    // Create the full terrain.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);  // terrain base texture
    kAttr.SetTCoordChannels(1,2);  // detail texture
    ColorRGBA kBorderColor(m_kFogColor[0],m_kFogColor[1],m_kFogColor[2],1.0f);
    m_spkTerrain = WM4_NEW Terrain("height","image",kAttr,m_spkCamera,
        m_fUVBias,&kBorderColor);
    m_spkScene->AttachChild(m_spkTerrain);

    int iRows = m_spkTerrain->GetRowQuantity();
    int iCols = m_spkTerrain->GetColQuantity();
    for (int iRow = 0; iRow < iRows; iRow++)
    {
        for (int iCol = 0; iCol < iCols; iCol++)
        {
            TerrainPage* pkPage = m_spkTerrain->GetPage(iRow,iCol);

#if 0
            // The default texturing uses a single base texture.  Change this
            // to use a base texture modulated by a detail texture and the
            // result blended with fog.
            MultitextureEffect* pkEffect = DynamicCast<MultitextureEffect>(
                pkPage->GetEffect(0));
            std::string kBaseName = pkEffect->GetImageName(0);
            pkEffect->SetTextureQuantity(2);
            pkEffect->SetImageName(0,kBaseName);
            pkEffect->SetImageName(1,"Detail");

            AlphaState* pkAState = pkEffect->GetBlending(1);
            pkAState->SrcBlend = AlphaState::SBF_DST_COLOR;
            pkAState->DstBlend = AlphaState::DBF_ZERO;
            pkEffect->Configure();

            Texture* pkTexture = pkEffect->GetPTexture(0,0);
            pkTexture->SetFilterType(Texture::LINEAR_LINEAR);
            pkTexture->SetWrapType(0,Texture::REPEAT);
            pkTexture->SetWrapType(1,Texture::REPEAT);

            pkTexture = pkEffect->GetPTexture(0,1);
            pkTexture->SetFilterType(Texture::LINEAR_LINEAR);
#else
            // The default texturing uses a single base texture.  Change this
            // to use a base texture modulated by a detail texture and the
            // result blended with fog.
            MultitextureEffect* pkEffect = DynamicCast<MultitextureEffect>(
                pkPage->GetEffect(0));
            std::string kBaseName = pkEffect->GetTextureName(0);
            pkPage->DetachEffect(pkEffect);

            TerrainEffect* pkTEffect = WM4_NEW TerrainEffect(
                kBaseName.c_str(),"Detail",m_kFogColor,m_fFogDensity);
            pkPage->AttachEffect(pkTEffect);
#endif
        }
    }

    // TO DO.  Add fog vertex/pixel shaders.
    // use fog to hide horizon
    //FogState* pkFS = WM4_NEW FogState;
    //pkFS->Enabled = true;
    //pkFS->Density = 0.0015f;
    //pkFS->Color = m_kFogColor;
    //pkFS->DensityFunction = FogState::DF_EXPSQR;
    //m_spkTerrain->SetGlobalState(pkFS);
}
//----------------------------------------------------------------------------
void Terrains::MoveForward ()
{
    WindowApplication3::MoveForward();

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void Terrains::MoveBackward ()
{
    WindowApplication3::MoveBackward();

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void Terrains::MoveDown ()
{
    if (m_fHeightAboveTerrain >= m_fTrnSpeed)
    {
        m_fHeightAboveTerrain -= m_fTrnSpeed;
    }

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void Terrains::MoveUp ()
{
    m_fHeightAboveTerrain += m_fTrnSpeed;

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
