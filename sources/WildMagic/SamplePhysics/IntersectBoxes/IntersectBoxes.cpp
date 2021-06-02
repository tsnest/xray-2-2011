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

#include "IntersectBoxes.h"

WM4_WINDOW_APPLICATION(IntersectBoxes);

float IntersectBoxes::ms_fSize = 256.0f;

// #define SINGLE_STEP

//----------------------------------------------------------------------------
IntersectBoxes::IntersectBoxes ()
    :
    WindowApplication3("IntersectingBoxes",0,0,640,480,
        ColorRGBA(0.75f,0.75f,0.75f,1.0f))
{
    m_pkIB = 0;
    m_bDoSimulation = true;
    m_fLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool IntersectBoxes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // create some axis-aligned boxes for intersection testing
    const int iMax = 16;
    int i;
    for (i = 0; i < iMax; i++)
    {
        float fXMin = 0.5f*ms_fSize*Mathf::SymmetricRandom();
        float fXMax = fXMin + Mathf::IntervalRandom(8.0f,32.0f);
        float fYMin = 0.5f*ms_fSize*Mathf::SymmetricRandom();
        float fYMax = fYMin + Mathf::IntervalRandom(8.0f,32.0f);
        float fZMin = 0.5f*ms_fSize*Mathf::SymmetricRandom();
        float fZMax = fZMin + Mathf::IntervalRandom(8.0f,32.0f);
        m_kBoxes.push_back(
            AxisAlignedBox3f(fXMin,fXMax,fYMin,fYMax,fZMin,fZMax));
    }
    m_pkIB = WM4_NEW IntersectingBoxesf(m_kBoxes);

    // scene graph for the visual representation of the boxes
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // materials for boxes, blue for nonintersecting and red for intersecting
    m_spkBlue = WM4_NEW MaterialState;
    m_spkBlue->Emissive = ColorRGB::BLACK;
    m_spkBlue->Ambient = ColorRGB(0.25f,0.25f,0.25f);
    m_spkBlue->Diffuse = ColorRGB(0.0f,0.0f,0.5f);
    m_spkBlue->Specular = ColorRGB::BLACK;
    m_spkBlue->Shininess = 1.0f;
    m_spkBlue->Alpha = 1.0f;

    m_spkRed = WM4_NEW MaterialState;
    m_spkRed->Emissive = ColorRGB::BLACK;
    m_spkRed->Ambient = ColorRGB(0.25f,0.25f,0.25f);
    m_spkRed->Diffuse = ColorRGB(0.5f,0.0f,0.0f);
    m_spkRed->Specular = ColorRGB::BLACK;
    m_spkRed->Shininess = 1.0f;
    m_spkRed->Alpha = 1.0f;

    // a light for the scene
    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB::WHITE;
    pkLight->Diffuse = ColorRGB::WHITE;
    pkLight->Specular = ColorRGB::BLACK;
    pkLight->SetDirection(Vector3f::UNIT_Z);
    m_spkScene->AttachLight(pkLight);

    // create visual representations of the boxes
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    for (i = 0; i < iMax; i++)
    {
        Vector3f kCenter(
            0.5f*(m_kBoxes[i].Min[0]+m_kBoxes[i].Max[0]),
            0.5f*(m_kBoxes[i].Min[1]+m_kBoxes[i].Max[1]),
            0.5f*(m_kBoxes[i].Min[2]+m_kBoxes[i].Max[2]));

        Transformation kXFrm;
        kXFrm.SetTranslate(kCenter);

        float fXExtent = 0.5f*(m_kBoxes[i].Max[0]-m_kBoxes[i].Min[0]);
        float fYExtent = 0.5f*(m_kBoxes[i].Max[1]-m_kBoxes[i].Min[1]);
        float fZExtent = 0.5f*(m_kBoxes[i].Max[2]-m_kBoxes[i].Min[2]);

        StandardMesh kSM(kAttr,false,&kXFrm);
        TriMesh* pkMesh = kSM.Box(fXExtent,fYExtent,fZExtent);

        pkMesh->AttachGlobalState(m_spkBlue);
        m_spkScene->AttachChild(pkMesh);
    }

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,0.0f,-ms_fSize);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.5f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void IntersectBoxes::OnTerminate ()
{
    WM4_DELETE m_pkIB;
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkBlue = 0;
    m_spkRed = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectBoxes::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS(0.0f);
    }

#ifndef SINGLE_STEP
    if (m_bDoSimulation)
    {
        float fCurrIdle = (float)System::GetTime();
        float fDiff = fCurrIdle - m_fLastIdle;
        if (fDiff >= 1.0f/30.0f)
        {
            ModifyBoxes();
            m_fLastIdle = fCurrIdle;
        }
    }
