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

#include "Delaunay3D.h"

WM4_WINDOW_APPLICATION(Delaunay3D);

//----------------------------------------------------------------------------
Delaunay3D::Delaunay3D ()
    :
    WindowApplication3("Delaunay3D",0,0,640,480,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_pkDel = 0;
}
//----------------------------------------------------------------------------
bool Delaunay3D::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,10000.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc(0.0f,0.0f,-4.0f);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.1f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Delaunay3D::OnTerminate ()
{
    m_spkScene = 0;
    WM4_DELETE m_pkDel;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Delaunay3D::OnIdle ()
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
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Delaunay3D::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 's':
    case 'S':
        DoSearch();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Delaunay3D::CreateScene ()
{
    int i, iVQuantity;
    Vector3f* akVertex;
#if 0
    // test a cube
    iVQuantity = 8;
    akVertex = WM4_NEW Vector3f[iVQuantity];
    akVertex[0] = Vector3f(0.0f,0.0f,0.0f);
    akVertex[1] = Vector3f(1.0f,0.0f,0.0f);
    akVertex[2] = Vector3f(1.0f,1.0f,0.0f);
    akVertex[3] = Vector3f(0.0f,1.0f,0.0f);
    akVertex[4] = Vector3f(0.0f,0.0f,1.0f);
    akVertex[5] = Vector3f(1.0f,0.0f,1.0f);
    akVertex[6] = Vector3f(1.0f,1.0f,1.0f);
    akVertex[7] = Vector3f(0.0f,1.0f,1.0f);
#endif
#if 1
    // randomly generated points
    iVQuantity = 128;
    akVertex = WM4_NEW Vector3f[iVQuantity];
    akVertex[0] = Vector3f::ZERO;
    for (i = 1; i < iVQuantity; i++)
    {
        akVertex[i].X() = Mathf::SymmetricRandom();
        akVertex[i].Y() = Mathf::SymmetricRandom();
        akVertex[i].Z() = Mathf::SymmetricRandom();
    }
#endif

    m_kMin = akVertex[0];
    m_kMax = akVertex[0];
    for (i = 1; i < iVQuantity; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float fValue = akVertex[i][j];
            if (fValue < m_kMin[j])
            {
                m_kMin[j] = fValue;
            }
            else if (fValue > m_kMax[j])
            {
                m_kMax[j] = fValue;
            }
        }
    }

    m_pkDel = WM4_NEW Delaunay3f(iVQuantity,akVertex,0.001f,true,
        Query::QT_INT64);

    m_spkScene = WM4_NEW Node;
    CullState* pkCS = WM4_NEW CullState;
    pkCS->Enabled = false;
    m_spkScene->AttachGlobalState(pkCS);

    m_spkScene->AttachChild(CreateSphere());
    for (int j = 0; j < m_pkDel->GetSimplexQuantity(); j++)
    {
        m_spkScene->AttachChild(CreateTetra(j));
    }
}
//----------------------------------------------------------------------------
TriMesh* Delaunay3D::CreateSphere () const
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    TriMesh* pkMesh = StandardMesh(kAttr).Sphere(8,8,0.01f);
    pkMesh->AttachEffect(WM4_NEW VertexColor3Effect);
    pkMesh->Culling = Spatial::CULL_ALWAYS;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* Delaunay3D::CreateTetra (int iIndex) const
{
    const Vector3f* akTVertex = m_pkDel->GetVertices();
    const int* aiTIndex = m_pkDel->GetIndices();

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,4);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = akTVertex[aiTIndex[4*iIndex  ]];
    pkVBuffer->Position3(1) = akTVertex[aiTIndex[4*iIndex+1]];
    pkVBuffer->Position3(2) = akTVertex[aiTIndex[4*iIndex+2]];
    pkVBuffer->Position3(3) = akTVertex[aiTIndex[4*iIndex+3]];
    pkVBuffer->Color4(0,0) = ColorRGBA(0.75f,0.75f,0.75f,1.0f);
    pkVBuffer->Color4(0,1) = ColorRGBA(0.75f,0.75f,0.75f,1.0f);
    pkVBuffer->Color4(0,2) = ColorRGBA(0.75f,0.75f,0.75f,1.0f);
    pkVBuffer->Color4(0,3) = ColorRGBA(0.75f,0.75f,0.75f,1.0f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(12);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] = 0;  aiIndex[ 1] = 1;  aiIndex[ 2] = 2;
    aiIndex[ 3] = 0;  aiIndex[ 4] = 3;  aiIndex[ 5] = 1;
    aiIndex[ 6] = 0;  aiIndex[ 7] = 2;  aiIndex[ 8] = 3;
    aiIndex[ 9] = 3;  aiIndex[10] = 2;  aiIndex[11] = 1;

    TriMesh* pkTetra = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    // wireframe
    WireframeState* pkWS = WM4_NEW WireframeState;
    pkWS->Enabled = true;
    pkTetra->AttachGlobalState(pkWS);

    // alpha blending for when the tetra is solid
    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    pkTetra->AttachGlobalState(pkAS);

    pkTetra->AttachEffect(WM4_NEW VertexColor4Effect);

    return pkTetra;
}
//----------------------------------------------------------------------------
void Delaunay3D::ChangeTetraStatus (int iIndex, const ColorRGBA& rkColor,
    bool bWireframe)
{
    Geometry* pkTetra = DynamicCast<Geometry>(m_spkScene->GetChild(1+iIndex));
    assert(pkTetra);
    for (int i = 0; i < 4; i++)
    {
        pkTetra->VBuffer->Color4(0,i) = rkColor;
    }
    pkTetra->VBuffer->Release();

    WireframeState* pkWS =
        (WireframeState*)pkTetra->GetGlobalState(GlobalState::WIREFRAME);
    assert(pkWS);
    pkWS->Enabled = bWireframe;
}
//----------------------------------------------------------------------------
void Delaunay3D::ChangeLastTetraStatus (int iIndex, int iVOpposite,
    const ColorRGBA& rkColor, const ColorRGBA& rkOppositeColor)
{
    Geometry* pkTetra = DynamicCast<Geometry>(m_spkScene->GetChild(1+iIndex));
    assert(pkTetra);
    for (int i = 0; i < 4; i++)
    {
        if (i != iVOpposite)
        {
            pkTetra->VBuffer->Color4(0,i) = rkColor;
        }
        else
        {
            pkTetra->VBuffer->Color4(0,i) = rkOppositeColor;
        }
    }
    pkTetra->VBuffer->Release();

    WireframeState* pkWS =
        (WireframeState*)pkTetra->GetGlobalState(GlobalState::WIREFRAME);
    assert(pkWS);
    pkWS->Enabled = false;
}
//----------------------------------------------------------------------------
void Delaunay3D::DoSearch ()
{
    // make all tetra wireframe
    int i;
    for (i = 0; i < m_pkDel->GetSimplexQuantity(); i++)
    {
        ChangeTetraStatus(i,ColorRGBA(0.75f,0.75f,0.75f,1.0f),true);
    }

    // generate random point in AABB of data set
    Vector3f kRandom;
    kRandom.X() = Mathf::IntervalRandom(m_kMin.X(),m_kMax.X());
    kRandom.Y() = Mathf::IntervalRandom(m_kMin.Y(),m_kMax.Y());
    kRandom.Z() = Mathf::IntervalRandom(m_kMin.Z(),m_kMax.Z());

    // move sphere to this location
    Spatial* pkSphere = m_spkScene->GetChild(0);
    pkSphere->Culling = Spatial::CULL_DYNAMIC;
    pkSphere->Local.SetTranslate(kRandom);
    pkSphere->UpdateGS();

    if (m_pkDel->GetContainingTetrahedron(kRandom) >= 0)
    {
        // make all tetra on the path solid
        for (i = 0; i <= m_pkDel->GetPathLast(); i++)
        {
            int iIndex = m_pkDel->GetPath()[i];
            float fRed, fBlue;
            if (m_pkDel->GetPathLast() > 0)
            {
                fRed = i/(float)m_pkDel->GetPathLast();
                fBlue = 1.0f - fRed;
            }
            else
            {
                fRed = 1.0f;
                fBlue = 0.0f;
            }
            ChangeTetraStatus(iIndex,ColorRGBA(fRed,0.0f,fBlue,0.5f),false);
        }
    }
    else
    {
        // The point is outside the convex hull.  Change the wireframe
        // color for the last visited face in the search path.
        int iIndex = m_pkDel->GetPath()[m_pkDel->GetPathLast()];
        int iV0, iV1, iV2, iV3;
        int iVOpposite = m_pkDel->GetLastFace(iV0,iV1,iV2,iV3);
        ChangeLastTetraStatus(iIndex,iVOpposite,
            ColorRGBA(0.0f,1.0f,0.0f,0.5f),ColorRGBA(0.0f,0.25f,0.0f,0.5f));
    }

    m_kCuller.ComputeVisibleSet(m_spkScene);
}
//----------------------------------------------------------------------------
