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
#include "Wm4IntrRay3Plane3.h"
#include "Wm4IntrLine3Plane3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Plane3<Real>::IntrRay3Plane3 (const Ray3<Real>& rkRay,
    const Plane3<Real>& rkPlane)
    :
    m_pkRay(&rkRay),
    m_pkPlane(&rkPlane)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Plane3<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrRay3Plane3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Plane3<Real>::Test ()
{
    return Find();
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Plane3<Real>::Find ()
{
    Line3<Real> kLine(m_pkRay->Origin,m_pkRay->Direction);
    IntrLine3Plane3<Real> kIntr(kLine,*m_pkPlane);
    if (kIntr.Find())
    {
        // The line intersects the plane, but possibly at a point that is
        // not on the ray.
        m_iIntersectionType = kIntr.GetIntersectionType();
        m_fRayT = kIntr.GetLineT();
        return m_fRayT >= (Real)0.0;
    }

    m_iIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrRay3Plane3<Real>::GetRayT () const
{
    return m_fRayT;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrRay3Plane3<float>;

template WM4_FOUNDATION_ITEM
class IntrRay3Plane3<double>;
//----------------------------------------------------------------------------
}
