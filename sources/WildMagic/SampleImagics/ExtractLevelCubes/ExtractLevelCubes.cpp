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
// Version: 4.0.2 (2007/06/16)

#include "ExtractLevelCubes.h"
#include "Wm4Imagics.h"

WM4_WINDOW_APPLICATION(ExtractLevelCubes);

//----------------------------------------------------------------------------
ExtractLevelCubes::ExtractLevelCubes ()
    :
    WindowApplication3("ExtractLevelCubes",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool ExtractLevelCubes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    if (!CreateModel())
    {
        return true;
    }

    // create turret
    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkTrnNode->AttachChild(m_spkModel);
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());

    // center-and-fit camera parameters
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(5.0f,0.1f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ExtractLevelCubes::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkModel = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ExtractLevelCubes::OnIdle ()
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
bool ExtractLevelCubes::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
bool ExtractLevelCubes::CreateModel ()
{
    // extract a level surface (image is 97x97x116)
    ImageInt3D kImage(System::GetPath("../../Data/Im/","Molecule.im"));
    if (kImage.GetDimensions() == 0)
    {
        return false;
    }

    ExtractSurfaceCubes kESC(kImage.GetBound(0),kImage.GetBound(1),
        kImage.GetBound(2),(int*)kImage.GetData());

    std::vector<Vector3f> kVA, kNA;
    std::vector<TriangleKey> kTA;

    kESC.ExtractContour(64.5f,kVA,kTA);
    kESC.MakeUnique(kVA,kTA);
    kESC.OrientTriangles(kVA,kTA,false);
    kESC.ComputeNormals(kVA,kTA,kNA);

    // create a triangle mesh for the surface
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    int iVQuantity = (int)kVA.size();
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i) = kVA[i];
        pkVBuffer->Normal3(i) = kNA[i];
    }

    int iTQuantity = (int)kTA.size(), iIQuantity = 3*iTQuantity;
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* piIndex = pkIBuffer->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        *piIndex++ = kTA[i].V[0];
        *piIndex++ = kTA[i].V[1];
        *piIndex++ = kTA[i].V[2];
    }

    m_spkModel = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    // wireframe
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkModel->AttachGlobalState(m_spkWireframe);

    // material
    MaterialState* pkMS = WM4_NEW MaterialState;
    pkMS->Emissive = ColorRGB(0.0f,0.0f,0.0f);
    pkMS->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    pkMS->Diffuse = ColorRGB(0.99607f,0.83920f,0.67059f);
    pkMS->Specular = ColorRGB(0.8f,0.8f,0.8f);
    pkMS->Shininess = 4.0f;
    pkMS->Alpha = 1.0f;
    m_spkModel->AttachGlobalState(pkMS);

    // lights
    ColorRGB kAmbient(0.25f,0.25f,0.25f);
    ColorRGB kDiffuse(0.5f,0.5f,0.5f);
    ColorRGB kSpecular(0.1f,0.1f,0.1f);

    Light* pkLight = WM4_NEW Light(Light::LT_AMBIENT);
    pkLight->Ambient = kAmbient;
    pkLight->Diffuse = kDiffuse;
    pkLight->Specular = kSpecular;
    m_spkModel->AttachLight(pkLight);

    pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = kAmbient;
    pkLight->Diffuse = kDiffuse;
    pkLight->Specular = kSpecular;
    pkLight->SetDirection(Vector3f::UNIT_Z);
    m_spkModel->AttachLight(pkLight);

    return true;
}
//----------------------------------------------------------------------------
