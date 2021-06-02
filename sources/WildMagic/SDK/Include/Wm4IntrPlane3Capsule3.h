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

#ifndef WM4INTRPLANE3CAPSULE3_H
#define WM4INTRPLANE3CAPSULE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Plane3.h"
#include "Wm4Capsule3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrPlane3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Capsule3 (const Plane3<Real>& rkPlane,
        const Capsule3<Real>& rkCapsule);

    // object access
    const Plane3<Real>& GetPlane () const;
    const Capsule3<Real>& GetCapsule () const;

    // static intersection query
    virtual bool Test ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The capsule is culled if it is on the negative
    // side of the plane.
    bool CapsuleIsCulled () const;

protected:
    // the objects to intersect
    const Plane3<Real>* m_pkPlane;
    const Capsule3<Real>* m_pkCapsule;
};

typedef IntrPlane3Capsule3<float> IntrPlane3Capsule3f;
typedef IntrPlane3Capsule3<double> IntrPlane3Capsule3d;

}

#endif
