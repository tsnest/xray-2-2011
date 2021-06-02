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

#include "PhysicsModule.h"
#include "Wm4Math.h"
using namespace Wm4;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
{
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime, double dY1,
    double dDY1, double dDY3)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // derived parameters
    m_dAlpha = Mass1 + Mass2 + Mass3;
    m_dBeta = Mass1 - Mass2;
    m_dGamma = Mass1 + Mass2 + Inertia/(Radius*Radius);
    double dTmp = SpringConstant*m_dGamma/(m_dAlpha*m_dGamma-m_dBeta*m_dBeta);
    assert(dTmp > 0.0);
    m_dOmega = Mathd::Sqrt(dTmp);
    m_dGDivOmegaSqr = Gravity/(m_dOmega*m_dOmega);
    m_dDelta = m_dBeta*m_dOmega*m_dOmega/m_dGamma;

    // initial conditions of the system
    m_dY1 = dY1;
    m_dDY1 = dDY1;
    m_dY2 = WireLength - Mathd::PI*Radius - m_dY1;
    m_dDY2 = -m_dDY1;
    m_dY3 = SpringLength;
    m_dDY3 = dDY3;

    // solution parameters
    m_dLPlusGDivOmegaSqr = SpringLength + m_dGDivOmegaSqr;
    m_dK1 = m_dDY3/m_dOmega;
    m_dK2 = m_dY3 - m_dLPlusGDivOmegaSqr;
    m_dTCoeff = m_dDY1 + m_dDelta*m_dK1/m_dOmega;
    m_dDeltaDivOmegaSqr = m_dDelta/(m_dOmega*m_dOmega);
    m_dTSqrCoeff = 0.5*Gravity*m_dDeltaDivOmegaSqr;

    // initial values of solution
    m_dY1Curr = m_dY1;
    m_dY2Curr = m_dY2;
    m_dY3Curr = m_dY3;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    if (m_dY1Curr <= m_dY3Curr || m_dY2Curr <= m_dY3Curr)
    {
        // One of the masses has "lodged" in the pulley.  No more motion is
        // allowed.
        return;
    }

    if (m_dY1Curr >= 255.0 || m_dY2Curr >= 255.0)
    {
        // One of the masses has reached the floor.  No more motion is
        // allowed.
        return;
    }

    m_dTime += m_dDeltaTime;

    double dScaledTime = m_dOmega*m_dTime;
    double dSin = Mathd::Sin(dScaledTime);
    double dCos = Mathd::Cos(dScaledTime);
    double dCombo = m_dK1*dSin + m_dK2*dCos;

    m_dY3Curr = dCombo + m_dLPlusGDivOmegaSqr;
    m_dY1Curr = m_dY1 + m_dTime*(m_dTCoeff + m_dTSqrCoeff*m_dTime) -
        m_dDeltaDivOmegaSqr*dCombo;
    m_dY2Curr = WireLength - Mathd::PI*Radius - m_dY1Curr;
}
//----------------------------------------------------------------------------
