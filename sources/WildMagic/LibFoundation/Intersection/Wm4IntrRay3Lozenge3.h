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

#ifndef WM4INTRRAY3LOZENGE3_H
#define WM4INTRRAY3LOZENGE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Ray3.h"
#include "Wm4Lozenge3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrRay3Lozenge3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Lozenge3 (const Ray3<Real>& rkRay,
        const Lozenge3<Real>& rkLozenge);

    // object access
    const Ray3<Real>& GetRay () const;
    const Lozenge3<Real>& GetLozenge () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Ray3<Real>* m_pkRay;
    const Lozenge3<Real>* m_pkLozenge;
};

typedef IntrRay3Lozenge3<float> IntrRay3Lozenge3f;
typedef IntrRay3Lozenge3<double> IntrRay3Lozenge3d;

}

#endif
