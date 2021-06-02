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

#include "IntersectConvexPolyhedra.h"

WM4_WINDOW_APPLICATION(IntersectConvexPolyhedra);

//----------------------------------------------------------------------------
IntersectConvexPolyhedra::IntersectConvexPolyhedra ()
    :
    WindowApplication3("IntersectConvexPolyhedra",0,0,640,480,
        ColorRGBA(0.75f,0.75f,0.75f,1.0f))
{
}
//----------------------------------------------------------------------------
bool IntersectConvexPolyhedra::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(60.0f,4.0f/3.0f,0.1f,1000.0f);
    Vector3f kCLoc(16.0f,0.0f,0.0f);
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
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::OnTerminate ()
{
    m_spkScene = 0;
    m_spkMeshPoly0 = 0;
    m_spkMeshPoly1 = 0;
    m_spkMeshIntersection = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::OnIdle ()
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
bool IntersectConvexPolyhedra::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    switch (ucKey)
    {
    case 's':
    case 'S':
        InitializeObjectMotion(m_spkScene);
        return true;
    case '0':
        InitializeObjectMotion(m_spkMeshPoly0);
        return true;
    case '1':
        InitializeObjectMotion(m_spkMeshPoly1);
        return true;
    case 'r':
    case 'R':
        ComputeIntersection();
        m_spkScene->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkMotionObject = m_spkScene;

    m_kAttr.SetPositionChannels(3);
    m_kAttr.SetColorChannels(0,3);
    m_spkEffect = WM4_NEW VertexColor3Effect;

    // Attach a dummy intersection mesh.  If the intersection is nonempty,
    // the Culling flag will be modified to CULL_DYNAMIC.  The intersection
    // is drawn as a solid.
    m_spkMeshIntersection = StandardMesh(m_kAttr).Tetrahedron();
    m_spkMeshIntersection->AttachEffect(m_spkEffect);
    m_spkMeshIntersection->Culling = Spatial::CULL_ALWAYS;
    m_spkScene->AttachChild(m_spkMeshIntersection);
    VertexBuffer* pkVB = m_spkMeshIntersection->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    int i, j;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB->Color3(0,i) = ColorRGB(0.0f,1.0f,0.0f);
    }

    // The two input polyhedra are drawn in wireframe mode.
    WireframeState* pkWS = WM4_NEW WireframeState;
    pkWS->Enabled = true;

    // The first polyhedron is an ellipsoid.
    ConvexPolyhedronf::CreateEggShape(Vector3f::ZERO,1.0f,1.0f,2.0f,2.0f,
        4.0f,4.0f,3,m_kWorldPoly0);

    // Build the corresponding mesh.
    iVQuantity = m_kWorldPoly0.GetVQuantity();
    int iTQuantity = m_kWorldPoly0.GetTQuantity();
    int iIQuantity = 3*iTQuantity;
    pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(iIQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB->Position3(i) = m_kWorldPoly0.Point(i);
        pkVB->Color3(0,i) = ColorRGB(1.0f,0.0f,0.0f);
    }
    int* aiIndex = pkIB->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        const MTTriangle& rkTri = m_kWorldPoly0.GetTriangle(i);
        for (j = 0; j < 3; j++)
        {
            aiIndex[3*i+j] = m_kWorldPoly0.GetVLabel(rkTri.GetVertex(j));
        }
    }
    m_spkMeshPoly0 = WM4_NEW TriMesh(pkVB,pkIB);
    m_spkMeshPoly0->AttachGlobalState(pkWS);
    m_spkMeshPoly0->AttachEffect(m_spkEffect);
    m_spkMeshPoly0->Local.SetTranslate(Vector3f(0.0f,2.0f,0.0f));
    m_spkScene->AttachChild(m_spkMeshPoly0);

    // The second polyhedron is egg shaped.
    ConvexPolyhedronf::CreateEggShape(Vector3f::ZERO,2.0f,2.0f,4.0f,4.0f,
        5.0f,3.0f,4,m_kWorldPoly1);

    // Build the corresponding mesh.
    iVQuantity = m_kWorldPoly1.GetVQuantity();
    iTQuantity = m_kWorldPoly1.GetTQuantity();
    iIQuantity = 3*iTQuantity;
    pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    pkIB = WM4_NEW IndexBuffer(iIQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB->Position3(i) = m_kWorldPoly1.Point(i);
        pkVB->Color3(0,i) = ColorRGB(0.0f,0.0f,1.0f);
    }
    aiIndex = pkIB->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        const MTTriangle& rkTri = m_kWorldPoly1.GetTriangle(i);
        for (j = 0; j < 3; j++)
        {
            aiIndex[3*i+j] = m_kWorldPoly1.GetVLabel(rkTri.GetVertex(j));
        }
    }
    m_spkMeshPoly1 = WM4_NEW TriMesh(pkVB,pkIB);
    m_spkMeshPoly1->AttachGlobalState(pkWS);
    m_spkMeshPoly1->AttachEffect(m_spkEffect);
    m_spkMeshPoly1->Local.SetTranslate(Vector3f(0.0f,-2.0f,0.0f));
    m_spkScene->AttachChild(m_spkMeshPoly1);

    ComputeIntersection();
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::ComputeIntersection ()
{
    // Transform the model-space vertices to world space.
    VertexBuffer* pkVB = m_spkMeshPoly0->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        m_kWorldPoly0.Point(i) =
            m_spkMeshPoly0->Local.ApplyForward(pkVB->Position3(i));
    }
    m_kWorldPoly0.UpdatePlanes();

    pkVB = m_spkMeshPoly1->VBuffer;
    iVQuantity = pkVB->GetVertexQuantity();
    for (i = 0; i < iVQuantity; i++)
    {
        m_kWorldPoly1.Point(i) =
            m_spkMeshPoly1->Local.ApplyForward(pkVB->Position3(i));
    }
    m_kWorldPoly1.UpdatePlanes();

    // Compute the intersection (if any) in world space.
    bool hasIntersection = m_kWorldPoly0.FindIntersection(m_kWorldPoly1,
        m_kIntersection);

    if (hasIntersection)
    {
        // Build the corresponding mesh.
        iVQuantity = m_kIntersection.GetVQuantity();
        int iTQuantity = m_kIntersection.GetTQuantity();
        int iIQuantity = 3*iTQuantity;
        pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
        IndexBuffer* pkIB = WM4_NEW IndexBuffer(iIQuantity);
        for (i = 0; i < iVQuantity; i++)
        {
            pkVB->Position3(i) = m_kIntersection.Point(i);
            pkVB->Color3(0,i) = ColorRGB(0.0f,1.0f,0.0f);
        }
        int* aiIndex = pkIB->GetData();
        for (i = 0; i < iTQuantity; i++)
        {
            const MTTriangle& rkTri = m_kIntersection.GetTriangle(i);
            for (int j = 0; j < 3; j++)
            {
                aiIndex[3*i+j] =
                    m_kIntersection.GetVLabel(rkTri.GetVertex(j));
            }
        }
        m_spkMeshIntersection = WM4_NEW TriMesh(pkVB,pkIB);
        m_spkMeshIntersection->AttachEffect(m_spkEffect);
        m_spkScene->SetChild(0, m_spkMeshIntersection);
        m_spkScene->UpdateRS();

        m_spkMeshIntersection->Culling = Spatial::CULL_DYNAMIC;
    }
    else
    {
        m_spkMeshIntersection->Culling = Spatial::CULL_ALWAYS;
    }
}
//----------------------------------------------------------------------------
