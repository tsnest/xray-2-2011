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

#ifndef WM4INTRRAY3SPHERE3_H
#define WM4INTRRAY3SPHERE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Ray3.h"
#include "Wm4Sphere3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrRay3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Sphere3 (const Ray3<Real>& rkRay, const Sphere3<Real>& rkSphere);

    // object access
    const Ray3<Real>& GetRay () const;
    const Sphere3<Real>& GetSphere () const;

    // test-intersection query
    virtual bool Test ();

    // find-intersection query
    virtual bool Find ();
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetRayT (int i) const;

private:
    // the objects to intersect
    const Ray3<Real>* m_pkRay;
    const Sphere3<Real>* m_pkSphere;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
    Real m_afRayT[2];
};

typedef IntrRay3Sphere3<float> IntrRay3Sphere3f;
typedef IntrRay3Sphere3<double> IntrRay3Sphere3d;

}

#endif
