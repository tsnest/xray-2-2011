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
// Version: 4.0.0 (2006/06/28)

#ifndef WM4CONTPOINTINPOLYGON2_H
#define WM4CONTPOINTINPOLYGON2_H

// Given a polygon as an order list of vertices (x[i],y[i]) for 0 <= i < N
// and a test point (xt,yt), return 'true' if (xt,yt) is in the polygon and
// 'false' if it is not.  All queries require that the number of vertices
// satisfies N >= 3.

#include "Wm4FoundationLIB.h"
#include "Wm4Vector2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM PointInPolygon2
{
public:
    PointInPolygon2 (int iQuantity, const Vector2<Real>* akVertex);

    // simple polygons (ray-intersection counting)
    bool Contains (const Vector2<Real>& rkP) const;

    // Algorithms for convex polygons.  The input polygons must have vertices
    // in counterclockwise order.

    // O(N) algorithm (which-side-of-edge tests)
    bool ContainsConvexOrderN (const Vector2<Real>& rkP) const;

    // O(log N) algorithm (bisection and recursion, like BSP tree)
    bool ContainsConvexOrderLogN (const Vector2<Real>& rkP) const;

    // The polygon must have exactly four vertices.  This method is like the
    // O(log N) and uses three which-side-of-segment test instead of four
    // which-side-of-edge tests.  If the polygon does not have four vertices,
    // the function returns false.
    bool ContainsQuadrilateral (const Vector2<Real>& rkP) const;

private:
    // for recursion in ContainsConvexOrderLogN
    bool SubContainsPoint (const Vector2<Real>& rkP, int i0, int i1) const;

    int m_iQuantity;
    const Vector2<Real>* m_akVertex;
};

typedef PointInPolygon2<float> PointInPolygon2f;
typedef PointInPolygon2<double> PointInPolygon2d;

}

#endif
