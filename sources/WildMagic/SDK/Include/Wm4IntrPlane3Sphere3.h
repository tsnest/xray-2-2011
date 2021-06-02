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
// Version: 4.0.2 (2008/08/17)

#ifndef WM4INTRPLANE3SPHERE3_H
#define WM4INTRPLANE3SPHERE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Plane3.h"
#include "Wm4Sphere3.h"
#include "Wm4Circle3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrPlane3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Sphere3 (const Plane3<Real>& rkPlane,
        const Sphere3<Real>& rkSphere);

    // object access
    const Plane3<Real>& GetPlane () const;
    const Sphere3<Real>& GetSphere () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The sphere is culled if it is on the negative
    // side of the plane.
    bool SphereIsCulled () const;

    // The intersection set.  The circle is valid only when Find() returns
    // 'true'.  If the intersection does not exist, the circle radius is
    // set to -1 as an additional indication that the circle is not valid.
    const Circle3<Real>& GetCircle () const;

protected:
    // The objects to intersect.
    const Plane3<Real>* m_pkPlane;
    const Sphere3<Real>* m_pkSphere;

    // The intersection set.
    Circle3<Real> m_kCircle;
};

typedef IntrPlane3Sphere3<float> IntrPlane3Sphere3f;
typedef IntrPlane3Sphere3<double> IntrPlane3Sphere3d;

}

#endif
