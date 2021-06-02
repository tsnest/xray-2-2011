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
// Version: 4.0.3 (2008/10/11)

#ifndef WM4INTRSEGMENT3BOX3_H
#define WM4INTRSEGMENT3BOX3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Segment3.h"
#include "Wm4Box3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrSegment3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Box3 (const Segment3<Real>& rkSegment,
        const Box3<Real>& rkBox, bool bSolid);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Box3<Real>& GetBox () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Static find-intersection query.  The point(s) of intersection are
    // accessed by GetQuantity() and GetPoint(i).
    virtual bool Find ();

    // Dynamic test-intersection query.
    virtual bool Test (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // Dynamic find-intersection query.  The first point of contact is
    // accessed by GetPoint(0), when there is a single contact, or by
    // GetPoint(0) and GetPoint(1), when the contact is a segment, in which
    // case the fetched points are the segment endpoints.  The first time of
    // contact is accessed by GetContactTime().
    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::m_fContactTime;
    using Intersector<Real,Vector3<Real> >::m_iIntersectionType;

    // The objects to intersect.
    const Segment3<Real>* m_pkSegment;
    const Box3<Real>* m_pkBox;
    bool m_bSolid;

    // Information about the intersection set.
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrSegment3Box3<float> IntrSegment3Box3f;
typedef IntrSegment3Box3<double> IntrSegment3Box3d;

}

#endif
