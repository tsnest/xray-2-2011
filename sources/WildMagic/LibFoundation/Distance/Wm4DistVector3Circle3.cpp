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
// Version: 4.0.2 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4DistVector3Circle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Circle3<Real>::DistVector3Circle3 (
    const Vector3<Real>& rkVector, const Circle3<Real>& rkCircle)
    :
    m_pkVector(&rkVector),
    m_pkCircle(&rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Circle3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle3<Real>& DistVector3Circle3<Real>::GetCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::GetSquared ()
{
    // signed distance from point to plane of circle
    Vector3<Real> kDiff0 = *m_pkVector - m_pkCircle->Center;
    Real fDist = kDiff0.Dot(m_pkCircle->N);

    // projection of P-C onto plane is Q-C = P-C - (fDist)*N
    Vector3<Real> kDiff1 = kDiff0 - fDist*m_pkCircle->N;
    Real fSqrLen = kDiff1.SquaredLength();
    Real fSqrDistance;

    if (fSqrLen >= Math<Real>::ZERO_TOLERANCE)
    {
        m_kClosestPoint1 = m_pkCircle->Center + (m_pkCircle->Radius/
            Math<Real>::Sqrt(fSqrLen))*kDiff1;
        Vector3<Real> kDiff2 = *m_pkVector - m_kClosestPoint1;
        fSqrDistance = kDiff2.SquaredLength();
    }
    else
    {
        m_kClosestPoint1 = Vector3<Real>(Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,Math<Real>::MAX_REAL);
        fSqrDistance = m_pkCircle->Radius*m_pkCircle->Radius + fDist*fDist;
    }

    m_kClosestPoint0 = *m_pkVector;

    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkCircle->Center + fT*rkVelocity1;
    Circle3<Real> kMCircle(kMCenter,m_pkCircle->U,m_pkCircle->V,
        m_pkCircle->N,m_pkCircle->Radius);
    return DistVector3Circle3<Real>(kMVector,kMCircle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Circle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkCircle->Center + fT*rkVelocity1;
    Circle3<Real> kMCircle(kMCenter,m_pkCircle->U,m_pkCircle->V,
        m_pkCircle->N,m_pkCircle->Radius);
    return DistVector3Circle3<Real>(kMVector,kMCircle).Get();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Circle3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Circle3<double>;
//----------------------------------------------------------------------------
}
