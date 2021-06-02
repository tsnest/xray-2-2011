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
using namespace Wm4;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule (int iRows, int iCols, float fStep,
    const Vector3f& rkGravity, const Vector3f& rkWind, float fViscosity,
    float fAmplitude)
    :
    MassSpringSurface3f(iRows,iCols,fStep),
    m_kGravity(rkGravity),
    m_kWind(rkWind)
{
    m_fViscosity = fViscosity;
    m_kDirection = m_kGravity.UnitCross(m_kWind);
    m_fAmplitude = fAmplitude;

    m_afPhase = WM4_NEW float[m_iNumParticles];
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            m_afPhase[GetIndex(iRow,iCol)] = Mathf::UnitRandom()*Mathf::PI;
        }
    }
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    WM4_DELETE[] m_afPhase;
}
//----------------------------------------------------------------------------
Vector3f PhysicsModule::ExternalAcceleration (int i, float fTime,
    const Vector3f*, const Vector3f* akVelocity)
{
    // acceleration due to gravity, wind, and viscosity
    Vector3f kAcceleration = m_kGravity + m_kWind -
        m_fViscosity*akVelocity[i];

    // sinusoidal perturbation
    float fAmplitude = m_fAmplitude*Mathf::Sin(2.0f*fTime+m_afPhase[i]);
    kAcceleration += fAmplitude*m_kDirection;

    return kAcceleration;
}
//----------------------------------------------------------------------------
