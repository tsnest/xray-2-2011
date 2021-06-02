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
// Version: 4.6.0 (2007/08/20)

#include "PointInPolyhedron.h"

WM4_WINDOW_APPLICATION(PointInPolyhedron);

#define TRIANGLE_FACES
//#define CONVEX_FACES_METHOD_0
//#define CONVEX_FACES_METHOD_1
//#define CONVEX_FACES_METHOD_2
//#define SIMPLE_FACES_METHOD_0
//#define SIMPLE_FACES_METHOD_1

//----------------------------------------------------------------------------
PointInPolyhedron::PointInPolyhedron ()
    :
    WindowApplication3("PointInPolyhedron",0,0,1280,960,
        ColorRGBA(0.75f,0.75f,0.75f,1.0f))
{
    m_pkQuery = 0;
    m_akTFace = 0;
    m_akCFace = 0;
    m_akSFace = 0;

    m_iRayQuantity = 5;
    m_akRayDirection = WM4_NEW Vector3f[m_iRayQuantity];
    for (int i = 0; i < m_iRayQuantity; i++)
    {
        double adPoint[3];
        RandomPointOnHypersphere(3,adPoint);
        for (int j = 0; j < 3; j++)
        {
            m_akRayDirection[i][j] = (float)adPoint[j];
        }
    }
}
//----------------------------------------------------------------------------
bool PointInPolyhedron::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(60.0f,4.0f/3.0f,0.001f,10.0f);
    Vector3f kCLoc(4.0f,0.0f,0.0f);
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

    m_pkRenderer->SetPointSize(2.0f);

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void PointInPolyhedron::OnTerminate ()
{
    WM4_DELETE[] m_akRayDirection;

    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkPoints = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void PointInPolyhedron::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    MoveObject();
    m_spkScene->UpdateGS(System::GetTime());
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
bool PointInPolyhedron::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        break;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"PointInPolyhedron.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void PointInPolyhedron::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Create a semitransparent sphere mesh.
    Attributes kPolyAttr;
    kPolyAttr.SetPositionChannels(3);
    TriMesh* pkMesh = StandardMesh(kPolyAttr).Sphere(16,16,1.0f);

    MaterialState* pkMS = WM4_NEW MaterialState;
    pkMS->Diffuse = ColorRGB(1.0f,0.0f,0.0f);
    pkMS->Alpha = 0.25f;
    pkMesh->AttachGlobalState(pkMS);

    AlphaState* pkAS = WM4_NEW AlphaState;
    pkAS->BlendEnabled = true;
    pkMesh->AttachGlobalState(pkAS);

    pkMesh->AttachEffect(WM4_NEW MaterialEffect);

    // Create the data structures for the polyhedron that represents the
    // sphere mesh.
    CreateQuery(pkMesh);

    // Create a set of random points.  Points inside the polyhedron are
    // colored white.  Points outside the polyhedron are colored blue.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    int iVQuantity = 1024;
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    for (int i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i) = Vector3f(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(),Mathf::SymmetricRandom());

        if (m_pkQuery->Contains(pkVBuffer->Position3(i)))
        {
            pkVBuffer->Color3(0,i) = ColorRGB::WHITE;
        }
        else
        {
            pkVBuffer->Color3(0,i) = ColorRGB(0.0f,0.0f,1.0f);
        }
    }

    DeleteQuery();

    m_spkPoints = WM4_NEW Polypoint(pkVBuffer);
    m_spkPoints->AttachEffect(WM4_NEW VertexColor3Effect);

    m_spkScene->AttachChild(m_spkPoints);
    m_spkScene->AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
