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

#ifndef WM4INTRTRIANGLE3SPHERE3_H
#define WM4INTRTRIANGLE3SPHERE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Triangle3.h"
#include "Wm4Sphere3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrTriangle3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Sphere3 (const Triangle3<Real>& rkTriangle,
        const Sphere3<Real>& rkSphere);

    // Object access.
    const Triangle3<Real>& GetTriangle () const;
    const Sphere3<Real>& GetSphere () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Dynamic find-intersection query.  If the query returns 'true', use
    // GetContactTime() to obtain the first time of contact.  If this time
    // is zero, the triangle is already intersecting the sphere and no
    // contact set is computed.  If this time is positive, obtain the
    // first point of contact using GetPoint().
    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    const Vector3<Real>& GetPoint () const;

private:
    using Intersector<Real,Vector3<Real> >::m_fContactTime;

    // Support for the dynamic query.
    bool FindTriangleSphereCoplanarIntersection (int iVertex,
        const Vector3<Real> akV[3], const Vector3<Real>& rkSideNorm,
        const Vector3<Real>& rkSide, Real fTMax,
        const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1);

    bool FindSphereVertexIntersection (const Vector3<Real>& rkVertex,
        Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // The objects to intersect.
    const Triangle3<Real>* m_pkTriangle;
    const Sphere3<Real>* m_pkSphere;

    // The point of contact for dynamic queries.
    Vector3<Real> m_kPoint;
};

typedef IntrTriangle3Sphere3<float> IntrTriangle3Sphere3f;
typedef IntrTriangle3Sphere3<double> IntrTriangle3Sphere3d;

}

#endif
