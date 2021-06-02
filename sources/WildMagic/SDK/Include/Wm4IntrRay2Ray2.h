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
// Version: 4.8.0 (2009/04/10)

#ifndef WM4INTRRAY2RAY2_H
#define WM4INTRRAY2RAY2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Ray2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrRay2Ray2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrRay2Ray2 (const Ray2<Real>& rkRay0, const Ray2<Real>& rkRay1);

    // object access
    const Ray2<Real>& GetRay0 () const;
    const Ray2<Real>& GetRay1 () const;

    // static intersection query
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the rays do not intersect, GetQuantity()
    // returns 0.  If the rays intersect in a single point, GetQuantity()
    // returns 1, in which case GetPoint() returns the point of intersection
    // and Intersector::GetIntersectionType() returns IT_POINT.  If the rayd
    // are collinear, GetQuantity() returns INT_MAX and
    // Intersector::GetIntersectionType() returns IT_RAY.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_RAY;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    // Determine the relationship between the lines that contain the rays.
    int Classify (Real* afS, Vector2<Real>* pkDiff, Vector2<Real>* pkDiffN);

    // the objects to intersect
    const Ray2<Real>* m_pkRay0;
    const Ray2<Real>* m_pkRay1;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_kPoint;
};

typedef IntrRay2Ray2<float> IntrRay2Ray2f;
typedef IntrRay2Ray2<double> IntrRay2Ray22d;

}

#endif
