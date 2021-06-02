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

#ifndef WM4INTRSEGMENT2CIRCLE2_H
#define WM4INTRSEGMENT2CIRCLE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Segment2.h"
#include "Wm4Circle2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrSegment2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrSegment2Circle2 (const Segment2<Real>& rkSegment,
        const Circle2<Real>& rkCircle);

    // object access
    const Segment2<Real>& GetSegment () const;
    const Circle2<Real>& GetCircle () const;

    // static intersection query
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    // the objects to intersect
    const Segment2<Real>* m_pkSegment;
    const Circle2<Real>* m_pkCircle;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrSegment2Circle2<float> IntrSegment2Circle2f;
typedef IntrSegment2Circle2<double> IntrSegment2Circle2d;

}

#endif
