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
void MakeDiagonal (Real fM00, Real fM11, RealM<Real,2,2>& rkM)
{
    rkM[0][0] = fM00;
    rkM[0][1] = (Real)0;
    rkM[1][0] = (Real)0;
    rkM[1][1] = fM11;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,2,2> Adjugate (const RealM<Real,2,2>& rkM)
{
    RealM<Real,2,2> kAdjugate;
    kAdjugate[0][0] = rkM[1][1];
    kAdjugate[0][1] = -rkM[0][1];
    kAdjugate[1][0] = -rkM[1][0];
    kAdjugate[1][1] = rkM[0][0];
    return kAdjugate;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Determinant (const RealM<Real,2,2>& rkM)
{
    return rkM[0][0]*rkM[1][1] - rkM[0][1]*rkM[1][0];
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,2,2> Inverse (const RealM<Real,2,2>& rkM)
{
    RealM<Real,2,2> kInverse;

    Real fDet = rkM[0][0]*rkM[1][1] - rkM[0][1]*rkM[1][0];
    if (Math<Real>::FAbs(fDet) > Math<Real>::ZERO_TOLERANCE)
    {
        Real fInvDet = ((Real)1)/fDet;
        kInverse[0][0] =  rkM[1][1]*fInvDet;
        kInverse[0][1] = -rkM[0][1]*fInvDet;
        kInverse[1][0] = -rkM[1][0]*fInvDet;
        kInverse[1][1] =  rkM[0][0]*fInvDet;
    }
    else
    {
        MakeZero(kInverse);
    }

    return kInverse;
}
//----------------------------------------------------------------------------
template <typename Real>
void FromAngle (Real fAngle, RealM<Real,2,2>& rkRot)
{
    rkRot[0][0] = Math<Real>::Cos(fAngle);
    rkRot[1][0] = Math<Real>::Sin(fAngle);
    rkRot[0][1] = -rkRot[1][0];
    rkRot[1][1] = rkRot[0][0];
}
//----------------------------------------------------------------------------
template <typename Real>
void ToAngle (const RealM<Real,2,2>& rkRot, Real& rfAngle)
{
    rfAngle = Math<Real>::ATan2(rkRot[1][0],rkRot[0][0]);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Orthonormalize (RealM<Real,2,2>& rkM)
{
    // The algorithm uses Gram-Schmidt orthogonalization applied to the
    // columns of M.
    RealV<Real,2> kQ0(rkM[0][0],rkM[1][0]);
    RealV<Real,2> kQ1(rkM[0][1],rkM[1][1]);
    if (Orthonormalize(kQ0,kQ1))
    {
        rkM[0][0] = kQ0[0];
        rkM[1][0] = kQ0[1];
        rkM[0][1] = kQ1[0];
        rkM[1][1] = kQ1[1];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition (const RealM<Real,2,2>& rkM, RealM<Real,2,2>& rkRot,
    RealM<Real,2,2>& rkDiag)
{
    Real fTrace = rkM[0][0] + rkM[1][1];
    Real fDiff = rkM[0][0] - rkM[1][1];
    Real fDiscr = Math<Real>::Sqrt(fDiff*fDiff +
        ((Real)4)*rkM[0][1]*rkM[0][1]);
    Real fEVal0 = ((Real)1/(Real)2)*(fTrace-fDiscr);
    Real fEVal1 = ((Real)1/(Real)2)*(fTrace+fDiscr);
    MakeDiagonal(fEVal0,fEVal1,rkDiag);

    Real fCos, fSin;
    if (fDiff >= (Real)0)
    {
        fCos = rkM[0][1];
        fSin = fEVal0 - rkM[0][0];
    }
    else
    {
        fCos = fEVal0 - rkM[1][1];
        fSin = rkM[0][1];
    }
    Real fTmp = Math<Real>::InvSqrt(fCos*fCos + fSin*fSin);
    fCos *= fTmp;
    fSin *= fTmp;

    rkRot[0][0] = fCos;
    rkRot[0][1] = -fSin;
    rkRot[1][0] = fSin;
    rkRot[1][1] = fCos;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateScaling (const RealV<Real,2>& rkP, const RealV<Real,2>& rkU0,
    const RealV<Real,2>& rkU1, Real fS0, Real fS1, RealM<Real,2,2>& rkM,
    RealV<Real,2>& rkB)
{
    RealM<Real,2,2> kU0U0Trn, kU1U1Trn;
    MakeTensorProduct(rkU0,rkU0,kU0U0Trn);
    MakeTensorProduct(rkU1,rkU1,kU1U1Trn);
    rkM = fS0*kU0U0Trn + fS1*kU1U1Trn;
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateRotation (const RealV<Real,2>& rkP, Real fAngle,
    RealM<Real,2,2>& rkM, RealV<Real,2>& rkB)
{
    FromAngle(fAngle,rkM);
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateReflection (const RealV<Real,2>& rkP, const RealV<Real,2>& rkU0,
    RealM<Real,2,2>& rkM, RealV<Real,2>& rkB)
{
    RealM<Real,2,2> kU0U0Trn, kIdentity;
    MakeTensorProduct(rkU0,rkU0,kU0U0Trn);
    MakeIdentity(kIdentity);
    rkM = ((Real)2)*kU0U0Trn - kIdentity;
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateShear (const RealV<Real,2>& rkP, const RealV<Real,2>& rkU0,
    const RealV<Real,2>& rkU1, Real fS, RealM<Real,2,2>& rkM,
    RealV<Real,2>& rkB)
{
    RealM<Real,2,2> kU0U1Trn, kIdentity;
    MakeTensorProduct(rkU0,rkU1,kU0U1Trn);
    MakeIdentity(kIdentity);
    rkM = kIdentity + fS*kU0U1Trn;
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
