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

#include "SwitchNodes.h"

WM4_WINDOW_APPLICATION(SwitchNodes);

//----------------------------------------------------------------------------
SwitchNodes::SwitchNodes ()
    :
    WindowApplication3("SwitchNodes",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool SwitchNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // set camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fCos = 0.866025f, fSin = 0.5f;
    Vector3f kCLoc(0.0f,-4.0f,2.0f);
    Vector3f kCDir(0.0f,fCos,-fSin);
    Vector3f kCUp(0.0f,fSin,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

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
void SwitchNodes::OnTerminate ()
{
    m_spkScene = 0;
    m_spkSwitch = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SwitchNodes::OnIdle ()
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
bool SwitchNodes::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'c':
    case 'C':
    {
        int iChild = m_spkSwitch->GetActiveChild();
        if (++iChild == m_spkSwitch->GetQuantity())
        {
            iChild = 0;
        }
        m_spkSwitch->SetActiveChild(iChild);
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    }
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"SwitchNodes.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SwitchNodes::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    m_spkSwitch = WM4_NEW SwitchNode;
    m_spkScene->AttachChild(m_spkSwitch);

    // Texture effect to be shared by all objects.
    TextureEffect* pkEffect = WM4_NEW TextureEffect("Flower");

    // mesh creator
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    TriMesh* pkMesh;

    // create a rectangle mesh (child 0)
    pkMesh = kSM.Rectangle(4,4,1.0f,1.0f);
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a disk mesh (child 1)
    pkMesh = kSM.Disk(8,16,1.0f);
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a box mesh (child 2)
    pkMesh = kSM.Box(1.0f,0.5f,0.25f);
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a closed cylinder mesh (child 3)
    pkMesh = kSM.Cylinder(8,16,1.0f,2.0f,false);
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a sphere mesh (child 4)
    pkMesh = kSM.Sphere(32,16,1.0f);
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a torus mesh (child 5)
    pkMesh = kSM.Torus(16,16,1.0f,0.25f);
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a tetrahedron (child 6)
    pkMesh = kSM.Tetrahedron();
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a hexahedron (child 7)
    pkMesh = kSM.Hexahedron();
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create an octahedron (child 8)
    pkMesh = kSM.Octahedron();
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create a dodecahedron (child 9)
    pkMesh = kSM.Dodecahedron();
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // create an icosahedron (child 10)
    pkMesh = kSM.Icosahedron();
    pkMesh->AttachEffect(pkEffect);
    m_spkSwitch->AttachChild(pkMesh);

    // set the active child (otherwise it is invalid)
    m_spkSwitch->SetActiveChild(0);
}
//----------------------------------------------------------------------------
