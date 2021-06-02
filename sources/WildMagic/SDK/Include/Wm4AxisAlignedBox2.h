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

#ifndef WM4AXISALIGNEDBOX2_H
#define WM4AXISALIGNEDBOX2_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class AxisAlignedBox2
{
public:
    // construction
    AxisAlignedBox2 ();  // uninitialized

    // The caller must ensure that fXMin <= fXMax and fYMin <= fYMax.  The
    // class will not check for validity of the input.
    AxisAlignedBox2 (Real fXMin, Real fXMax, Real fYMin, Real fYMax);

    // Overlap testing is in the strict sense.  If the two boxes are just
    // touching along a common edge, the boxes are reported as overlapping.
    bool HasXOverlap (const AxisAlignedBox2& rkBox) const;
    bool HasYOverlap (const AxisAlignedBox2& rkBox) const;
    bool TestIntersection (const AxisAlignedBox2& rkBox) const;

    // The return value is 'true' if there is overlap.  In this case the
    // intersection is stored in rkIntr.  If the return value is 'false',
    // if there is no overlap.  In this case rkIntr is undefined.
    bool FindIntersection (const AxisAlignedBox2& rkBox,
        AxisAlignedBox2& rkIntr) const;

    Real Min[2], Max[2];
};

#include "Wm4AxisAlignedBox2.inl"

typedef AxisAlignedBox2<float> AxisAlignedBox2f;
typedef AxisAlignedBox2<double> AxisAlignedBox2d;

}

#endif
