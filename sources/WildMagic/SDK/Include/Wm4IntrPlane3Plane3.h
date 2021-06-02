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

#ifndef WM4INTRPLANE3PLANE3_H
#define WM4INTRPLANE3PLANE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Plane3.h"
#include "Wm4Line3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrPlane3Plane3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Plane3 (const Plane3<Real>& rkPlane0,
        const Plane3<Real>& rkPlane1);

    // object access
    const Plane3<Real>& GetPlane0 () const;
    const Plane3<Real>& GetPlane1 () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // dynamic intersection queries
    virtual bool Test (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // Information about the intersection set.  Only get the specific object
    // of intersection corresponding to the intersection type (IT_LINE or
    // IT_PLANE).
    const Line3<Real>& GetIntersectionLine () const;
    const Plane3<Real>& GetIntersectionPlane () const;

protected:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_LINE;
    using Intersector<Real,Vector3<Real> >::IT_PLANE;
    using Intersector<Real,Vector3<Real> >::m_iIntersectionType;
    using Intersector<Real,Vector3<Real> >::m_fContactTime;

    // the objects to intersect
    const Plane3<Real>* m_pkPlane0;
    const Plane3<Real>* m_pkPlane1;

    // information about the intersection set
    Line3<Real> m_kIntrLine;
    Plane3<Real> m_kIntrPlane;
};

typedef IntrPlane3Plane3<float> IntrPlane3Plane3f;
typedef IntrPlane3Plane3<double> IntrPlane3Plane3d;

}

#endif
