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

#ifndef WM4TORUS_H
#define WM4TORUS_H

// Implementation of torus centered at (0,0,0) with z-axis as the
// axis of symmetry (axis about which circle is rotated to generate
// the torus).
//
// algebraic form
//      Ro > 0 is radius from center of torus
//      Ri > 0 is radius of tube of torus
//      p^2 = x^2+y^2+z^2
//      p^4-2*(Ro^2+Ri^2)*p^2+4*Ro^2*z^2+(Ro^2-Ri^2)^2 = 0
//
// parametric form
//      0 <= s <= 1, 0 <= t <= 1
//      Rc = Ro+Ri*cos(2*PI*t)
//      x = Rc*cos(2*PI*s)
//      y = Rc*sin(2*PI*s)
//      z = Ri*sin(2*PI*t)

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class Torus3
{
public:
    // construction
    Torus3 ();  // uninitialized
    Torus3 (Real fRo, Real fRi);

    Vector3<Real> Position (Real fS, Real fT);
    Vector3<Real> Normal (Real fS, Real fT);

    // for use in intersection testing
    void GetParameters (const Vector3<Real>& rkPos, Real& rfS,
        Real& rfT) const;

    Real Ro, Ri;
};

#include "Wm4Torus3.inl"

typedef Torus3<float> Torus3f;
typedef Torus3<double> Torus3d;

}

#endif
