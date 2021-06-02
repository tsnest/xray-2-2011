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

#include "RoughPlaneParticle2.h"

WM4_WINDOW_APPLICATION(RoughPlaneParticle2);

const int g_iSize = 256;

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneParticle2::RoughPlaneParticle2 ()
    :
    WindowApplication2("RoughPlaneParticle2",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_iIteration = 0;
    m_iMaxIteration = 512;
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle2::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // set up the physics module
    m_kModule.Gravity = 10.0;
    m_kModule.Mass1 = 10.0;
    m_kModule.Mass2 = 20.0;
    m_kModule.Friction1 = 1.0;
    m_kModule.Friction2 = 1.0;

    // initialize the differential equations
    double dTime = 0.0;
    double dDeltaTime = 1.0/60.0;
    double dX1 = 16.0;
    double dY1 = 116.0;
    double dX2 = 100.0;
    double dY2 = 200.0;
    double dXDot = 10.0;
    double dYDot = -10.0;
    double dThetaDot = 0.5;
    m_kModule.Initialize(dTime,dDeltaTime,dX1,dY1,dX2,dY2,dXDot,dYDot,
        dThetaDot);

    // use right-handed coordinates
    DoFlip(true);

    // drawing might extend outside the application window
    ClampToWindow() = true;

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneParticle2::OnIdle ()
{
#ifndef SINGLE_STEP
    if (m_iIteration < m_iMaxIteration)
    {
        m_kModule.Update();
        OnDisplay();
        m_iIteration++;
    }
#endif
}
//----------------------------------------------------------------------------
void RoughPlaneParticle2::OnDisplay ()
{
    ClearScreen();

    Color kBlack(0,0,0), kGray(128,128,128), kBlue(0,0,255);
    const int iThick = 2;
    int iDx, iDy;

    // draw rod
    double dX1, dY1, dX2, dY2;
    m_kModule.Get(dX1,dY1,dX2,dY2);
    int iX1 = (int)(dX1+0.5);
    int iY1 = (int)(dY1+0.5);
    int iX2 = (int)(dX2+0.5);
    int iY2 = (int)(dY2+0.5);
    DrawLine(iX1,iY1,iX2,iY2,kGray);

    // draw masses
    for (iDy = -iThick; iDy <= iThick; iDy++)
    {
        for (iDx = -iThick; iDx <= iThick; iDx++)
        {
            SetPixel(iX1+iDx,iY1+iDy,kBlack);
        }
    }
    for (iDy = -iThick; iDy <= iThick; iDy++)
    {
        for (iDx = -iThick; iDx <= iThick; iDx++)
        {
            SetPixel(iX2+iDx,iY2+iDy,kBlack);
        }
    }

    // draw center of mass
    int iX = (int)(m_kModule.GetX()+0.5);
    int iY = (int)(m_kModule.GetY()+0.5);
    for (iDy = -iThick; iDy <= iThick; iDy++)
    {
        for (iDx = -iThick; iDx <= iThick; iDx++)
        {
            SetPixel(iX+iDx,iY+iDy,kBlue);
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle2::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        if (m_iIteration < m_iMaxIteration)
        {
            m_kModule.Update();
            OnDisplay();
            m_iIteration++;
        }
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
