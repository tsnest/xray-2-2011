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
    Mass1 = 0.0;
    Mass2 = 0.0;
    Length1 = 0.0;
    Length2 = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    memset(m_adState,0,4*sizeof(double));
    memset(m_adAux,0,4*sizeof(double));
    m_pkSolver = 0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE m_pkSolver;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime,
    double dTheta1, double dTheta2, double dTheta1Dot, double dTheta2Dot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // state variables
    m_adState[0] = dTheta1;
    m_adState[1] = dTheta1Dot;
    m_adState[2] = dTheta2;
    m_adState[3] = dTheta2Dot;

    // auxiliary variables
    m_adAux[0] = Gravity;
    m_adAux[1] = Length1;
    m_adAux[2] = Length2;
    m_adAux[3] = Mass2/(Mass1+Mass2);

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(4,m_dDeltaTime,OdeFunction,m_adAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::GetPositions (double& rdX1, double& rdY1, double& rdX2,
    double& rdY2) const
{
    rdX1 = JointX + Length1*Mathd::Sin(m_adState[0]);
    rdY1 = JointY - Length1*Mathd::Cos(m_adState[0]);
    rdX2 = rdX1 + Length2*Mathd::Sin(m_adState[2]);
    rdY2 = rdY1 - Length2*Mathd::Cos(m_adState[2]);
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
void PhysicsModule::OdeFunction (double, const double* adState,
    void* pvData, double* adFValue)
{
    double* adAux = (double*)pvData;

    double dAngleD = adState[0] - adState[2];
    double dCosD = Mathd::Cos(dAngleD);
    double dSinD = Mathd::Sin(dAngleD);
    double dInvDet = 1.0/(adAux[1]*adAux[2]*(1.0 - adAux[3]*dCosD*dCosD));
    double dSin0 = Mathd::Sin(adState[0]);
    double dSin2 = Mathd::Sin(adState[2]);
    double dB1 = -adAux[0]*dSin0 -
        adAux[3]*adAux[2]*dSinD*adState[3]*adState[3];
    double dB2 = -adAux[0]*dSin2+adAux[1]*dSinD*adState[1]*adState[1];
    double dTheta1DotFunction = (dB1 - adAux[3]*dCosD*dB2)*adAux[2]*dInvDet;
    double dTheta2DotFunction = (dB2 - dCosD*dB1)*adAux[1]*dInvDet;

    // theta1 function
    adFValue[0] = adState[1];

    // dot(theta1) function
    adFValue[1] = dTheta1DotFunction;

    // theta2 function
    adFValue[2] = adState[3];

    // dot(theta2) function
    adFValue[3] = dTheta2DotFunction;
}
//----------------------------------------------------------------------------
