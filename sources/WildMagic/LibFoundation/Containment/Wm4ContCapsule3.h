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

#ifndef WM4CONTCAPSULE3_H
#define WM4CONTCAPSULE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Capsule3.h"
#include "Wm4Sphere3.h"

namespace Wm4
{

// Compute axis of capsule segment using least-squares fit.  Radius is
// maximum distance from points to axis.  Hemispherical caps are chosen
// as close together as possible.
template <class Real> WM4_FOUNDATION_ITEM
Capsule3<Real> ContCapsule (int iQuantity, const Vector3<Real>* akPoint);

// Test for containment of a point by a capsule.
template <class Real> WM4_FOUNDATION_ITEM
bool InCapsule (const Vector3<Real>& rkPoint,
    const Capsule3<Real>& rkCapsule);

// Test for containment of a sphere by a capsule.
template <class Real> WM4_FOUNDATION_ITEM
bool InCapsule (const Sphere3<Real>& rkSphere,
    const Capsule3<Real>& rkCapsule);

// Test for containment of a capsule by a capsule.
template <class Real> WM4_FOUNDATION_ITEM
bool InCapsule (const Capsule3<Real>& rkTestCapsule,
    const Capsule3<Real>& rkCapsule);

// Compute a capsule that contains the input capsules.  The returned capsule
// is not necessarily the one of smallest volume that contains the inputs.
template <class Real> WM4_FOUNDATION_ITEM
Capsule3<Real> MergeCapsules (const Capsule3<Real>& rkCapsule0,
    const Capsule3<Real>& rkCapsule1);

}

#endif
