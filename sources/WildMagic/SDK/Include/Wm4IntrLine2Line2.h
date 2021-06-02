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

#ifndef WM4INTRLINE2LINE2_H
#define WM4INTRLINE2LINE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Line2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrLine2Line2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Line2 (const Line2<Real>& rkLine0, const Line2<Real>& rkLine1);

    // object access
    const Line2<Real>& GetLine0 () const;
    const Line2<Real>& GetLine1 () const;

    // static intersection query
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the lines do not intersect, GetQuantity()
    // returns 0.  If the lines intersect in a single point, GetQuantity()
    // returns 1, in which case GetPoint() returns the point of intersection
    // and Intersector::GetIntersectionType() returns IT_POINT.  If the lines
    // are the same geometric entity, GetQuantity() returns INT_MAX and
    // Intersector::GetIntersectionType() returns IT_LINE.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_LINE;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    // Determine the relationship between the two lines.
    int Classify (Real* afS, Vector2<Real>* pkDiff, Vector2<Real>* pkDiffN);

    // the objects to intersect
    const Line2<Real>* m_pkLine0;
    const Line2<Real>* m_pkLine1;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_kPoint;
};

typedef IntrLine2Line2<float> IntrLine2Line2f;
typedef IntrLine2Line2<double> IntrLine2Line2d;

}

#endif
