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
using namespace Wm4;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
{
    Gravity = 0.0;
    Mass = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    m_dEccentricity = 0.0;
    m_dRho = 0.0;
    m_dMajorAxis = 0.0;
    m_dMinorAxis = 0.0;
    memset(m_adState,0,4*sizeof(double));
    memset(m_adAux,0,5*sizeof(double));
    m_pkSolver = 0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE m_pkSolver;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime,
    double dRadius, double dTheta, double dRadiusDot, double dThetaDot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // state variables
    m_adState[0] = dTheta;
    m_adState[1] = dThetaDot;
    m_adState[2] = dRadius;
    m_adState[3] = dRadiusDot;

    // compute c0 and c1 in the potential energy function V(theta)
    double dGM = Gravity*Mass;
    double dGM2 = dGM*Mass;
    double dRadiusSqr = dRadius*dRadius;
    double dAlpha = Mass*dRadiusSqr*dThetaDot;
    double dG2M4dA2 = dGM2*dGM2/(dAlpha*dAlpha);
    double dV0 = -dGM/dRadius;
    double dDV0 = dGM2*dRadiusDot/dAlpha;
    double dV0Plus = dV0 + dG2M4dA2;
    double dSinTheta0 = Mathd::Sin(dTheta);
    double dCosTheta0 = Mathd::Cos(dTheta);
    double dC0 = dV0Plus*dSinTheta0 + dDV0*dCosTheta0;
    double dC1 = dV0Plus*dCosTheta0 - dDV0*dSinTheta0;

    // auxiliary variables needed by function DTheta(...)
    m_adAux[0] = dC0;
    m_adAux[1] = dC1;
    m_adAux[2] = dG2M4dA2;
    m_adAux[3] = dAlpha/(dGM*dGM2);

    // ellipse parameters
    double dGamma0 = dRadiusSqr*Mathd::FAbs(dThetaDot);
    double dTmp0 = dRadiusSqr*dRadius*dThetaDot*dThetaDot - dGM;
    double dTmp1 = dRadiusSqr*dRadiusDot*dThetaDot;
    double dGamma1 = Mathd::Sqrt(dTmp0*dTmp0+dTmp1*dTmp1);
    m_dEccentricity = dGamma1/dGM;
    m_dRho = dGamma0*dGamma0/dGamma1;
    double dTmp2 = 1.0 - m_dEccentricity*m_dEccentricity;
    assert( dTmp2 > 0.0 );
    m_dMajorAxis = m_dRho*m_dEccentricity/dTmp2;
    m_dMinorAxis = m_dMajorAxis*Mathd::Sqrt(dTmp2);

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(1,m_dDeltaTime,OdeFunction,m_adAux);
}
//----------------------------------------------------------------------------
double PhysicsModule::GetPeriod () const
{
    return Mathd::TWO_PI*Mathd::Pow(m_dMajorAxis,1.5) /
        Mathd::Sqrt(Gravity*Mass);
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

    // compute dot(theta) for application access
    double dSin = Mathd::Sin(m_adState[0]);
    double dCos = Mathd::Cos(m_adState[0]);
    double dV = m_adAux[0]*dSin + m_adAux[1]*dCos - m_adAux[2];
    m_adState[1] = m_adAux[3]*dV*dV;

    // compute radius for application access
    m_adState[2] = m_dEccentricity*m_dRho/(1.0 + m_dEccentricity*dCos);

    // compute dot(radius) for application access
    m_adState[3] = m_adState[2]*m_adState[2]*m_adState[1]*dSin/m_dRho;
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* adState,
    void* pvData, double* adFValue)
{
    double* adAux = (double*)pvData;

    double dSin = Mathd::Sin(adState[2]);
    double dCos = Mathd::Cos(adState[2]);
    double dV = adAux[0]*dSin + adAux[1]*dCos - adAux[2];
    double dThetaDotFunction = adAux[3]*dV*dV;

    // dot(theta) function
    adFValue[0] = dThetaDotFunction;
}
//----------------------------------------------------------------------------
