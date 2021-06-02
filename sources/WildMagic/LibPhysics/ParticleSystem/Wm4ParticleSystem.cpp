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
#include "Wm4ParticleSystem.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real, class TVector>
ParticleSystem<Real,TVector>::ParticleSystem (int iNumParticles, Real fStep)
{
    m_iNumParticles = iNumParticles;
    SetStep(fStep);

    m_afMass = WM4_NEW Real[m_iNumParticles];
    m_afInvMass = WM4_NEW Real[m_iNumParticles];
    m_akPosition = WM4_NEW TVector[m_iNumParticles];
    m_akVelocity = WM4_NEW TVector[m_iNumParticles];

    memset(m_afMass,0,m_iNumParticles*sizeof(Real));
    memset(m_afInvMass,0,m_iNumParticles*sizeof(Real));
    memset(m_akPosition,0,m_iNumParticles*sizeof(TVector));
    memset(m_akVelocity,0,m_iNumParticles*sizeof(TVector));

    m_akPTmp = WM4_NEW TVector[m_iNumParticles];
    m_akDPTmp1 = WM4_NEW TVector[m_iNumParticles];
    m_akDPTmp2 = WM4_NEW TVector[m_iNumParticles];
    m_akDPTmp3 = WM4_NEW TVector[m_iNumParticles];
    m_akDPTmp4 = WM4_NEW TVector[m_iNumParticles];
    m_akVTmp = WM4_NEW TVector[m_iNumParticles];
    m_akDVTmp1 = WM4_NEW TVector[m_iNumParticles];
    m_akDVTmp2 = WM4_NEW TVector[m_iNumParticles];
    m_akDVTmp3 = WM4_NEW TVector[m_iNumParticles];
    m_akDVTmp4 = WM4_NEW TVector[m_iNumParticles];
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
ParticleSystem<Real,TVector>::~ParticleSystem ()
{
    WM4_DELETE[] m_afMass;
    WM4_DELETE[] m_afInvMass;
    WM4_DELETE[] m_akPosition;
    WM4_DELETE[] m_akVelocity;
    WM4_DELETE[] m_akPTmp;
    WM4_DELETE[] m_akDPTmp1;
    WM4_DELETE[] m_akDPTmp2;
    WM4_DELETE[] m_akDPTmp3;
    WM4_DELETE[] m_akDPTmp4;
    WM4_DELETE[] m_akVTmp;
    WM4_DELETE[] m_akDVTmp1;
    WM4_DELETE[] m_akDVTmp2;
    WM4_DELETE[] m_akDVTmp3;
    WM4_DELETE[] m_akDVTmp4;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
int ParticleSystem<Real,TVector>::GetNumParticles () const
{
    return m_iNumParticles;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void ParticleSystem<Real,TVector>::SetMass (int i, Real fMass)
{
    assert(0 <= i && i < m_iNumParticles && fMass > (Real)0.0);
    m_afMass[i] = fMass;
    if (fMass != Math<Real>::MAX_REAL)
    {
        m_afInvMass[i] = ((Real)1.0)/fMass;
    }
    else
    {
        m_afInvMass[i] = (Real)0.0;
    }
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real ParticleSystem<Real,TVector>::GetMass (int i) const
{
    assert(0 <= i && i < m_iNumParticles);
    return m_afMass[i];
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector* ParticleSystem<Real,TVector>::Positions () const
{
    return m_akPosition;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector& ParticleSystem<Real,TVector>::Position (int i)
{
    assert(0 <= i && i < m_iNumParticles);
    return m_akPosition[i];
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector* ParticleSystem<Real,TVector>::Velocities () const
{
    return m_akVelocity;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
TVector& ParticleSystem<Real,TVector>::Velocity (int i)
{
    assert(0 <= i && i < m_iNumParticles);
    return m_akVelocity[i];
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void ParticleSystem<Real,TVector>::SetStep (Real fStep)
{
    m_fStep = fStep;
    m_fHalfStep = ((Real)0.5)*m_fStep;
    m_fSixthStep = m_fStep/(Real)6.0;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real ParticleSystem<Real,TVector>::GetStep () const
{
    return m_fStep;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void ParticleSystem<Real,TVector>::Update (Real fTime)
{
    // Runge-Kutta fourth-order solver
    Real fHalfTime = fTime + m_fHalfStep;
    Real fFullTime = fTime + m_fStep;

    // first step
    int i;
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akDPTmp1[i] = m_akVelocity[i];
            m_akDVTmp1[i] = Acceleration(i,fTime,m_akPosition,m_akVelocity);
        }
    }
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akPTmp[i] = m_akPosition[i] + m_fHalfStep*m_akDPTmp1[i];
            m_akVTmp[i] = m_akVelocity[i] + m_fHalfStep*m_akDVTmp1[i];
        }
        else
        {
            m_akPTmp[i] = m_akPosition[i];
            m_akVTmp[i] = TVector::ZERO;
        }
    }

    // second step
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akDPTmp2[i] = m_akVTmp[i];
            m_akDVTmp2[i] = Acceleration(i,fHalfTime,m_akPTmp,m_akVTmp);
        }
    }
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akPTmp[i] = m_akPosition[i] + m_fHalfStep*m_akDPTmp2[i];
            m_akVTmp[i] = m_akVelocity[i] + m_fHalfStep*m_akDVTmp2[i];
        }
        else
        {
            m_akPTmp[i] = m_akPosition[i];
            m_akVTmp[i] = TVector::ZERO;
        }
    }

    // third step
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akDPTmp3[i] = m_akVTmp[i];
            m_akDVTmp3[i] = Acceleration(i,fHalfTime,m_akPTmp,m_akVTmp);
        }
    }
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akPTmp[i] = m_akPosition[i] + m_fStep*m_akDPTmp3[i];
            m_akVTmp[i] = m_akVelocity[i] + m_fStep*m_akDVTmp3[i];
        }
        else
        {
            m_akPTmp[i] = m_akPosition[i];
            m_akVTmp[i] = TVector::ZERO;
        }
    }

    // fourth step
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akDPTmp4[i] = m_akVTmp[i];
            m_akDVTmp4[i] = Acceleration(i,fFullTime,m_akPTmp,m_akVTmp);
        }
    }
    for (i = 0; i < m_iNumParticles; i++)
    {
        if (m_afInvMass[i] > (Real)0.0)
        {
            m_akPosition[i] += m_fSixthStep*(m_akDPTmp1[i] +
                ((Real)2.0)*(m_akDPTmp2[i] + m_akDPTmp3[i]) + m_akDPTmp4[i]);
            m_akVelocity[i] += m_fSixthStep*(m_akDVTmp1[i] +
                ((Real)2.0)*(m_akDVTmp2[i] + m_akDVTmp3[i]) + m_akDVTmp4[i]);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class ParticleSystem<float,Vector2f>;

template WM4_PHYSICS_ITEM
class ParticleSystem<float,Vector3f>;

template WM4_PHYSICS_ITEM
class ParticleSystem<double,Vector2d>;

template WM4_PHYSICS_ITEM
class ParticleSystem<double,Vector3d>;
//----------------------------------------------------------------------------
}
