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

#ifndef WM4ARC2_H
#define WM4ARC2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Circle2.h"

namespace Wm4
{

template <class Real>
class Arc2 : public Circle2<Real>
{
public:
    // The arc is defined by two points End0 and End1 on the circle so that
    // End1 is obtained from End0 by traversing counterclockwise.  The
    // application is responsible for ensuring that End0 and End1 are on the
    // circle and that they are properly ordered.

    Arc2 ();  // uninitialized
    Arc2 (const Vector2<Real>& rkCenter, Real fRadius,
        const Vector2<Real>& rkEnd0, const Vector2<Real>& rkEnd1);

    // Test if P is on the arc.  The application must ensure that P is on the
    // circle; that is, |P-C| = R.  This test works regardless of the angle
    // between B-C and A-C.
    bool Contains (const Vector2<Real>& rkP) const;

    Vector2<Real> End0, End1;
};

#include "Wm4Arc2.inl"

typedef Arc2<float> Arc2f;
typedef Arc2<double> Arc2d;

}

#endif
