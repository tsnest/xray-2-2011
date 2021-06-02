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

#include "ClodTerrains.h"

WM4_WINDOW_APPLICATION(ClodTerrains);

//----------------------------------------------------------------------------
ClodTerrains::ClodTerrains ()
    :
    WindowApplication3("ClodTerrains",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f)),m_kFogColor(
        ColorRGBA(145.0f/255.0f,185.0f/255.0f,213.0f/255.0f,1.0f))
{
    // For lower-resolution data, change these paths to use Height64/Image64,
    // or Height32/Image32.
    System::InsertDirectory("../../Data/Terrain/Height128");
    System::InsertDirectory("../../Data/Terrain/Image128/");

    // The sky dome model, skyline image, and detail texture are found here.
    System::InsertDirectory("../../Data/Terrain");

    m_fUVBias = 0.0f;
    m_fHeightAboveTerrain = 20.0f;
    m_bAllowSimplification = true;
}
//----------------------------------------------------------------------------
bool ClodTerrains::OnInitialize ()
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

    InitializeCameraMotion(1.0f,0.01f);
    MoveForward();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ClodTerrains::OnTerminate ()
{
    m_spkWireframe = 0;
    m_spkScene = 0;
    m_spkSkyDome = 0;
    m_spkTerrain = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ClodTerrains::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        // The sky dome moves with the camera so that it is always in view.
        Vector3f kTrn = m_spkCamera->GetLocation();
        kTrn.Z() = 0.0f;
        m_spkSkyDome->Local.SetTranslate(kTrn);
        m_spkSkyDome->UpdateGS();
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
bool ClodTerrains::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    case 'a':
    case 'A':
        m_bAllowSimplification = !m_bAllowSimplification;
        return true;
    case 'm':
    case 'M':
        m_spkTerrain->Simplify();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case '+':
    case '=':
        m_spkTerrain->SetPixelTolerance(
            m_spkTerrain->GetPixelTolerance()+1.0f);
        m_spkTerrain->Simplify();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case '-':
    case '_':
        if (m_spkTerrain->GetPixelTolerance() > 1.0f)
        {
            m_spkTerrain->SetPixelTolerance(
                m_spkTerrain->GetPixelTolerance()-1.0f);
            m_spkTerrain->Simplify();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        }
        return true;
    case 'c':
    case 'C':
        m_spkTerrain->CloseAssumption() = !m_spkTerrain->CloseAssumption();
        m_spkTerrain->Simplify();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"ClodTerrains.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ClodTerrains::CreateSkyDome ()
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
void ClodTerrains::CreateTerrain ()
{
    // Create the full terrain.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);  // terrain base texture
    kAttr.SetTCoordChannels(1,2);  // detail texture
    m_spkTerrain = WM4_NEW ClodTerrain("height","image",kAttr,m_pkRenderer,
        m_fUVBias,&m_kFogColor);
    m_spkScene->AttachChild(m_spkTerrain);

    int iRows = m_spkTerrain->GetRowQuantity();
    int iCols = m_spkTerrain->GetColQuantity();
    for (int iRow = 0; iRow < iRows; iRow++)
    {
        for (int iCol = 0; iCol < iCols; iCol++)
        {
            ClodTerrainPage* pkPage = m_spkTerrain->GetPage(iRow,iCol);

            // The default texturing uses a single base texture.  Change this
            // to use a base texture modulated by a detail texture.
            MultitextureEffect* pkEffect = DynamicCast<MultitextureEffect>(
                pkPage->GetEffect(0));
            std::string kBaseName = pkEffect->GetTextureName(0);
            pkEffect->SetTextureQuantity(2);
            pkEffect->SetTextureName(0,kBaseName);
            pkEffect->SetTextureName(1,"Detail");

            Texture* pkTexture = pkEffect->GetPTexture(0,0);
            pkTexture->SetFilterType(Texture::LINEAR_LINEAR);
            pkTexture->SetWrapType(0,Texture::REPEAT);
            pkTexture->SetWrapType(1,Texture::REPEAT);

            AlphaState* pkAState = pkEffect->GetBlending(1);
            pkAState->SrcBlend = AlphaState::SBF_DST_COLOR;
            pkAState->DstBlend = AlphaState::DBF_ZERO;
            pkEffect->Configure();
        }
    }

    // TO DO.  Add fog vertex/pixel shaders.
    //FogState* pkFS = WM4_NEW FogState;
    //pkFS->Enabled = true;
    //pkFS->Density = 0.0015f;
    //pkFS->Color = m_kFogColor;
    //pkFS->DensityFunction = FogState::DF_EXPSQR;
    //m_spkTerrain->SetGlobalState(pkFS);
}
//----------------------------------------------------------------------------
void ClodTerrains::MoveForward ()
{
    WindowApplication3::MoveForward();

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
void ClodTerrains::MoveBackward ()
{
    WindowApplication3::MoveBackward();

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
void ClodTerrains::MoveDown ()
{
    if (m_fHeightAboveTerrain >= m_fTrnSpeed)
    {
        m_fHeightAboveTerrain -= m_fTrnSpeed;
    }

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
void ClodTerrains::MoveUp ()
{
    m_fHeightAboveTerrain += m_fTrnSpeed;

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
void ClodTerrains::TurnLeft ()
{
    WindowApplication3::TurnLeft();
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
void ClodTerrains::TurnRight ()
{
    WindowApplication3::TurnRight();
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
void ClodTerrains::LookUp ()
{
    WindowApplication3::LookUp();
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
void ClodTerrains::LookDown ()
{
    WindowApplication3::LookDown();
    if (m_bAllowSimplification)
    {
        m_spkTerrain->Simplify();
    }
}
//----------------------------------------------------------------------------
