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

#ifndef WM4INTRLINE3CAPSULE3_H
#define WM4INTRLINE3CAPSULE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Line3.h"
#include "Wm4Capsule3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrLine3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Capsule3 (const Line3<Real>& rkLine,
        const Capsule3<Real>& rkCapsule);

    // object access
    const Line3<Real>& GetLine () const;
    const Capsule3<Real>& GetCapsule () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::m_iIntersectionType;

    // the objects to intersect
    const Line3<Real>* m_pkLine;
    const Capsule3<Real>* m_pkCapsule;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];

// internal use (shared by IntrRay3Capsule3 and IntrSegment3Capsule3)
public:
    static int Find (const Vector3<Real>& rkOrigin,
        const Vector3<Real>& rkDir, const Capsule3<Real>& rkCapsule,
        Real afT[2]);
};

typedef IntrLine3Capsule3<float> IntrLine3Capsule3f;
typedef IntrLine3Capsule3<double> IntrLine3Capsule3d;

}

#endif
