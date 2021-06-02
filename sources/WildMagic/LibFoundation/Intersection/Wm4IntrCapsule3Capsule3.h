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

#ifndef WM4INTRCAPSULE3CAPSULE3_H
#define WM4INTRCAPSULE3CAPSULE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Capsule3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrCapsule3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrCapsule3Capsule3 (const Capsule3<Real>& rkCapsule0,
        const Capsule3<Real>& rkCapsule1);

    // object access
    const Capsule3<Real>& GetCapsule0 () const;
    const Capsule3<Real>& GetCapsule1 () const;

    // static test-intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Capsule3<Real>* m_pkCapsule0;
    const Capsule3<Real>* m_pkCapsule1;
};

typedef IntrCapsule3Capsule3<float> IntrCapsule3Capsule3f;
typedef IntrCapsule3Capsule3<double> IntrCapsule3Capsule3d;

}

#endif
