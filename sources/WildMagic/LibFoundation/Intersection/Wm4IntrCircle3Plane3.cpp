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
#include "Wm4IntrCircle3Plane3.h"
#include "Wm4IntrPlane3Plane3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrCircle3Plane3<Real>::IntrCircle3Plane3 (
    const Circle3<Real>& rkCircle, const Plane3<Real>& rkPlane)
    :
    m_pkCircle(&rkCircle),
    m_pkPlane(&rkPlane)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Circle3<Real>& IntrCircle3Plane3<Real>::GetCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrCircle3Plane3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrCircle3Plane3<Real>::Test ()
{
    m_iQuantity = 0;

    // construct the plane of the circle
    Plane3<Real> kCPlane(m_pkCircle->N,m_pkCircle->Center);

    // compute the intersection of this plane with the input plane
    IntrPlane3Plane3<Real> kIntr(*m_pkPlane,kCPlane);
    if (!kIntr.Find())
    {
        // planes are parallel and nonintersectin
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    if (kIntr.GetIntersectionType() == IT_PLANE)
    {
        // planes are the same, the circle is the common intersection set
        m_iIntersectionType = IT_OTHER;
        return true;
    }

    // the planes intersect in a line
    const Line3<Real>& rkLine = kIntr.GetIntersectionLine();

    // Locate one or two points that are on the circle and line.  If the
    // line is t*D+P, the circle center is C, and the circle radius is r,
    // then r^2 = |t*D+P-C|^2 = |D|^2*t^2 + 2*Dot(D,P-C)*t + |P-C|^2.  This
    // is a quadratic equation of the form:  a2*t^2 + 2*a1*t + a0 = 0.
    Vector3<Real> kDiff = rkLine.Origin - m_pkCircle->Center;
    Real fA2 = rkLine.Direction.SquaredLength();
    Real fA1 = kDiff.Dot(rkLine.Direction);
    Real fA0 = kDiff.SquaredLength() - m_pkCircle->Radius*m_pkCircle->Radius;

    // real-valued roots imply an intersection
    Real fDiscr = fA1*fA1 - fA0*fA2;
    m_iIntersectionType = (fDiscr >= (Real)0.0 ? IT_POINT : IT_EMPTY);
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrCircle3Plane3<Real>::Find ()
{
    m_iQuantity = 0;

    // construct the plane of the circle
    Plane3<Real> kCPlane(m_pkCircle->N,m_pkCircle->Center);

    // compute the intersection of this plane with the input plane
    IntrPlane3Plane3<Real> kIntr(*m_pkPlane,kCPlane);
    if (!kIntr.Find())
    {
        // planes are parallel and nonintersectin
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    if (kIntr.GetIntersectionType() == IT_PLANE)
    {
        // planes are the same, the circle is the common intersection set
        m_iIntersectionType = IT_OTHER;
        return true;
    }

    // the planes intersect in a line
    const Line3<Real>& rkLine = kIntr.GetIntersectionLine();

    // Locate one or two points that are on the circle and line.  If the
    // line is t*D+P, the circle center is C, and the circle radius is r,
    // then r^2 = |t*D+P-C|^2 = |D|^2*t^2 + 2*Dot(D,P-C)*t + |P-C|^2.  This
    // is a quadratic equation of the form:  a2*t^2 + 2*a1*t + a0 = 0.
    Vector3<Real> kDiff = rkLine.Origin - m_pkCircle->Center;
    Real fA2 = rkLine.Direction.SquaredLength();
    Real fA1 = kDiff.Dot(rkLine.Direction);
    Real fA0 = kDiff.SquaredLength() - m_pkCircle->Radius*m_pkCircle->Radius;

    Real fDiscr = fA1*fA1 - fA0*fA2;
    if (fDiscr < (Real)0.0)
    {
        // no real roots, the circle does not intersect the plane
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    m_iIntersectionType = IT_POINT;

    Real fInvA2 = ((Real)1.0)/fA2;
    if (fDiscr < Math<Real>::ZERO_TOLERANCE)
    {
        // one repeated root, the circle just touches the plane
        m_iQuantity = 1;
        m_akPoint[0] = rkLine.Origin - (fA1*fInvA2)*rkLine.Direction;
        return true;
    }

    // Two distinct, real-valued roots, the circle intersects the plane in
    // two points.
    Real fRoot = Math<Real>::Sqrt(fDiscr);
    m_iQuantity = 2;
    m_akPoint[0] = rkLine.Origin - ((fA1 + fRoot)*fInvA2)*rkLine.Direction;
    m_akPoint[1] = rkLine.Origin - ((fA1 - fRoot)*fInvA2)*rkLine.Direction;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrCircle3Plane3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrCircle3Plane3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Circle3<Real>& IntrCircle3Plane3<Real>::GetIntersectionCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrCircle3Plane3<float>;

template WM4_FOUNDATION_ITEM
class IntrCircle3Plane3<double>;
//----------------------------------------------------------------------------
}
