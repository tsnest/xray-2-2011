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
#include "Wm4DistSegment3Box3.h"
#include "Wm4DistVector3Box3.h"
#include "Wm4DistLine3Box3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistSegment3Box3<Real>::DistSegment3Box3 (const Segment3<Real>& rkSegment,
    const Box3<Real>& rkBox)
    :
    m_pkSegment(&rkSegment),
    m_pkBox(&rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistSegment3Box3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& DistSegment3Box3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Box3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Box3<Real>::GetSquared ()
{
    Line3<Real> kLine(m_pkSegment->Origin,m_pkSegment->Direction);
    DistLine3Box3<Real> kLBDist(kLine,*m_pkBox);
    Real fSqrDistance = kLBDist.GetSquared();
    Real fLParam = kLBDist.GetLineParameter();

    if (fLParam >= -m_pkSegment->Extent)
    {
        if (fLParam <= m_pkSegment->Extent)
        {
            m_kClosestPoint0 = kLBDist.GetClosestPoint0();
            m_kClosestPoint1 = kLBDist.GetClosestPoint1();
        }
        else
        {
            DistVector3Box3<Real> kVBDist(m_pkSegment->GetPosEnd(),*m_pkBox);
            fSqrDistance = kVBDist.GetSquared();
            m_kClosestPoint0 = kVBDist.GetClosestPoint0();
            m_kClosestPoint1 = kVBDist.GetClosestPoint1();
        }
    }
    else
    {
        DistVector3Box3<Real> kVBDist(m_pkSegment->GetNegEnd(),*m_pkBox);
        fSqrDistance = kVBDist.GetSquared();
        m_kClosestPoint0 = kVBDist.GetClosestPoint0();
        m_kClosestPoint1 = kVBDist.GetClosestPoint1();
    }
    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Box3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkBox->Center + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    Box3<Real> kMBox(kMCenter,m_pkBox->Axis,m_pkBox->Extent);
    return DistSegment3Box3<Real>(kMSegment,kMBox).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Box3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkSegment->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkBox->Center + fT*rkVelocity1;
    Segment3<Real> kMSegment(kMOrigin,m_pkSegment->Direction,
        m_pkSegment->Extent);
    Box3<Real> kMBox(kMCenter,m_pkBox->Axis,m_pkBox->Extent);
    return DistSegment3Box3<Real>(kMSegment,kMBox).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistSegment3Box3<float>;

template WM4_FOUNDATION_ITEM
class DistSegment3Box3<double>;
//----------------------------------------------------------------------------
}
