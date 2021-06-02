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

#ifndef WM4APPRLINEFIT2_H
#define WM4APPRLINEFIT2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Line2.h"

namespace Wm4
{

// Least-squares fit of a line to (x,f(x)) data by using distance
// measurements in the y-direction.  The resulting line is represented by
// y = A*x + B.  The return value is 'false' if the 2x2 coefficient matrix
// in the linear system that defines A and B is (nearly) singular.  In this
// case, A and B are returned as MAX_REAL.
template <class Real> WM4_FOUNDATION_ITEM
bool HeightLineFit2 (int iQuantity, const Vector2<Real>* akPoint, Real& rfA,
    Real& rfB);

// Least-squares fit of a line to (x,y,z) data by using distance measurements
// orthogonal to the proposed line.
template <class Real> WM4_FOUNDATION_ITEM
Line2<Real> OrthogonalLineFit2 (int iQuantity, const Vector2<Real>* akPoint);

}

#endif
