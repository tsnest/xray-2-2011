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

#include "KeplerPolarForm.h"

WM4_WINDOW_APPLICATION(KeplerPolarForm);

const int g_iSize = 256;

//----------------------------------------------------------------------------
KeplerPolarForm::KeplerPolarForm ()
    :
    WindowApplication2("KeplerPolarForm",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool KeplerPolarForm::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // set up the physics module
    m_kModule.Gravity = 10.0;
    m_kModule.Mass = 1.0;

    double dTime = 0.0;
    double dDeltaTime = 0.01;
    double dRadius = 10.0;
    double dTheta = 0.25*Mathd::PI;
    double dRadiusDot = 0.1;
    double dThetaDot = 0.1;
    m_kModule.Initialize(dTime,dDeltaTime,dRadius,dTheta,dRadiusDot,
        dThetaDot);

    const int iMax = (int)(m_kModule.GetPeriod()/dDeltaTime);
    m_kPosition.resize(iMax);
    for (int i = 0; i < iMax; i++)
    {
        double dX = 0.5*g_iSize + 10.0*dRadius*Mathd::Cos(dTheta);
        double dY = 0.5*g_iSize + 10.0*dRadius*Mathd::Sin(dTheta);
        m_kPosition[i] = Vector2d(dX,dY);

        m_kModule.Update();

        dTime = m_kModule.GetTime();
        dRadius = m_kModule.GetRadius();
        dRadiusDot = m_kModule.GetRadiusDot();
        dTheta = m_kModule.GetTheta();
        dThetaDot = m_kModule.GetThetaDot();
    }

    // All drawing is in flipped y-values to show the objects in right-handed
    // coordinates.
    DoFlip(true);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void KeplerPolarForm::OnDisplay ()
{
    ClearScreen();

    const int iHSize = g_iSize/2, iSizeM1 = g_iSize-1;

    // Draw the coordinate axes.
    Color kGray(192,192,192);
    DrawLine(0,iHSize,iSizeM1,iHSize,kGray);
    DrawLine(iHSize,0,iHSize,iSizeM1,kGray);

    // Draw a ray from the Sun's location to the initial point.
    int iX = (int)(m_kPosition[1].X() + 0.5);
    int iY = (int)(m_kPosition[1].Y() + 0.5);
    DrawLine(iHSize,iHSize,iX,iY,kGray);

    // Draw the Sun's location.  The Sun is at the origin which happens to
    // be a focal point of the ellipse.
    const int iThick = 1;
    Color kRed(255,0,0);
    for (int iDY = -iThick; iDY <= iThick; iDY++)
    {
        for (int iDX = -iThick; iDX <= iThick; iDX++)
        {
            SetPixel(iHSize+iDX,iHSize+iDY,kRed);
        }
    }

    // Draw Earth's orbit.  The orbit starts in green, finishes in blue, and
    // is a blend of the two colors between.
    int iPSize = (int)m_kPosition.size();
    float fInvPSize = 1.0f/iPSize;
    for (int i = 1; i < iPSize; i++)
    {
        float fW = i*fInvPSize, fOmW = 1.0f - fW;
        unsigned char ucB = (unsigned char)(255.0f*fOmW);
        unsigned char ucG = (unsigned char)(255.0f*fW);
        iX = (int)(m_kPosition[i].X() + 0.5);
        iY = (int)(m_kPosition[i].Y() + 0.5);
        SetPixel(iX,iY,Color(0,ucG,ucB));
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
