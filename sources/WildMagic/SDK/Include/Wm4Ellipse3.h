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
// Version: 4.7.0 (2008/08/17)

#ifndef WM4ELLIPSE3_H
#define WM4ELLIPSE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class Ellipse3
{
public:
    // Plane containing ellipse is Dot(N,X-C) = 0 where X is any point in the
    // plane.  Vectors U, V, and N form an orthonormal right-handed set
    // (matrix [U V N] is orthonormal and has determinant 1).  Ellipse within
    // the plane is parameterized by X = C + a*cos(t)*U + b*sin(t)*V where
    // t is an angle in [0,2*pi) and where a >= b > 0.  The symbols in this
    // discussion are related to the class members as follows.  The member
    // 'Center' is C, 'Normal' is N, 'Major' is U, 'Minor' is V, 'MajorLength'
    // is a, and 'MinorLength' is b.

    // construction
    Ellipse3 ();  // uninitialized
    Ellipse3 (const Vector3<Real>& rkCenter, const Vector3<Real>& rkNormal,
        const Vector3<Real>& rkMajor, const Vector3<Real>& rkMinor, 
        Real fMajorLength, Real fMinorLength);

    Vector3<Real> Center, Normal, Major, Minor;
    Real MajorLength, MinorLength;
};

#include "Wm4Ellipse3.inl"

typedef Ellipse3<float> Ellipse3f;
typedef Ellipse3<double> Ellipse3d;

}

#endif
