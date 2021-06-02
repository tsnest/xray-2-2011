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
#include "Wm4OdeRungeKutta4.h"
#include "Wm4Integrate1.h"
using namespace Wm4;

int PhysicsModule::ms_iOrder = 16;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
{
    Length = 0.0;
    MassDensity = 0.0;
    Friction = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    memset(m_adState,0,6*sizeof(double));
    memset(m_adAux,0,9*sizeof(double));
    m_pkSolver = 0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE m_pkSolver;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime, double dX,
    double dY, double dTheta, double dXDot, double dYDot, double dThetaDot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // state variables
    m_adState[0] = dX;
    m_adState[1] = dXDot;
    m_adState[2] = dY;
    m_adState[3] = dYDot;
    m_adState[4] = dTheta;
    m_adState[5] = dThetaDot;

    // auxiliary variable
    double dMu0 = MassDensity*Length;
    double dMu2 = MassDensity*Length*Length*Length/12.0;
    m_dHalfLength = 0.5*Length;
    m_adAux[0] = m_dHalfLength;
    m_adAux[1] = -Friction/dMu0;
    m_adAux[2] = -Friction/dMu2;
    m_adAux[3] = m_adState[1];  // need dot(x) for integration
    m_adAux[4] = m_adState[3];  // need dot(y) for integration
    m_adAux[5] = m_adState[4];  // need theta for integration
    m_adAux[6] = m_adState[5];  // need dot(theta) for integration

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(6,m_dDeltaTime,OdeFunction,m_adAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::Get (double& rdX1, double& rdY1, double& rdX2,
    double& rdY2) const
{
    double dCos = Mathd::Cos(m_adState[4]);
    double dSin = Mathd::Sin(m_adState[4]);
    rdX1 = m_adState[0] + m_dHalfLength*dCos;
    rdY1 = m_adState[2] + m_dHalfLength*dSin;
    rdX2 = m_adState[0] - m_dHalfLength*dCos;
    rdY2 = m_adState[2] - m_dHalfLength*dSin;
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

    m_adAux[3] = m_adState[1];
    m_adAux[4] = m_adState[3];
    m_adAux[5] = m_adState[4];
    m_adAux[6] = m_adState[5];
}
//----------------------------------------------------------------------------
double PhysicsModule::FXIntegrand (double dL, void* pvData)
{
    // Compute all integrands here.  Return one value, store the others for
    // later access.
    double* adAux = (double*)pvData;

    double dCos = Mathd::Cos(adAux[2]);
    double dSin = Mathd::Sin(adAux[2]);

    double dTmp1 = adAux[3] - dL*adAux[6]*dSin;
    double dTmp2 = adAux[4] + dL*adAux[6]*dCos;
    double dLen = Mathd::Sqrt(dTmp1*dTmp1+dTmp2*dTmp2);
    if (dLen > Mathd::ZERO_TOLERANCE)
    {
        double dInvLen = 1.0/dLen;

        // FY integrand
        adAux[7] = adAux[1]*dTmp2*dInvLen;

        // FTheta integrand
        double dTmp3 = dL*(dL*adAux[6]-adAux[3]*dSin+adAux[4]*dCos);
        adAux[8] = adAux[2]*dTmp3*dInvLen;

        // FX integrand
        return adAux[1]*dTmp1*dInvLen;
    }

    // FY integrand
    adAux[7] = 0.0;

    // FTheta integrand
    adAux[8] = 0.0;

    // FX integrand
    return 0.0;
}
//----------------------------------------------------------------------------
double PhysicsModule::FYIntegrand (double, void* pvData)
{
    double* adAux = (double*)pvData;
    return adAux[7];
}
//----------------------------------------------------------------------------
double PhysicsModule::FThetaIntegrand (double, void* pvData)
{
    double* adAux = (double*)pvData;
    return adAux[8];
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* adState,
    void* pvData, double* adFValue)
{
    double* adAux = (double*)pvData;

    // x function
    adFValue[0] = adState[1];

    // dot(x) function
    adFValue[1] = Integrate1d::RombergIntegral(ms_iOrder,-adAux[4],adAux[4],
        FXIntegrand,adAux);

    // y function
    adFValue[2] = adState[3];

    // dot(y) function
    adFValue[3] = Integrate1d::RombergIntegral(ms_iOrder,-adAux[4],adAux[4],
        FYIntegrand,adAux);

    // theta function
    adFValue[4] = adState[5];

    // dot(theta) function
    adFValue[5] = Integrate1d::RombergIntegral(ms_iOrder,-adAux[4],adAux[4],
        FThetaIntegrand,adAux);
}
//----------------------------------------------------------------------------
