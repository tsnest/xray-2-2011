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

#ifndef WM4RAY3_H
#define WM4RAY3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class Ray3
{
public:
    // The ray is represented as P+t*D, where P is the ray origin, D is a
    // unit-length direction vector, and t >= 0.  The user must ensure that
    // the direction vector satisfies this condition.

    // construction
    Ray3 ();  // uninitialized
    Ray3 (const Vector3<Real>& rkOrigin, const Vector3<Real>& rkDirection);

    Vector3<Real> Origin, Direction;
};

#include "Wm4Ray3.inl"

typedef Ray3<float> Ray3f;
typedef Ray3<double> Ray3d;

}

#endif
