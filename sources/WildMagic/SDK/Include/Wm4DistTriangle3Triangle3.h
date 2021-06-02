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

#ifndef WM4DISTTRIANGLE3TRIANGLE3_H
#define WM4DISTTRIANGLE3TRIANGLE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Triangle3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistTriangle3Triangle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistTriangle3Triangle3 (const Triangle3<Real>& rkTriangle0,
        const Triangle3<Real>& rkTriangle1);

    // object access
    const Triangle3<Real>& GetTriangle0 () const;
    const Triangle3<Real>& GetTriangle1 () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // Information about the closest points.
    Real GetTriangleBary0 (int i) const;
    Real GetTriangleBary1 (int i) const;

private:
    using Distance<Real,Vector3<Real> >::m_kClosestPoint0;
    using Distance<Real,Vector3<Real> >::m_kClosestPoint1;

    const Triangle3<Real>* m_pkTriangle0;
    const Triangle3<Real>* m_pkTriangle1;

    // Information about the closest points.
    Real m_afTriangleBary0[3];  // closest0 = sum_{i=0}^2 bary0[i]*vertex0[i]
    Real m_afTriangleBary1[3];  // closest1 = sum_{i=0}^2 bary1[i]*vertex1[i]
};

typedef DistTriangle3Triangle3<float> DistTriangle3Triangle3f;
typedef DistTriangle3Triangle3<double> DistTriangle3Triangle3d;

}

#endif
