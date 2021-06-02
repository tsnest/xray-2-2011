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

#ifndef WM4BOX2_H
#define WM4BOX2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector2.h"

namespace Wm4
{

template <class Real>
class Box2
{
public:
    // A box has center C, axis directions U[0] and U[1] (both unit-length
    // vectors), and extents e[0] and e[1] (both nonnegative numbers).  A
    // point X = C+y[0]*U[0]+y[1]*U[1] is inside or on the box whenever
    // |y[i]| <= e[i] for all i.

    // construction
    Box2 ();  // uninitialized
    Box2 (const Vector2<Real>& rkCenter, const Vector2<Real>* akAxis,
        const Real* afExtent);
    Box2 (const Vector2<Real>& rkCenter, const Vector2<Real>& rkAxis0,
        const Vector2<Real>& rkAxis1, Real fExtent0, Real fExtent1);

    void ComputeVertices (Vector2<Real> akVertex[4]) const;

    Vector2<Real> Center;
    Vector2<Real> Axis[2];  // must be an orthonormal set of vectors
    Real Extent[2];         // must be nonnegative
};

#include "Wm4Box2.inl"

typedef Box2<float> Box2f;
typedef Box2<double> Box2d;

}

#endif
