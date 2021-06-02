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

#ifndef WM4INTRSEGMENT3CAPSULE3_H
#define WM4INTRSEGMENT3CAPSULE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Segment3.h"
#include "Wm4Capsule3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrSegment3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Capsule3 (const Segment3<Real>& rkSegment,
        const Capsule3<Real>& rkCapsule);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Capsule3<Real>& GetCapsule () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetParameter (int i) const;  // segment parameters for points

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::m_iIntersectionType;

    // the objects to intersect
    const Segment3<Real>* m_pkSegment;
    const Capsule3<Real>* m_pkCapsule;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
    Real m_afParameter[2];
};

typedef IntrSegment3Capsule3<float> IntrSegment3Capsule3f;
typedef IntrSegment3Capsule3<double> IntrSegment3Capsule3d;

}

#endif
