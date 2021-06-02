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
// Version: 4.7.0 (2008/09/15)

#include "ClipMesh.h"
#include "PartitionMesh.h"

WM4_WINDOW_APPLICATION(ClipMesh);

//----------------------------------------------------------------------------
ClipMesh::ClipMesh ()
    :
    WindowApplication3("ClipMesh",0,0,640,480,
        ColorRGBA(0.75f,0.75f,0.75f,1.0f))
{
}
//----------------------------------------------------------------------------
bool ClipMesh::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(60.0f,4.0f/3.0f,0.1f,1000.0f);
    Vector3f kCLoc(16.0f,0.0f,4.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkTorus);
    return true;
}
//----------------------------------------------------------------------------
void ClipMesh::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTorus = 0;
    m_spkMeshPlane = 0;
    m_spkWireframe = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ClipMesh::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    if (MoveObject())
    {
        Update();
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
bool ClipMesh::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void ClipMesh::CreateScene ()
{
    m_spkScene = WM4_NEW Node;

    m_kAttr.SetPositionChannels(3);
    m_kAttr.SetColorChannels(0,3);
    m_spkEffect = WM4_NEW VertexColor3Effect;

    // The plane is fixed at z = 0.
    m_kPlane.Normal = Vector3f::UNIT_Z;
    m_kPlane.Constant = 0.0f;
    m_spkMeshPlane = StandardMesh(m_kAttr).Rectangle(32,32,16.0f,16.0f);
    int iVQuantity = m_spkMeshPlane->VBuffer->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        m_spkMeshPlane->VBuffer->Color3(0,i) = ColorRGB(0.0f,1.0f,0.0f);
    }
    WireframeState* pkWS = WM4_NEW WireframeState;
    pkWS->Enabled = true;
    m_spkMeshPlane->AttachGlobalState(pkWS);
    m_spkMeshPlane->AttachEffect(m_spkEffect);
    m_spkScene->AttachChild(m_spkMeshPlane);

    // Get the positions and indices for a torus.
    m_spkTorus = StandardMesh(m_kAttr).Torus(64,64,4.0f,1.0f);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkTorus->AttachGlobalState(m_spkWireframe);
    m_spkTorus->AttachEffect(m_spkEffect);
    m_spkScene->AttachChild(m_spkTorus);
    iVQuantity = m_spkTorus->VBuffer->GetVertexQuantity();
    m_kTorusVerticesMS.resize(iVQuantity);
    m_kTorusVerticesWS.resize(iVQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        m_kTorusVerticesMS[i] = m_spkTorus->VBuffer->Position3(i);
        m_kTorusVerticesWS[i] = m_kTorusVerticesMS[i];
        m_spkTorus->VBuffer->Color3(0,i) = ColorRGB::BLACK;
    }
    int iIQuantity = m_spkTorus->IBuffer->GetIndexQuantity();
    m_kTorusIndices.resize(iIQuantity);
    memcpy(&m_kTorusIndices[0],m_spkTorus->IBuffer->GetData(),
        iIQuantity*sizeof(int));

    Update();
}
//----------------------------------------------------------------------------
void ClipMesh::Update ()
{
    // Transform the model-space vertices to world space.
    int iVQuantity = (int)m_kTorusVerticesMS.size();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        m_kTorusVerticesWS[i] = m_spkTorus->Local.ApplyForward(
            m_kTorusVerticesMS[i]);
    }

    // Partition the torus mesh.
    std::vector<Vector3f> kClipVertices;
    std::vector<int> kNegIndices, kPosIndices;
    PartitionMesh(m_kTorusVerticesWS,m_kTorusIndices,m_kPlane,kClipVertices,
        kNegIndices,kPosIndices);

    // Replace the torus vertex buffer.
    iVQuantity = (int)kClipVertices.size();
    m_spkTorus->VBuffer = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        // Transform the world-space vertex to model space.
        m_spkTorus->VBuffer->Position3(i) =
            m_spkTorus->Local.ApplyInverse(kClipVertices[i]);

        m_spkTorus->VBuffer->Color3(0,i) = ColorRGB::BLACK;
    }

    // Modify the vertex color based on which mesh the vertices lie.
    int iIQuantity = (int)kNegIndices.size();
    for (i = 0; i < iIQuantity; i++)
    {
        m_spkTorus->VBuffer->Color3(0,kNegIndices[i]).B() = 1.0f;
    }
    iIQuantity = (int)kPosIndices.size();
    for (i = 0; i < iIQuantity; i++)
    {
        m_spkTorus->VBuffer->Color3(0,kPosIndices[i]).R() = 1.0f;
    }
}
//----------------------------------------------------------------------------
