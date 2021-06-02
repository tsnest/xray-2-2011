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

#ifndef WM4CONTSCRIBECIRCLE3SPHERE3_H
#define WM4CONTSCRIBECIRCLE3SPHERE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Circle3.h"
#include "Wm4Sphere3.h"

namespace Wm4
{

// All functions return 'true' if circle/sphere has been constructed,
// 'false' otherwise (input points are linearly dependent).

// circle containing three 3D points
template <class Real> WM4_FOUNDATION_ITEM
bool Circumscribe (const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
    const Vector3<Real>& rkV2, Circle3<Real>& rkCircle);

// sphere containing four 3D points
template <class Real> WM4_FOUNDATION_ITEM
bool Circumscribe (const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
    const Vector3<Real>& rkV2, const Vector3<Real>& rkV3,
    Sphere3<Real>& rkSphere);

// circle inscribing triangle of three 3D points
template <class Real> WM4_FOUNDATION_ITEM
bool Inscribe (const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
    const Vector3<Real>& rkV2, Circle3<Real>& rkCircle);

// sphere inscribing tetrahedron of four 3D points
template <class Real> WM4_FOUNDATION_ITEM
bool Inscribe (const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
    const Vector3<Real>& rkV2, const Vector3<Real>& rkV3,
    Sphere3<Real>& rkSphere);

}

#endif
