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

#include "RoughPlaneParticle1.h"

WM4_WINDOW_APPLICATION(RoughPlaneParticle1);

const int g_iSize = 256;

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneParticle1::RoughPlaneParticle1 ()
    :
    WindowApplication2("RoughPlaneParticle1",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_bContinueSolving = true;
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle1::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // set up the physics module
    m_kModule.Gravity = 10.0;
    m_kModule.Mass = 10.0;
    m_kModule.Friction = 1.0;
    m_kModule.Angle = 0.125*Mathd::PI;

    // initialize the differential equations
    double dTime = 0.0;
    double dDeltaTime = 1.0/60.0;
    double dX = 0.0;
    double dW = 0.0;
    double dXDot = 10.0;
    double dWDot = 40.0;
    m_kModule.Initialize(dTime,dDeltaTime,dX,dXDot,dW,dWDot);

    // initialize the coefficients for the viscous friction solution
    m_dR = m_kModule.Friction/m_kModule.Mass;
    m_dA0 = -dXDot/m_dR;
    m_dA1 = dX - m_dA0;
    m_dB1 = -m_kModule.Gravity*Mathd::Sin(m_kModule.Angle)/m_dR;
    m_dB2 = (dWDot + m_dR*dW - m_dB1)/m_dR;
    m_dB0 = dW - m_dB2;

    // save path of motion
    m_kVFPosition.push_back(GetVFPosition(dTime));
    m_kSFPosition.push_back(Vector2d(dX,dW));

    // use right-handed coordinates
    DoFlip(true);

    // mass drawing might extend outside the application window
    ClampToWindow() = true;

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneParticle1::OnIdle ()
{
#ifndef SINGLE_STEP
    if (m_bContinueSolving)
    {
        m_kModule.Update();
        if (m_kModule.GetX() > 0.0 && m_kModule.GetW() <= 0.0)
        {
            m_bContinueSolving = false;
            return;
        }

        m_kVFPosition.push_back(GetVFPosition(m_kModule.GetTime()));
        m_kSFPosition.push_back(Vector2d(m_kModule.GetX(),
            m_kModule.GetW()));
        OnDisplay();
    }
#endif
}
//----------------------------------------------------------------------------
void RoughPlaneParticle1::OnDisplay ()
{
    ClearScreen();

    Color kBlack(0,0,0), kGray(128,128,128);
    Color kBlue(0,0,128), kLBlue(0,0,255);
    const int iThick = 2;
    int iX0, iW0, iX1, iW1, i, iDx, iDw;
    Vector2d kPos;

    const double dXScale = 1.25;
    const double dWScale = 0.75;
    const int iWOffset = 96;

    // draw viscous friction path of motion
    kPos = m_kVFPosition[0];
    iX0 = (int)(dXScale*kPos.X()+0.5);
    iW0 = (int)(dWScale*kPos.Y()+0.5) + iWOffset;
    iX1 = iX0;
    iW1 = iW0;
    for (i = 1; i < (int)m_kVFPosition.size(); i++)
    {
        kPos = m_kVFPosition[i];
        iX1 = (int)(dXScale*kPos.X()+0.5);
        iW1 = (int)(dWScale*kPos.Y()+0.5) + iWOffset;
        DrawLine(iX0,iW0,iX1,iW1,kLBlue);
        iX0 = iX1;
        iW0 = iW1;
    }

    // draw mass
    for (iDw = -iThick; iDw <= iThick; iDw++)
    {
        for (iDx = -iThick; iDx <= iThick; iDx++)
        {
            SetPixel(iX1+iDx,iW1+iDw,kBlue);
        }
    }

    // draw static friction path of motion
    kPos = m_kSFPosition[0];
    iX0 = (int)(dXScale*kPos.X()+0.5);
    iW0 = (int)(dWScale*kPos.Y()+0.5) + iWOffset;
    iX1 = iX0;
    iW1 = iW0;
    for (i = 1; i < (int)m_kSFPosition.size(); i++)
    {
        kPos = m_kSFPosition[i];
        iX1 = (int)(dXScale*kPos.X()+0.5);
        iW1 = (int)(dWScale*kPos.Y()+0.5) + iWOffset;
        DrawLine(iX0,iW0,iX1,iW1,kGray);
        iX0 = iX1;
        iW0 = iW1;
    }

    // draw mass
    for (iDw = -iThick; iDw <= iThick; iDw++)
    {
        for (iDx = -iThick; iDx <= iThick; iDx++)
            SetPixel(iX1+iDx,iW1+iDw,kBlack);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle1::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        if (m_bContinueSolving)
        {
            m_kModule.Update();
            if (m_kModule.GetX() > 0.0 && m_kModule.GetW() <= 0.0)
            {
                m_bContinueSolving = false;
                return true;
            }
            m_kVFPosition.push_back(GetVFPosition(m_kModule.GetTime()));
            m_kSFPosition.push_back(Vector2d(m_kModule.GetX(),
                m_kModule.GetW()));
            OnDisplay();
        }
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
Vector2d RoughPlaneParticle1::GetVFPosition (double dTime)
{
    Vector2d kPos;

    double dExp = Mathd::Exp(-m_dR*dTime);
    kPos.X() = m_dA0*dExp + m_dA1;
    kPos.Y() = m_dB0*dExp + m_dB1*dTime + m_dB2;

    return kPos;
}
//----------------------------------------------------------------------------
