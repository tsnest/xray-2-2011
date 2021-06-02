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

#include "FreeFormDeformation.h"

WM4_WINDOW_APPLICATION(FreeFormDeformation);

//----------------------------------------------------------------------------
FreeFormDeformation::FreeFormDeformation ()
    :
    WindowApplication3("FreeFormDeformation",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
    m_iQuantity = 4;
    m_iDegree = 3;
    m_pkVolume = 0;
    m_akParameter = 0;
    m_bDoRandom = false;
    m_fAmplitude = 0.01f;
    m_fRadius = 0.25f;
    m_fLastUpdateTime = (float)System::GetTime();
    m_bMouseDown = false;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the scene graph.  The TrnNode objects allows the application
    // to rotate the scene about its center.  Global wireframe and depth
    // buffering is used.
    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Setup();

    // center-and-fit mesh for viewing by camera
    m_spkMesh->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());

    // set up camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -4.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.02f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnTerminate ()
{
    WM4_DELETE m_pkVolume;
    WM4_DELETE[] m_akParameter;
    m_kPicker.Records.clear();
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkWireframe = 0;
    m_spkMesh = 0;
    m_spkPolylineRoot = 0;
    m_spkControlRoot = 0;
    m_spkSelected = 0;
    m_spkControlActive = 0;
    m_spkControlInactive = 0;
    m_spkControlLight = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnIdle ()
{
    MeasureTime();

    bool bNeedsCulling = false;
    if (MoveCamera())
    {
        bNeedsCulling = true;
    }

    if (MoveObject())
    {
        m_spkScene->UpdateGS();
        bNeedsCulling = true;
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    if (m_bDoRandom)
    {
        // deform the mesh no faster than 30 frames per second
        float fTime = (float)System::GetTime();
        if (fTime - m_fLastUpdateTime >= 0.0333333f)
        {
            m_fLastUpdateTime = fTime;
            DoRandomControlPoints();
            m_spkScene->UpdateGS(0.0f);
            bNeedsCulling = true;
        }
    }

    if (bNeedsCulling)
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    // draw the scene as fast as possible (not limited to 30 fps)
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
bool FreeFormDeformation::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    case 'a':  // toggle automated random versus user-adjusted controls
    case 'A':
        m_bDoRandom = !m_bDoRandom;
        return true;
    case 'c':  // toggle whether or not the polylines/boxes are drawn
    case 'C':
        if (m_spkPolylineRoot->Culling == Spatial::CULL_ALWAYS)
        {
            m_spkPolylineRoot->Culling = Spatial::CULL_DYNAMIC;
        }
        else
        {
            m_spkPolylineRoot->Culling = Spatial::CULL_ALWAYS;
        }

        if (m_spkControlRoot->Culling == Spatial::CULL_ALWAYS)
        {
            m_spkControlRoot->Culling = Spatial::CULL_DYNAMIC;
        }
        else
        {
            m_spkControlRoot->Culling = Spatial::CULL_ALWAYS;
        }

        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if (m_bDoRandom || iButton != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (iState == MOUSE_DOWN)
    {
        m_bMouseDown = true;
        OnMouseDown(iX,iY);
    }
    else
    {
        m_bMouseDown = false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnMotion (int, int iX, int iY, unsigned int)
{
    if (m_bDoRandom || !m_bMouseDown || !m_spkSelected)
    {
        return false;
    }

    OnMouseMove(iX,iY);
    return true;
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnMouseDown (int iX, int iY)
{
    // the current selected control point is deactivated
    if (m_spkSelected)
    {
        m_spkSelected->AttachGlobalState(m_spkControlInactive);
        m_spkSelected->UpdateRS();
        m_spkSelected = 0;
    }

    // Determine which control point has been selected (if any).
    Vector3f kPos, kDir;
    m_spkCamera->GetPickRay(iX,iY,m_iWidth,m_iHeight,kPos,kDir);
    m_kPicker.Execute(m_spkControlRoot,kPos,kDir,0.0f,Mathf::MAX_REAL);
    if (m_kPicker.Records.size() > 0)
    {
        const PickRecord& rkRecord = m_kPicker.GetClosestNonnegative();
        TriMesh* pkMesh = DynamicCast<TriMesh>(rkRecord.Intersected);
        if (pkMesh)
        {
            Triangle3f kTri;
            pkMesh->GetWorldTriangle(rkRecord.Triangle,kTri);
            m_kOldWorldPos = kTri.V[0]*rkRecord.B0 + kTri.V[1]*rkRecord.B1 +
                kTri.V[2]*rkRecord.B2;

            m_spkSelected = pkMesh;
            m_spkSelected->AttachGlobalState(m_spkControlActive);
            m_spkSelected->UpdateRS();
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnMouseMove (int iX, int iY)
{
    // Construct a pick ray.  Te want to move the control point from its
    // current location to this ray.
    Vector3f kPos, kDir;
    if (!m_spkCamera->GetPickRay(iX,iY,m_iWidth,m_iHeight,kPos,kDir))
    {
        return;
    }

    // Let E be the camera world origin, D be the camera world direction, and
    // U be the pick ray world direction.  Let C0 be the current location of
    // the picked point and let C1 be its desired new location.  We need to
    // choose t for which C1 = E + t*U.  Two possibilities are provided here,
    // both requiring computing: Diff = C0 - E.
    Vector3f kDiff = m_kOldWorldPos - m_spkCamera->GetLocation();

    float fT;
#if 0
    // The new world position is chosen to be at a distance along the pick
    // ray that is equal to the distance from the camera location to the old
    // world position.  That is, we require
    //   Length(C0-E) = Length(C1-E) = Length(t*U) = t.
    fT = kDiff.Length();
#else
    // The new world position is in the same plane perpendicular to the
    // camera direction as the old world position is.  This plane is
    // Dot(D,X-C0) = 0, in which case we need
    //   0 = Dot(D,C1-C0) = Dot(D,E+t*U-C0) = Dot(D,E-C0) + t*Dot(D,U)
    // Solving for t, we have
    //   t = Dot(D,C0-E)/Dot(D,U)
    fT = m_spkCamera->GetDVector().Dot(kDiff) /
        m_spkCamera->GetDVector().Dot(kDir);
#endif
    Vector3f kNewWorldPos = kPos + fT*kDir;

    // Move the control point to the new world location.  The technical
    // problem is that we need to modify the world coordinates for the
    // selected control point.  Thus, we need to determine how to change the
    // local translation in order to cause the correct world translation.
    Node* pkParent = (Node*)m_spkSelected->GetParent();
    kDiff = kNewWorldPos - m_kOldWorldPos;
    Vector3f kLocalDiff = pkParent->World.InvertVector(kDiff);
    m_spkSelected->Local.SetTranslate(m_spkSelected->Local.GetTranslate() +
        kLocalDiff);
    m_spkSelected->UpdateGS(0.0f);
    m_kOldWorldPos = kNewWorldPos;

    // Modify the control point itself.  It is known that the name string
    // has three single-digit numbers separated by blanks.
    const std::string& rkName = m_spkSelected->GetName();
    int i0 = (int)(rkName[0] - '0');
    int i1 = (int)(rkName[2] - '0');
    int i2 = (int)(rkName[4] - '0');
    m_pkVolume->SetControlPoint(i0,i1,i2,m_spkSelected->Local.GetTranslate());

    // TO DO.  Only need to update mesh vertices that are affected by the
    // change in one control point.  This requires working with the B-spline
    // basis function and knowing which (u,v,w) to evaluate at (i.e. determine
    // the local control region).
    UpdateMesh();

    // TO DO.  Only need to update neighboring lines.
    UpdatePolylines();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::Setup ()
{
    Stream kInfile;
    const char* acFilename = System::GetPath("SmallBrainPT2.wmof",
        System::SM_READ);
    bool bLoaded = kInfile.Load(acFilename);
    assert(bLoaded);
    (void)bLoaded;  // avoid warning in release build
    m_spkMesh = (TriMesh*)kInfile.GetObjectAt(0);
    m_spkMesh->AttachEffect(WM4_NEW TextureEffect("Quartz"));

    m_spkTrnNode->AttachChild(m_spkMesh);

    CreateBSplineVolume();
    CreatePolylines();
    CreateControlBoxes();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreateBSplineVolume ()
{
    // generate the B-spline volume function
    m_pkVolume = WM4_NEW BSplineVolumef(m_iQuantity,m_iQuantity,m_iQuantity,
        m_iDegree,m_iDegree,m_iDegree);

    // get AABB of form [xmin,xmax]x[ymin,ymax]x[zmin,zmax]
    VertexBuffer* pkVB = m_spkMesh->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    m_fXMin = pkVB->Position3(0).X();
    m_fYMin = pkVB->Position3(0).Y();
    m_fZMin = pkVB->Position3(0).Z();
    float fXMax = m_fXMin, fYMax = m_fYMin, fZMax = m_fZMin;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        if (pkVB->Position3(i).X() < m_fXMin)
        {
            m_fXMin = pkVB->Position3(i).X();
        }
        else if (pkVB->Position3(i).X() > fXMax)
        {
            fXMax = pkVB->Position3(i).X();
        }

        if (pkVB->Position3(i).Y() < m_fYMin)
        {
            m_fYMin = pkVB->Position3(i).Y();
        }
        else if (pkVB->Position3(i).Y() > fYMax)
        {
            fYMax = pkVB->Position3(i).Y();
        }

        if (pkVB->Position3(i).Z() < m_fZMin)
        {
            m_fZMin = pkVB->Position3(i).Z();
        }
        else if (pkVB->Position3(i).Z() > fZMax)
        {
            fZMax = pkVB->Position3(i).Z();
        }
    }

    // generate the control points
    float fXRange = fXMax - m_fXMin;
    float fYRange = fYMax - m_fYMin;
    float fZRange = fZMax - m_fZMin;
    m_fDX = fXRange/(float)(m_iQuantity-1);
    m_fDY = fYRange/(float)(m_iQuantity-1);
    m_fDZ = fZRange/(float)(m_iQuantity-1);
    Vector3f kCtrl;
    int i0, i1, i2;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        kCtrl.X() = m_fXMin + m_fDX*i0;
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            kCtrl.Y() = m_fYMin + m_fDY*i1;
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                kCtrl.Z() = m_fZMin + m_fDZ*i2;
                m_pkVolume->SetControlPoint(i0,i1,i2,kCtrl);
            }
        }
    }

    // compute the (u,v,w) values of the mesh relative to the B-spline volume
    float fInvXRange = 1.0f/fXRange;
    float fInvYRange = 1.0f/fYRange;
    float fInvZRange = 1.0f/fZRange;
    m_akParameter = WM4_NEW Vector3f[iVQuantity];
    for (i = 0; i < iVQuantity; i++)
    {
        m_akParameter[i].X() = (pkVB->Position3(i).X() - m_fXMin)*fInvXRange;
        m_akParameter[i].Y() = (pkVB->Position3(i).Y() - m_fYMin)*fInvYRange;
        m_akParameter[i].Z() = (pkVB->Position3(i).Z() - m_fZMin)*fInvZRange;
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreatePolylines ()
{
    // generate the polylines that connect adjacent control points
    m_spkPolylineRoot = WM4_NEW Node;
    m_spkTrnNode->AttachChild(m_spkPolylineRoot);

    VertexColor3Effect* pkEffect = WM4_NEW VertexColor3Effect;
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVB;
    Polyline* pkPoly;
    int i0, i1, i2;

    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity-1; i2++)
            {
                pkVB = WM4_NEW VertexBuffer(kAttr,2);
                pkVB->Position3(0) = m_pkVolume->GetControlPoint(i0,i1,i2);
                pkVB->Position3(1) = m_pkVolume->GetControlPoint(i0,i1,i2+1);
                pkVB->Color3(0,0) = ColorRGB(0.0f,0.0f,0.75f);
                pkVB->Color3(0,1) = ColorRGB(0.0f,0.0f,0.75f);
                pkPoly = WM4_NEW Polyline(pkVB,false,true);
                pkPoly->AttachEffect(pkEffect);
                m_spkPolylineRoot->AttachChild(pkPoly);
            }
        }

        for (i2 = 0; i2 < m_iQuantity; i2++)
        {
            for (i1 = 0; i1 < m_iQuantity-1; i1++)
            {
                pkVB = WM4_NEW VertexBuffer(kAttr,2);
                pkVB->Position3(0) = m_pkVolume->GetControlPoint(i0,i1,i2);
                pkVB->Position3(1) = m_pkVolume->GetControlPoint(i0,i1+1,i2);
                pkVB->Color3(0,0) = ColorRGB(0.0f,0.75f,0.0f);
                pkVB->Color3(0,1) = ColorRGB(0.0f,0.75f,0.0f);
                pkPoly = WM4_NEW Polyline(pkVB,false,true);
                pkPoly->AttachEffect(pkEffect);
                m_spkPolylineRoot->AttachChild(pkPoly);
            }
        }
    }

    for (i0 = 0; i0 < m_iQuantity-1; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                pkVB = WM4_NEW VertexBuffer(kAttr,2);
                pkVB->Position3(0) = m_pkVolume->GetControlPoint(i0,i1,i2);
                pkVB->Position3(1) = m_pkVolume->GetControlPoint(i0+1,i1,i2);
                pkVB->Color3(0,0) = ColorRGB(0.75f,0.0f,0.0f);
                pkVB->Color3(0,1) = ColorRGB(0.75f,0.0f,0.0f);
                pkPoly = WM4_NEW Polyline(pkVB,false,true);
                pkPoly->AttachEffect(pkEffect);
                m_spkPolylineRoot->AttachChild(pkPoly);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreateControlBoxes ()
{
    // generate small boxes to represent the control points
    m_spkControlRoot = WM4_NEW Node;
    m_spkTrnNode->AttachChild(m_spkControlRoot);

    // create a single box to be copied by each control point box
    const float fHalfWidth = 0.02f;
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(kAttr,8);
    pkVB->Position3(0) = Vector3f(-fHalfWidth,-fHalfWidth,-fHalfWidth);
    pkVB->Position3(1) = Vector3f(+fHalfWidth,-fHalfWidth,-fHalfWidth);
    pkVB->Position3(2) = Vector3f(+fHalfWidth,+fHalfWidth,-fHalfWidth);
    pkVB->Position3(3) = Vector3f(-fHalfWidth,+fHalfWidth,-fHalfWidth);
    pkVB->Position3(4) = Vector3f(-fHalfWidth,-fHalfWidth,+fHalfWidth);
    pkVB->Position3(5) = Vector3f(+fHalfWidth,-fHalfWidth,+fHalfWidth);
    pkVB->Position3(6) = Vector3f(+fHalfWidth,+fHalfWidth,+fHalfWidth);
    pkVB->Position3(7) = Vector3f(-fHalfWidth,+fHalfWidth,+fHalfWidth);

    IndexBuffer* pkIB = WM4_NEW IndexBuffer(36);
    int* aiIndex = pkIB->GetData();
    aiIndex[ 0] = 0;  aiIndex[ 1] = 2;  aiIndex[ 2] = 1;
    aiIndex[ 3] = 0;  aiIndex[ 4] = 3;  aiIndex[ 5] = 2;
    aiIndex[ 6] = 4;  aiIndex[ 7] = 5;  aiIndex[ 8] = 6;
    aiIndex[ 9] = 4;  aiIndex[10] = 6;  aiIndex[11] = 7;
    aiIndex[12] = 0;  aiIndex[13] = 5;  aiIndex[14] = 4;
    aiIndex[15] = 0;  aiIndex[16] = 1;  aiIndex[17] = 5;
    aiIndex[18] = 3;  aiIndex[19] = 7;  aiIndex[20] = 6;
    aiIndex[21] = 3;  aiIndex[22] = 6;  aiIndex[23] = 2;
    aiIndex[24] = 1;  aiIndex[25] = 2;  aiIndex[26] = 6;
    aiIndex[27] = 1;  aiIndex[28] = 6;  aiIndex[29] = 5;
    aiIndex[30] = 0;  aiIndex[31] = 4;  aiIndex[32] = 7;
    aiIndex[33] = 0;  aiIndex[34] = 7;  aiIndex[35] = 3;

    // create the materials and light to be attached to each box
    m_spkControlActive = WM4_NEW MaterialState;
    m_spkControlActive->Emissive = ColorRGB::BLACK;
    m_spkControlActive->Ambient = ColorRGB(1.0f,0.0f,0.0f);
    m_spkControlActive->Diffuse = ColorRGB(1.0f,0.0f,0.0f);
    m_spkControlActive->Specular = ColorRGB::BLACK;
    m_spkControlActive->Shininess = 1.0f;
    m_spkControlActive->Alpha = 1.0f;

    m_spkControlInactive = WM4_NEW MaterialState;
    m_spkControlInactive->Emissive = ColorRGB::BLACK;
    m_spkControlInactive->Ambient = ColorRGB(0.75f,0.75f,0.75f);
    m_spkControlInactive->Diffuse = ColorRGB(0.75f,0.75f,0.75f);
    m_spkControlInactive->Specular = ColorRGB::BLACK;
    m_spkControlInactive->Shininess = 1.0f;
    m_spkControlInactive->Alpha = 1.0f;

    m_spkControlLight = WM4_NEW Light(Light::LT_AMBIENT);
    m_spkControlLight->Ambient = ColorRGB::WHITE;
    m_spkControlLight->Diffuse = ColorRGB::WHITE;
    m_spkControlLight->Specular = ColorRGB::BLACK;

    int i0, i1, i2;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                TriMesh* pkBox = WM4_NEW TriMesh(pkVB,pkIB);
                pkBox->Local.SetTranslate(
                    m_pkVolume->GetControlPoint(i0,i1,i2));

                // Encode the indices in the name for later use.  This will
                // allow fast lookup of volume control points.
                const size_t uiSize = 32;
                char acName[uiSize];
                System::Sprintf(acName,uiSize,"%d %d %d",i0,i1,i2);
                pkBox->SetName(acName);

                pkBox->AttachGlobalState(m_spkControlInactive);
                pkBox->AttachLight(m_spkControlLight);

                m_spkControlRoot->AttachChild(pkBox);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdateMesh ()
{
    // Update the mesh points.  Typically you would update the model bound
    // after modifying the vertices, but in this application the mesh is
    // always visible, so an accurate model bound for culling purposes is
    // not necessary--the model bound is never updated.

    int iVQuantity = m_spkMesh->VBuffer->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        m_spkMesh->VBuffer->Position3(i) = m_pkVolume->GetPosition(
            m_akParameter[i].X(),m_akParameter[i].Y(),m_akParameter[i].Z());
    }
    m_spkMesh->VBuffer->Release();

    // Typical calls after modification, but not in this application.
    // m_spkMesh->UpdateModelBound();
    // m_spkMesh->UpdateModelNormals();  // if lighting affects mesh
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdatePolylines ()
{
    // update the polyline mesh
    VertexBuffer* pkVB;
    Polyline* pkPoly;
    int i0, i1, i2, i = 0;

    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity-1; i2++)
            {
                pkPoly = StaticCast<Polyline>(m_spkPolylineRoot->GetChild(i));
                pkVB = pkPoly->VBuffer;
                pkVB->Position3(0) = m_pkVolume->GetControlPoint(i0,i1,i2);
                pkVB->Position3(1) = m_pkVolume->GetControlPoint(i0,i1,i2+1);
                pkVB->Release();
                i++;
            }
        }

        for (i2 = 0; i2 < m_iQuantity; i2++)
        {
            for (i1 = 0; i1 < m_iQuantity-1; i1++)
            {
                pkPoly = StaticCast<Polyline>(m_spkPolylineRoot->GetChild(i));
                pkVB = pkPoly->VBuffer;
                pkVB->Position3(0) = m_pkVolume->GetControlPoint(i0,i1,i2);
                pkVB->Position3(1) = m_pkVolume->GetControlPoint(i0,i1+1,i2);
                pkVB->Release();
                i++;
            }
        }
    }

    for (i0 = 0; i0 < m_iQuantity-1; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                pkPoly = StaticCast<Polyline>(m_spkPolylineRoot->GetChild(i));
                pkVB = pkPoly->VBuffer;
                pkVB->Position3(0) = m_pkVolume->GetControlPoint(i0,i1,i2);
                pkVB->Position3(1) = m_pkVolume->GetControlPoint(i0+1,i1,i2);
                pkVB->Release();
                i++;
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdateControlBoxes ()
{
    int i0, i1, i2, i = 0;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                TriMesh* pkBox = StaticCast<TriMesh>(
                    m_spkControlRoot->GetChild(i));
                pkBox->Local.SetTranslate(
                    m_pkVolume->GetControlPoint(i0,i1,i2));
                i++;
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::DoRandomControlPoints ()
{
    // Randomly perturb the control points, but stay near the original
    // control points.
    int i0, i1, i2;
    Vector3f kCtrl;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        kCtrl.X() = m_fXMin + m_fDX*i0;
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            kCtrl.Y() = m_fYMin + m_fDY*i1;
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                kCtrl.Z() = m_fZMin + m_fDZ*i2;

                Vector3f kNewCtrl = m_pkVolume->GetControlPoint(i0,i1,i2) +
                    m_fAmplitude*Vector3f(Mathf::SymmetricRandom(),
                    Mathf::SymmetricRandom(),Mathf::SymmetricRandom());

                Vector3f kDiff = kNewCtrl - kCtrl;
                float fLength = kDiff.Length();
                if (fLength > m_fRadius)
                {
                    kDiff *= m_fRadius/fLength;
                }

                m_pkVolume->SetControlPoint(i0,i1,i2,kCtrl+kDiff);
            }
        }
    }

    UpdateMesh();
    UpdatePolylines();
    UpdateControlBoxes();
}
//----------------------------------------------------------------------------
