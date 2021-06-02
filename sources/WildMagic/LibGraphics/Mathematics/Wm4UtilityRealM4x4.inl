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

//----------------------------------------------------------------------------
template <typename Real>
void MakeDiagonal (Real fM00, Real fM11, Real fM22, Real fM33,
    RealM<Real,4,4>& rkM)
{
    rkM[0][0] = fM00;
    rkM[0][1] = (Real)0;
    rkM[0][2] = (Real)0;
    rkM[0][3] = (Real)0;
    rkM[1][0] = (Real)0;
    rkM[1][1] = fM11;
    rkM[1][2] = (Real)0;
    rkM[1][3] = (Real)0;
    rkM[2][0] = (Real)0;
    rkM[2][1] = (Real)0;
    rkM[2][2] = fM22;
    rkM[2][3] = (Real)0;
    rkM[3][0] = (Real)0;
    rkM[3][1] = (Real)0;
    rkM[3][2] = (Real)0;
    rkM[3][3] = fM33;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,4,4> Adjugate (const RealM<Real,4,4>& rkM)
{
    Real fA0 = rkM[0][0]*rkM[1][1] - rkM[0][1]*rkM[1][0];
    Real fA1 = rkM[0][0]*rkM[1][2] - rkM[0][2]*rkM[1][0];
    Real fA2 = rkM[0][0]*rkM[1][3] - rkM[0][3]*rkM[1][0];
    Real fA3 = rkM[0][1]*rkM[1][2] - rkM[0][2]*rkM[1][1];
    Real fA4 = rkM[0][1]*rkM[1][3] - rkM[0][3]*rkM[1][1];
    Real fA5 = rkM[0][2]*rkM[1][3] - rkM[0][3]*rkM[1][2];
    Real fB0 = rkM[2][0]*rkM[3][1] - rkM[2][1]*rkM[3][0];
    Real fB1 = rkM[2][0]*rkM[3][2] - rkM[2][2]*rkM[3][0];
    Real fB2 = rkM[2][0]*rkM[3][3] - rkM[2][3]*rkM[3][0];
    Real fB3 = rkM[2][1]*rkM[3][2] - rkM[2][2]*rkM[3][1];
    Real fB4 = rkM[2][1]*rkM[3][3] - rkM[2][3]*rkM[3][1];
    Real fB5 = rkM[2][2]*rkM[3][3] - rkM[2][3]*rkM[3][2];

    RealM<Real,4,4> kAdjugate;
    kAdjugate[0][0] = + rkM[1][1]*fB5 - rkM[1][2]*fB4 + rkM[1][3]*fB3;
    kAdjugate[0][1] = - rkM[0][1]*fB5 + rkM[0][2]*fB4 - rkM[0][3]*fB3;
    kAdjugate[0][2] = + rkM[3][1]*fA5 - rkM[3][2]*fA4 + rkM[3][3]*fA3;
    kAdjugate[0][3] = - rkM[2][1]*fA5 + rkM[2][2]*fA4 - rkM[2][3]*fA3;
    kAdjugate[1][0] = - rkM[1][0]*fB5 + rkM[1][2]*fB2 - rkM[1][3]*fB1;
    kAdjugate[1][1] = + rkM[0][0]*fB5 - rkM[0][2]*fB2 + rkM[0][3]*fB1;
    kAdjugate[1][2] = - rkM[3][0]*fA5 + rkM[3][2]*fA2 - rkM[3][3]*fA1;
    kAdjugate[1][3] = + rkM[2][0]*fA5 - rkM[2][2]*fA2 + rkM[2][3]*fA1;
    kAdjugate[2][0] = + rkM[1][0]*fB4 - rkM[1][1]*fB2 + rkM[1][3]*fB0;
    kAdjugate[2][1] = - rkM[0][0]*fB4 + rkM[0][1]*fB2 - rkM[0][3]*fB0;
    kAdjugate[2][2] = + rkM[3][0]*fA4 - rkM[3][1]*fA2 + rkM[3][3]*fA0;
    kAdjugate[2][3] = - rkM[2][0]*fA4 + rkM[2][1]*fA2 - rkM[2][3]*fA0;
    kAdjugate[3][0] = - rkM[1][0]*fB3 + rkM[1][1]*fB1 - rkM[1][2]*fB0;
    kAdjugate[3][1] = + rkM[0][0]*fB3 - rkM[0][1]*fB1 + rkM[0][2]*fB0;
    kAdjugate[3][2] = - rkM[3][0]*fA3 + rkM[3][1]*fA1 - rkM[3][2]*fA0;
    kAdjugate[3][3] = + rkM[2][0]*fA3 - rkM[2][1]*fA1 + rkM[2][2]*fA0;
    return kAdjugate;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Determinant (const RealM<Real,4,4>& rkM)
{
    Real fA0 = rkM[0][0]*rkM[1][1] - rkM[0][1]*rkM[1][0];
    Real fA1 = rkM[0][0]*rkM[1][2] - rkM[0][2]*rkM[1][0];
    Real fA2 = rkM[0][0]*rkM[1][3] - rkM[0][3]*rkM[1][0];
    Real fA3 = rkM[0][1]*rkM[1][2] - rkM[0][2]*rkM[1][1];
    Real fA4 = rkM[0][1]*rkM[1][3] - rkM[0][3]*rkM[1][1];
    Real fA5 = rkM[0][2]*rkM[1][3] - rkM[0][3]*rkM[1][2];
    Real fB0 = rkM[2][0]*rkM[3][1] - rkM[2][1]*rkM[3][0];
    Real fB1 = rkM[2][0]*rkM[3][2] - rkM[2][2]*rkM[3][0];
    Real fB2 = rkM[2][0]*rkM[3][3] - rkM[2][3]*rkM[3][0];
    Real fB3 = rkM[2][1]*rkM[3][2] - rkM[2][2]*rkM[3][1];
    Real fB4 = rkM[2][1]*rkM[3][3] - rkM[2][3]*rkM[3][1];
    Real fB5 = rkM[2][2]*rkM[3][3] - rkM[2][3]*rkM[3][2];
    Real fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
    return fDet;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,4,4> Inverse (const RealM<Real,4,4>& rkM)
{
    RealM<Real,4,4> kInverse;

    Real fA0 = rkM[0][0]*rkM[1][1] - rkM[0][1]*rkM[1][0];
    Real fA1 = rkM[0][0]*rkM[1][2] - rkM[0][2]*rkM[1][0];
    Real fA2 = rkM[0][0]*rkM[1][3] - rkM[0][3]*rkM[1][0];
    Real fA3 = rkM[0][1]*rkM[1][2] - rkM[0][2]*rkM[1][1];
    Real fA4 = rkM[0][1]*rkM[1][3] - rkM[0][3]*rkM[1][1];
    Real fA5 = rkM[0][2]*rkM[1][3] - rkM[0][3]*rkM[1][2];
    Real fB0 = rkM[2][0]*rkM[3][1] - rkM[2][1]*rkM[3][0];
    Real fB1 = rkM[2][0]*rkM[3][2] - rkM[2][2]*rkM[3][0];
    Real fB2 = rkM[2][0]*rkM[3][3] - rkM[2][3]*rkM[3][0];
    Real fB3 = rkM[2][1]*rkM[3][2] - rkM[2][2]*rkM[3][1];
    Real fB4 = rkM[2][1]*rkM[3][3] - rkM[2][3]*rkM[3][1];
    Real fB5 = rkM[2][2]*rkM[3][3] - rkM[2][3]*rkM[3][2];
    Real fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
    if (Math<Real>::FAbs(fDet) > Math<Real>::ZERO_TOLERANCE)
    {
        kInverse[0][0] = + rkM[1][1]*fB5 - rkM[1][2]*fB4 + rkM[1][3]*fB3;
        kInverse[1][0] = - rkM[1][0]*fB5 + rkM[1][2]*fB2 - rkM[1][3]*fB1;
        kInverse[2][0] = + rkM[1][0]*fB4 - rkM[1][1]*fB2 + rkM[1][3]*fB0;
        kInverse[3][0] = - rkM[1][0]*fB3 + rkM[1][1]*fB1 - rkM[1][2]*fB0;
        kInverse[0][1] = - rkM[0][1]*fB5 + rkM[0][2]*fB4 - rkM[0][3]*fB3;
        kInverse[1][1] = + rkM[0][0]*fB5 - rkM[0][2]*fB2 + rkM[0][3]*fB1;
        kInverse[2][1] = - rkM[0][0]*fB4 + rkM[0][1]*fB2 - rkM[0][3]*fB0;
        kInverse[3][1] = + rkM[0][0]*fB3 - rkM[0][1]*fB1 + rkM[0][2]*fB0;
        kInverse[0][2] = + rkM[3][1]*fA5 - rkM[3][2]*fA4 + rkM[3][3]*fA3;
        kInverse[1][2] = - rkM[3][0]*fA5 + rkM[3][2]*fA2 - rkM[3][3]*fA1;
        kInverse[2][2] = + rkM[3][0]*fA4 - rkM[3][1]*fA2 + rkM[3][3]*fA0;
        kInverse[3][2] = - rkM[3][0]*fA3 + rkM[3][1]*fA1 - rkM[3][2]*fA0;
        kInverse[0][3] = - rkM[2][1]*fA5 + rkM[2][2]*fA4 - rkM[2][3]*fA3;
        kInverse[1][3] = + rkM[2][0]*fA5 - rkM[2][2]*fA2 + rkM[2][3]*fA1;
        kInverse[2][3] = - rkM[2][0]*fA4 + rkM[2][1]*fA2 - rkM[2][3]*fA0;
        kInverse[3][3] = + rkM[2][0]*fA3 - rkM[2][1]*fA1 + rkM[2][2]*fA0;

        Real fInvDet = ((Real)1)/fDet;
        kInverse[0][0] *= fInvDet;
        kInverse[0][1] *= fInvDet;
        kInverse[0][2] *= fInvDet;
        kInverse[0][3] *= fInvDet;
        kInverse[1][0] *= fInvDet;
        kInverse[1][1] *= fInvDet;
        kInverse[1][2] *= fInvDet;
        kInverse[1][3] *= fInvDet;
        kInverse[2][0] *= fInvDet;
        kInverse[2][1] *= fInvDet;
        kInverse[2][2] *= fInvDet;
        kInverse[2][3] *= fInvDet;
        kInverse[3][0] *= fInvDet;
        kInverse[3][1] *= fInvDet;
        kInverse[3][2] *= fInvDet;
        kInverse[3][3] *= fInvDet;
    }
    else
    {
        MakeZero(kInverse);
    }

    return kInverse;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,4,4> MakeObliqueProjection (const RealV<Real,3>& rkNormal,
    const RealV<Real,3>& rkPoint, const RealV<Real,3>& rkDirection)
{
    // The projection plane is Dot(N,X-P) = 0 where N is a 3-by-1 unit-length
    // normal vector and P is a 3-by-1 point on the plane.  The projection
    // is oblique to the plane, in the direction of the 3-by-1 vector D.
    // Necessarily Dot(N,D) is not zero for this projection to make sense.
    // Given a 3-by-1 point U, compute the intersection of the line U+t*D
    // with the plane to obtain t = -Dot(N,U-P)/Dot(N,D).  Then
    //
    //   projection(U) = P + [I - D*N^T/Dot(N,D)]*(U-P)
    //
    // A 4-by-4 homogeneous transformation representing the projection is
    //
    //       +-                               -+
    //   M = | D*N^T - Dot(N,D)*I   -Dot(N,P)D |
    //       |          0^T          -Dot(N,D) |
    //       +-                               -+
    //
    // where M applies to [U^T 1]^T by M*[U^T 1]^T.  The matrix is chosen so
    // that M[3][3] > 0 whenever Dot(N,D) < 0 (projection is onto the
    // "positive side" of the plane).

    Real fNdD = Dot(rkNormal,rkDirection);
    Real fNdP = Dot(rkNormal,rkPoint);

    RealM<Real,4,4> kProjection;
    kProjection[0][0] = rkDirection[0]*rkNormal[0] - fNdD;
    kProjection[0][1] = rkDirection[0]*rkNormal[1];
    kProjection[0][2] = rkDirection[0]*rkNormal[2];
    kProjection[0][3] = -fNdP*rkDirection[0];
    kProjection[1][0] = rkDirection[1]*rkNormal[0];
    kProjection[1][1] = rkDirection[1]*rkNormal[1] - fNdD;
    kProjection[1][2] = rkDirection[1]*rkNormal[2];
    kProjection[1][3] = -fNdP*rkDirection[1];
    kProjection[2][0] = rkDirection[2]*rkNormal[0];
    kProjection[2][1] = rkDirection[2]*rkNormal[1];
    kProjection[2][2] = rkDirection[2]*rkNormal[2] - fNdD;
    kProjection[2][3] = -fNdP*rkDirection[2];
    kProjection[3][0] = (Real)0;
    kProjection[3][1] = (Real)0;
    kProjection[3][2] = (Real)0;
    kProjection[3][3] = -fNdD;
    return kProjection;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,4,4> MakePerspectiveProjection (const RealV<Real,3>& rkNormal,
    const RealV<Real,3>& rkPoint, const RealV<Real,3>& rkEye)
{
    //     +-                                                 -+
    // M = | Dot(N,E-P)*I - E*N^T    -(Dot(N,E-P)*I - E*N^T)*E |
    //     |        -N^t                      Dot(N,E)         |
    //     +-                                                 -+
    //
    // where E is the eye point, P is a point on the plane, and N is a
    // unit-length plane normal.

    Real fNdEmP = Dot(rkNormal,rkEye-rkPoint);

    RealM<Real,4,4> kProjection;
    kProjection[0][0] = fNdEmP - rkEye[0]*rkNormal[0];
    kProjection[0][1] = -rkEye[0]*rkNormal[1];
    kProjection[0][2] = -rkEye[0]*rkNormal[2];
    kProjection[0][3] = -(
        kProjection[0][0]*rkEye[0] +
        kProjection[0][1]*rkEye[1] +
        kProjection[0][2]*rkEye[2]);
    kProjection[1][0] = -rkEye[1]*rkNormal[0];
    kProjection[1][1] = fNdEmP - rkEye[1]*rkNormal[1];
    kProjection[1][2] = -rkEye[1]*rkNormal[2];
    kProjection[1][3] = -(
        kProjection[1][0]*rkEye[0] +
        kProjection[1][1]*rkEye[1] +
        kProjection[1][2]*rkEye[2]);
    kProjection[2][0] = -rkEye[2]*rkNormal[0];
    kProjection[2][1] = -rkEye[2]*rkNormal[1];
    kProjection[2][2] = fNdEmP- rkEye[2]*rkNormal[2];
    kProjection[2][3] = -(
        kProjection[2][0]*rkEye[0] +
        kProjection[2][1]*rkEye[1] +
        kProjection[2][2]*rkEye[2]);
    kProjection[3][0] = -rkNormal[0];
    kProjection[3][1] = -rkNormal[1];
    kProjection[3][2] = -rkNormal[2];
    kProjection[3][3] = Dot(rkNormal,rkEye);
    return kProjection;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,4,4> MakeReflection (const RealV<Real,3>& rkNormal,
    const RealV<Real,3>& rkPoint)
{
    //     +-                         -+
    // M = | I-2*N*N^T    2*Dot(N,P)*N |
    //     |     0^T            1      |
    //     +-                         -+
    //
    // where P is a point on the plane and N is a unit-length plane normal.

    Real fTwoNdP = ((Real)2)*Dot(rkNormal,rkPoint);

    RealM<Real,4,4> kReflection;
    kReflection[0][0] = (Real)1 - ((Real)2)*rkNormal[0]*rkNormal[0];
    kReflection[0][1] = -((Real)2)*rkNormal[0]*rkNormal[1];
    kReflection[0][2] = -((Real)2)*rkNormal[0]*rkNormal[2];
    kReflection[0][3] = fTwoNdP*rkNormal[0];
    kReflection[1][0] = -((Real)2)*rkNormal[1]*rkNormal[0];
    kReflection[1][1] = (Real)1 - ((Real)2)*rkNormal[1]*rkNormal[1];
    kReflection[1][2] = -((Real)2)*rkNormal[1]*rkNormal[2];
    kReflection[1][3] = fTwoNdP*rkNormal[1];
    kReflection[2][0] = -((Real)2)*rkNormal[2]*rkNormal[0];
    kReflection[2][1] = -((Real)2)*rkNormal[2]*rkNormal[1];
    kReflection[2][2] = (Real)1 - ((Real)2)*rkNormal[2]*rkNormal[2];
    kReflection[2][3] = fTwoNdP*rkNormal[2];
    kReflection[3][0] = (Real)0;
    kReflection[3][1] = (Real)0;
    kReflection[3][2] = (Real)0;
    kReflection[3][3] = (Real)1;
    return kReflection;
}
//----------------------------------------------------------------------------
