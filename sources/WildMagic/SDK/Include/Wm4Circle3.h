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

#ifndef WM4CIRCLE3_H
#define WM4CIRCLE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class Circle3
{
public:
    // Plane containing circle is Dot(N,X-C) = 0 where X is any point in the
    // plane.  Vectors U, V, and N form an orthonormal right-handed set
    // (matrix [U V N] is orthonormal and has determinant 1).  Circle within
    // the plane is parameterized by X = C + R*(cos(A)*U + sin(A)*V) where
    // A is an angle in [0,2*pi).

    // construction
    Circle3 ();  // uninitialized
    Circle3 (const Vector3<Real>& rkCenter, const Vector3<Real>& rkU,
        const Vector3<Real>& rkV, const Vector3<Real>& rkN, Real fRadius);

    Vector3<Real> Center, U, V, N;
    Real Radius;
};

#include "Wm4Circle3.inl"

typedef Circle3<float> Circle3f;
typedef Circle3<double> Circle3d;

}

#endif
