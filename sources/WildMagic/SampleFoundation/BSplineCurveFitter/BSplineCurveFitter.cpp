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
// Version: 4.0.2 (2008/09/07)

#include "BSplineCurveFitter.h"

WM4_WINDOW_APPLICATION(BSplineCurveFitter);

//----------------------------------------------------------------------------
BSplineCurveFitter::BSplineCurveFitter ()
    :
    WindowApplication3("BSplineCurveFitter",0,0,640,480,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_pkBSpline = 0;
    m_akSample = 0;
}
//----------------------------------------------------------------------------
bool BSplineCurveFitter::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc(0.0f,0.0f,-4.0f);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);
    m_kCuller.SetCamera(m_spkCamera);

    CreateScene();

    InitializeCameraMotion(0.1f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::OnTerminate ()
{
    WM4_DELETE m_pkBSpline;
    m_spkScene = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::OnIdle ()
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
        m_pkRenderer->Draw(8,16,ColorRGBA::BLACK,m_acMessage);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BSplineCurveFitter::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'd':  // reduce the degree
        if (m_iDegree > 1)
        {
            m_iDegree--;
            CreateBSplinePolyline();
        }
        return true;

    case 'D':  // increase the degree
        m_iDegree++;
        CreateBSplinePolyline();
        return true;

    case 's':  // (small) reduce the number of control points by 1
        if (m_iCtrlQuantity - 1 > m_iDegree+1)
        {
            m_iCtrlQuantity--;
            CreateBSplinePolyline();
        }
        return true;

    case 'S':  // (small) increase the number of control points by 1
        if (m_iCtrlQuantity + 1 < m_iSampleQuantity)
        {
            m_iCtrlQuantity++;
            CreateBSplinePolyline();
        }
        return true;

    case 'm':  // (medium) reduce the number of control points by 10
        if (m_iCtrlQuantity - 10 > m_iDegree+1)
        {
            m_iCtrlQuantity -= 10;
            CreateBSplinePolyline();
        }
        return true;

    case 'M':  // (medium) increase the number of control points by 10
        if (m_iCtrlQuantity + 10 < m_iSampleQuantity)
        {
            m_iCtrlQuantity += 10;
            CreateBSplinePolyline();
        }
        return true;

    case 'l':  // (large) reduce the number of control points by 100
        if (m_iCtrlQuantity - 100 > m_iDegree+1)
        {
            m_iCtrlQuantity -= 100;
            CreateBSplinePolyline();
        }
        return true;

    case 'L':  // (large) increase the number of control points by 100
        if (m_iCtrlQuantity + 100 < m_iSampleQuantity)
        {
            m_iCtrlQuantity += 100;
            CreateBSplinePolyline();
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::CreateScene ()
{
    m_spkScene = WM4_NEW Node;

    // Generate a spiral curve on a sphere and visualize as a randomly
    // colored polyline.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    m_iSampleQuantity = 1000;
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,m_iSampleQuantity);
    m_akSample = WM4_NEW Vector3f[m_iSampleQuantity];
    float fMult = 2.0f/(m_iSampleQuantity - 1), fT;
    int i;
    for (i = 0; i < m_iSampleQuantity; i++)
    {
        fT = -1.0f + fMult*i;
        float fAngle = 2.0f*Mathf::TWO_PI*fT;
        float fAmplitude = 1.0f - fT*fT;

        m_akSample[i].X() = fAmplitude*Mathf::Cos(fAngle);
        m_akSample[i].Y() = fAmplitude*Mathf::Sin(fAngle);
        m_akSample[i].Z() = fT;
        m_akSample[i].Normalize();
        pkVBuffer->Position3(i) = m_akSample[i];
        pkVBuffer->Color3(0,i).R() = Mathf::UnitRandom();
        pkVBuffer->Color3(0,i).G() = Mathf::UnitRandom();
        pkVBuffer->Color3(0,i).B() = Mathf::UnitRandom();
    }

    m_spkEffect = WM4_NEW VertexColor3Effect;

    Polyline* pkPLine = WM4_NEW Polyline(pkVBuffer,false,true);
    pkPLine->AttachEffect(m_spkEffect);
    m_spkScene->AttachChild(pkPLine);

    // Generate a least-squares fit to the spiral.  The degree and number of
    // control points can be modified during run time.
    m_iDegree = 3;
    m_iCtrlQuantity = m_iSampleQuantity/2;
    CreateBSplinePolyline();
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::CreateBSplinePolyline ()
{
    WM4_DELETE m_pkBSpline;

    // Create the curve from the current parameters.
    m_pkBSpline = WM4_NEW BSplineCurveFitf(3,m_iSampleQuantity,
        (const float*)m_akSample,m_iDegree,m_iCtrlQuantity);

    // Sample it the same number of times as the original data.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,m_iSampleQuantity);
    float fMult = 1.0f/(m_iSampleQuantity - 1);
    int i;
    for (i = 0; i < m_iSampleQuantity; i++)
    {
        float fT = fMult*i;
        m_pkBSpline->GetPosition(fT,(float*)pkVBuffer->Position3(i));
    }

    // Compute error measurements.
    float fAvrError = 0.0f;
    float fRMSError = 0.0f;
    for (i = 0; i < m_iSampleQuantity; i++)
    {
        Vector3f kDiff = m_akSample[i] - pkVBuffer->Position3(i);
        float fSqrLength = kDiff.SquaredLength();
        fRMSError += fSqrLength;
        float fLength = Mathf::Sqrt(fSqrLength);
        fAvrError += fLength;
    }
    fAvrError /= (float)m_iSampleQuantity;
    fRMSError /= (float)m_iSampleQuantity;
    fRMSError = Mathf::Sqrt(fRMSError);

    Polyline* pkPLine = WM4_NEW Polyline(pkVBuffer,false,true);
    pkPLine->AttachEffect(m_spkEffect);
    m_spkScene->SetChild(1,pkPLine);

    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();
    m_kCuller.ComputeVisibleSet(m_spkScene);

    System::Sprintf(m_acMessage,MESSAGE_SIZE,
        "samples = %d, degree = %d, controls = %d, avr error = %f, "
        "rms error = %f",m_iSampleQuantity,m_iDegree,m_iCtrlQuantity,
        fAvrError,fRMSError);
}
//----------------------------------------------------------------------------
