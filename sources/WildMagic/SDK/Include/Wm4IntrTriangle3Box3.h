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

#ifndef WM4INTRTRIANGLE3BOX3_H
#define WM4INTRTRIANGLE3BOX3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Triangle3.h"
#include "Wm4Box3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrTriangle3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Box3 (const Triangle3<Real>& rkTriangle,
        const Box3<Real>& rkBox);

    // Object access.
    const Triangle3<Real>& GetTriangle () const;
    const Box3<Real>& GetBox () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Static test-intersection query.
    virtual bool Find ();

    // Dynamic test-intersection query.
    virtual bool Test (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // Dynamic find-intersection query.
    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::m_fContactTime;

    // The objects to intersect.
    const Triangle3<Real>* m_pkTriangle;
    const Box3<Real>* m_pkBox;

    // The intersections for static query.  A triangle can intersect a box
    // in at most 7 vertices.
    int m_iQuantity;
    Vector3<Real> m_akPoint[7];
};

typedef IntrTriangle3Box3<float> IntrTriangle3Box3f;
typedef IntrTriangle3Box3<double> IntrTriangle3Box3d;

}

#endif
