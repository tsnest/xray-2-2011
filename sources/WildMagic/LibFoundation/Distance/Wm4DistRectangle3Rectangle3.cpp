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
#include "Wm4DistRectangle3Rectangle3.h"
#include "Wm4DistSegment3Rectangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistRectangle3Rectangle3<Real>::DistRectangle3Rectangle3 (
    const Rectangle3<Real>& rkRectangle0,
    const Rectangle3<Real>& rkRectangle1)
    :
    m_pkRectangle0(&rkRectangle0),
    m_pkRectangle1(&rkRectangle1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistRectangle3Rectangle3<Real>::GetRectangle0 () const
{
    return *m_pkRectangle0;
}
//----------------------------------------------------------------------------
template <class Real>
const Rectangle3<Real>& DistRectangle3Rectangle3<Real>::GetRectangle1 () const
{
    return *m_pkRectangle1;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::GetSquared ()
{
    // compare edges of rectangle0 to the interior of rectangle1
    Real fSqrDist = Math<Real>::MAX_REAL, fSqrDistTmp;
    Segment3<Real> kEdge;
    int i0, i1;
    for (i1 = 0; i1 < 2; i1++)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            kEdge.Origin = m_pkRectangle0->Center +
                (i0*m_pkRectangle0->Extent[1-i1]) *
                m_pkRectangle0->Axis[1-i1];
            kEdge.Direction = m_pkRectangle0->Axis[i1];
            kEdge.Extent = m_pkRectangle0->Extent[i1];
            DistSegment3Rectangle3<Real> kSRDist(kEdge,*m_pkRectangle1);
            fSqrDistTmp = kSRDist.GetSquared();
            if (fSqrDistTmp < fSqrDist)
            {
                m_kClosestPoint0 = kSRDist.GetClosestPoint0();
                m_kClosestPoint1 = kSRDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }

    // compare edges of rectangle1 to the interior of rectangle0
    for (i1 = 0; i1 < 2; i1++)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            kEdge.Origin = m_pkRectangle1->Center +
                (i0*m_pkRectangle1->Extent[1-i1]) *
                m_pkRectangle1->Axis[1-i1];
            kEdge.Direction = m_pkRectangle1->Axis[i1];
            kEdge.Extent = m_pkRectangle1->Extent[i1];
            DistSegment3Rectangle3<Real> kSRDist(kEdge,*m_pkRectangle0);
            fSqrDistTmp = kSRDist.GetSquared();
            if (fSqrDistTmp < fSqrDist)
            {
                m_kClosestPoint0 = kSRDist.GetClosestPoint0();
                m_kClosestPoint1 = kSRDist.GetClosestPoint1();
                fSqrDist = fSqrDistTmp;
            }
        }
    }

    return fSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMCenter0 = m_pkRectangle0->Center + fT*rkVelocity0;
    Vector3<Real> kMCenter1 = m_pkRectangle1->Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle0(kMCenter0,m_pkRectangle0->Axis,
        m_pkRectangle0->Extent);
    Rectangle3<Real> kMRectangle1(kMCenter1,m_pkRectangle1->Axis,
        m_pkRectangle1->Extent);
    return DistRectangle3Rectangle3<Real>(kMRectangle0,kMRectangle1).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRectangle3Rectangle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMCenter0 = m_pkRectangle0->Center + fT*rkVelocity0;
    Vector3<Real> kMCenter1 = m_pkRectangle1->Center + fT*rkVelocity1;
    Rectangle3<Real> kMRectangle0(kMCenter0,m_pkRectangle0->Axis,
        m_pkRectangle0->Extent);
    Rectangle3<Real> kMRectangle1(kMCenter1,m_pkRectangle1->Axis,
        m_pkRectangle1->Extent);
    return DistRectangle3Rectangle3<Real>(kMRectangle0,
        kMRectangle1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistRectangle3Rectangle3<float>;

template WM4_FOUNDATION_ITEM
class DistRectangle3Rectangle3<double>;
//----------------------------------------------------------------------------
}
