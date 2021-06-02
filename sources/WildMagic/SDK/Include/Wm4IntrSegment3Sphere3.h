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

#ifndef WM4INTRSEGMENT3SPHERE3_H
#define WM4INTRSEGMENT3SPHERE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Segment3.h"
#include "Wm4Sphere3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrSegment3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Sphere3 (const Segment3<Real>& rkSegment,
        const Sphere3<Real>& rkSphere);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Sphere3<Real>& GetSphere () const;

    // test-intersection query
    virtual bool Test ();

    // find-intersection query
    virtual bool Find ();
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetSegmentT (int i) const;

    // dynamic test-intersection query
    virtual bool Test (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // Dynamic find-intersection query.  The first point of contact is
    // accessed by GetPoint(0).  The first time of contact is accessed by
    // GetContactTime().
    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    Real ZeroThreshold;  // default = Math<Real>::ZERO_TOLERANCE

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_OTHER;
    using Intersector<Real,Vector3<Real> >::m_iIntersectionType;
    using Intersector<Real,Vector3<Real> >::m_fContactTime;

    // the objects to intersect
    const Segment3<Real>* m_pkSegment;
    const Sphere3<Real>* m_pkSphere;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
    Real m_afSegmentT[2];
};

typedef IntrSegment3Sphere3<float> IntrSegment3Sphere3f;
typedef IntrSegment3Sphere3<double> IntrSegment3Sphere3d;

}

#endif
