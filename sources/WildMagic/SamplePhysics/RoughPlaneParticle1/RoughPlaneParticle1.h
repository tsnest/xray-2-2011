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

#ifndef ROUGHPLANEPARTICLE1_H
#define ROUGHPLANEPARTICLE1_H

#include "Wm4WindowApplication2.h"
#include "PhysicsModule.h"
using namespace Wm4;

class RoughPlaneParticle1 : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    RoughPlaneParticle1 ();

    virtual bool OnInitialize ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    PhysicsModule m_kModule;
    std::vector<Vector2d> m_kSFPosition;  // path with static friction
    bool m_bContinueSolving;

    // viscous solution:
    //   x(t) = a0*exp(-r*t)+a1
    //   w(t) = b0*exp(-r*t)+b1*t+b2
    //   r = c/m
    //   a0 = -xdot(0)/r
    //   a1 = x(0)-a0
    //   b1 = -g*sin(phi)/r
    //   b2 = (wdot(0)+r*w(0)-b1)/r
    //   b0 = w(0)-b2
    Vector2d GetVFPosition (double dTime);
    double m_dR, m_dA0, m_dA1, m_dB0, m_dB1, m_dB2;
    std::vector<Vector2d> m_kVFPosition;  // path with viscous friction
};

WM4_REGISTER_INITIALIZE(RoughPlaneParticle1);

#endif
