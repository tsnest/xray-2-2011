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

#ifndef WM4UTILITYREALM2X2_H
#define WM4UTILITYREALM2X2_H

#include "Wm4GraphicsLIB.h"
#include "Wm4UtilityRealM.h"
#include "Wm4UtilityRealV2.h"

namespace Wm4
{
// Create a diagonal matrix.  The matrix can be used as a scale matrix or as
// a reflection, where the application is in the standard axis directions
// (1,0) and (0,1).
template <typename Real>
void MakeDiagonal (Real fM00, Real fM11, RealM<Real,2,2>& rkM);

// Compute the adjugate matrix of the matrix M, which is the transpose of the
// matrix of cofactors of M.
template <typename Real>
RealM<Real,2,2> Adjugate (const RealM<Real,2,2>& rkM);

// Compute the determinant of the matrix M.
template <typename Real>
Real Determinant (const RealM<Real,2,2>& rkM);

// Compute the inverse of the matrix M.  If the matrix is not invertible, then
// the function returns the zero matrix.
template <typename Real>
RealM<Real,2,2> Inverse (const RealM<Real,2,2>& rkM);

// Create a rotation matrix from an angle (in radians).
template <typename Real>
void FromAngle (Real fAngle, RealM<Real,2,2>& rkRot);

// Extract the angle (in radians) from a rotation matrix.
template <typename Real>
void ToAngle (const RealM<Real,2,2>& rkRot, Real& rfAngle);

// Orthonormalize a matrix using Gram-Schmidt orthonormalization.  This is
// useful for rotation matrices that have some numerical round-off errors.
// The function returns 'true' iff the process was successful (the columns
// of the input matrix are linearly independent).  If the function fails, the
// input matrix is unchanged.
template <typename Real>
bool Orthonormalize (RealM<Real,2,2>& rkM);

// The input matrix M must be symmetric.  Factor M = R * D * R^T, where
// R = [u0|u1] is a rotation matrix with columns u0 and u1 and D = diag(d0,d1)
// is a diagonal matrix whose diagonal entries are d0 and d1.  The eigenvector
// u[i] corresponds to eigenvalue d[i].  The eigenvalues are ordered as
// d0 <= d1.
template <typename Real>
void EigenDecomposition (const RealM<Real,2,2>& rkM, RealM<Real,2,2>& rkRot,
    RealM<Real,2,2>& rkDiag);

// Create a scaling transformation in the coordinate system with origin P and
// axis directions U0 and U1.  The set {U0,U1} is orthonormal.  The point
// X = P + y0*U0 + y1*U1 is scaled to X' = P + s0*y0*U0 + s1*y1*U1.  The
// transformation is X' = M*X + B, where M = s0*U0*U0^T+s1*U1*U1^T and
// B = (I-M)*P are the outputs of this function (I is the identity matrix).
template <typename Real>
void CreateScaling (const RealV<Real,2>& rkP, const RealV<Real,2>& rkU0,
    const RealV<Real,2>& rkU1, Real fS0, Real fS1, RealM<Real,2,2>& rkM,
    RealV<Real,2>& rkB);

// Create a rotation transformation in the coordinate system with origin P
// and axis directions U0 and U1.  The set {U0,U1} is orthonormal.  The
// center of rotation is P and the angle of rotation is A (in radians).  A
// positive angle corresponds to a counterclockwise rotation in the
// (U0,U1)-plane.  The point X = P + y0*U0 + y1*U1 is rotated about P to
// X' = P + (c*y0-s*y1)*U0 + (s*y0+c*y1)*U1, where c = cos(A) and s = sin(A).
// The transformation is X' = M*X + B, where M = c*I + s*J with
// J = {{0,-1},{1,0}} (the rows of the matrix are listed), and B = (I-M)*P.
template <typename Real>
void CreateRotation (const RealV<Real,2>& rkP, Real fAngle,
    RealM<Real,2,2>& rkM, RealV<Real,2>& rkB);

// Create a reflection transformation in the coordinate system with origin P
// and axis directions U0 and U1.  The set {U0,U1} is orthonormal.  The point
// X = P + y0*U0 + y1*U1 is reflected through the line P + t*U0 to the point
// X' = P + y0*U0 - y1*U1.  The transformation is X' = M*X + B, where
// M = 2*U0*U0^T - I and B = (I-M)*P are the outputs of this function (I is
// the identity matrix).
template <typename Real>
void CreateReflection (const RealV<Real,2>& rkP, const RealV<Real,2>& rkU0,
    RealM<Real,2,2>& rkM, RealV<Real,2>& rkB);

// Create a shearing transformation in the coordinate system with origin P
// and axis directions U0 and U1.  The set {U0,U1} is orthonormal.  The point
// X = P + y0*U0 + y1*U1 is sheared to X' = P + (y0+s*y1)*U0 + y1*U1.  The
// transformation is X' = M*X + B, where M = I + s*U0*U1^T and B = (I-M)*P are
// the outputs of this function (I is the identity matrix).
template <typename Real>
void CreateShear (const RealV<Real,2>& rkP, const RealV<Real,2>& rkU0,
    const RealV<Real,2>& rkU1, Real fS, RealM<Real,2,2>& rkM,
    RealV<Real,2>& rkB);

#include "Wm4UtilityRealM2x2.inl"
}

#endif
