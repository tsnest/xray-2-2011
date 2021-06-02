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

#include "Wm4PhysicsPCH.h"
#include "Wm4MassSpringCurve.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real, class TVector>
MassSpringCurve<Real,TVector>::MassSpringCurve (int iNumParticles, Real fStep)
    :
    ParticleSystem<Real,TVector>(iNumParticles,fStep)
{
    m_iNumSprings = m_iNumParticles - 1;
    m_afConstant = WM4_NEW Real[m_iNumSprings];
    m_afLength = WM4_NEW Real[m_iNumSprings];
    memset(m_afConstant,0,m_iNumSprings*sizeof(Real));
    memset(m_afLength,0,m_iNumSprings*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
MassSpringCurve<Real,TVector>::~MassSpringCurve ()
{
    WM4_DELETE[] m_afConstant;
    WM4_DELETE[] m_afLength;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
int MassSpringCurve<Real,TVector>::GetNumSprings () const
{
    return m_iNumSprings;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real& MassSpringCurve<Real,TVector>::Constant (int i)
{
    assert(0 <= i && i < m_iNumSprings);
    return m_afConstant[i];
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real& MassSpringCurve<Real,TVector>::Length (int i)
{
    assert(0 <= i && i < m_iNumSprings);
    return m_afLength[i];
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector MassSpringCurve<Real,TVector>::Acceleration (int i, Real fTime,
    const TVector* akPosition, const TVector* akVelocity)
{
    // Compute spring forces on position X[i].  The positions are not
    // necessarily m_akPosition since the RK4 solver in ParticleSystem
    // evaluates the acceleration function at intermediate positions.  The end
    // points of the curve of masses must be handled separately since each
    // has only one spring attached to it.

    TVector kAcceleration = ExternalAcceleration(i,fTime,akPosition,
        akVelocity);

    TVector kDiff, kForce;
    Real fRatio;

    if (i > 0)
    {
        int iM1 = i-1;
        kDiff = akPosition[iM1] - akPosition[i];
        fRatio = m_afLength[iM1]/kDiff.Length();
        kForce = m_afConstant[iM1]*(((Real)1.0)-fRatio)*kDiff;
        kAcceleration += m_afInvMass[i]*kForce;
    }

    int iP1 = i+1;
    if (iP1 < m_iNumParticles)
    {
        kDiff = akPosition[iP1] - akPosition[i];
        fRatio = m_afLength[i]/kDiff.Length();
        kForce = m_afConstant[i]*(((Real)1.0)-fRatio)*kDiff;
        kAcceleration += m_afInvMass[i]*kForce;
    }

    return kAcceleration;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector MassSpringCurve<Real,TVector>::ExternalAcceleration (int, Real,
    const TVector*, const TVector*)
{
    return TVector::ZERO;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class MassSpringCurve<float,Vector2f>;

template WM4_PHYSICS_ITEM
class MassSpringCurve<float,Vector3f>;

template WM4_PHYSICS_ITEM
class MassSpringCurve<double,Vector2d>;

template WM4_PHYSICS_ITEM
class MassSpringCurve<double,Vector3d>;
//----------------------------------------------------------------------------
}
