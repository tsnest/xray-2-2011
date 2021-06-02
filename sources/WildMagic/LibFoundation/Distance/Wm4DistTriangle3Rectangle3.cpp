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
#include "Wm4DistTriangle3Rectangle3.h"
#include "Wm4DistSegment3Triangle3.h"
#include "Wm4DistSegment3Rectangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistTriangle3Rectangle3<Real>::DistTriangle3Rectangle3 (
    const Triangle3<Real>& rkTriangle, const Rectangle3<Real>& rkRectangle)
    :
    m_pkTriangle(&rkTriangle),
    m_pkRectangle(&rkRectangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& DistTriangle3Rectangle3<Real>::GetTriangle () const
{
    return *m_pkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistTriangle3Rectangle3<Real>::GetRectangle () const
{
    return *m_pkRectangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::GetSquared ()
{
    // compare edges of triangle to the interior of rectangle
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    Segment3<Real> kEdge;
    int i0, i1;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        kEdge.Origin = ((Real)0.5)*(m_pkTriangle->V[i0]+m_pkTriangle->V[i1]);
        kEdge.Direction = m_pkTriangle->V[i1] - m_pkTriangle->V[i0];
        kEdge.Extent = ((Real)0.5)*kEdge.Direction.Normalize();
        DistSegment3Rectangle3<Real> kSRDist(kEdge,*m_pkRectangle);
        fSqrDistTmp = kSRDist.GetSquared();
        if (fSqrDistTmp < fSqrDist)
        {
            m_kClosestPoint0 = kSRDist.GetClosestPoint0();
            m_kClosestPoint1 = kSRDist.GetClosestPoint1();
            fSqrDist = fSqrDistTmp;
        }
    }

    // compare edges of rectangle to the interior of triangle
    for (i1 = 0; i1 < 2; i1++)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            kEdge.Origin = m_pkRectangle->Center +
                (i0*m_pkRectangle->Extent[1-i1]) *
                m_pkRectangle->Axis[1-i1];
            kEdge.Direction = m_pkRectangle->Axis[i1];
            kEdge.Extent = m_pkRectangle->Extent[i1];
            DistSegment3Triangle3<Real> kSTDist(kEdge,*m_pkTriangle);
            fSqrDistTmp = kSTDist.GetSquared();
            if (fSqrDistTmp < fSqrDist)
            {
                m_kClosestPoint0 = kSTDist.GetClosestPoint0();
                m_kClosestPoint1 = kSTDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMV0 = m_pkTriangle->V[0] + fT*rkVelocity0;
    Vector3<Real> kMV1 = m_pkTriangle->V[1] + fT*rkVelocity0;
    Vector3<Real> kMV2 = m_pkTriangle->V[2] + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkRectangle->Center + fT*rkVelocity1;
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    Rectangle3<Real> kMRectangle(kMCenter,m_pkRectangle->Axis,
        m_pkRectangle->Extent);
    return DistTriangle3Rectangle3<Real>(kMTriangle,kMRectangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistTriangle3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMV0 = m_pkTriangle->V[0] + fT*rkVelocity0;
    Vector3<Real> kMV1 = m_pkTriangle->V[1] + fT*rkVelocity0;
    Vector3<Real> kMV2 = m_pkTriangle->V[2] + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkRectangle->Center + fT*rkVelocity1;
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    Rectangle3<Real> kMRectangle(kMCenter,m_pkRectangle->Axis,
        m_pkRectangle->Extent);
    return DistTriangle3Rectangle3<Real>(kMTriangle,kMRectangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistTriangle3Rectangle3<float>;

template WM4_FOUNDATION_ITEM
class DistTriangle3Rectangle3<double>;
//----------------------------------------------------------------------------
}
