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

#ifndef WM4CONTCYLINDER3_H
#define WM4CONTCYLINDER3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Cylinder3.h"

namespace Wm4
{

// Compute the cylinder axis segment using least-squares fit.  The radius is
// the maximum distance from points to the axis.  The height is determined by
// projection of points onto the axis and determining the containing interval.
template <class Real> WM4_FOUNDATION_ITEM
Cylinder3<Real> ContCylinder (int iQuantity, const Vector3<Real>* akPoint);

}

#endif
