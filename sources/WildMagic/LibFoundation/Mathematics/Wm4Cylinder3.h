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

#ifndef WM4CYLINDER3_H
#define WM4CYLINDER3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Segment3.h"

namespace Wm4
{

template <class Real>
class Cylinder3
{
public:
    // Cylinder line segment has end points C-(H/2)*D and C+(H/2)*D where
    // D is a unit-length vector.  H is infinity for infinite cylinder.

    // construction
    Cylinder3 ();  // uninitialized
    Cylinder3 (const Segment3<Real>& rkSegment, Real fHeight, Real fRadius);

    Segment3<Real> Segment;
    Real Height, Radius;
};

#include "Wm4Cylinder3.inl"

typedef Cylinder3<float> Cylinder3f;
typedef Cylinder3<double> Cylinder3d;

}

#endif
