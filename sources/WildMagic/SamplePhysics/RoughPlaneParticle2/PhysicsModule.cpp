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
#include "Wm4Vector2.h"
#include "Wm4OdeRungeKutta4.h"
using namespace Wm4;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
{
    Gravity = 0.0;
    Mass1 = 0.0;
    Mass2 = 0.0;
    Friction1 = 0.0;
    Friction2 = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    memset(m_adState,0,6*sizeof(double));
    memset(m_adAux,0,7*sizeof(double));
    m_pkSolver = 0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE m_pkSolver;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime, double dX1,
    double dY1, double dX2, double dY2, double dXDot, double dYDot,
    double dThetaDot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // compute length and moments
    double dDeltaX = dX2 - dX1;
    double dDeltaY = dY2 - dY1;
    double dLength = Mathd::Sqrt(dDeltaX*dDeltaX + dDeltaY*dDeltaY);
    double dMu0 = Mass1 + Mass2;
    double dInvMu0 = 1.0/dMu0;
    double dWeight1 = Mass1*dInvMu0;
    double dWeight2 = Mass2*dInvMu0;
    m_dLength1 = dWeight2*dLength;
    m_dLength2 = dWeight1*dLength;
    double dMu2 = Mass1*m_dLength1*m_dLength1 + Mass2*m_dLength2*m_dLength2;
    double dInvMu2 = 1.0/dMu2;

    // state variables
    m_adState[0] = dWeight1*dX1 + dWeight2*dX2;
    m_adState[1] = dXDot;
    m_adState[2] = dWeight1*dY1 + dWeight2*dY2;
    m_adState[3] = dYDot;
    m_adState[4] = Mathd::ATan2(dDeltaY,dDeltaX);
    m_adState[5] = dThetaDot;

    // auxiliary variable
    m_adAux[0] = Gravity;
    m_adAux[1] = m_dLength1;
    m_adAux[2] = m_dLength2;
    m_adAux[3] = -Friction1*dInvMu0;
    m_adAux[4] = -Friction2*dInvMu0;
    m_adAux[5] = -Friction1*dInvMu2;
    m_adAux[6] = -Friction2*dInvMu2;

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(6,m_dDeltaTime,OdeFunction,m_adAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    assert(m_pkSolver);
    if (!m_pkSolver)
    {
        return;
    }

    // take a single step in the ODE solver
    m_pkSolver->Update(m_dTime,m_adState,m_dTime,m_adState);
}
//----------------------------------------------------------------------------
void PhysicsModule::Get (double& rdX1, double& rdY1, double& rdX2,
    double& rdY2) const
{
    double dCos = Mathd::Cos(m_adState[4]);
    double dSin = Mathd::Sin(m_adState[4]);
    rdX1 = m_adState[0] + m_dLength1*dCos;
    rdY1 = m_adState[2] + m_dLength1*dSin;
    rdX2 = m_adState[0] - m_dLength2*dCos;
    rdY2 = m_adState[2] - m_dLength2*dSin;
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* adState,
    void* pvData, double* adFValue)
{
    double* adAux = (double*)pvData;

    // Solve the entire system here and save the results to be returned
    // in the later functions.

    double dCos = Mathd::Cos(adState[4]);
    double dSin = Mathd::Sin(adState[4]);
    double dAngCos = adState[5]*dCos;
    double dAngSin = adState[5]*dSin;

    // Compute the friction vectors.  The Normalize function will set a
    // vector to zero if its length is smaller than Mathd::ZERO_TOLERANCE.
    Vector2d kF1(adState[1]-adAux[1]*dAngSin,adState[3]+adAux[1]*dAngCos);
    Vector2d kF2(adState[1]+adAux[2]*dAngSin,adState[3]-adAux[2]*dAngCos);
    kF1.Normalize();
    kF2.Normalize();
    double dXDotFunction = adAux[3]*kF1.X() + adAux[4]*kF2.X();
    double dYDotFunction = adAux[3]*kF1.Y() + adAux[4]*kF2.Y();
    double dTmp1 = adAux[1]*adAux[5]*(dCos*kF1.Y() - dSin*kF1.X());
    double dTmp2 = adAux[2]*adAux[6]*(dSin*kF2.X() - dCos*kF2.Y());
    double dThetaDotFunction = dTmp1 + dTmp2;

    // x function
    adFValue[0] = adState[1];

    // dot(x) function
    adFValue[1] = dXDotFunction;

    // y function
    adFValue[2] = adState[3];

    // dot(y) function
    adFValue[3] = dYDotFunction;

    // theta function
    adFValue[4] = adState[5];

    // dot(theta) function
    adFValue[5] = dThetaDotFunction;
}
//----------------------------------------------------------------------------
