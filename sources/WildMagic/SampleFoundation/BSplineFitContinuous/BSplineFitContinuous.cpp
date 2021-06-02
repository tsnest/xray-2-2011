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

#include "BSplineFitContinuous.h"

WM4_WINDOW_APPLICATION(BSplineFitContinuous);

//----------------------------------------------------------------------------
BSplineFitContinuous::BSplineFitContinuous ()
    :
    WindowApplication3("BSplineFitContinuous",0,0,512,512,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_iDegree = 3;
}
//----------------------------------------------------------------------------
bool BSplineFitContinuous::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // create model
    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);

    // get control points for input curve
    std::ifstream kIStr(System::GetPath("ControlPoints.txt",System::SM_READ));
    int iCtrlQuantity = 0;
    kIStr >> iCtrlQuantity;
    Vector3d* akCtrl = WM4_NEW Vector3d[iCtrlQuantity];
    for (int i = 0; i < iCtrlQuantity; i++)
    {
        kIStr >> akCtrl[i].X();
        kIStr >> akCtrl[i].Y();
        kIStr >> akCtrl[i].Z();
    }

    // create polyline connecting control points of input B-spline curve
    double dFraction = 0.10;
    Polyline* pkPoly = OriginalPolyline(iCtrlQuantity,akCtrl);
    m_spkTrnNode->AttachChild(pkPoly);

    // create polyline that approximates the reduced B-spline curve
    pkPoly = ReducedPolyline(iCtrlQuantity,akCtrl,dFraction);
    m_spkTrnNode->AttachChild(pkPoly);
    WM4_DELETE[] akCtrl;

    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());

    // sample camera code
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10000.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(10.0f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void BSplineFitContinuous::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTrnNode = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineFitContinuous::OnIdle ()
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
Polyline* BSplineFitContinuous::OriginalPolyline (int iCtrlQuantity,
    Vector3d* akCtrl)
{
    BSplineCurve3d kSpline(iCtrlQuantity,akCtrl,m_iDegree,false,true);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iCtrlQuantity);
    for (int i = 0; i < iCtrlQuantity; i++)
    {
        double dTime = i/(double)iCtrlQuantity;
        Vector3d kPos = kSpline.GetPosition(dTime);
        pkVBuffer->Position3(i).X() = (float)kPos.X();
        pkVBuffer->Position3(i).Y() = (float)kPos.Y();
        pkVBuffer->Position3(i).Z() = (float)kPos.Z();
        pkVBuffer->Color3(0,i) = ColorRGB(1.0f,0.0f,0.0f);
    }

    Polyline* pkPLine = WM4_NEW Polyline(pkVBuffer,false,true);
    pkPLine->AttachEffect(WM4_NEW VertexColor3Effect);
    return pkPLine;
}
//----------------------------------------------------------------------------
Polyline* BSplineFitContinuous::ReducedPolyline (int iCtrlQuantity,
    Vector3d* akCtrl, double dFraction)
{
    int iLSCtrlQuantity;
    Vector3d* akLSCtrl;
    BSplineReduction3d(iCtrlQuantity,akCtrl,m_iDegree,dFraction,
        iLSCtrlQuantity,akLSCtrl);

    BSplineCurve3d kSpline(iLSCtrlQuantity,akLSCtrl,m_iDegree,false,true);
    WM4_DELETE[] akLSCtrl;

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iCtrlQuantity);
    for (int i = 0; i < iCtrlQuantity; i++)
    {
        double dTime = i/(double)iCtrlQuantity;
        Vector3d kPos = kSpline.GetPosition(dTime);
        pkVBuffer->Position3(i).X() = (float)kPos.X();
        pkVBuffer->Position3(i).Y() = (float)kPos.Y();
        pkVBuffer->Position3(i).Z() = (float)kPos.Z();
        pkVBuffer->Color3(0,i) = ColorRGB(0.0f,0.0f,1.0f);
    }

    Polyline* pkPLine = WM4_NEW Polyline(pkVBuffer,false,true);
    pkPLine->AttachEffect(WM4_NEW VertexColor3Effect);
    return pkPLine;
}
//----------------------------------------------------------------------------
