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
    Mu = 0.0;
    Gravity = 0.0;
    Angle = 0.0;
    SinAngle = 0.0;
    CosAngle = 0.0;
    XLocExt = 0.0;
    YLocExt = 0.0;
    ZLocExt = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    memset(m_adState,0,6*sizeof(double));
    memset(m_adAux,0,2*sizeof(double));
    m_pkSolver = 0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE m_pkSolver;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime, double dX,
    double dW, double dTheta, double dXDot, double dWDot, double dThetaDot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // state variables
    m_adState[0] = dX;
    m_adState[1] = dXDot;
    m_adState[2] = dW;
    m_adState[3] = dWDot;
    m_adState[4] = dTheta;
    m_adState[5] = dThetaDot;

    // auxiliary variables
    SinAngle = Mathd::Sin(Angle);
    CosAngle = Mathd::Cos(Angle);
    m_adAux[0] = Mu*Gravity;  // c/m in the one-particle system example
    m_adAux[1] = Gravity*SinAngle;

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(4,m_dDeltaTime,OdeFunction,m_adAux);

    // set up for angular speed
    m_dTheta0 = dTheta;
    m_dThetaDer0 = dThetaDot;

    double dXX = XLocExt*XLocExt;
    double dXY = XLocExt*YLocExt;
    double dYY = YLocExt*YLocExt;
    double dTmp1 = dXX + dYY;
    double dTmp2 = Mathd::Sqrt(dTmp1);
    double dTmp3 = 4.0*dXY/3.0;
    double dTmp4 = 0.5*Mathd::Log((dTmp2+XLocExt)/(dTmp2-XLocExt));
    double dTmp5 = 0.5*Mathd::Log((dTmp2+YLocExt)/(dTmp2-YLocExt));
    double dNumer = dTmp3*dTmp2 + XLocExt*dXX*dTmp5 + YLocExt*dYY*dTmp4;
    double dDenom = dTmp3*dTmp1;
    double dCoeff = Mu*Gravity*dNumer/dDenom;

    double dAngSpeed = Mathd::FAbs(dThetaDot);
    if (dAngSpeed > Mathd::ZERO_TOLERANCE)
    {
        m_dAngVelCoeff = dCoeff/dAngSpeed;
    }
    else
    {
        m_dAngVelCoeff = 0.0;
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::GetRectangle (double& rdX00, double& rdY00, double& rdX10,
    double& rdY10, double& rdX11, double& rdY11, double& rdX01, double& rdY01)
    const
{
    // P = (x,y) + sx*XLocExt*(cos(A),sin(A)) + sy*YLocExt*(-sin(A),cos(A))
    // where |sx| = 1 and |sy| = 1 (four choices on sign)

    double dCos = Mathd::Cos(m_adState[4]);
    double dSin = Mathd::Sin(m_adState[4]);

    // sx = -1, sy = -1
    rdX00 = m_adState[0] - XLocExt*dCos + YLocExt*dSin;
    rdY00 = m_adState[2] - XLocExt*dSin - YLocExt*dCos;

    // sx = +1, sy = -1
    rdX10 = m_adState[0] + XLocExt*dCos + YLocExt*dSin;
    rdY10 = m_adState[2] + XLocExt*dSin - YLocExt*dCos;

    // sx = +1, sy = +1
    rdX11 = m_adState[0] + XLocExt*dCos - YLocExt*dSin;
    rdY11 = m_adState[2] + XLocExt*dSin + YLocExt*dCos;

    // sx = -1, sy = +1
    rdX01 = m_adState[0] - XLocExt*dCos - YLocExt*dSin;
    rdY01 = m_adState[2] - XLocExt*dSin + YLocExt*dCos;
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

    // update for angular speed
    double dATmp = m_dAngVelCoeff*m_dTime;
    double dAngVelMult = 1.0 - dATmp;
    if (dAngVelMult > 0.0)
    {
        m_adState[5] = dAngVelMult*m_dThetaDer0;
        m_adState[4] = m_dTheta0 + m_dTime*(1.0-0.5*dATmp)*m_dThetaDer0;
    }
    else
    {
        m_adState[5] = 0.0;
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* adState,
    void* pvData, double* adFValue)
{
    double* adAux = (double*)pvData;

    double dVLen = Mathd::Sqrt(adState[1]*adState[1]+adState[3]*adState[3]);
    double dXDotFunction, dWDotFunction;
    if (dVLen > Mathd::ZERO_TOLERANCE)
    {
        double dTemp = -adAux[0]/dVLen;
        dXDotFunction = dTemp*adState[1];
        dWDotFunction = dTemp*adState[3] - adAux[1];
    }
    else
    {
        // velocity is effectively zero, so frictional force is zero
        dXDotFunction = 0.0;
        dWDotFunction = -adAux[1];
    }

    // x function
    adFValue[0] = adState[1];

    // dot(x) function
    adFValue[1] = dXDotFunction;

    // w function
    adFValue[2] = adState[3];

    // dot(w) function
    adFValue[3] = dWDotFunction;
}
//----------------------------------------------------------------------------
