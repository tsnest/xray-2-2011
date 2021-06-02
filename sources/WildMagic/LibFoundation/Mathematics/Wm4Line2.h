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

#ifndef WM4LINE2_H
#define WM4LINE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector2.h"

namespace Wm4
{

template <class Real>
class Line2
{
public:
    // The line is represented as P+t*D where P is the line origin and D is
    // a unit-length direction vector.  The user must ensure that the
    // direction vector satisfies this condition.

    // construction
    Line2 ();  // uninitialized
    Line2 (const Vector2<Real>& rkOrigin, const Vector2<Real>& rkDirection);

    Vector2<Real> Origin, Direction;
};

#include "Wm4Line2.inl"

typedef Line2<float> Line2f;
typedef Line2<double> Line2d;

}

#endif
