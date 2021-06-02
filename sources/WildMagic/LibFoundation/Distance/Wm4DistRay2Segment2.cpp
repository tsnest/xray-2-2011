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
#include "Wm4DistRay2Segment2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistRay2Segment2<Real>::DistRay2Segment2 (const Ray2<Real>& rkRay,
    const Segment2<Real>& rkSegment)
    :
    m_pkRay(&rkRay),
    m_pkSegment(&rkSegment)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray2<Real>& DistRay2Segment2<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment2<Real>& DistRay2Segment2<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Segment2<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Segment2<Real>::GetSquared ()
{
    Vector2<Real> kDiff = m_pkRay->Origin- m_pkSegment->Origin;
    Real fA01 = -m_pkRay->Direction.Dot(m_pkSegment->Direction);
    Real fB0 = kDiff.Dot(m_pkRay->Direction);
    Real fB1 = -kDiff.Dot(m_pkSegment->Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fS0, fS1, fSqrDist, fExtDet;

    if (fDet >= Math<Real>::ZERO_TOLERANCE)
    {
        // The ray and segment are not parallel.
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;
        fExtDet = m_pkSegment->Extent*fDet;

        if (fS0 >= (Real)0.0)
        {
            if (fS1 >= -fExtDet)
            {
                if (fS1 <= fExtDet)  // region 0
                {
                    // minimum at interior points of ray and segment
                    Real fInvDet = ((Real)1.0)/fDet;
                    fS0 *= fInvDet;
                    fS1 *= fInvDet;
                    fSqrDist = (Real)0.0;
                }
                else  // region 1
                {
                    fS1 = m_pkSegment->Extent;
                    fS0 = -(fA01*fS1+fB0);
                    if (fS0 > (Real)0.0)
                    {
                        fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = (Real)0.0;
                        fSqrDist = fS1*(fS1+((Real)2.0)*fB1)+fC;
                    }
                }
            }
            else  // region 5
            {
                fS1 = -m_pkSegment->Extent;
                fS0 = -(fA01*fS1+fB0);
                if (fS0 > (Real)0.0)
                {
                    fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
                }
                else
                {
                    fS0 = (Real)0.0;
                    fSqrDist = fS1*(fS1+((Real)2.0)*fB1)+fC;
                }
            }
        }
        else
        {
            if (fS1 <= -fExtDet)  // region 4
            {
                fS0 = -(-fA01*m_pkSegment->Extent+fB0);
                if (fS0 > (Real)0.0)
                {
                    fS1 = -m_pkSegment->Extent;
                    fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
                }
                else
                {
                    fS0 = (Real)0.0;
                    fS1 = -fB1;
                    if (fS1 < -m_pkSegment->Extent)
                    {
                        fS1 = -m_pkSegment->Extent;
                    }
                    else if (fS1 > m_pkSegment->Extent)
                    {
                        fS1 = m_pkSegment->Extent;
                    }
                    fSqrDist = fS1*(fS1+((Real)2.0)*fB1)+fC;
                }
            }
            else if (fS1 <= fExtDet)  // region 3
            {
                fS0 = (Real)0.0;
                fS1 = -fB1;
                if (fS1 < -m_pkSegment->Extent)
                {
                    fS1 = -m_pkSegment->Extent;
                }
                else if (fS1 > m_pkSegment->Extent)
                {
                    fS1 = m_pkSegment->Extent;
                }
                fSqrDist = fS1*(fS1+((Real)2.0)*fB1)+fC;
            }
            else  // region 2
            {
                fS0 = -(fA01*m_pkSegment->Extent+fB0);
                if (fS0 > (Real)0.0)
                {
                    fS1 = m_pkSegment->Extent;
                    fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
                }
                else
                {
                    fS0 = (Real)0.0;
                    fS1 = -fB1;
                    if (fS1 < -m_pkSegment->Extent)
                    {
                        fS1 = -m_pkSegment->Extent;
                    }
                    else if (fS1 > m_pkSegment->Extent)
                    {
                        fS1 = m_pkSegment->Extent;
                    }
                    fSqrDist = fS1*(fS1+((Real)2.0)*fB1)+fC;
                }
            }
        }
    }
    else
    {
        // ray and segment are parallel
        if (fA01 > (Real)0.0)
        {
            // opposite direction vectors
            fS1 = -m_pkSegment->Extent;
        }
        else
        {
            // same direction vectors
            fS1 = m_pkSegment->Extent;
        }

        fS0 = -(fA01*fS1+fB0);
        if (fS0 > (Real)0.0)
        {
            fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
        }
        else
        {
            fS0 = (Real)0.0;
            fSqrDist = fS1*(fS1+((Real)2.0)*fB1)+fC;
        }
    }

    m_kClosestPoint0 = m_pkRay->Origin + fS0*m_pkRay->Direction;
    m_kClosestPoint1 = m_pkSegment->Origin + fS1*m_pkSegment->Direction;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Segment2<Real>::Get (Real fT, const Vector2<Real>& rkVelocity0,
    const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMOrigin0 = m_pkRay->Origin + fT*rkVelocity0;
    Vector2<Real> kMOrigin1 = m_pkSegment->Origin + fT*rkVelocity1;
    Ray2<Real> kMRay(kMOrigin0,m_pkRay->Direction);
    Segment2<Real> kMSegment(kMOrigin1,m_pkSegment->Direction,
        m_pkSegment->Extent);
    return DistRay2Segment2<Real>(kMRay,kMSegment).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Segment2<Real>::GetSquared (Real fT,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMOrigin0 = m_pkRay->Origin + fT*rkVelocity0;
    Vector2<Real> kMOrigin1 = m_pkSegment->Origin + fT*rkVelocity1;
    Ray2<Real> kMRay(kMOrigin0,m_pkRay->Direction);
    Segment2<Real> kMSegment(kMOrigin1,m_pkSegment->Direction,
        m_pkSegment->Extent);
    return DistRay2Segment2<Real>(kMRay,kMSegment).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistRay2Segment2<float>;

template WM4_FOUNDATION_ITEM
class DistRay2Segment2<double>;
//----------------------------------------------------------------------------
}
