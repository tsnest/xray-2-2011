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

#ifndef WM4INTRCIRCLE2CIRCLE2_H
#define WM4INTRCIRCLE2CIRCLE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Circle2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrCircle2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrCircle2Circle2 (const Circle2<Real>& rkCircle0,
        const Circle2<Real>& rkCircle1);

    // object access
    const Circle2<Real>& GetCircle0 () const;
    const Circle2<Real>& GetCircle1 () const;

    // static find-intersection query
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q is
    // 0, 1, or 2.  When Q > 0, the interpretation depends on the intersection
    // type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The circles are the same.  One of the circle objects is
    //              returned by GetIntersectionCircle.  Q is invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Circle2<Real>& GetIntersectionCircle () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    // the objects to intersect
    const Circle2<Real>* m_pkCircle0;
    const Circle2<Real>* m_pkCircle1;

    // points of intersection
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrCircle2Circle2<float> IntrCircle2Circle2f;
typedef IntrCircle2Circle2<double> IntrCircle2Circle2d;

}

#endif
