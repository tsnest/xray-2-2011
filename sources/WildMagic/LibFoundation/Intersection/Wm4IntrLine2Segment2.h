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
// Version: 4.8.0 (2009/04/10)

#ifndef WM4INTRLINE2SEGMENT2_H
#define WM4INTRLINE2SEGMENT2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Line2.h"
#include "Wm4Segment2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrLine2Segment2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Segment2 (const Line2<Real>& rkLine,
        const Segment2<Real>& rkSegment);

    // object access
    const Line2<Real>& GetLine () const;
    const Segment2<Real>& GetSegment () const;

    // static intersection query
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the line and ray do not intersect,
    // GetQuantity() returns 0.  If the line and segment intersect in a single
    // point, GetQuantity() returns 1, in which case GetPoint() returns the
    // point of intersection and Intersector::GetIntersectionType() returns
    // IT_POINT.  If the line and segment are collinear and intersect in a
    // segment, GetQuantity() returns INT_MAX and
    // Intersector::GetIntersectionType() returns IT_SEGMENT.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    // Determine the relationship between the line and the line that contains
    // the segment.
    int Classify (Real* afS, Vector2<Real>* pkDiff, Vector2<Real>* pkDiffN);

    // the objects to intersect
    const Line2<Real>* m_pkLine;
    const Segment2<Real>* m_pkSegment;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_kPoint;
};

typedef IntrLine2Segment2<float> IntrLine2Segment2f;
typedef IntrLine2Segment2<double> IntrLine2Segment2d;

}

#endif
