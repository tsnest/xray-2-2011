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

#ifndef WM4CIRCLE2_H
#define WM4CIRCLE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector2.h"

namespace Wm4
{

template <class Real>
class Circle2
{
public:
    // construction
    Circle2 ();  // uninitialized
    Circle2 (const Vector2<Real>& rkCenter, Real fRadius);

    Vector2<Real> Center;
    Real Radius;
};

#include "Wm4Circle2.inl"

typedef Circle2<float> Circle2f;
typedef Circle2<double> Circle2d;

}

#endif
