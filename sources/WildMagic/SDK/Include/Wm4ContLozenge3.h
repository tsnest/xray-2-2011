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

#ifndef WM4CONTLOZENGE3_H
#define WM4CONTLOZENGE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Lozenge3.h"

namespace Wm4
{

// Compute plane of lozenge rectangle using least-squares fit.  Parallel
// planes are chosen close enough together so that all the data points lie
// between them.  The radius is half the distance between the two planes.
// The half-cylinder and quarter-cylinder side pieces are chosen using a
// method similar to that used for fitting by capsules.
template <class Real> WM4_FOUNDATION_ITEM
Lozenge3<Real> ContLozenge (int iQuantity, const Vector3<Real>* akPoint);

// Test for containment of a point x by a lozenge.
template <class Real> WM4_FOUNDATION_ITEM
bool InLozenge (const Vector3<Real>& rkPoint,
    const Lozenge3<Real>& rkLozenge);

}

#endif
