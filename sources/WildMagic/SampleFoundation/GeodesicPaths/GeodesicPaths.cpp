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
// Version: 4.0.0 (2006/06/28)

#include "GeodesicPaths.h"
using namespace Wm4;

WM4_WINDOW_APPLICATION(GeodesicPaths);
const int g_iSize = 512;

//----------------------------------------------------------------------------
GeodesicPaths::GeodesicPaths ()
    :
    WindowApplication2("GeodesicPaths",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f)),
    m_kEG(1.0f,1.0f,1.0f)
{
    m_kEG.RefineCallback = &GeodesicPaths::RefineCallback;

    m_iTrueQuantity = 129;
    m_akTruePoint = WM4_NEW GVectorf[m_iTrueQuantity];
    int i;
    for (i = 0; i < m_iTrueQuantity; i++)
    {
        m_akTruePoint[i].SetSize(2);
    }

    m_iApprQuantity = (1 << m_kEG.Subdivisions) + 1;
    m_akApprPoint = WM4_NEW GVectorf[m_iApprQuantity];
    for (i = 0; i < m_iApprQuantity; i++)
    {
        m_akApprPoint[i].SetSize(2);
    }

    m_kParam0.SetSize(2);
    m_kParam1.SetSize(2);
}
//----------------------------------------------------------------------------
GeodesicPaths::~GeodesicPaths ()
{
    WM4_DELETE[] m_akApprPoint;
    WM4_DELETE[] m_akTruePoint;
}
//----------------------------------------------------------------------------
bool GeodesicPaths::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Only process the first octant of the ellipsoid.
    m_fXMin = 0.0f;
    m_fXMax = Mathf::HALF_PI;
    m_fXDelta = (m_fXMax - m_fXMin)/(float)g_iSize;
    m_fYMin = Mathf::HALF_PI/(float)g_iSize;
    m_fYMax = Mathf::HALF_PI;
    m_fYDelta = (m_fYMax - m_fYMin)/(float)g_iSize;

    ComputeTruePath();

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void GeodesicPaths::OnTerminate ()
{
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void GeodesicPaths::OnDisplay ()
{
    ClearScreen();

    int i, iX0, iY0, iX1, iY1;

    // draw true path
    ParamToXY(m_akTruePoint[0],iX0,iY0);
    for (i = 1; i < m_iTrueQuantity; i++)
    {
        ParamToXY(m_akTruePoint[i],iX1,iY1);
        DrawLine(iX0,iY0,iX1,iY1,Color(0,255,0));
        iX0 = iX1;
        iY0 = iY1;
    }

    // draw approximate path
    int iApprQuantity = m_kEG.GetCurrentQuantity();
    if (iApprQuantity == 0)
    {
        iApprQuantity = m_iCurrApprQuantity;
    }

    ParamToXY(m_akApprPoint[0],iX0,iY0);
    for (i = 1; i < iApprQuantity; i++)
    {
        ParamToXY(m_akApprPoint[i],iX1,iY1);
        DrawLine(iX0,iY0,iX1,iY1,Color(255,0,0));
        iX0 = iX1;
        iY0 = iY1;
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
void GeodesicPaths::ScreenOverlay ()
{
    const size_t uiSize = 512;
    char acMsg[512];
    System::Sprintf(acMsg,uiSize,
        "true dist = %f, appr dist = %f, appr curv = %f",
        m_fTrueDistance,m_fApprDistance,m_fApprCurvature);

    m_pkRenderer->Draw(8,16,ColorRGBA::BLACK,acMsg);

    System::Sprintf(acMsg,uiSize,"sub = %d, ref = %d, currquan = %d",
        m_kEG.GetSubdivisionStep(),m_kEG.GetRefinementStep(),
        m_kEG.GetCurrentQuantity());

    m_pkRenderer->Draw(8,32,ColorRGBA::BLACK,acMsg);
}
//----------------------------------------------------------------------------
bool GeodesicPaths::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case '0':
        ComputeTruePath();
        OnDisplay();
        return true;
    case '1':
        ComputeApprPath(true);
        OnDisplay();
        return true;
    case '2':
        ComputeApprPath(false);
        OnDisplay();
        return true;
    case '3':
        WM4_DELETE[] m_akApprPoint;
        m_kEG.ComputeGeodesic(m_kParam0,m_kParam1,m_iCurrApprQuantity,
            m_akApprPoint);
        ComputeApprLength();
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void GeodesicPaths::ComputeTruePath ()
{
    // Random selection of end points.  The angles are (theta,phi) with
    // 0 <= theta < 2*pi and 0 <= phi < pi/2, thus placing the points on the
    // the first octant of the ellipsoid.
    m_kParam0[0] = Mathf::IntervalRandom(0.0f,Mathf::HALF_PI);
    m_kParam0[1] = Mathf::IntervalRandom(0.0f,Mathf::HALF_PI);
    m_kParam1[0] = Mathf::IntervalRandom(0.0f,Mathf::HALF_PI);
    m_kParam1[1] = Mathf::IntervalRandom(0.0f,Mathf::HALF_PI);

    // Compute the true geodesic path.
    Vector3f kP0 = m_kEG.ComputePosition(m_kParam0);
    Vector3f kP1 = m_kEG.ComputePosition(m_kParam1);
    float fAngle = Mathf::ACos(kP0.Dot(kP1));
    for (int i = 0; i < m_iTrueQuantity; i++)
    {
        float fT = i/(float)(m_iTrueQuantity-1);
        float fSin0 = Mathf::Sin((1.0f-fT)*fAngle);
        float fSin1 = Mathf::Sin(fT*fAngle);
        float fSin = Mathf::Sin(fAngle);
        Vector3f kP = (fSin0*kP0 + fSin1*kP1)/fSin;
        m_akTruePoint[i][0] = Mathf::ATan2(kP.Y(),kP.X());
        m_akTruePoint[i][1] = Mathf::ACos(kP.Z());
    }

    // Compute the true length of the geodesic path.
    m_fTrueDistance = fAngle;

    // Initialize the approximate path.
    m_iCurrApprQuantity = 2;
    m_akApprPoint[0] = m_kParam0;
    m_akApprPoint[1] = m_kParam1;
    ComputeApprLength();
}
//----------------------------------------------------------------------------
void GeodesicPaths::ComputeApprPath (bool bSubdivide)
{
    int i;

    if (bSubdivide)
    {
        int iNewApprQuantity = 2*m_iCurrApprQuantity-1;
        if (iNewApprQuantity > m_iApprQuantity)
        {
            return;
        }

        // Copy the old points so that there are slots for the midpoints
        // during the subdivision interleaved between the old points.
        for (i = m_iCurrApprQuantity-1; i > 0; i--)
        {
            m_akApprPoint[2*i] = m_akApprPoint[i];
        }

        for (i = 0; i <= m_iCurrApprQuantity-2; i++)
        {
            m_kEG.Subdivide(m_akApprPoint[2*i],m_akApprPoint[2*i+1],
                m_akApprPoint[2*i+2]);
        }

        m_iCurrApprQuantity = iNewApprQuantity;
    }
    else // refine
    {
        for (i = 1; i <= m_iCurrApprQuantity-2; i++)
        {
            m_kEG.Refine(m_akApprPoint[i-1],m_akApprPoint[i],
                m_akApprPoint[i+1]);
        }
    }

    ComputeApprLength();
}
//----------------------------------------------------------------------------
void GeodesicPaths::ComputeApprLength ()
{
    int iApprQuantity = m_kEG.GetCurrentQuantity();
    if (iApprQuantity == 0)
    {
        iApprQuantity = m_iCurrApprQuantity;
    }

    m_fApprDistance = m_kEG.ComputeTotalLength(iApprQuantity,m_akApprPoint);
    m_fApprCurvature = m_kEG.ComputeTotalCurvature(iApprQuantity,
        m_akApprPoint);
}
//----------------------------------------------------------------------------
void GeodesicPaths::ParamToXY (const GVectorf& rkParam, int& riX, int& riY)
{
    // Only the first octant of the ellipsoid is used.
    riX = (int)((rkParam[0] - m_fXMin)/m_fXDelta + 0.5f);
    riY = (int)((rkParam[1] - m_fYMin)/m_fYDelta + 0.5f);
}
//----------------------------------------------------------------------------
void GeodesicPaths::XYToParam (int iX, int iY, GVectorf& rkParam)
{
    rkParam[0] = m_fXMin + iX*m_fXDelta;
    rkParam[1] = m_fYMin + iY*m_fYDelta;
}
//----------------------------------------------------------------------------
void GeodesicPaths::RefineCallback ()
{
    GeodesicPaths* pkApp = (GeodesicPaths*)TheApplication;
    pkApp->ComputeApprLength();
    pkApp->OnDisplay();
}
//----------------------------------------------------------------------------
