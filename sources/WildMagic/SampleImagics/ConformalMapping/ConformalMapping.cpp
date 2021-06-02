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

#include "ConformalMapping.h"

WM4_WINDOW_APPLICATION(ConformalMapping);

//----------------------------------------------------------------------------
ConformalMapping::ConformalMapping ()
    :
    WindowApplication3("ConformalMapping",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool ConformalMapping::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,-6.5f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // load brain mesh
    Stream kInfile;
    const char* acPath = System::GetPath("BrainP.wmof",System::SM_READ);
    if (!acPath)
    {
        return true;
    }
    kInfile.Load(acPath);
    if (kInfile.GetObjectCount() == 0)
    {
        return true;
    }

    m_spkMesh = DynamicCast<TriMesh>(kInfile.GetObjectAt(0));
    if (!m_spkMesh)
    {
        // Object must be a TriMesh for this application.  Moreover, the
        // TriMesh must be a closed mesh that is topologically equivalent
        // to a sphere.
        return true;
    }

    // The mesh has only positions.  Reallocate the vertex buffer to include
    // vertex colors.
    VertexBuffer* pkOldVB = m_spkMesh->VBuffer;
    int iVQuantity = pkOldVB->GetVertexQuantity();
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkNewVB = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    for (int i = 0; i < iVQuantity; i++)
    {
        pkNewVB->Position3(i) = pkOldVB->Position3(i);
    }
    m_spkMesh->VBuffer = pkNewVB;

    // use pseudocoloring based on mean curvature
    PseudocolorVertices();

    // Numerical preconditioning for computing the conformal map.  The choice
    // of fExtreme is based on knowledge of the size of the vertices in the
    // input mesh m_spkMesh.
    float fExtreme = 10.0f;
    ScaleToCube(fExtreme);

    DoMapping();

    // set up scene graph
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // subtree for mesh
    m_spkMeshTree = WM4_NEW Node;
    m_spkMeshTree->Local.SetTranslate(Vector3f(2.0f,0.0f,0.0f));
    m_spkMeshTree->Local.SetUniformScale(1.0f/fExtreme);
    m_spkScene->AttachChild(m_spkMeshTree);
    Node* pkMeshParent = WM4_NEW Node;
    m_spkMeshTree->AttachChild(pkMeshParent);
    pkMeshParent->AttachChild(m_spkMesh);
    pkMeshParent->Local.SetTranslate(-m_spkMesh->ModelBound->GetCenter());

    // subtree for sphere
    m_spkSphereTree = WM4_NEW Node;
    m_spkSphereTree->Local.SetTranslate(Vector3f(-2.0f,0.0f,0.0f));
    m_spkScene->AttachChild(m_spkSphereTree);
    Node* pkSphereParent = WM4_NEW Node;
    m_spkSphereTree->AttachChild(pkSphereParent);
    pkSphereParent->AttachChild(m_spkSphere);
    pkSphereParent->Local.SetTranslate(-m_spkSphere->ModelBound->GetCenter());

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ConformalMapping::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkMeshTree = 0;
    m_spkSphereTree = 0;
    m_spkMesh = 0;
    m_spkSphere = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ConformalMapping::OnIdle ()
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
bool ConformalMapping::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    switch (ucKey)
    {
    case 'w':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 'm':
        m_spkMotionObject = m_spkMeshTree;
        return true;
    case 's':
        m_spkMotionObject = m_spkSphereTree;
        return true;
    case 'r':
        m_spkMotionObject = m_spkScene;
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void ConformalMapping::ScaleToCube (float fExtreme)
{
    // Uniformly scale the cube to [-extreme,extreme]^3 for numerical
    // preconditioning for the conformal mapping.

    int iVQuantity = m_spkMesh->VBuffer->GetVertexQuantity();

    float fMin = m_spkMesh->VBuffer->Position3(0).X(), fMax = fMin;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m_spkMesh->VBuffer->Position3(i)[j] < fMin)            {
                fMin = m_spkMesh->VBuffer->Position3(i)[j];
            }
            else if (m_spkMesh->VBuffer->Position3(i)[j] > fMax)
            {
                fMax = m_spkMesh->VBuffer->Position3(i)[j];
            }
        }
    }
    float fHalfRange = 0.5f*(fMax - fMin);
    float fMult = fExtreme/fHalfRange;

    Vector3f kOne(1.0f,1.0f,1.0f);
    for (i = 0; i < iVQuantity; i++)
    {
        m_spkMesh->VBuffer->Position3(i) = -fExtreme*kOne +
            fMult*(m_spkMesh->VBuffer->Position3(i) - fMin*kOne);
    }

    m_spkMesh->UpdateMS(false);
}
//----------------------------------------------------------------------------
void ConformalMapping::PseudocolorVertices ()
{
    // color the vertices according to mean curvature

    int iVQuantity = m_spkMesh->VBuffer->GetVertexQuantity();
    int iTQuantity = m_spkMesh->IBuffer->GetIndexQuantity()/3;
    int* aiIndex = m_spkMesh->IBuffer->GetData();
    m_spkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

    Vector3f* akPoint = WM4_NEW Vector3f[iVQuantity];
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        akPoint[i] = m_spkMesh->VBuffer->Position3(i);
    }
    MeshCurvaturef kMC(iVQuantity,akPoint,iTQuantity,aiIndex);
    WM4_DELETE[] akPoint;

    const float* afMinCurv = kMC.GetMinCurvatures();
    const float* afMaxCurv = kMC.GetMaxCurvatures();
    float fMinMeanCurvature = afMinCurv[0] + afMaxCurv[0];
    float fMaxMeanCurvature = fMinMeanCurvature;
    float* afMeanCurvature = WM4_NEW float[iVQuantity];
    for (i = 0; i < iVQuantity; i++)
    {
        afMeanCurvature[i] = afMinCurv[i] + afMaxCurv[i];
        if (afMeanCurvature[i] < fMinMeanCurvature)
        {
            fMinMeanCurvature = afMeanCurvature[i];
        }
        else if (afMeanCurvature[i] > fMaxMeanCurvature)
        {
            fMaxMeanCurvature = afMeanCurvature[i];
        }
    }

    for (i = 0; i < iVQuantity; i++)
    {
        if (afMeanCurvature[i] > 0.0f)
        {
            m_spkMesh->VBuffer->Color3(0,i).R() =
                0.5f*(1.0f + afMeanCurvature[i]/fMaxMeanCurvature);
            m_spkMesh->VBuffer->Color3(0,i).G() =
                m_spkMesh->VBuffer->Color3(0,i).R();
            m_spkMesh->VBuffer->Color3(0,i).B() = 0.0f;
        }
        else if (afMeanCurvature[i] < 0.0f)
        {
            m_spkMesh->VBuffer->Color3(0,i).R() = 0.0f;
            m_spkMesh->VBuffer->Color3(0,i).G() = 0.0f;
            m_spkMesh->VBuffer->Color3(0,i).B() =
                0.5f*(1.0f - afMeanCurvature[i]/fMinMeanCurvature);
        }
        else
        {
            m_spkMesh->VBuffer->Color3(0,i) = ColorRGB::BLACK;
        }
    }

    WM4_DELETE[] afMeanCurvature;
}
//----------------------------------------------------------------------------
void ConformalMapping::DoMapping ()
{
    int iVQuantity = m_spkMesh->VBuffer->GetVertexQuantity();
    int iTQuantity = m_spkMesh->IBuffer->GetIndexQuantity()/3;
    const int* aiIndex = m_spkMesh->IBuffer->GetData();

    Vector3f* akVertex = WM4_NEW Vector3f[iVQuantity];
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        akVertex[i] = m_spkMesh->VBuffer->Position3(i);
    }

    // color the punctured triangle red
    m_spkMesh->VBuffer->Color3(0,aiIndex[0]) = ColorRGB(1.0f,0.0f,0.0f);
    m_spkMesh->VBuffer->Color3(0,aiIndex[1]) = ColorRGB(1.0f,0.0f,0.0f);
    m_spkMesh->VBuffer->Color3(0,aiIndex[2]) = ColorRGB(1.0f,0.0f,0.0f);

    // conformally map mesh to plane, sphere, and cylinder
    ConformalMapf kCMap(iVQuantity,akVertex,iTQuantity,aiIndex,0);
    const Vector3f* akSVertex = kCMap.GetSphereCoordinates();

    // create a representation of the conformal sphere
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i) = akSVertex[i];
        pkVBuffer->Color3(0,i) = m_spkMesh->VBuffer->Color3(0,i);
    }

    m_spkSphere = WM4_NEW TriMesh(pkVBuffer,m_spkMesh->IBuffer);
    m_spkSphere->AttachEffect(m_spkMesh->GetEffect(0));
}
//----------------------------------------------------------------------------
