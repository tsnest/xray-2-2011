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
#include "Wm4DistVector3Segment3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Segment3<Real>::DistVector3Segment3 (const Vector3<Real>& rkVector,
    const Segment3<Real>& rkSegment)
    :
    m_pkVector(&rkVector),
    m_pkSegment(&rkSegment)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Segment3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistVector3Segment3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Segment3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Segment3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = *m_pkVector - m_pkSegment->Origin;
    m_fSegmentParameter = m_pkSegment->Direction.Dot(kDiff);

    if (-m_pkSegment->Extent < m_fSegmentParameter)
    {
        if (m_fSegmentParameter < m_pkSegment->Extent)
        {
            m_kClosestPoint1 = m_pkSegment->Origin +
                m_fSegmentParameter*m_pkSegment->Direction;
        }
        else
        {
            m_kClosestPoint1 = m_pkSegment->Origin +
                m_pkSegment->Extent*m_pkSegment->Direction;
        }
    }
    else
    {
        m_kClosestPoint1 = m_pkSegment->Origin -
            m_pkSegment->Extent*m_pkSegment->Direction;
    }

    m_kClosestPoint0 = *m_pkVector;
    kDiff = m_kClosestPoint1 - m_kClosestPoint0;
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Segment3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    return DistVector3Segment3<Real>(kMVector,kMSegment).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Segment3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    return DistVector3Segment3<Real>(kMVector,kMSegment).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Segment3<Real>::GetSegmentParameter () const
{
    return m_fSegmentParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Segment3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Segment3<double>;
//----------------------------------------------------------------------------
}
