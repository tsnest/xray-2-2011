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
// Version: 4.0.2 (2008/07/09)

#ifndef WM4CONTBOX2_H
#define WM4CONTBOX2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Box2.h"
#include "Wm4Query.h"

namespace Wm4
{

// Compute the smallest axis-aligned bounding box of the points.
template <class Real> WM4_FOUNDATION_ITEM
Box2<Real> ContAlignedBox (int iQuantity, const Vector2<Real>* akPoint);

// Compute an oriented bounding box of the points.  The box center is the
// average of the points.  The box axes are the eigenvectors of the covariance
// matrix.
template <class Real> WM4_FOUNDATION_ITEM
Box2<Real> ContOrientedBox (int iQuantity, const Vector2<Real>* akPoint);

// Compute a minimum area oriented box containing the specified points.  The
// algorithm uses the rotating calipers method.  If the input points represent
// a counterclockwise-ordered polygon, set bIsConvexPolygon to 'true'.
template <class Real> WM4_FOUNDATION_ITEM
Box2<Real> ContMinBox (int iQuantity, const Vector2<Real>* akPoint,
    Real fEpsilon, Query::Type eQueryType, bool bIsConvexPolygon);

// Test for containment.  Let X = C + y0*U0 + y1*U1 where C is the box center
// and U0 and U1 are the orthonormal axes of the box.  X is in the box if
// |y_i| <= E_i for all i where E_i are the extents of the box.
template <class Real> WM4_FOUNDATION_ITEM
bool InBox (const Vector2<Real>& rkPoint, const Box2<Real>& rkBox);

// Construct an oriented box that contains two other oriented boxes.  The
// result is not guaranteed to be the minimum volume box containing the
// input boxes.
template <class Real> WM4_FOUNDATION_ITEM
Box2<Real> MergeBoxes (const Box2<Real>& rkBox0, const Box2<Real>& rkBox1);

}

#endif
