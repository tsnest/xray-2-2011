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

#ifndef WM4INTRBOX2CIRCLE2_H
#define WM4INTRBOX2CIRCLE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Box2.h"
#include "Wm4Circle2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrBox2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrBox2Circle2 (const Box2<Real>& rkBox, const Circle2<Real>& rkCircle);

    // object access
    const Box2<Real>& GetBox () const;
    const Circle2<Real>& GetCircle () const;

    // static test-intersection query
    virtual bool Test ();

    // dynamic find-intersection query
    virtual bool Find (Real fTMax, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);

    // intersection set for dynamic find-intersection query
    const Vector2<Real>& GetContactPoint () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;
    using Intersector<Real,Vector2<Real> >::m_fContactTime;

    // Support for dynamic Find.  Both functions return -1 if the objects are
    // initially intersecting, 0 if no intersection, or +1 if they intersect
    // at some positive time.
    int TestVertexRegion (Real fCx, Real fCy, Real fVx, Real fVy, Real fEx,
        Real fEy, Real& rfIx, Real& rfIy);

    int TestEdgeRegion (Real fCx, Real fCy, Real fVx, Real fVy, Real fEx,
        Real fEy, Real& rfIx, Real& rfIy);

    // the objects to intersect
    const Box2<Real>* m_pkBox;
    const Circle2<Real>* m_pkCircle;

    // point of intersection
    Vector2<Real> m_kContactPoint;
};

typedef IntrBox2Circle2<float> IntrBox2Circle2f;
typedef IntrBox2Circle2<double> IntrBox2Circle2d;

}

#endif