#ifdef TRIANGLE_FACES
void PointInPolyhedron::CreateQuery (TriMesh* pkMesh)
{
    VertexBuffer* pkSVBuffer = pkMesh->VBuffer;
    int iSVQuantity = pkSVBuffer->GetVertexQuantity();
    Vector3f* akSVertex = (Vector3f*)pkSVBuffer->GetData();

    IndexBuffer* pkSIBuffer = pkMesh->IBuffer;
    int iSIQuantity = pkSIBuffer->GetIndexQuantity();
    int* aiSIndex = pkSIBuffer->GetData();
    int iFQuantity = iSIQuantity/3;
    m_akTFace = WM4_NEW PointInPolyhedron3f::TriangleFace[iFQuantity];

    const int* piSIndex = aiSIndex;
    int i;
    for (i = 0; i < iFQuantity; i++)
    {
        int iV0 = *piSIndex++;
        int iV1 = *piSIndex++;
        int iV2 = *piSIndex++;

        m_akTFace[i].Indices[0] = iV0;
        m_akTFace[i].Indices[1] = iV1;
        m_akTFace[i].Indices[2] = iV2;
        m_akTFace[i].Plane = Plane3f(akSVertex[iV0],akSVertex[iV1],
            akSVertex[iV2]);
    }

    m_pkQuery = WM4_NEW PointInPolyhedron3f(iSVQuantity,akSVertex,iFQuantity,
        m_akTFace,m_iRayQuantity,m_akRayDirection);
}
//----------------------------------------------------------------------------
void PointInPolyhedron::DeleteQuery ()
{
    WM4_DELETE m_pkQuery;
    WM4_DELETE[] m_akTFace;
}
#endif
//----------------------------------------------------------------------------
#ifdef CONVEX_FACES_METHOD_0
void PointInPolyhedron::CreateQuery (TriMesh* pkMesh)
{
    VertexBuffer* pkSVBuffer = pkMesh->VBuffer;
    int iSVQuantity = pkSVBuffer->GetVertexQuantity();
    Vector3f* akSVertex = (Vector3f*)pkSVBuffer->GetData();

    IndexBuffer* pkSIBuffer = pkMesh->IBuffer;
    int iSIQuantity = pkSIBuffer->GetIndexQuantity();
    int* aiSIndex = pkSIBuffer->GetData();
    int iFQuantity = iSIQuantity/3;
    m_akCFace = WM4_NEW PointInPolyhedron3f::ConvexFace[iFQuantity];

    const int* piSIndex = aiSIndex;
    int i;
    for (i = 0; i < iFQuantity; i++)
    {
        int iV0 = *piSIndex++;
        int iV1 = *piSIndex++;
        int iV2 = *piSIndex++;

        m_akCFace[i].Indices.resize(3);
        m_akCFace[i].Indices[0] = iV0;
        m_akCFace[i].Indices[1] = iV1;
        m_akCFace[i].Indices[2] = iV2;
        m_akCFace[i].Plane = Plane3f(akSVertex[iV0],akSVertex[iV1],
            akSVertex[iV2]);
    }

    m_pkQuery = WM4_NEW PointInPolyhedron3f(iSVQuantity,akSVertex,iFQuantity,
        m_akCFace,m_iRayQuantity,m_akRayDirection,0);
}
//----------------------------------------------------------------------------
void PointInPolyhedron::DeleteQuery ()
{
    WM4_DELETE m_pkQuery;
    WM4_DELETE[] m_akCFace;
}
#endif
//----------------------------------------------------------------------------
#ifdef CONVEX_FACES_METHOD_1
void PointInPolyhedron::CreateQuery (TriMesh* pkMesh)
{
    VertexBuffer* pkSVBuffer = pkMesh->VBuffer;
    int iSVQuantity = pkSVBuffer->GetVertexQuantity();
    Vector3f* akSVertex = (Vector3f*)pkSVBuffer->GetData();

    IndexBuffer* pkSIBuffer = pkMesh->IBuffer;
    int iSIQuantity = pkSIBuffer->GetIndexQuantity();
    int* aiSIndex = pkSIBuffer->GetData();
    int iFQuantity = iSIQuantity/3;
    m_akCFace = WM4_NEW PointInPolyhedron3f::ConvexFace[iFQuantity];

    const int* piSIndex = aiSIndex;
    int i;
    for (i = 0; i < iFQuantity; i++)
    {
        int iV0 = *piSIndex++;
        int iV1 = *piSIndex++;
        int iV2 = *piSIndex++;

        m_akCFace[i].Indices.resize(3);
        m_akCFace[i].Indices[0] = iV0;
        m_akCFace[i].Indices[1] = iV1;
        m_akCFace[i].Indices[2] = iV2;
        m_akCFace[i].Plane = Plane3f(akSVertex[iV0],akSVertex[iV1],
            akSVertex[iV2]);
    }

    m_pkQuery = WM4_NEW PointInPolyhedron3f(iSVQuantity,akSVertex,iFQuantity,
        m_akCFace,m_iRayQuantity,m_akRayDirection,1);
}
//----------------------------------------------------------------------------
void PointInPolyhedron::DeleteQuery ()
{
    WM4_DELETE m_pkQuery;
    WM4_DELETE[] m_akCFace;
}
#endif
//----------------------------------------------------------------------------
#ifdef CONVEX_FACES_METHOD_2
void PointInPolyhedron::CreateQuery (TriMesh* pkMesh)
{
    VertexBuffer* pkSVBuffer = pkMesh->VBuffer;
    int iSVQuantity = pkSVBuffer->GetVertexQuantity();
    Vector3f* akSVertex = (Vector3f*)pkSVBuffer->GetData();

    IndexBuffer* pkSIBuffer = pkMesh->IBuffer;
    int iSIQuantity = pkSIBuffer->GetIndexQuantity();
    int* aiSIndex = pkSIBuffer->GetData();
    int iFQuantity = iSIQuantity/3;
    m_akCFace = WM4_NEW PointInPolyhedron3f::ConvexFace[iFQuantity];

    const int* piSIndex = aiSIndex;
    int i;
    for (i = 0; i < iFQuantity; i++)
    {
        int iV0 = *piSIndex++;
        int iV1 = *piSIndex++;
        int iV2 = *piSIndex++;

        m_akCFace[i].Indices.resize(3);
        m_akCFace[i].Indices[0] = iV0;
        m_akCFace[i].Indices[1] = iV1;
        m_akCFace[i].Indices[2] = iV2;
        m_akCFace[i].Plane = Plane3f(akSVertex[iV0],akSVertex[iV1],
            akSVertex[iV2]);
    }

    m_pkQuery = WM4_NEW PointInPolyhedron3f(iSVQuantity,akSVertex,iFQuantity,
        m_akCFace,m_iRayQuantity,m_akRayDirection,2);
}
//----------------------------------------------------------------------------
void PointInPolyhedron::DeleteQuery ()
{
    WM4_DELETE m_pkQuery;
    WM4_DELETE[] m_akCFace;
}
#endif
//----------------------------------------------------------------------------
#ifdef SIMPLE_FACES_METHOD_0
void PointInPolyhedron::CreateQuery (TriMesh* pkMesh)
{
    VertexBuffer* pkSVBuffer = pkMesh->VBuffer;
    int iSVQuantity = pkSVBuffer->GetVertexQuantity();
    Vector3f* akSVertex = (Vector3f*)pkSVBuffer->GetData();

    IndexBuffer* pkSIBuffer = pkMesh->IBuffer;
    int iSIQuantity = pkSIBuffer->GetIndexQuantity();
    int* aiSIndex = pkSIBuffer->GetData();
    int iFQuantity = iSIQuantity/3;
    m_akSFace = WM4_NEW PointInPolyhedron3f::SimpleFace[iFQuantity];

    const int* piSIndex = aiSIndex;
    int i;
    for (i = 0; i < iFQuantity; i++)
    {
        int iV0 = *piSIndex++;
        int iV1 = *piSIndex++;
        int iV2 = *piSIndex++;

        m_akSFace[i].Indices.resize(3);
        m_akSFace[i].Indices[0] = iV0;
        m_akSFace[i].Indices[1] = iV1;
        m_akSFace[i].Indices[2] = iV2;
        m_akSFace[i].Plane = Plane3f(akSVertex[iV0],akSVertex[iV1],
            akSVertex[iV2]);

        m_akSFace[i].Triangles.resize(3);
        m_akSFace[i].Triangles[0] = iV0;
        m_akSFace[i].Triangles[1] = iV1;
        m_akSFace[i].Triangles[2] = iV2;
    }

    m_pkQuery = WM4_NEW PointInPolyhedron3f(iSVQuantity,akSVertex,iFQuantity,
        m_akSFace,m_iRayQuantity,m_akRayDirection,0);
}
//----------------------------------------------------------------------------
void PointInPolyhedron::DeleteQuery ()
{
    WM4_DELETE m_pkQuery;
    WM4_DELETE[] m_akSFace;
}
#endif
//----------------------------------------------------------------------------
#ifdef SIMPLE_FACES_METHOD_1
void PointInPolyhedron::CreateQuery (TriMesh* pkMesh)
{
    VertexBuffer* pkSVBuffer = pkMesh->VBuffer;
    int iSVQuantity = pkSVBuffer->GetVertexQuantity();
    Vector3f* akSVertex = (Vector3f*)pkSVBuffer->GetData();

    IndexBuffer* pkSIBuffer = pkMesh->IBuffer;
    int iSIQuantity = pkSIBuffer->GetIndexQuantity();
    int* aiSIndex = pkSIBuffer->GetData();
    int iFQuantity = iSIQuantity/3;
    m_akSFace = WM4_NEW PointInPolyhedron3f::SimpleFace[iFQuantity];

    const int* piSIndex = aiSIndex;
    int i;
    for (i = 0; i < iFQuantity; i++)
    {
        int iV0 = *piSIndex++;
        int iV1 = *piSIndex++;
        int iV2 = *piSIndex++;

        m_akSFace[i].Indices.resize(3);
        m_akSFace[i].Indices[0] = iV0;
        m_akSFace[i].Indices[1] = iV1;
        m_akSFace[i].Indices[2] = iV2;
        m_akSFace[i].Plane = Plane3f(akSVertex[iV0],akSVertex[iV1],
            akSVertex[iV2]);

        m_akSFace[i].Triangles.resize(3);
        m_akSFace[i].Triangles[0] = iV0;
        m_akSFace[i].Triangles[1] = iV1;
        m_akSFace[i].Triangles[2] = iV2;
    }

    m_pkQuery = WM4_NEW PointInPolyhedron3f(iSVQuantity,akSVertex,iFQuantity,
        m_akSFace,m_iRayQuantity,m_akRayDirection,1);
}
//----------------------------------------------------------------------------
void PointInPolyhedron::DeleteQuery ()
{
    WM4_DELETE m_pkQuery;
    WM4_DELETE[] m_akSFace;
}
#endif
//----------------------------------------------------------------------------
