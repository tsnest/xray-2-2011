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
// Version: 4.0.2 (2008/02/06)

#include "ExtremalQuery.h"
#include "Wm4ExtremalQuery3BSP.h"
#include "Wm4ExtremalQuery3PRJ.h"

WM4_WINDOW_APPLICATION(ExtremalQuery);

//----------------------------------------------------------------------------
ExtremalQuery::ExtremalQuery ()
    :
    WindowApplication3("ExtremalQuery",0,0,640,480,
        ColorRGBA(0.8f,0.8f,0.8f,1.0f))
{
    m_pkConvexPolyhedron = 0;
    m_pkExtremalQuery = 0;
}
//----------------------------------------------------------------------------
bool ExtremalQuery::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up an orthogonal camera.  This projection type is used to make it
    // clear that the displayed extreme points really are extreme!  (The
    // perspective projection is deceptive.)
    m_spkCamera->Perspective = false;
    m_spkCamera->SetFrustum(-2.0,2.0f,-1.5f,1.5f,1.0f,1000.0f);
    Vector3f kCLoc(4.0f,0.0f,0.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // set up scene
    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ExtremalQuery::OnTerminate ()
{
    WM4_DELETE m_pkConvexPolyhedron;
    WM4_DELETE m_pkExtremalQuery;

    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkCull = 0;
    m_spkMaxSphere = 0;
    m_spkMinSphere = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ExtremalQuery::OnIdle ()
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
bool ExtremalQuery::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
bool ExtremalQuery::OnMotion (int iButton, int iX, int iY,
    unsigned int uiModifiers)
{
    WindowApplication3::OnMotion(iButton,iX,iY,uiModifiers);
    UpdateExtremePoints();
    return true;
}
//----------------------------------------------------------------------------
void ExtremalQuery::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);
    m_spkCull = WM4_NEW CullState;
    m_spkCull->Enabled = false;
    m_spkScene->AttachGlobalState(m_spkCull);

    const int iPQuantity = 32;
    CreateConvexPolyhedron(iPQuantity);
    m_spkScene->AttachChild(CreateVisualConvexPolyhedron());

    // Use small spheres to show the extreme points in the camera's right
    // direction.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    StandardMesh kSM(kAttr);

    // maximum sphere
    m_spkMaxSphere = kSM.Sphere(8,8,0.05f);
    m_spkMaxSphere->AttachEffect(WM4_NEW VertexColor3Effect);
    m_spkScene->AttachChild(m_spkMaxSphere);

    // minimum sphere
    m_spkMinSphere = kSM.Sphere(8,8,0.05f);
    m_spkMinSphere->AttachEffect(WM4_NEW VertexColor3Effect);
    m_spkScene->AttachChild(m_spkMinSphere);

    UpdateExtremePoints();
}
//----------------------------------------------------------------------------
void ExtremalQuery::CreateConvexPolyhedron (int iVQuantity)
{
    // Create the convex hull of a randomly generated set of points on the
    // unit sphere.
    Vector3f* akVertex = WM4_NEW Vector3f[iVQuantity];
    int i, j;
    for (i = 0; i < iVQuantity; i++)
    {
        for (j = 0; j < 3; j++)
        {
            akVertex[i][j] = Mathf::SymmetricRandom();
        }
        akVertex[i].Normalize();
    }

    ConvexHull3f kHull(iVQuantity,akVertex,0.001f,false,Query::QT_INT64);
    assert(kHull.GetDimension() == 3);
    int iIQuantity = 3*kHull.GetSimplexQuantity();
    int* aiIndex = WM4_NEW int[iIQuantity];
    memcpy(aiIndex,kHull.GetIndices(),iIQuantity*sizeof(int));

    m_pkConvexPolyhedron = WM4_NEW ConvexPolyhedron3f(iVQuantity,akVertex,
        iIQuantity/3,aiIndex,0,true);

#if 0
    m_pkExtremalQuery = WM4_NEW ExtremalQuery3PRJf(*m_pkConvexPolyhedron);
#else
    m_pkExtremalQuery = WM4_NEW ExtremalQuery3BSPf(*m_pkConvexPolyhedron);
#endif

#if 0
    // For timing purposes and determination of asymptotic order.
    const int iMax = 10000000;
    Vector3f* akDirection = WM4_NEW Vector3f[iMax];
    for (i = 0; i < iMax; i++)
    {
        for (j = 0; j < 3; j++)
        {
            akDirection[i][j] = Mathf::SymmetricRandom();
        }
        akDirection[i].Normalize();
    }
    int iPos, iNeg;

    clock_t lStart = clock();
    for (i = 0; i < iMax; i++)
    {
        m_pkExtremalQuery->GetExtremeVertices(akDirection[i],iPos,iNeg);
    }
    clock_t lFinal = clock();
    long lDiff = lFinal - lStart;
    double dTime = ((double)lDiff)/(double)CLOCKS_PER_SEC;
    std::ofstream kOStr("timing.txt");
    kOStr << "time = " << dTime << " seconds" << std::endl;
    kOStr.close();

    WM4_DELETE[] akDirection;
#endif
}
//----------------------------------------------------------------------------
Node* ExtremalQuery::CreateVisualConvexPolyhedron ()
{
    const Vector3f* akVertex = m_pkConvexPolyhedron->GetVertices();
    int iTQuantity = m_pkConvexPolyhedron->GetTQuantity();
    int iIQuantity = 3*iTQuantity;
    const int* aiIndex = m_pkConvexPolyhedron->GetIndices();

    // Visualize the convex polyhedron as a collection of face-colored
    // triangles.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iIQuantity);
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int i;
    for (i = 0; i < iIQuantity; i++)
    {
        pkVBuffer->Position3(i) = akVertex[aiIndex[i]];
        pkIBuffer->GetData()[i] = i;
    }
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    // Use randomly generated vertex colors.
    for (i = 0; i < iTQuantity; i++)
    {
        ColorRGB kColor;
        for (int j = 0; j < 3; j++)
        {
            kColor[j] = Mathf::UnitRandom();
        }

        pkVBuffer->Color3(0,3*i  ) = kColor;
        pkVBuffer->Color3(0,3*i+1) = kColor;
        pkVBuffer->Color3(0,3*i+2) = kColor;
    }
    pkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

    Node* pkRoot = WM4_NEW Node;
    pkRoot->AttachChild(pkMesh);

    return pkRoot;
}
//----------------------------------------------------------------------------
void ExtremalQuery::UpdateExtremePoints ()
{
    Vector3f kDirection = m_spkScene->World.InvertVector(
        m_spkCamera->GetRVector());

    int iPosDir, iNegDir;
    m_pkExtremalQuery->GetExtremeVertices(kDirection,iPosDir,iNegDir);

    Vector3f kExtreme = m_pkConvexPolyhedron->GetVertex(iPosDir);
    m_spkMaxSphere->Local.SetTranslate(kExtreme);

    kExtreme = m_pkConvexPolyhedron->GetVertex(iNegDir);
    m_spkMinSphere->Local.SetTranslate(kExtreme);

    m_spkScene->UpdateGS();
}
//----------------------------------------------------------------------------