#endif
    m_kCuller.ComputeVisibleSet(m_spkScene);

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool IntersectBoxes::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        m_bDoSimulation = !m_bDoSimulation;
        return true;
#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        ModifyBoxes();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void IntersectBoxes::ModifyBoxes ()
{
    int i;
    for (i = 0; i < (int)m_kBoxes.size(); i++)
    {
        AxisAlignedBox3f kBox = m_kBoxes[i];

        float fDX = Mathf::IntervalRandom(-4.0f,4.0f);
        if (kBox.Min[0]+fDX >= -ms_fSize && kBox.Max[0]+fDX <= ms_fSize)
        {
            kBox.Min[0] += fDX;
            kBox.Max[0] += fDX;
        }

        float fDY = Mathf::IntervalRandom(-4.0f,4.0f);
        if (kBox.Min[1]+fDY >= -ms_fSize && kBox.Max[1]+fDY <= ms_fSize)
        {
            kBox.Min[1] += fDY;
            kBox.Max[1] += fDY;
        }

        float fDZ = Mathf::IntervalRandom(-4.0f,4.0f);
        if (kBox.Min[2]+fDZ >= -ms_fSize && kBox.Max[2]+fDZ <= ms_fSize)
        {
            kBox.Min[2] += fDZ;
            kBox.Max[2] += fDZ;
        }

        m_pkIB->SetBox(i,kBox);
        ModifyMesh(i);
    }

    m_pkIB->Update();
    m_spkScene->UpdateGS(0.0f);

    // switch material to red for any box that overlaps another
    TriMesh* pkMesh;
    for (i = 0; i < (int)m_kBoxes.size(); i++)
    {
        // reset all boxes to blue
        pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));
        pkMesh->AttachGlobalState(m_spkBlue);
    }

    const std::set<EdgeKey>& rkOverlap = m_pkIB->GetOverlap();
    std::set<EdgeKey>::const_iterator pkIter = rkOverlap.begin();
    for (/**/; pkIter != rkOverlap.end(); pkIter++)
    {
        // set intersecting boxes to red
        i = pkIter->V[0];
        pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));
        pkMesh->AttachGlobalState(m_spkRed);
        i = pkIter->V[1];
        pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));
        pkMesh->AttachGlobalState(m_spkRed);
    }

    m_spkScene->UpdateRS();
}
//----------------------------------------------------------------------------
void IntersectBoxes::ModifyMesh (int i)
{
    Vector3f kCenter(
        0.5f*(m_kBoxes[i].Min[0]+m_kBoxes[i].Max[0]),
        0.5f*(m_kBoxes[i].Min[1]+m_kBoxes[i].Max[1]),
        0.5f*(m_kBoxes[i].Min[2]+m_kBoxes[i].Max[2]));

    float fXExtent = 0.5f*(m_kBoxes[i].Max[0]-m_kBoxes[i].Min[0]);
    float fYExtent = 0.5f*(m_kBoxes[i].Max[1]-m_kBoxes[i].Min[1]);
    float fZExtent = 0.5f*(m_kBoxes[i].Max[2]-m_kBoxes[i].Min[2]);

    Vector3f kXTerm = fXExtent*Vector3f::UNIT_X;
    Vector3f kYTerm = fYExtent*Vector3f::UNIT_Y;
    Vector3f kZTerm = fZExtent*Vector3f::UNIT_Z;

    TriMesh* pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));

    pkMesh->VBuffer->Position3(0) = kCenter - kXTerm - kYTerm - kZTerm;
    pkMesh->VBuffer->Position3(1) = kCenter + kXTerm - kYTerm - kZTerm;
    pkMesh->VBuffer->Position3(2) = kCenter + kXTerm + kYTerm - kZTerm;
    pkMesh->VBuffer->Position3(3) = kCenter - kXTerm + kYTerm - kZTerm;
    pkMesh->VBuffer->Position3(4) = kCenter - kXTerm - kYTerm + kZTerm;
    pkMesh->VBuffer->Position3(5) = kCenter + kXTerm - kYTerm + kZTerm;
    pkMesh->VBuffer->Position3(6) = kCenter + kXTerm + kYTerm + kZTerm;
    pkMesh->VBuffer->Position3(7) = kCenter - kXTerm + kYTerm + kZTerm;

    pkMesh->UpdateMS(true);
    pkMesh->VBuffer->Release();
}
//----------------------------------------------------------------------------
