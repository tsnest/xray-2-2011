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
#include "Wm4OdeRungeKutta4.h"
#include "Wm4Math.h"
using namespace Wm4;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
{
    memset(m_adState,0,4*sizeof(double));
    memset(m_adAux,0,3*sizeof(double));
    m_pkSolver = 0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE m_pkSolver;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime,
    double dTheta, double dPhi, double dThetaDot, double dPhiDot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // state variables
    m_adState[0] = dTheta;
    m_adState[1] = dThetaDot;
    m_adState[2] = dPhi;
    m_adState[3] = dPhiDot;

    // auxiliary variables
    m_adAux[0] = AngularSpeed*Mathd::Sin(Latitude);  // w*sin(lat)
    m_adAux[1] = AngularSpeed*Mathd::Cos(Latitude);  // w*cos(lat)
    m_adAux[2] = GDivL;

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(4,m_dDeltaTime,OdeFunction,m_adAux);
}
//----------------------------------------------------------------------------
Matrix3f PhysicsModule::GetOrientation () const
{
    float fCosTheta = (float)Mathd::Cos(m_adState[0]);
    float fSinTheta = (float)Mathd::Sin(m_adState[0]);
    float fSinPhi = (float)Mathd::Sin(m_adState[2]);
    float fCosPhi = (float)Mathd::Cos(m_adState[2]);
    float fOmCosPhi = 1.0f - fCosPhi;

    Matrix3f rkRot;
    rkRot[0][0] = 1.0f - fOmCosPhi*fCosTheta*fCosTheta;
    rkRot[0][1] = -fOmCosPhi*fSinTheta*fCosTheta;
    rkRot[0][2] = -fSinPhi*fCosTheta;
    rkRot[1][0] = rkRot[0][1];
    rkRot[1][1] = 1.0f - fOmCosPhi*fSinTheta*fSinTheta;
    rkRot[1][2] = -fSinPhi*fSinTheta;
    rkRot[2][0] = -rkRot[0][2];
    rkRot[2][1] = -rkRot[1][2];
    rkRot[2][2] = fCosPhi;

    return rkRot;
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

    double dSinTheta = Mathd::Sin(adState[0]);
    double dSinPhi = Mathd::Sin(adState[2]);
    double dCosPhi = Mathd::Cos(adState[2]);

    // This function has a removable discontinuity at phi = 0.  When sin(phi)
    // is nearly zero, switch to the function that is defined at phi=0. 
    double dTheta1DotFunction;
    if (Mathd::FAbs(dSinPhi) < 1e-06)
    {
        dTheta1DotFunction = (2.0/3.0)*adAux[1]*adState[3]*dSinTheta;
    }
    else
    {
        dTheta1DotFunction = -2.0*adState[3]*(-adAux[1]*dSinTheta +
            dCosPhi*(adState[1]+adAux[0])/dSinPhi);
    }

    double dTheta2DotFunction = dSinPhi*(adState[1]*adState[1]*dCosPhi +
        2.0*adState[1]*(adAux[1]*dSinTheta*dSinPhi - adAux[0]*dCosPhi) -
        adAux[2]);

    // theta function
    adFValue[0] = adState[1];

    // dot(theta) function
    adFValue[1] = dTheta1DotFunction;

    // phi function
    adFValue[2] = adState[3];

    // dot(phi) function
    adFValue[3] = dTheta2DotFunction;
}
//----------------------------------------------------------------------------
