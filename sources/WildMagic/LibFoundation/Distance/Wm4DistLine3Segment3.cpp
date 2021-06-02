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
#include "Wm4DistLine3Segment3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistLine3Segment3<Real>::DistLine3Segment3 (const Line3<Real>& rkLine,
    const Segment3<Real>& rkSegment)
    :
    m_pkLine(&rkLine),
    m_pkSegment(&rkSegment)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistLine3Segment3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistLine3Segment3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_pkLine->Origin - m_pkSegment->Origin;
    Real fA01 = -m_pkLine->Direction.Dot(m_pkSegment->Direction);
    Real fB0 = kDiff.Dot(m_pkLine->Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist, fExtDet;

    if (fDet >= Math<Real>::ZERO_TOLERANCE)
    {
        // The line and segment are not parallel.
        fB1 = -kDiff.Dot(m_pkSegment->Direction);
        fS1 = fA01*fB0-fB1;
        fExtDet = m_pkSegment->Extent*fDet;

        if (fS1 >= -fExtDet)
        {
            if (fS1 <= fExtDet)
            {
                // Two interior points are closest, one on the line and one
                // on the segment.
                Real fInvDet = ((Real)1.0)/fDet;
                fS0 = (fA01*fB1-fB0)*fInvDet;
                fS1 *= fInvDet;
                fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
                    fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
            }
            else
            {
                // The end point e1 of the segment and an interior point of
                // the line are closest.
                fS1 = m_pkSegment->Extent;
                fS0 = -(fA01*fS1+fB0);
                fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
            }
        }
        else
        {
            // The end point e0 of the segment and an interior point of the
            // line are closest.
            fS1 = -m_pkSegment->Extent;
            fS0 = -(fA01*fS1+fB0);
            fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
        }
    }
    else
    {
        // The line and segment are parallel.  Choose the closest pair so that
        // one point is at segment origin.
        fS1 = (Real)0.0;
        fS0 = -fB0;
        fSqrDist = fB0*fS0+fC;
    }

    m_kClosestPoint0 = m_pkLine->Origin + fS0*m_pkLine->Direction;
    m_kClosestPoint1 = m_pkSegment->Origin + fS1*m_pkSegment->Direction;
    m_fLineParameter = fS0;
    m_fSegmentParameter = fS1;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_pkLine->Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_pkSegment->Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin0,m_pkLine->Direction);
    Segment3<Real> kMSegment(kMOrigin1,m_pkSegment->Direction,
        m_pkSegment->Extent);
    return DistLine3Segment3<Real>(kMLine,kMSegment).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_pkLine->Origin + fT*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_pkSegment->Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin0,m_pkLine->Direction);
    Segment3<Real> kMSegment(kMOrigin1,m_pkSegment->Direction,
        m_pkSegment->Extent);
    return DistLine3Segment3<Real>(kMLine,kMSegment).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::GetLineParameter () const
{
    return m_fLineParameter;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Segment3<Real>::GetSegmentParameter () const
{
    return m_fSegmentParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistLine3Segment3<float>;

template WM4_FOUNDATION_ITEM
class DistLine3Segment3<double>;
//----------------------------------------------------------------------------
}
