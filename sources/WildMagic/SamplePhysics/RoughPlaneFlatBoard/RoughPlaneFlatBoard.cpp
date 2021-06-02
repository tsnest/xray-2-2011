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

#include "RoughPlaneFlatBoard.h"

WM4_WINDOW_APPLICATION(RoughPlaneFlatBoard);

const int g_iSize = 256;

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneFlatBoard::RoughPlaneFlatBoard ()
    :
    WindowApplication2("RoughPlaneFlatBoard",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool RoughPlaneFlatBoard::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // set up the physics module
    m_kModule.MuGravity = 5.0;
    m_kModule.XLocExt = 16.0;
    m_kModule.YLocExt = 8.0;

    // initialize the differential equations
    double dTime = 0.0;
    double dDeltaTime = 1.0/60.0;
    double dX = 20.0;
    double dY = 230.0;
    double dTheta = 0.25*Mathd::PI;
    double dXDer = 30.0;
    double dYDer = -30.0;
    double dThetaDer = 4.0;
    m_kModule.Initialize(dTime,dDeltaTime,dX,dY,dTheta,dXDer,dYDer,dThetaDer);

    // use right-handed coordinates
    DoFlip(true);

    // drawing might extend outside the application window
    ClampToWindow() = true;

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneFlatBoard::OnIdle ()
{
#ifndef SINGLE_STEP
    m_kModule.Update();
    OnDisplay();
#endif
}
//----------------------------------------------------------------------------
void RoughPlaneFlatBoard::OnDisplay ()
{
    ClearScreen();

    Color kBlack(0,0,0);

    // draw board
    double dX00, dY00, dX10, dY10, dX11, dY11, dX01, dY01;
    m_kModule.GetRectangle(dX00,dY00,dX10,dY10,dX11,dY11,dX01,dY01);
    int iX00 = (int)(dX00+0.5);
    int iY00 = (int)(dY00+0.5);
    int iX10 = (int)(dX10+0.5);
    int iY10 = (int)(dY10+0.5);
    int iX11 = (int)(dX11+0.5);
    int iY11 = (int)(dY11+0.5);
    int iX01 = (int)(dX01+0.5);
    int iY01 = (int)(dY01+0.5);
    DrawLine(iX00,iY00,iX10,iY10,kBlack);
    DrawLine(iX10,iY10,iX11,iY11,kBlack);
    DrawLine(iX11,iY11,iX01,iY01,kBlack);
    DrawLine(iX01,iY01,iX00,iY00,kBlack);

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool RoughPlaneFlatBoard::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

#ifdef SINGLE_STEP
    switch (ucKey)
    {
    case 'g':
    case 'G':
        m_kModule.Update();
        OnDisplay();
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
