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
// Version: 4.0.4 (2007/06/16)

#include "ConvexHull3D.h"
#include "Wm4ConvexHull3.h"

WM4_WINDOW_APPLICATION(ConvexHull3D);

//----------------------------------------------------------------------------
ConvexHull3D::ConvexHull3D ()
    :
    WindowApplication3("ConvexHull3D",0,0,640,480,ColorRGBA::WHITE)
{
    System::InsertDirectory("Data");

    m_iFileQuantity = 46;
    m_iCurrentFile = 1;
    m_iVQuantity = 0;
    m_akVertex = 0;
    m_akColor = 0;
    m_iLimitedQuantity = 0;
    m_eQueryType = Query::QT_INT64;
    m_pkHull = 0;
    System::Strcpy(m_acHeader,STRING_SIZE,"query type = INT64");
}
//----------------------------------------------------------------------------
bool ConvexHull3D::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10000.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetAxes(kCDir,kCUp,kCRight);

    m_kCuller.SetCamera(m_spkCamera);

    // center-and-fit the scene
    CreateScene();
    m_spkScene->UpdateGS();
    m_spkScene->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetLocation(kCLoc);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(1.0f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ConvexHull3D::OnTerminate ()
{
    m_kPicker.Records.clear();
    m_spkScene = 0;
    m_spkHull = 0;
    m_spkSphere = 0;
    m_spkWireframe = 0;
    WM4_DELETE[] m_akVertex;
    WM4_DELETE[] m_akColor;
    WM4_DELETE m_pkHull;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ConvexHull3D::OnIdle ()
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
        m_pkRenderer->Draw(8,16,ColorRGBA::BLACK,m_acHeader);
        m_pkRenderer->Draw(8,GetHeight()-8,ColorRGBA::BLACK,m_acFooter);

        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ConvexHull3D::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    switch (ucKey)
    {
    // load a new data set
    case 'd':
    case 'D':
        if (++m_iCurrentFile == m_iFileQuantity)
        {
            m_iCurrentFile = 1;
        }

        LoadData();
        return true;

    // query type INT64
    case 'n':
    case 'N':
        m_eQueryType = Query::QT_INT64;
        System::Strcpy(m_acHeader,STRING_SIZE,"query type = INT64");
        RegenerateHull();
        return true;

    // query type INTEGER
    case 'i':
    case 'I':
        m_eQueryType = Query::QT_INTEGER;
        System::Strcpy(m_acHeader,STRING_SIZE,"query type = INTEGER");
        RegenerateHull();
        return true;

    // query type RATIONAL
    case 'r':
    case 'R':
        m_eQueryType = Query::QT_RATIONAL;
        System::Strcpy(m_acHeader,STRING_SIZE,"query type = RATIONAL");
        RegenerateHull();
        return true;

    // query type REAL (float)
    case 'f':
    case 'F':
        m_eQueryType = Query::QT_REAL;
        System::Strcpy(m_acHeader,STRING_SIZE,"query type = REAL");
        RegenerateHull();
        return true;

    // query type FILTERED
    case 'c':
    case 'C':
        m_eQueryType = Query::QT_FILTERED;
        System::Strcpy(m_acHeader,STRING_SIZE,"query type = FILTERED");
        RegenerateHull();
        return true;

    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    // Read the notes in ConvexHul3D.h about how to use m_iLimitedQuantity.
    case '+':
    case '=':
        if (m_iLimitedQuantity < m_iVQuantity)
        {
            for (int i = 2; i < m_iLimitedQuantity+2; i++)
            {
                m_spkScene->DetachChildAt(i);
            }
            m_iLimitedQuantity++;
            CreateHull();
        }
        return true;
    case '-':
    case '_':
        if (m_iLimitedQuantity > 3)
        {
            for (int i = 2; i < m_iLimitedQuantity+2; i++)
            {
                m_spkScene->DetachChildAt(i);
            }
            m_iLimitedQuantity--;
            CreateHull();
        }
        return true;
    }

    return WindowApplication::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
bool ConvexHull3D::OnMouseClick (int iButton, int iState, int iX, int iY,
    unsigned int uiModifiers)
{
    WindowApplication3::OnMouseClick(iButton,iState,iX,iY,uiModifiers);

    if (iButton == MOUSE_RIGHT_BUTTON)
    {
        Vector3f kPos, kDir;
        m_spkCamera->GetPickRay(iX,iY,GetWidth(),GetHeight(),kPos,kDir);
        m_kPicker.Execute(m_spkScene,kPos,kDir,0.0f,Mathf::MAX_REAL);

        if (m_kPicker.Records.size() > 0)
        {
            const PickRecord& rkRecord = m_kPicker.GetClosestNonnegative();
            TriMeshPtr spkMesh = StaticCast<TriMesh>(rkRecord.Intersected);
            float fMaxBary = rkRecord.B0;
            int iIndex = 0;
            if (rkRecord.B1 > fMaxBary)
            {
                fMaxBary = rkRecord.B1;
                iIndex = 1;
            }
            if (rkRecord.B2 > fMaxBary)
            {
                fMaxBary = rkRecord.B2;
                iIndex = 2;
            }
            int* aiIndex = spkMesh->IBuffer->GetData();
            System::Sprintf(m_acFooter,STRING_SIZE,
                "intr = %d, tri = %d, ver = %d",(int)m_kPicker.Records.size(),
                rkRecord.Triangle,aiIndex[3*rkRecord.Triangle+iIndex]);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void ConvexHull3D::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);
    CullState* pkCS = WM4_NEW CullState;
    pkCS->Enabled = false;
    m_spkScene->AttachGlobalState(pkCS);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    TriMesh* pkSphere = StandardMesh(kAttr).Sphere(8,8,0.01f);
    pkSphere->AttachEffect(WM4_NEW VertexColor3Effect);
    m_spkScene->SetChild(1,pkSphere);

    // The current file is "Data/data01.txt".
    LoadData();
}
//----------------------------------------------------------------------------
void ConvexHull3D::LoadData ()
{
    const size_t uiSize = 32;
    char acFilename[uiSize];
    if (m_iCurrentFile < 10)
    {
        System::Sprintf(acFilename,uiSize,"data0%d.txt",m_iCurrentFile);
    }
    else
    {
        System::Sprintf(acFilename,uiSize,"data%d.txt",m_iCurrentFile);
    }

    const char* acPath = System::GetPath(acFilename,System::SM_READ);
    assert(acPath);
    std::ifstream kIStr(acPath);
    assert(kIStr);
    kIStr >> m_iVQuantity;

    WM4_DELETE[] m_akVertex;
    m_akVertex = WM4_NEW Vector3f[m_iVQuantity];
    int i;
    for (i = 0; i < m_iVQuantity; i++)
    {
        kIStr >> m_akVertex[i][0];
        kIStr >> m_akVertex[i][1];
        kIStr >> m_akVertex[i][2];
    }

    WM4_DELETE[] m_akColor;
    m_akColor = WM4_NEW ColorRGB[m_iVQuantity];
    for (i = 0; i < m_iVQuantity; i++)
    {
        m_akColor[i] = ColorRGB(Mathf::UnitRandom(),Mathf::UnitRandom(),
            Mathf::UnitRandom());
    }

    // Discard previous scene spheres.
    for (i = 2; i < m_iLimitedQuantity+2; i++)
    {
        m_spkScene->DetachChildAt(i);
    }

    m_iLimitedQuantity = m_iVQuantity;
    CreateHull();
}
//----------------------------------------------------------------------------
void ConvexHull3D::CreateHull ()
{
    int iVQuantity = m_iLimitedQuantity;
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i) = m_akVertex[i];
        pkVBuffer->Color3(0,i) = m_akColor[i];
    }

    RegenerateHull();

    int iTQuantity = 0;
    TriMesh* pkMesh = 0;

    switch (m_pkHull->GetDimension())
    {
    case 0:
        System::Sprintf(m_acFooter,STRING_SIZE,"point: v = %d, t = %d",
            iVQuantity,iTQuantity);
        return;
    case 1:
        System::Sprintf(m_acFooter,STRING_SIZE,"linear: v = %d, t = %d",
            iVQuantity,iTQuantity);
        return;
    case 2:
    {
        iTQuantity = (int)(m_pkHull->GetSimplexQuantity() - 2);
        const int* aiHull = m_pkHull->GetIndices();
        IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(3*iTQuantity);
        int* piIndex = pkIBuffer->GetData();
        int i0 = 1, i1 = 2;
        for (int iT = 0; iT < iTQuantity; iT++)
        {
            *piIndex++ = aiHull[0];
            *piIndex++ = aiHull[i0];
            *piIndex++ = aiHull[i1];
            i0++;
            i1++;
        }
        pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
        pkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

        System::Sprintf(m_acFooter,STRING_SIZE,"planar: v = %d, t = %d",
            iVQuantity,iTQuantity);
        break;
    }
    case 3:
        iTQuantity = m_pkHull->GetSimplexQuantity();
        const int* aiHullIndex = m_pkHull->GetIndices();
        IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(3*iTQuantity);
        int* aiIndex = pkIBuffer->GetData();
        memcpy(aiIndex,aiHullIndex,3*iTQuantity*sizeof(int));
        pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
        pkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

        System::Sprintf(m_acFooter,STRING_SIZE,"spatial: v = %d, t = %d",
            iVQuantity,iTQuantity);
        break;
    }

    // translate to center of mass
    Vector3f kCenter = m_akVertex[0];
    for (i = 1; i < m_iLimitedQuantity; i++)
    {
        kCenter += m_akVertex[i];
    }
    kCenter /= (float)m_iLimitedQuantity;
    pkMesh->Local.SetTranslate(-kCenter);
    m_spkScene->SetChild(0,pkMesh);

    for (i = 2; i < m_iLimitedQuantity+2; i++)
    {
        m_spkScene->SetChild(i,CreateSphere(m_akVertex[i-2]-kCenter));
    }

    TriMesh* pkSphere = StaticCast<TriMesh>(m_spkScene->GetChild(1));
    pkSphere->Local.SetTranslate(m_akVertex[m_iLimitedQuantity-1]-kCenter);

    // update the scene, center-and-fit to frustum
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();
    m_spkScene->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*
        m_spkCamera->GetDVector();
    m_spkCamera->SetLocation(kCLoc);

    m_kCuller.ComputeVisibleSet(m_spkScene);
}
//----------------------------------------------------------------------------
void ConvexHull3D::RegenerateHull ()
{
    WM4_DELETE m_pkHull;
    m_pkHull = WM4_NEW ConvexHull3f(m_iLimitedQuantity,m_akVertex,0.001f,
        false,m_eQueryType);

    if (m_pkHull->GetDimension() == 2)
    {
        ConvexHull3f* pkSave = (ConvexHull3f*)m_pkHull;
        m_pkHull = pkSave->GetConvexHull2();
        WM4_DELETE pkSave;
    }
}
//----------------------------------------------------------------------------
TriMesh* ConvexHull3D::CreateSphere (const Vector3f& rkCenter,
    ColorRGB kColor, float fRadius)
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    TriMesh* pkSphere = StandardMesh(kAttr).Sphere(8,8,fRadius);
    int iVQuantity = pkSphere->VBuffer->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        pkSphere->VBuffer->Color3(0,i) = kColor;
    }

    pkSphere->AttachEffect(WM4_NEW VertexColor3Effect);

    return pkSphere;
}
//----------------------------------------------------------------------------
