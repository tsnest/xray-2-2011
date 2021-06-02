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

#ifndef WM4INTRRAY3TRIANGLE3_H
#define WM4INTRRAY3TRIANGLE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Ray3.h"
#include "Wm4Triangle3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrRay3Triangle3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Triangle3 (const Ray3<Real>& rkRay,
        const Triangle3<Real>& rkTriangle);

    // object access
    const Ray3<Real>& GetRay () const;
    const Triangle3<Real>& GetTriangle () const;

    // test-intersection query
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    //   P = origin + t*direction = b0*V0 + b1*V1 + b2*V2
    virtual bool Find ();
    Real GetRayT () const;
    Real GetTriB0 () const;
    Real GetTriB1 () const;
    Real GetTriB2 () const;

private:
    // the objects to intersect
    const Ray3<Real>* m_pkRay;
    const Triangle3<Real>* m_pkTriangle;

    // information about the intersection set
    Real m_fRayT, m_fTriB0, m_fTriB1, m_fTriB2;
};

typedef IntrRay3Triangle3<float> IntrRay3Triangle3f;
typedef IntrRay3Triangle3<double> IntrRay3Triangle3d;

}

#endif
