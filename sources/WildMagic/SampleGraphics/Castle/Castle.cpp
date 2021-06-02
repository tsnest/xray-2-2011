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
// Version: 4.0.2 (2007/09/24)

#include "Castle.h"

WM4_WINDOW_APPLICATION(Castle);

//----------------------------------------------------------------------------
Castle::Castle ()
    :
    WindowApplication3("Castle",0,0,640,480,ColorRGBA(0.9f,0.9f,0.9f,1.0f))
{
    // The location of the wmif and wmof files for this application.
    System::InsertDirectory("Data");

    m_fVerticalDistance = 5.0f;
    m_acPicked[0] = 0;

    // generate pick ray information
    m_iNumRays = 5;
    m_fHalfAngle = 0.25f*Mathf::PI;
    m_afCos = WM4_NEW float[m_iNumRays];
    m_afSin = WM4_NEW float[m_iNumRays];
    m_afTolerance = WM4_NEW float[m_iNumRays];
    float fMult = 1.0f/(m_iNumRays/2);
    for (int i = 0; i < m_iNumRays; i++)
    {
        float fUnit = i*fMult - 1.0f;  // in [-1,1]
        float fAngle = Mathf::HALF_PI + m_fHalfAngle*fUnit;
        m_afCos[i] = Mathf::Cos(fAngle);
        m_afSin[i] = Mathf::Sin(fAngle);
        m_afTolerance[i] = 2.0f - 1.5f*Mathf::FAbs(fUnit);  // in [1/2,1]
    }
}
//----------------------------------------------------------------------------
Castle::~Castle ()
{
    WM4_DELETE[] m_afCos;
    WM4_DELETE[] m_afSin;
    WM4_DELETE[] m_afTolerance;
}
//----------------------------------------------------------------------------
bool Castle::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Load the castle data set.
    Stream kIStream;
    kIStream.Load(System::GetPath("Castle.wmof",System::SM_READ));
    m_spkModel = (Node*)kIStream.GetObjectAt(0);
    SetAllWireframe(m_spkModel);

    m_spkScene = WM4_NEW Node;
    Node* pkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(pkTrnNode);
    pkTrnNode->AttachChild(m_spkModel);

    // center-and-fit (based on old castle scene)
    Vector3f kWCenter(1.3778250f,-0.70154405f,2205.9973f);
    float fWRadius = 5561.5498f;
    pkTrnNode->Local.SetTranslate(-kWCenter);

    // set up camera
    float fNear = 1.0f;
    float fFar = 8.0f*fWRadius;
    float fRight = 0.44f*fNear;
    float fLeft = -fRight;
    float fTop = 0.4125f*fNear;
    float fBottom = -fTop;
    m_spkCamera->SetFrustum(fLeft,fRight,fBottom,fTop,fNear,fFar);
    Vector3f kCLoc(527.394f,86.8992f,-2136.00f);
    Vector3f kCDir(1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // Initial culling of scene.  Resize the visible set to a reasonable
    // size for the scene.  The scene is known to have 494 leaf nodes.
    m_kCuller.GetVisibleSet().Resize(512,512);
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(1.0f,0.01f);
    InitializeObjectMotion(m_spkScene);
    MoveForward();
    return true;
}
//----------------------------------------------------------------------------
void Castle::OnTerminate ()
{
    m_kPicker.Records.clear();
    m_spkScene = 0;
    m_spkModel = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Castle::OnIdle ()
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
        m_pkRenderer->Draw(320,GetHeight()-8,ColorRGBA::WHITE,m_acPicked);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Castle::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':
    case 'W':
        ToggleAllWireframe(m_spkModel);
        return true;
    case '+':
    case '=':
        m_fVerticalDistance += 0.1f;
        AdjustVerticalDistance();
        return true;
    case '-':
    case '_':
        m_fVerticalDistance -= 0.1f;
        AdjustVerticalDistance();
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkModel,128,128,640,480,"Castle.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Castle::OnMouseClick (int iButton, int iState, int iX, int iY,
    unsigned int)
{
    if (iState != MOUSE_DOWN)
    {
        return false;
    }

    // Do a picking operation.
    Vector3f kPos, kDir;
    m_spkCamera->GetPickRay(iX,iY,GetWidth(),GetHeight(),kPos,kDir);
    m_kPicker.Execute(m_spkScene,kPos,kDir,0.0f,Mathf::MAX_REAL);

    if (m_kPicker.Records.size() > 0)
    {
        const PickRecord& rkRecord = m_kPicker.GetClosestNonnegative();
        Spatial* pkIntersected = rkRecord.Intersected;

        // display the selected object's name
        if (pkIntersected->GetName().size() > 0)
        {
            System::Strcpy(m_acPicked,PICKED_SIZE,
                pkIntersected->GetName().c_str());
        }
        else
        {
            System::Strcpy(m_acPicked,PICKED_SIZE,"unnamed object");
        }

        if (iButton == MOUSE_RIGHT_BUTTON)
        {
            // toggle the selected object's wireframe
            WireframeState* pkWireframe = StaticCast<WireframeState>(
                pkIntersected->GetGlobalState(GlobalState::WIREFRAME));
            assert(pkWireframe);
            pkWireframe->Enabled = !pkWireframe->Enabled;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Castle::AdjustVerticalDistance ()
{
    // Retain vertical distance above "ground".
    Vector3f kPos = m_spkCamera->GetLocation();
    Vector3f kDir = -Vector3f::UNIT_Z;
    m_kPicker.Execute(m_spkScene,kPos,kDir,0.0f,Mathf::MAX_REAL);

    if (m_kPicker.Records.size() > 0)
    {
        const PickRecord& rkRecord = m_kPicker.GetClosestNonnegative();
        TriMesh* pkMesh = DynamicCast<TriMesh>(rkRecord.Intersected);
        assert(pkMesh);

        Triangle3f kTri;
        pkMesh->GetWorldTriangle(rkRecord.Triangle,kTri);
        Vector3f kClosest = rkRecord.B0*kTri.V[0] + rkRecord.B1*kTri.V[1] +
            rkRecord.B2*kTri.V[2];

        kClosest.Z() += m_fVerticalDistance;
        m_spkCamera->SetLocation(kClosest);
    }
}
//----------------------------------------------------------------------------
bool Castle::AllowMotion (float fSign)
{
    // Take a step forward or backward, depending on sign.  Check if objects
    // are far enough away.  If so keep the step.  If not, stay where you are.
    Vector3f kPos = m_spkCamera->GetLocation()
        + fSign*m_fTrnSpeed*m_akWorldAxis[0]
        - 0.5f*m_fVerticalDistance*m_akWorldAxis[1];

    for (int i = 0; i < m_iNumRays; i++)
    {
        Vector3f kDir = m_afCos[i]*m_akWorldAxis[2] +
            fSign*m_afSin[i]*m_akWorldAxis[0];

        m_kPicker.Execute(m_spkScene,kPos,kDir,0.0f,Mathf::MAX_REAL);
        if (m_kPicker.Records.size() > 0)
        {
            const PickRecord& rkRecord = m_kPicker.GetClosestNonnegative();
            if (rkRecord.T <= m_afTolerance[i])
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Castle::MoveForward ()
{
    if (AllowMotion(1.0f))
    {
        WindowApplication3::MoveForward();
        AdjustVerticalDistance();
    }
}
//----------------------------------------------------------------------------
void Castle::MoveBackward ()
{
    if (AllowMotion(-1.0f))
    {
        WindowApplication3::MoveBackward();
        AdjustVerticalDistance();
    }
}
//----------------------------------------------------------------------------
void Castle::SetAllWireframe (Spatial* pkSpatial)
{
    TriMesh* pkMesh = DynamicCast<TriMesh>(pkSpatial);
    if (pkMesh)
    {
        pkMesh->AttachGlobalState(WM4_NEW WireframeState);
        return;
    }

    Node* pkNode = DynamicCast<Node>(pkSpatial);
    assert(pkNode);
    for (int i = 0; i < pkNode->GetQuantity(); i++)
    {
        Spatial* pkChild = pkNode->GetChild(i);
        if (pkChild)
        {
            SetAllWireframe(pkChild);
        }
    }
}
//----------------------------------------------------------------------------
void Castle::ToggleAllWireframe (Spatial* pkSpatial)
{
    TriMesh* pkMesh = DynamicCast<TriMesh>(pkSpatial);
    if (pkMesh)
    {
        WireframeState* pkWireframe = StaticCast<WireframeState>(
            pkMesh->GetGlobalState(GlobalState::WIREFRAME));
        assert(pkWireframe);
        pkWireframe->Enabled = !pkWireframe->Enabled;
        return;
    }

    Node* pkNode = DynamicCast<Node>(pkSpatial);
    assert(pkNode);
    for (int i = 0; i < pkNode->GetQuantity(); i++)
    {
        Spatial* pkChild = pkNode->GetChild(i);
        if (pkChild)
        {
            ToggleAllWireframe(pkChild);
        }
    }
}
//----------------------------------------------------------------------------
