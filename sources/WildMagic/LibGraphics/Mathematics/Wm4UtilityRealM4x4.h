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
// Version: 4.0.0 (2006/09/06)

#ifndef WM4UTILITYREALM4X4_H
#define WM4UTILITYREALM4X4_H

#include "Wm4GraphicsLIB.h"
#include "Wm4UtilityRealM.h"

namespace Wm4
{
// Create a diagonal matrix.
template <typename Real>
void MakeDiagonal (Real fM00, Real fM11, Real fM22, Real fM33,
    RealM<Real,4,4>& rkM);

// Compute the adjugate matrix of the matrix M, which is the transpose of the
// matrix of cofactors of M.
template <typename Real>
RealM<Real,4,4> Adjugate (const RealM<Real,4,4>& rkM);

// Compute the determinant of the matrix M.
template <typename Real>
Real Determinant (const RealM<Real,4,4>& rkM);

// Compute the inverse of the matrix M.  If the matrix is not invertible, then
// the function returns the zero matrix.
template <typename Real>
RealM<Real,4,4> Inverse (const RealM<Real,4,4>& rkM);

// Create a homogeneous matrix for projecting points onto a plane.  The
// direction of projection is not necessarily perpendicular to the plane but
// cannot be parallel to the plane.  The inputs are the plane normal, a point
// on the plane, and the direction of projection.
template <typename Real>
RealM<Real,4,4> MakeObliqueProjection (const RealV<Real,3>& rkNormal,
    const RealV<Real,3>& rkPoint, const RealV<Real,3>& rkDirection);

// Create a homogeneous matrix for perspective projection of points onto a
// plane.  The inputs are the plane normal, a point on the plane, and the eye
// point of the observer.
template <typename Real>
RealM<Real,4,4> MakePerspectiveProjection (const RealV<Real,3>& rkNormal,
    const RealV<Real,3>& rkPoint, const RealV<Real,3>& rkEye);

// Create a homogeneous matrix for reflecting points through a plane.  The
// inputs are the plane normal and a point on the plane.
template <typename Real>
RealM<Real,4,4> MakeReflection (const RealV<Real,3>& rkNormal,
    const RealV<Real,3>& rkPoint);

#include "Wm4UtilityRealM4x4.inl"
}

#endif
