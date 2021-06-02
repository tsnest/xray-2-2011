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

#include "PolyhedronDistance.h"

WM4_WINDOW_APPLICATION(PolyhedronDistance);

//----------------------------------------------------------------------------
PolyhedronDistance::PolyhedronDistance ()
    :
    WindowApplication3("PolyhedronDistance",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::OnInitialize()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // ** layout of scene graph **
    // scene
    //     tetra[4]
    //     plane
    //     line[2]

    // create objects
    m_spkScene = WM4_NEW Node;
    TriMesh* pkPlane = CreatePlane();
    int i;
    for (i = 0; i < 2; i++)
    {
        // build the display tetrahedra
        float fSize = 0.3f+0.2f*(i+1);
        if (i == 0)
        {
            m_fEdgeLength = fSize;
        }
        m_apkTetra[i] = CreateTetra(fSize,false);
        m_aspkLine[i] = CreateLine();

        // build the point tetrahedra;
        m_fSmall = 0.02f;
        m_apkTetra[i+2] = CreateTetra(m_fSmall,true);
    }

    // convenience for line manipulations
    for (i = 0; i < 2; i++)
    {
        m_apkVBuffer[i] = m_aspkLine[i]->VBuffer;
    }

    // tetrahedra faces
    m_akFace = WM4_NEW TTuple<3,int>[4];
    m_akFace[0][0] = 1, m_akFace[0][1] = 2, m_akFace[0][2] = 0;
    m_akFace[1][0] = 0, m_akFace[1][1] = 3, m_akFace[1][2] = 2;
    m_akFace[2][0] = 0, m_akFace[2][1] = 1, m_akFace[2][2] = 3;
    m_akFace[3][0] = 1, m_akFace[3][1] = 2, m_akFace[3][2] = 3;

    InitialState();

    // set parent-child links
    m_spkScene->AttachChild(pkPlane);
    for (i = 0; i < 2; i++)
    {
        m_spkScene->AttachChild(m_apkTetra[i]);
        m_spkScene->AttachChild(m_aspkLine[i]);
        m_spkScene->AttachChild(m_apkTetra[i+2]);
    }

    // wireframe
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();
    UpdateLine();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.01f);
    return true;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::OnTerminate()
{
    WM4_DELETE[] m_akFace;
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_aspkLine[0] = 0;
    m_aspkLine[1] = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (Transform(ucKey))
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    }

    switch (ucKey)
    {
    case '0':  // restart
        InitialState();
        m_spkScene->UpdateGS(0.0f);
        UpdateLine();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case 'w':  // toggle wireframe
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void PolyhedronDistance::OnIdle ()
{
    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());

        const size_t uiSize = 256;
        char acMsg[uiSize];
        System::Sprintf(acMsg,uiSize,"separation = %3.2f",
            m_fSeparation/(Mathf::Sqrt(2.0)*m_fEdgeLength));
        m_pkRenderer->Draw(8,GetHeight()-8,ColorRGBA::WHITE,acMsg);
        System::Sprintf(acMsg,uiSize," small tetrahedron sides." );
        m_pkRenderer->Draw(140,GetHeight()-8,ColorRGBA::WHITE,acMsg);
        
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();
}
//----------------------------------------------------------------------------
TriMesh* PolyhedronDistance::CreateTetra (float fSize, bool bBlack)
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = -(fSize/3.0f)*Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(+fSize,0.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(0.0f,+fSize,0.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,0.0f,+fSize);

    if (bBlack)
    {
        // black tetrahedra for the small ones used as points.
        pkVBuffer->Color3(0,0) = ColorRGB(0.0f,0.0f,0.0f);
        pkVBuffer->Color3(0,1) = ColorRGB(0.0f,0.0f,0.0f);
        pkVBuffer->Color3(0,2) = ColorRGB(0.0f,0.0f,0.0f);
        pkVBuffer->Color3(0,3) = ColorRGB(0.0f,0.0f,0.0f);
    }
    else
    {
        // colorful colors for the tetrahedra under study
        pkVBuffer->Color3(0,0) = ColorRGB(0.0f,0.0f,1.0f);
        pkVBuffer->Color3(0,1) = ColorRGB(0.0f,1.0f,0.0f);
        pkVBuffer->Color3(0,2) = ColorRGB(1.0f,0.0f,0.0f);
        pkVBuffer->Color3(0,3) = ColorRGB(1.0f,1.0f,1.0f);
    }

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(12);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] = 0; aiIndex[ 1] = 2; aiIndex[ 2] = 1;
    aiIndex[ 3] = 0; aiIndex[ 4] = 3; aiIndex[ 5] = 2;
    aiIndex[ 6] = 0; aiIndex[ 7] = 1; aiIndex[ 8] = 3;
    aiIndex[ 9] = 1; aiIndex[10] = 2; aiIndex[11] = 3;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

    return pkMesh;
}
//----------------------------------------------------------------------------
Polyline* PolyhedronDistance::CreateLine ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,2);
    pkVBuffer->Position3(0) = Vector3f::ZERO;
    pkVBuffer->Position3(1) = Vector3f::UNIT_X;
    pkVBuffer->Color3(0,0) = ColorRGB::WHITE;
    pkVBuffer->Color3(0,1) = ColorRGB::WHITE;

    Polyline* pkPLine = WM4_NEW Polyline(pkVBuffer,false,true);
    pkPLine->AttachEffect(WM4_NEW VertexColor3Effect);

    return pkPLine;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::UpdateLine ()
{
    // two lines make the line easier to see
    Vector3f aakU[2][4];

    // Offset the polyhedra so far into the first octant that we are unlikely
    // to translate them out of that octant during a run.
    m_fOffset = 20.0f;
    Vector3f kTOffset = m_fOffset*Vector3f(1.0f,1.0f,1.0f);
    
    int i;
    for (i = 0; i < 2; i++)
    {
        m_akTV[i] = m_apkTetra[i]->World.GetTranslate();
        m_akTM[i] = m_apkTetra[i]->World.GetRotate();
        VertexBuffer* pkVB = m_apkTetra[i]->VBuffer;
        for (int j = 0; j < 4; j++)
        {
            aakU[i][j] = m_akTV[i]+kTOffset+ m_akTM[i]*pkVB->Position3(j);
        }
    }

    int iStatusCode;

    Vector3f akVertex[2] =
    {
        m_apkVBuffer[0]->Position3(0),
        m_apkVBuffer[0]->Position3(1)
    };

    LCPPolyDist3(4,aakU[0],4,m_akFace,4,aakU[1],4,m_akFace,iStatusCode,
        m_fSeparation,akVertex);

    m_apkVBuffer[0]->Position3(0) = akVertex[0];
    m_apkVBuffer[0]->Position3(1) = akVertex[1];

    if (iStatusCode != LCPPolyDist3::SC_FOUND_SOLUTION &&
        iStatusCode != LCPPolyDist3::SC_TEST_POINTS_TEST_FAILED &&
        iStatusCode != LCPPolyDist3::SC_FOUND_TRIVIAL_SOLUTION ||
        m_fSeparation < 0.0f)
    {
        // Do not draw the line joining nearest points if returns from
        // LCPPolyDist are not appropriate.
        for (i = 0; i < 2; i++)
        {
            m_apkVBuffer[0]->Position3(i) = -kTOffset;
        }
    }

    // correct for the offset and set up end "points" for the line
    for (i = 0; i < 2; i++)
    {
        m_apkVBuffer[0]->Position3(i) -= kTOffset;

        // the adjustment with m_fSmall "centers" the end point tetra
        // on the solution points
        m_apkTetra[i+2]->Local.SetTranslate(m_apkVBuffer[0]->Position3(i)
            -(m_fSmall/3.0f)*Vector3f(1.0f,1.0f,1.0f));
    }
    const float fEpsilon = 0.002f; // double-up the line for better visibility
    for (i = 0; i < 2; i++)
    {
        m_apkVBuffer[1]->Position3(i) = m_apkVBuffer[0]->Position3(i) +
            fEpsilon*Vector3f(1.0f,1.0f,1.0f);
    }

    for (i = 0; i < 2; i++)
    {
        m_aspkLine[i]->UpdateMS(false);
        m_aspkLine[i]->UpdateGS(0.0f);
        m_aspkLine[i]->VBuffer->Release();
    }
    m_spkScene->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
TriMesh* PolyhedronDistance::CreatePlane ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    float fSize = 16.0f;

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-fSize,-fSize,-0.1f);
    pkVBuffer->Position3(1) = Vector3f(+fSize,-fSize,-0.1f);
    pkVBuffer->Position3(2) = Vector3f(+fSize,+fSize,-0.1f);
    pkVBuffer->Position3(3) = Vector3f(-fSize,+fSize,-0.1f);
    pkVBuffer->Color3(0,0) = ColorRGB(0.0f,0.50f,0.00f);
    pkVBuffer->Color3(0,1) = ColorRGB(0.0f,0.25f,0.00f);
    pkVBuffer->Color3(0,2) = ColorRGB(0.0f,0.75f,0.00f);
    pkVBuffer->Color3(0,3) = ColorRGB(0.0f,1.0f,0.00f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0; aiIndex[1] = 1; aiIndex[2] = 2;
    aiIndex[3] = 0; aiIndex[4] = 2; aiIndex[5] = 3;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

    return pkMesh;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::InitialState ()
{
    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,-2.0f,0.5f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // transform objects
    m_apkTetra[0]->Local.SetRotate(
        Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,1.1f));
    m_apkTetra[0]->Local.SetTranslate(Vector3f(-0.25f,0.1f,0.3f));
    m_apkTetra[1]->Local.SetRotate(
        Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,0.3f));
    m_apkTetra[1]->Local.SetTranslate(Vector3f(0.25f,0.4f,0.5f));
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::Transform (unsigned char ucKey)
{
    Vector3f kTrn;
    Matrix3f kRot, kIncr;
    float fTrnSpeed = 0.1f;
    float fRotSpeed = 0.1f;

    switch (ucKey)
    {
        case 'x':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.X() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'X':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.X() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'y':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Y() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'Y':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Y() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'z':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Z() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'Z':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Z() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 's':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.X() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'S':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.X() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 't':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Y() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'T':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Y() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'u':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Z() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'U':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Z() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'a':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            break;
        case 'A':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,-fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            break;
        case 'b':
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        case 'B':
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,-fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        case 'c':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,1.3f*fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,-fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        case 'C':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,-1.3f*fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        default:
            return false;
    }

    // Prevent solution point coordinates from being < 0.
    // The polyhedron distance calculator expects solution points
    // to be in the first octant. Vertices are offset by
    // m_fOffset*Vector3f(1,1,1) in UpdateLine() before the call to the 
    // distance routine.
    // Here we make sure that no translation of a polyhedron takes it so
    // far into one of the other 7 octants that the offset will not be
    // sufficient to guarantee that the solution points lie in the first 
    // octant.
    float fThreshold = -m_fOffset+fTrnSpeed;
    for (int j = 0; j < 2; j++)
    {
        Vector3f kTV = m_apkTetra[j]->World.GetTranslate();
        Matrix3f kTM = m_apkTetra[j]->World.GetRotate();
        for (int i = 0; i < 4; i++)
        {
            Vector3f kTemp = kTV + kTM*m_apkTetra[j]->VBuffer->Position3(i);
            Vector3f kTrn;
            for (int k = 0; k < 3; k++)
            {
                if (kTemp[k] < fThreshold)
                {
                    kTrn = m_apkTetra[j]->Local.GetTranslate();
                    kTrn[k] += fTrnSpeed;
                    m_apkTetra[j]->Local.SetTranslate(kTrn);
                }
            }
        }
    }

    m_spkScene->UpdateGS(0.0f);
    UpdateLine();
    return true;
}
//----------------------------------------------------------------------------
