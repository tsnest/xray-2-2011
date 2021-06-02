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

#include "GelatinBlob.h"
#include "Wm4EdgeKey.h"
#include <set>

WM4_WINDOW_APPLICATION(GelatinBlob);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
GelatinBlob::GelatinBlob ()
    :
    WindowApplication3("GelatinBlob",0,0,640,480,
        ColorRGBA(0.713725f,0.807843f,0.929411f,1.0f))
{
    m_pkModule = 0;
}
//----------------------------------------------------------------------------
bool GelatinBlob::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // center-and-fit for camera viewing
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    // camera turret and tumble mode
    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void GelatinBlob::OnTerminate ()
{
    WM4_DELETE m_pkModule;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkSegments = 0;
    m_spkWireframe = 0;
    m_spkIcosahedron = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GelatinBlob::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        m_spkScene->UpdateGS(0.0f);
    }

#ifndef SINGLE_STEP
    DoPhysical();
#endif
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
bool GelatinBlob::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        DoPhysical();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateSphere ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    m_spkIcosahedron = StandardMesh(kAttr).Icosahedron();

    // texture for the water objects
    TextureEffect* pkEffect = WM4_NEW TextureEffect("WaterA");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    m_spkIcosahedron->AttachEffect(pkEffect);

    // the texture has an alpha channel of 1/2
    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    m_spkIcosahedron->AttachGlobalState(pkAS);
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateSprings ()
{
    // The icosahedron has 12 vertices and 30 edges.  Each vertex is a
    // particle in the system.  Each edge represents a spring.  To keep the
    // icosahedron from collapsing, 12 immovable particles are added, each
    // outside the icosahedron in the normal direction above a vertex.  The
    // immovable particles are connected to their corresponding vertices with
    // springs.
    int iNumParticles = 24, iNumSprings = 42;

    // Viscous forces applied.  If you set viscosity to zero, the cuboid
    // wiggles indefinitely since there is no dissipation of energy.  If
    // the viscosity is set to a positive value, the oscillations eventually
    // stop.  The length of time to steady state is inversely proportional
    // to the viscosity.
#ifdef _DEBUG
    float fStep = 0.1f;
#else
    float fStep = 0.01f;  // simulation needs to run slower in release mode
#endif
    float fViscosity = 0.01f;
    m_pkModule = WM4_NEW PhysicsModule(iNumParticles,iNumSprings,fStep,
        fViscosity);

    // Set positions and velocities.  The first 12 positions are the vertices
    // of the icosahedron.  The last 12 are the extra particles added to
    // stabilize the system.
    int i;
    for (i = 0; i < 12; i++)
    {
        m_pkModule->SetMass(i,1.0f);
        m_pkModule->Position(i) = m_spkIcosahedron->VBuffer->Position3(i);
        m_pkModule->Velocity(i) = 0.1f*Vector3f(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(),Mathf::SymmetricRandom());
    }
    for (i = 12; i < 24; i++)
    {
        m_pkModule->SetMass(i,Mathf::MAX_REAL);
        m_pkModule->Position(i) =
            2.0f*m_spkIcosahedron->VBuffer->Position3(i-12);
        m_pkModule->Velocity(i) = Vector3f::ZERO;
    }

    // get unique set of edges for icosahedron
    std::set<EdgeKey> kEdge;
    int iTQuantity = m_spkIcosahedron->IBuffer->GetIndexQuantity()/3;
    const int* piIndex = m_spkIcosahedron->IBuffer->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        kEdge.insert(EdgeKey(iV0,iV1));
        kEdge.insert(EdgeKey(iV1,iV2));
        kEdge.insert(EdgeKey(iV2,iV0));
    }

    // springs are at rest in the initial configuration
    const float fConstant = 10.0f;
    Vector3f kDiff;
    int iSpring = 0;
    std::set<EdgeKey>::iterator pkIter;
    for (pkIter = kEdge.begin(); pkIter != kEdge.end(); pkIter++)
    {
        const EdgeKey& rkKey = *pkIter;
        int iV0 = rkKey.V[0], iV1 = rkKey.V[1];
        kDiff = m_pkModule->Position(iV1) - m_pkModule->Position(iV0);
        m_pkModule->SetSpring(iSpring,iV0,iV1,fConstant,kDiff.Length());
        iSpring++;
    }
    for (i = 0; i < 12; i++)
    {
        kDiff = m_pkModule->Position(i+12) - m_pkModule->Position(i);
        m_pkModule->SetSpring(iSpring,i,i+12,fConstant,kDiff.Length());
        iSpring++;
    }
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateSegments ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexColor3Effect* pkEffect = WM4_NEW VertexColor3Effect;

    int iNumSprings = m_pkModule->GetNumSprings();
    m_spkSegments = WM4_NEW Node;
    for (int i = 0; i < iNumSprings; i++)
    {
        int iV0, iV1;
        float fConstant, fLength;
        m_pkModule->GetSpring(i,iV0,iV1,fConstant,fLength);

        VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,2);
        pkVBuffer->Position3(0) = m_pkModule->Position(iV0);
        pkVBuffer->Position3(1) = m_pkModule->Position(iV1);
        pkVBuffer->Color3(0,0) = ColorRGB::WHITE;
        pkVBuffer->Color3(0,1) = ColorRGB::WHITE;
        Polyline* pkPoly = WM4_NEW Polyline(pkVBuffer,false,true);
        pkPoly->AttachEffect(pkEffect);

        m_spkSegments->AttachChild(pkPoly);
    }
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    CreateSphere();
    CreateSprings();
    CreateSegments();

    // segments are opaque--draw first (sphere is transparent)
    m_spkTrnNode->AttachChild(m_spkSegments);
    m_spkTrnNode->AttachChild(m_spkIcosahedron);
}
//----------------------------------------------------------------------------
void GelatinBlob::DoPhysical ()
{
    m_pkModule->Update((float)System::GetTime());

    // Update sphere surface.  The particle system and sphere maintain their
    // own copy of the vertices, so this update is necessary.
    int i;
    for (i = 0; i < 12; i++)
    {
        m_spkIcosahedron->VBuffer->Position3(i) = m_pkModule->Position(i);
    }

    m_spkIcosahedron->UpdateMS(false);
    m_spkIcosahedron->VBuffer->Release();

    // update the segments representing the springs
    int iNumSprings = m_pkModule->GetNumSprings();
    for (i = 0; i < iNumSprings; i++)
    {
        int iV0, iV1;
        float fConstant, fLength;
        m_pkModule->GetSpring(i,iV0,iV1,fConstant,fLength);

        Polyline* pkPoly = StaticCast<Polyline>(m_spkSegments->GetChild(i));
        pkPoly->VBuffer->Position3(0) = m_pkModule->Position(iV0);
        pkPoly->VBuffer->Position3(1) = m_pkModule->Position(iV1);
        pkPoly->VBuffer->Release();
    }
}
//----------------------------------------------------------------------------
