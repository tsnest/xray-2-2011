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
#include "Wm4DistSegment3Triangle3.h"
#include "Wm4DistLine3Triangle3.h"
#include "Wm4DistVector3Triangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistSegment3Triangle3<Real>::DistSegment3Triangle3 (
    const Segment3<Real>& rkSegment, const Triangle3<Real>& rkTriangle)
    :
    m_pkSegment(&rkSegment),
    m_pkTriangle(&rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistSegment3Triangle3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& DistSegment3Triangle3<Real>::GetTriangle () const
{
    return *m_pkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::GetSquared ()
{
    DistLine3Triangle3<Real> kLTDist(Line3<Real>(m_pkSegment->Origin,
        m_pkSegment->Direction),*m_pkTriangle);
    Real fSqrDist = kLTDist.GetSquared();
    m_fSegmentParameter = kLTDist.GetLineParameter();

    if (m_fSegmentParameter >= -m_pkSegment->Extent)
    {
        if (m_fSegmentParameter <= m_pkSegment->Extent)
        {
            m_kClosestPoint0 = kLTDist.GetClosestPoint0();
            m_kClosestPoint1 = kLTDist.GetClosestPoint1();
            m_afTriangleBary[0] = kLTDist.GetTriangleBary(0);
            m_afTriangleBary[1] = kLTDist.GetTriangleBary(1);
            m_afTriangleBary[2] = kLTDist.GetTriangleBary(2);
        }
        else
        {
            m_kClosestPoint0 = m_pkSegment->GetPosEnd();
            DistVector3Triangle3<Real> kVTDist(m_kClosestPoint0,
                *m_pkTriangle);
            fSqrDist = kVTDist.GetSquared();
            m_kClosestPoint1 = kVTDist.GetClosestPoint1();
            m_fSegmentParameter = m_pkSegment->Extent;
            m_afTriangleBary[0] = kVTDist.GetTriangleBary(0);
            m_afTriangleBary[1] = kVTDist.GetTriangleBary(1);
            m_afTriangleBary[2] = kVTDist.GetTriangleBary(2);
        }
    }
    else
    {
        m_kClosestPoint0 = m_pkSegment->GetNegEnd();
        DistVector3Triangle3<Real> kVTDist(m_kClosestPoint0,*m_pkTriangle);
        fSqrDist = kVTDist.GetSquared();
        m_kClosestPoint1 = kVTDist.GetClosestPoint1();
        m_fSegmentParameter = -m_pkSegment->Extent;
        m_afTriangleBary[0] = kVTDist.GetTriangleBary(0);
        m_afTriangleBary[1] = kVTDist.GetTriangleBary(1);
        m_afTriangleBary[2] = kVTDist.GetTriangleBary(2);
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_pkTriangle->V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_pkTriangle->V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_pkTriangle->V[2] + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    return DistSegment3Triangle3<Real>(kMSegment,kMTriangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_pkTriangle->V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_pkTriangle->V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_pkTriangle->V[2] + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    Triangle3<Real> kMTriangle(kMV0,kMV1,kMV2);
    return DistSegment3Triangle3<Real>(kMSegment,kMTriangle).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::GetSegmentParameter () const
{
    return m_fSegmentParameter;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Triangle3<Real>::GetTriangleBary (int i) const
{
    assert(0 <= i && i < 3);
    return m_afTriangleBary[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistSegment3Triangle3<float>;

template WM4_FOUNDATION_ITEM
class DistSegment3Triangle3<double>;
//----------------------------------------------------------------------------
}
