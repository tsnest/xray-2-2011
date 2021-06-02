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
// Version: 4.7.0 (2008/09/15)

#ifndef WM4INTRHALFSPACE3BOX3_H
#define WM4INTRHALFSPACE3BOX3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Plane3.h"
#include "Wm4Box3.h"

// A halfspace is the set of points on the side of a plane to which the plane
// normal points.  The queries here are for intersection of a box and a
// halfspace.  In the dynamice find query, if the box is already
// intersecting the halfspace, the return value is 'false'.  The idea is to
// find first time of contact.

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrHalfspace3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrHalfspace3Box3 (const Plane3<Real>& rkHalfspace,
        const Box3<Real>& rkBox);

    // object access
    const Plane3<Real>& GetHalfspace () const;
    const Box3<Real>& GetBox () const;

    // Static queries.
    virtual bool Test ();

    // Dynamic queries.
    virtual bool Test (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // The contact set is empty, a point, a segment, or a rectangle.  The
    // function GetQuantity() returns 0, 1, 2, or 4.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

protected:
    using Intersector<Real,Vector3<Real> >::m_fContactTime;

    // The objects to intersect.
    const Plane3<Real>* m_pkHalfspace;
    const Box3<Real>* m_pkBox;

    // Information about the intersection set.
    int m_iQuantity;
    Vector3<Real> m_akPoint[4];
};

typedef IntrHalfspace3Box3<float> IntrHalfspace3Box3f;
typedef IntrHalfspace3Box3<double> IntrHalfspace3Box3d;

}

#endif
