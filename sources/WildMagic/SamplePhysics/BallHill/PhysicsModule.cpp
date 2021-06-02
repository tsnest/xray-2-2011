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
using namespace Wm4;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
{
    Gravity = 0.0;
    A1 = 0.0;
    A2 = 0.0;
    A3 = 0.0;
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
void PhysicsModule::Initialize (double dTime, double dDeltaTime, double dY1,
    double dY2, double dY1Dot, double dY2Dot)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;

    // state variables
    m_adState[0] = dY1;     // y1(0)
    m_adState[1] = dY1Dot;  // y1'(0)
    m_adState[2] = dY2;     // y2(0)
    m_adState[3] = dY2Dot;  // y2'(0)

    // auxiliary variables
    m_adAux[0] = A1*A1;   // a1^2
    m_adAux[1] = A2*A2;   // a2^2
    m_adAux[2] = Gravity; // g

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    WM4_DELETE m_pkSolver;
    m_pkSolver = WM4_NEW OdeRungeKutta4d(4,m_dDeltaTime,OdeFunction,m_adAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::GetData (Vector3f& rkCenter, Matrix3f& rkIncrRot) const
{
    // position is a point exactly on the hill
    Vector3f kPos;
    kPos.X() = (float)(A1*m_adState[0]);
    kPos.Y() = (float)(A2*m_adState[2]);
    kPos.Z() = (float)(A3 - m_adState[0]*m_adState[0] -
        m_adState[2]*m_adState[2]);

    // Lift this point off the hill in the normal direction by the radius of
    // the ball so that the ball just touches the hill.  The hill is
    // implicitly specified by F(x,y,z) = z - [a3 - (x/a1)^2 - (y/a2)^2]
    // where (x,y,z) is the position on the hill.  The gradient of F is a
    // normal vector, Grad(F) = (2*x/a1^2,2*y/a2^2,1).
    Vector3f kNor(2.0f*kPos.X()/(float)m_adAux[0],
        2.0f*kPos.Y()/(float)m_adAux[1],1.0f);
    kNor.Normalize();
    rkCenter = kPos + ((float)Radius)*kNor;

    // Let the ball rotate as it rolls down hill.  The axis of rotation is
    // the perpendicular to hill normal and ball velocity.  The angle of
    // rotation from the last position is A = speed*deltaTime/radius.
    Vector3f kVel;
    kVel.X() = (float)(A1*m_adState[1]);
    kVel.Y() = (float)(A1*m_adState[3]);
    kVel.Z() = -2.0f*(kVel.X()*(float)m_adState[0] +
        kVel.Y()*(float)m_adState[2]);
    float fSpeed = kVel.Normalize();
    float fAngle = fSpeed*((float)m_dDeltaTime)/((float)Radius);
    Vector3f kAxis = kNor.UnitCross(kVel);
    rkIncrRot = Matrix3f(kAxis,fAngle);
}
//----------------------------------------------------------------------------
float PhysicsModule::GetHeight (float fX, float fY) const
{
    double dXScaled = ((double)fX)/A1;
    double dYScaled = ((double)fY)/A2;
    return (float)(A3 - dXScaled*dXScaled - dYScaled*dYScaled);
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

    double dMat00 = adAux[0] + 4.0*adState[0]*adState[0];
    double dMat01 = 4.0*adState[0]*adState[2];
    double dMat11 = adAux[1] + 4.0*adState[2]*adState[2];
    double dInvDet = 1.0/(dMat00*dMat11 - dMat01*dMat01);
    double dSqrLen = adState[1]*adState[1] + adState[3]*adState[3];
    double dRhs0 = 2.0*adState[0]*(adAux[2]-2.0*adState[0]*dSqrLen);
    double dRhs1 = 2.0*adState[2]*(adAux[2]-2.0*adState[2]*dSqrLen);
    double dY1DotFunction = (dMat11*dRhs0 - dMat01*dRhs1)*dInvDet;
    double dY2DotFunction = (dMat00*dRhs1 - dMat01*dRhs0)*dInvDet;

    // Y1 function
    adFValue[0] = adState[1];

    // dot(Y1) function
    adFValue[1] = dY1DotFunction;

    // Y2 function
    adFValue[2] = adState[3];

    // dot(Y2) function
    adFValue[3] = dY2DotFunction;
}
//----------------------------------------------------------------------------
