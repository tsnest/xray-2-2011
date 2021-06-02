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

#ifndef WM4INTRRAY3PLANE3_H
#define WM4INTRRAY3PLANE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Ray3.h"
#include "Wm4Plane3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrRay3Plane3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Plane3 (const Ray3<Real>& rkRay, const Plane3<Real>& rkPlane);

    // object access
    const Ray3<Real>& GetRay () const;
    const Plane3<Real>& GetPlane () const;

    // test-intersection query
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    // P = origin + t*direction, with t >= 0.
    virtual bool Find ();
    Real GetRayT () const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::m_iIntersectionType;

    // the objects to intersect
    const Ray3<Real>* m_pkRay;
    const Plane3<Real>* m_pkPlane;

    // information about the intersection set
    Real m_fRayT;
};

typedef IntrRay3Plane3<float> IntrRay3Plane3f;
typedef IntrRay3Plane3<double> IntrRay3Plane3d;

}

#endif
