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

#ifndef WM4UTILITYREALM3X3_H
#define WM4UTILITYREALM3X3_H

#include "Wm4GraphicsLIB.h"
#include "Wm4UtilityRealM.h"
#include "Wm4UtilityRealV3.h"

namespace Wm4
{
// Create a diagonal matrix.
template <typename Real>
void MakeDiagonal (Real fM00, Real fM11, Real fM22, RealM<Real,3,3>& rkM);

// Compute the adjugate matrix of the matrix M, which is the transpose of the
// matrix of cofactors of M.
template <typename Real>
RealM<Real,3,3> Adjugate (const RealM<Real,3,3>& rkM);

// Compute the determinant of the matrix M.
template <typename Real>
Real Determinant (const RealM<Real,3,3>& rkM);

// Compute the inverse of the matrix M.  If the matrix is not invertible, then
// the function returns the zero matrix.
template <typename Real>
RealM<Real,3,3> Inverse (const RealM<Real,3,3>& rkM);

// Create a rotation matrix from a unit-length axis and an angle (in radians).
template <typename Real>
void FromAxisAngle (const RealV<Real,3>& rkAxis, Real fAngle,
    RealM<Real,3,3>& rkRot);

// Extract a unit-length axis and an angle (in radians) from a rotation
// matrix.
template <typename Real>
void ToAxisAngle (const RealM<Real,3,3>& rkRot, RealV<Real,3>& rkAxis,
    Real& rfAngle);

// Create rotation matrices whose axes are (1,0,0), (0,1,0), or (0,0,1).  The
// angle is in radians.  A positive angle corresponds to a counterclockwise
// rotation as you observe with direction the negative of the axis direction.
template <typename Real>
void MakeXRotation (Real fAngle, RealM<Real,3,3>& rkRot);

template <typename Real>
void MakeYRotation (Real fAngle, RealM<Real,3,3>& rkRot);

template <typename Real>
void MakeZRotation (Real fAngle, RealM<Real,3,3>& rkRot);

// Create rotation matrices from Euler angles.
template <typename Real>
void FromEulerAnglesXYZ (Real fXAngle, Real fYAngle, Real fZAngle,
    RealM<Real,3,3>& rkRot);

template <typename Real>
void FromEulerAnglesXZY (Real fXAngle, Real fZAngle, Real fYAngle,
    RealM<Real,3,3>& rkRot);

template <typename Real>
void FromEulerAnglesYXZ (Real fYAngle, Real fXAngle, Real fZAngle,
    RealM<Real,3,3>& rkRot);

template <typename Real>
void FromEulerAnglesYZX (Real fYAngle, Real fZAngle, Real fXAngle,
    RealM<Real,3,3>& rkRot);

template <typename Real>
void FromEulerAnglesZXY (Real fZAngle, Real fXAngle, Real fYAngle,
    RealM<Real,3,3>& rkRot);

template <typename Real>
void FromEulerAnglesZYX (Real fZAngle, Real fYAngle, Real fXAngle,
    RealM<Real,3,3>& rkRot);

// Extract Euler angles from rotation matrices.  The return value is 'true'
// iff the factorization is unique relative to certain angle ranges.  That is,
// if (U,V,W) is some permutation of (X,Y,Z), the angle ranges for the outputs
// from the function call ToEulerAnglesUVW(uAngle,vAngle,wAngle) are
// uAngle in [-pi,pi], vAngle in [-pi/2,pi/2], and wAngle in [-pi,pi].  If
// the function returns 'false', wAngle is 0 and vAngle is either pi/2 or
// -pi/2.
template <typename Real>
bool ToEulerAnglesXYZ (const RealM<Real,3,3>& rkRot, Real& rfXAngle,
    Real& rfYAngle, Real& rfZAngle);

template <typename Real>
bool ToEulerAnglesXZY (const RealM<Real,3,3>& rkRot, Real& rfXAngle,
    Real& rfZAngle, Real& rfYAngle);

template <typename Real>
bool ToEulerAnglesYXZ (const RealM<Real,3,3>& rkRot, Real& rfYAngle,
    Real& rfXAngle, Real& rfZAngle);

template <typename Real>
bool ToEulerAnglesYZX (const RealM<Real,3,3>& rkRot, Real& rfYAngle,
    Real& rfZAngle, Real& rfXAngle);

template <typename Real>
bool ToEulerAnglesZXY (const RealM<Real,3,3>& rkRot, Real& rfZAngle,
    Real& rfXAngle, Real& rfYAngle);

template <typename Real>
bool ToEulerAnglesZYX (const RealM<Real,3,3>& rkRot, Real& rfZAngle,
    Real& rfYAngle, Real& rfXAngle);

// Orthonormalize a matrix using Gram-Schmidt orthonormalization.  This is
// useful for rotation matrices that have some numerical round-off errors.
// The function returns 'true' iff the process was successful (the columns
// of the input matrix are linearly independent).  If the function fails, the
// input matrix is unchanged.
template <typename Real>
bool Orthonormalize (RealM<Real,3,3>& rkM);

// Support for eigendecomposition.  The Tridiagonalize function applies a
// Householder transformation to the matrix.  If that transformation is the
// identity (the matrix is already tridiagonal), then the return value is
// 'false'.  Otherwise, the transformation is a reflection and the return
// value is 'true'.  The QLAlgorithm returns 'true' iff the QL iteration
// scheme converged.  These functions are intended for private use by
// EigenDecomposition.
template <typename Real>
bool Tridiagonalize (RealM<Real,3,3>& rkM, Real afDiag[3], Real afSubd[2]);

template <typename Real>
bool QLAlgorithm (RealM<Real,3,3>& rkM, Real afDiag[3], Real afSubd[2]);

// The input matrix M must be symmetric.  Factor M = R * D * R^T, where
// R = [u0|u1|u2] is a rotation matrix with columns u0, u1, and u2 and
// D = diag(d0,d1,d2) is a diagonal matrix whose diagonal entries are d0,
// d1, and d2.  The eigenvector u[i] corresponds to eigenvalue d[i].  The
// eigenvalues are ordered as d0 <= d1 <= d2.
template <typename Real>
void EigenDecomposition (const RealM<Real,3,3>& rkM, RealM<Real,3,3>& rkRot,
    RealM<Real,3,3>& rkDiag);

// Factor an invertible matrix as M = Q*D*U, where Q is a rotation matrix, D
// is diagonal, and U is upper triangular with ones on its diagonal.  The
// return value is 'true' iff M is invertible.
template <typename Real>
bool QDUDecomposition (const RealM<Real,3,3>& rkM, RealM<Real,3,3>& rkQ,
    RealM<Real,3,3>& rkD, RealM<Real,3,3>& rkU);

// Support for singular value decomposition.    These functions are intended
// for private use by SingularValueDecomposition.
template <typename Real>
void Bidiagonalize (RealM<Real,3,3>& rkA, RealM<Real,3,3>& rkL,
    RealM<Real,3,3>& rkR);

template <typename Real>
void GolubKahanStep (RealM<Real,3,3>& rkA, RealM<Real,3,3>& rkL,
    RealM<Real,3,3>& rkR);

// Singular value decomposition, M = L*S*R, where L and R are orthogonal
// and S is a diagonal matrix whose diagonal entries are nonnegative.
template <typename Real>
void SingularValueDecomposition (const RealM<Real,3,3>& rkM,
    RealM<Real,3,3>& rkL, RealM<Real,3,3>& rkS, RealM<Real,3,3>& rkR);

// SLERP (spherical linear interpolation) without quaternions.  The function
// computes R(t) = R0*(Transpose(R0)*R1)^t, where M^t denotes raising a
// matrix to the real-valued power t.  If Q is a rotation matrix with
// unit-length axis U and angle A, then Q^t is a rotation matrix with
// unit-length axis U and rotation angle t*A.
template <typename Real>
RealM<Real,3,3> Slerp (Real fT, const RealM<Real,3,3>& rkR0,
    const RealM<Real,3,3>& rkR1);

// Create a scaling transformation in the coordinate system with origin P and
// axis directions U0, U1, U2.  The set {U0,U1,U2} is orthonormal.  The point
// X = P + y0*U0 + y1*U1 + y2*U2 is scaled to X' = P + s0*y0*U0 + s1*y1*U1 +
// s2*U2*Y2.  The transformation is X' = M*X + B, where M = s0*U0*U0^T +
// s1*U1*U1^T + s2*U2*U2^T and B = (I-M)*P are the outputs of this function
// (I is the identity matrix).
template <typename Real>
void CreateScaling (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    const RealV<Real,3>& rkU1, const RealV<Real,3>& rkU2, Real fS0, Real fS1,
    Real fS2, RealM<Real,3,3>& rkM, RealV<Real,3>& rkB);

// Create a rotation transformation in the coordinate system with origin P
// and axis directions U0, U1, and U2.  The set {U0,U1,U2} is orthonormal.
// The center of rotation is P, the axis of rotation is U2, and the angle of
// rotation is A (in radians).  A positive angle corresponds to a
// counterclockwise rotation in the (U0,U1)-plane.  The point X = P + y0*U0 +
// y1*U1 + y2*U2 is rotated to X' = P + (c*y0-s*y1)*U0 + (s*y0+c*y1)*U1 +
// y2*U2, where c = cos(A) and s = sin(A).  The transformation is
// X' = M*X + B, where M = I + s*J + (1-c)*J^2 with J = skew(U2).  If U2 =
// (x,y,z), the J is a skew-symmetric matrix with J00 = J11 = J22 = 0 and
// J01 = -z, J02 = y, J12 = -x, J10 = z, J20 = -y, and J21 = x.  Also,
// B = (I-M)*P.
template <typename Real>
void CreateRotation (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU2,
    Real fAngle, RealM<Real,3,3>& rkM, RealV<Real,2>& rkB);

// Create a reflection transformation in the coordinate system with origin P
// and axis directions U0, U1, and U2.  The set {U0,U1,U2} is orthonormal.
// The point X = P + y0*U0 + y1*U1 + y2*U2 is reflected through the line
// P + t*U0 to the point X' = P + y0*U0 - y1*U1 - y2*U2.  The transformation
// is X' = M*X + B, where M = 2*U0*U0^T - I and B = (I-M)*P are the outputs of
// this function (I is the identity matrix).
template <typename Real>
void CreateReflection (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    RealM<Real,3,3>& rkM, RealV<Real,3>& rkB);

// Create a reflection transformation in the coordinate system with origin P
// and axis directions U0, U1, and U2.  The set {U0,U1,U2} is orthonormal.
// The point X = P + y0*U0 + y1*U1 + y2*U2 is reflected through the plane
// P + t0*U0 + t1*U1 to the point X' = P + y0*U0 + y1*U1 - y2*U2.  The
// transformation is X' = M*X + B, where M = 2*U0*U0^T + 2*U1*U1^T - I and
// B = (I-M)*P are the outputs of this function (I is the identity matrix).
template <typename Real>
void CreateReflection (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    const RealV<Real,3>& rkU1, RealM<Real,3,3>& rkM, RealV<Real,3>& rkB);

// Create a shearing transformation in the coordinate system with origin P
// and axis directions U0, U1, and U2.  The set {U0,U1,U2} is orthonormal.
// The point X = P + y0*U0 + y1*U1 + y2*U2 is sheared to X' = P +
// (y0+s*y1)*U0 + y1*U1 + y2*U2.  The transformation is X' = M*X + B, where
// M = I + s*U0*U1^T and B = (I-M)*P are the outputs of this function (I is
// the identity matrix).
template <typename Real>
void CreateShear (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    const RealV<Real,3>& rkU1, Real fS, RealM<Real,3,3>& rkM,
    RealV<Real,3>& rkB);

#include "Wm4UtilityRealM3x3.inl"
}

#endif
