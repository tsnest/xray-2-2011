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
    :
    m_kPosition(Vector2d::ZERO),
    m_kVelocity(Vector2d::ZERO)
{
    SpringConstant = 0.0;
    Mass = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    m_dFrequency = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
void PhysicsModule::Evaluate ()
{
    double dArg = m_dFrequency*m_dTime;
    double dSin = Mathd::Sin(dArg);
    double dCos = Mathd::Cos(dArg);
    m_kPosition = dCos*m_kInitPos + dSin*m_kVelDivFreq;
    m_kVelocity = (m_kVelDivFreq*dCos - m_kInitPos*dSin)*m_dFrequency;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime,
    const Vector2d& rkInitPos, const Vector2d& rkInitVel)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;
    m_kInitPos = rkInitPos;
    m_dFrequency = Mathd::Sqrt(SpringConstant/Mass);
    m_kVelDivFreq = rkInitVel/m_dFrequency;

    Evaluate();
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    m_dTime += m_dDeltaTime;
    Evaluate();
}
//----------------------------------------------------------------------------
