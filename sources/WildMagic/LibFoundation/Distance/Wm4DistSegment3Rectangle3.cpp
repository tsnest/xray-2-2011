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
#include "Wm4DistSegment3Rectangle3.h"
#include "Wm4DistLine3Rectangle3.h"
#include "Wm4DistVector3Rectangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistSegment3Rectangle3<Real>::DistSegment3Rectangle3 (
    const Segment3<Real>& rkSegment, const Rectangle3<Real>& rkRectangle)
    :
    m_pkSegment(&rkSegment),
    m_pkRectangle(&rkRectangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistSegment3Rectangle3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistSegment3Rectangle3<Real>::GetRectangle () const
{
    return *m_pkRectangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::GetSquared ()
{
    DistLine3Rectangle3<Real> kLRDist(Line3<Real>(m_pkSegment->Origin,
        m_pkSegment->Direction),*m_pkRectangle);
    Real fSqrDist = kLRDist.GetSquared();
    m_fSegmentParameter = kLRDist.GetLineParameter();

    if (m_fSegmentParameter >= -m_pkSegment->Extent)
    {
        if (m_fSegmentParameter <= m_pkSegment->Extent)
        {
            m_kClosestPoint0 = kLRDist.GetClosestPoint0();
            m_kClosestPoint1 = kLRDist.GetClosestPoint1();
            m_afRectCoord[0] = kLRDist.GetRectangleCoordinate(0);
            m_afRectCoord[1] = kLRDist.GetRectangleCoordinate(1);
        }
        else
        {
            m_kClosestPoint0 = m_pkSegment->GetPosEnd();
            DistVector3Rectangle3<Real> kVTDist(m_kClosestPoint0,
                *m_pkRectangle);
            fSqrDist = kVTDist.GetSquared();
            m_kClosestPoint1 = kVTDist.GetClosestPoint1();
            m_fSegmentParameter = m_pkSegment->Extent;
            m_afRectCoord[0] = kVTDist.GetRectangleCoordinate(0);
            m_afRectCoord[1] = kVTDist.GetRectangleCoordinate(1);
        }
    }
    else
    {
        m_kClosestPoint0 = m_pkSegment->GetNegEnd();
        DistVector3Rectangle3<Real> kVTDist(m_kClosestPoint0,*m_pkRectangle);
        fSqrDist = kVTDist.GetSquared();
        m_kClosestPoint1 = kVTDist.GetClosestPoint1();
        m_fSegmentParameter = -m_pkSegment->Extent;
        m_afRectCoord[0] = kVTDist.GetRectangleCoordinate(0);
        m_afRectCoord[1] = kVTDist.GetRectangleCoordinate(1);
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkRectangle->Center + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    Rectangle3<Real> kMRectangle(kMCenter,m_pkRectangle->Axis,
        m_pkRectangle->Extent);
    return DistSegment3Rectangle3<Real>(kMSegment,kMRectangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkRectangle->Center + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    Rectangle3<Real> kMRectangle(kMCenter,m_pkRectangle->Axis,
        m_pkRectangle->Extent);
    return DistSegment3Rectangle3<Real>(kMSegment,kMRectangle).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::GetSegmentParameter () const
{
    return m_fSegmentParameter;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Rectangle3<Real>::GetRectangleCoordinate (int i) const
{
    assert(0 <= i && i < 2);
    return m_afRectCoord[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistSegment3Rectangle3<float>;

template WM4_FOUNDATION_ITEM
class DistSegment3Rectangle3<double>;
//----------------------------------------------------------------------------
}
