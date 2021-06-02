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
#include "Wm4MassSpringArbitrary.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real, class TVector>
MassSpringArbitrary<Real,TVector>::MassSpringArbitrary (int iNumParticles,
    int iNumSprings, Real fStep)
    :
    ParticleSystem<Real,TVector>(iNumParticles,fStep)
{
    m_iNumSprings = iNumSprings;
    m_akSpring = WM4_NEW Spring[m_iNumSprings];
    memset(m_akSpring,0,m_iNumSprings*sizeof(Spring));

    m_akAdjacent = WM4_NEW std::set<int>[m_iNumParticles];
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
MassSpringArbitrary<Real,TVector>::~MassSpringArbitrary ()
{
    WM4_DELETE[] m_akSpring;
    WM4_DELETE[] m_akAdjacent;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
int MassSpringArbitrary<Real,TVector>::GetNumSprings () const
{
    return m_iNumSprings;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void MassSpringArbitrary<Real,TVector>::SetSpring (int iSpring,
    int iParticle0, int iParticle1, Real fConstant, Real fLength)
{
    assert(0 <= iSpring && iSpring < m_iNumSprings
        && 0 <= iParticle0 && iParticle0 < m_iNumParticles
        && 0 <= iParticle1 && iParticle1 < m_iNumParticles);

    m_akSpring[iSpring].Particle0 = iParticle0;
    m_akSpring[iSpring].Particle1 = iParticle1;
    m_akSpring[iSpring].Constant = fConstant;
    m_akSpring[iSpring].Length = fLength;

    m_akAdjacent[iParticle0].insert(iSpring);
    m_akAdjacent[iParticle1].insert(iSpring);
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void MassSpringArbitrary<Real,TVector>::GetSpring (int iSpring,
    int& riParticle0, int& riParticle1, Real& rfConstant, Real& rfLength)
    const
{
    assert(0 <= iSpring && iSpring < m_iNumSprings);
    riParticle0 = m_akSpring[iSpring].Particle0;
    riParticle1 = m_akSpring[iSpring].Particle1;
    rfConstant = m_akSpring[iSpring].Constant;
    rfLength = m_akSpring[iSpring].Length;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real& MassSpringArbitrary<Real,TVector>::Constant (int iSpring)
{
    assert(0 <= iSpring && iSpring < m_iNumSprings);
    return m_akSpring[iSpring].Constant;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real& MassSpringArbitrary<Real,TVector>::Length (int iSpring)
{
    assert(0 <= iSpring && iSpring < m_iNumSprings);
    return m_akSpring[iSpring].Length;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector MassSpringArbitrary<Real,TVector>::Acceleration (int i, Real fTime,
    const TVector* akPosition, const TVector* akVelocity)
{
    // Compute spring forces on position X[i].  The positions are not
    // necessarily m_akPosition since the RK4 solver in ParticleSystem
    // evaluates the acceleration function at intermediate positions.

    TVector kAcceleration = ExternalAcceleration(i,fTime,akPosition,
        akVelocity);

    TVector kDiff, kForce;
    Real fRatio;

    std::set<int>::iterator pkIter = m_akAdjacent[i].begin();
    for (/**/; pkIter != m_akAdjacent[i].end(); pkIter++)
    {
        // process a spring connected to particle i
        const Spring& rkSpring = m_akSpring[*pkIter];
        if (i != rkSpring.Particle0)
        {
            kDiff = akPosition[rkSpring.Particle0] - akPosition[i];
        }
        else
        {
            kDiff = akPosition[rkSpring.Particle1] - akPosition[i];
        }

        fRatio = rkSpring.Length/kDiff.Length();
        kForce = rkSpring.Constant*(((Real)1.0)-fRatio)*kDiff;
        kAcceleration += m_afInvMass[i]*kForce;
    }

    return kAcceleration;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector MassSpringArbitrary<Real,TVector>::ExternalAcceleration (int, Real,
    const TVector*, const TVector*)
{
    return TVector::ZERO;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class MassSpringArbitrary<float,Vector2f>;

template WM4_PHYSICS_ITEM
class MassSpringArbitrary<float,Vector3f>;

template WM4_PHYSICS_ITEM
class MassSpringArbitrary<double,Vector2d>;

template WM4_PHYSICS_ITEM
class MassSpringArbitrary<double,Vector3d>;
//----------------------------------------------------------------------------
}
