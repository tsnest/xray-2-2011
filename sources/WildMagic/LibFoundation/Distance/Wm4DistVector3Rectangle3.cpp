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
// Version: 4.0.1 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4DistVector3Rectangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Rectangle3<Real>::DistVector3Rectangle3 (
    const Vector3<Real>& rkVector, const Rectangle3<Real>& rkRectangle)
    :
    m_pkVector(&rkVector),
    m_pkRectangle(&rkRectangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Rectangle3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistVector3Rectangle3<Real>::GetRectangle () const
{
    return *m_pkRectangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_pkRectangle->Center - *m_pkVector;
    Real fB0 = kDiff.Dot(m_pkRectangle->Axis[0]);
    Real fB1 = kDiff.Dot(m_pkRectangle->Axis[1]);
    Real fS0 = -fB0, fS1 = -fB1;
    Real fSqrDistance = kDiff.SquaredLength();

    if (fS0 < -m_pkRectangle->Extent[0])
    {
        fS0 = -m_pkRectangle->Extent[0];
    }
    else if (fS0 > m_pkRectangle->Extent[0])
    {
        fS0 = m_pkRectangle->Extent[0];
    }
    fSqrDistance += fS0*(fS0 + ((Real)2.0)*fB0);

    if (fS1 < -m_pkRectangle->Extent[1])
    {
        fS1 = -m_pkRectangle->Extent[1];
    }
    else if (fS1 > m_pkRectangle->Extent[1])
    {
        fS1 = m_pkRectangle->Extent[1];
    }
    fSqrDistance += fS1*(fS1 + ((Real)2.0)*fB1);

    // account for numerical round-off error
    if (fSqrDistance < (Real)0.0)
    {
        fSqrDistance = (Real)0.0;
    }

    m_kClosestPoint0 = *m_pkVector;
    m_kClosestPoint1 = m_pkRectangle->Center + fS0*m_pkRectangle->Axis[0] +
        fS1*m_pkRectangle->Axis[1];
    m_afRectCoord[0] = fS0;
    m_afRectCoord[1] = fS1;

    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkRectangle->Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle(kMCenter,m_pkRectangle->Axis[0],
        m_pkRectangle->Axis[1],m_pkRectangle->Extent[0],
        m_pkRectangle->Extent[1]);
    return DistVector3Rectangle3<Real>(kMVector,kMRectangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkRectangle->Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle(kMCenter,m_pkRectangle->Axis[0],
        m_pkRectangle->Axis[1],m_pkRectangle->Extent[0],
        m_pkRectangle->Extent[1]);
    return DistVector3Rectangle3<Real>(kMVector,kMRectangle).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Rectangle3<Real>::GetRectangleCoordinate (int i) const
{
    assert(0 <= i && i < 2);
    return m_afRectCoord[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Rectangle3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Rectangle3<double>;
//----------------------------------------------------------------------------
}
