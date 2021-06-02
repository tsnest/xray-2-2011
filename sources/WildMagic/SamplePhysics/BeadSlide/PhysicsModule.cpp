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
    m_dQ = 0.0;
    m_dQDer = 0.0;
    memset(m_adState,0,2*sizeof(double));
    memset(m_adAux,0,sizeof(double));
    m_pkSolver = 0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE m_pkSolver;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime, double dQ,
    double dQDot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // state variables
    m_adState[0] = dQ;
    m_adState[1] = dQDot;

    // auxiliary variable
    m_adAux[0] = Gravity;

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(2,m_dDeltaTime,OdeFunction,m_adAux);
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

    double dQ2 = adState[0]*adState[0];
    double dQDer2 = adState[1]*adState[1];
    double dNumer = -3.0*adAux[0]*dQ2 - 2.0*adState[0]*(2.0+9.0*dQ2)*dQDer2;
    double dDenom = 1.0 + dQ2*(4.0 + 9.0*dQ2);
    double dQDotFunction = dNumer/dDenom;

    // q function
    adFValue[0] = adState[1];

    // dot(q) function
    adFValue[1] = dQDotFunction;
}
//----------------------------------------------------------------------------
