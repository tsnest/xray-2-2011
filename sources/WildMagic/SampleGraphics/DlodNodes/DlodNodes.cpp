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

#include "DlodNodes.h"

WM4_WINDOW_APPLICATION(DlodNodes);

//----------------------------------------------------------------------------
DlodNodes::DlodNodes ()
    :
    WindowApplication3("DlodNodes",0,0,640,480,ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool DlodNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLoc(0.0f,-4.0f,0.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
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
void DlodNodes::OnTerminate ()
{
    m_spkScene = 0;
    m_spkDlod = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void DlodNodes::OnIdle ()
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
bool DlodNodes::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"DlodNodes.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void DlodNodes::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    m_spkDlod = WM4_NEW DlodNode;
    m_spkScene->AttachChild(m_spkDlod);

    // A lighting effect to be shared by all objects.
    MaterialState* pkMS = WM4_NEW MaterialState;
    pkMS->Diffuse = ColorRGB(0.5f,0.0f,0.5f);
    m_spkScene->AttachGlobalState(pkMS);

    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    pkLight->Diffuse = ColorRGB::WHITE;
    pkLight->Specular = ColorRGB::BLACK;
    pkLight->DVector = Vector3f::UNIT_Y;
    m_spkScene->AttachLight(pkLight);

    // mesh creator
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    StandardMesh kSM(kAttr);

    // create a sphere mesh (child 0)
    m_spkDlod->AttachChild(kSM.Sphere(32,16,1.0f));

    // create an icosahedron (child 1)
    m_spkDlod->AttachChild(kSM.Icosahedron());

    // create a dodecahedron (child 2)
    m_spkDlod->AttachChild(kSM.Dodecahedron());

    // create an octahedron (child 3)
    m_spkDlod->AttachChild(kSM.Octahedron());

    // create a hexahedron (child 4)
    m_spkDlod->AttachChild(kSM.Hexahedron());

    // create a tetrahedron (child 5)
    m_spkDlod->AttachChild(kSM.Tetrahedron());

    // set distance intervals
    m_spkDlod->SetModelDistance(0,0.0f,10.0f);
    m_spkDlod->SetModelDistance(1,10.0f,20.0f);
    m_spkDlod->SetModelDistance(2,20.0f,30.0f);
    m_spkDlod->SetModelDistance(3,30.0f,40.0f);
    m_spkDlod->SetModelDistance(4,40.0f,50.0f);
    m_spkDlod->SetModelDistance(5,50.0f,100.0f);

    // set model LOD center
    m_spkDlod->ModelCenter() = Vector3f::ZERO;
}
//----------------------------------------------------------------------------
