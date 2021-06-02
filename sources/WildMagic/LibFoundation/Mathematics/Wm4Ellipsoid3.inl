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
// Version: 4.0.1 (2007/04/01)

//----------------------------------------------------------------------------
template <class Real>
Ellipsoid3<Real>::Ellipsoid3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Ellipsoid3<Real>::Ellipsoid3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>* akAxis, const Real* afExtent)
    :
    Center(rkCenter)
{
    for (int i = 0; i < 3; i++)
    {
        Axis[i] = akAxis[i];
        Extent[i] = afExtent[i];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Ellipsoid3<Real>::Ellipsoid3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>& rkAxis0, const Vector3<Real>& rkAxis1,
    const Vector3<Real>& rkAxis2, Real fExtent0, Real fExtent1, Real fExtent2)
    :
    Center(rkCenter)
{
    Axis[0] = rkAxis0;
    Axis[1] = rkAxis1;
    Axis[2] = rkAxis2;
    Extent[0] = fExtent0;
    Extent[1] = fExtent1;
    Extent[2] = fExtent2;
}
//----------------------------------------------------------------------------
template <class Real>
Ellipsoid3<Real>::~Ellipsoid3 ()
{
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipsoid3<Real>::GetM (Matrix3<Real>& rkM) const
{
    Vector3<Real> kRatio0 = Axis[0]/Extent[0];
    Vector3<Real> kRatio1 = Axis[1]/Extent[1];
    Vector3<Real> kRatio2 = Axis[2]/Extent[2];
    rkM = Matrix3<Real>(kRatio0,kRatio0) + Matrix3<Real>(kRatio1,kRatio1) +
        Matrix3<Real>(kRatio2,kRatio2);
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipsoid3<Real>::GetMInverse (Matrix3<Real>& rkMInverse) const
{
    Vector3<Real> kRatio0 = Axis[0]*Extent[0];
    Vector3<Real> kRatio1 = Axis[1]*Extent[1];
    Vector3<Real> kRatio2 = Axis[2]*Extent[2];
    rkMInverse = Matrix3<Real>(kRatio0,kRatio0) +
        Matrix3<Real>(kRatio1,kRatio1) + Matrix3<Real>(kRatio2,kRatio2);
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipsoid3<Real>::ToCoefficients (Real afCoeff[10]) const
{
    Matrix3<Real> kA;
    Vector3<Real> kB;
    Real fC;
    ToCoefficients(kA,kB,fC);
    Convert(kA,kB,fC,afCoeff);

    // Arrange for one of the x0^2, x1^2, or x2^2 coefficients to be 1.
    Real fMax = Math<Real>::FAbs(afCoeff[4]);
    int iMax = 4;
    Real fAbs = Math<Real>::FAbs(afCoeff[7]);
    if (fAbs > fMax)
    {
        fMax = fAbs;
        iMax = 7;
    }
    fAbs = Math<Real>::FAbs(afCoeff[9]);
    if (fAbs > fMax)
    {
        fMax = fAbs;
        iMax = 9;
    }

    Real fInvMax = ((Real)1.0)/fMax;
    for (int i = 0; i < 10; i++)
    {
        if (i != iMax)
        {
            afCoeff[i] *= fInvMax;
        }
        else
        {
            afCoeff[i] = (Real)1.0;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipsoid3<Real>::ToCoefficients (Matrix3<Real>& rkA, Vector3<Real>& rkB,
    Real& rfC) const
{
    Vector3<Real> kRatio0 = Axis[0]/Extent[0];
    Vector3<Real> kRatio1 = Axis[1]/Extent[1];
    Vector3<Real> kRatio2 = Axis[2]/Extent[2];
    rkA = Matrix3<Real>(kRatio0,kRatio0) + Matrix3<Real>(kRatio1,kRatio1) +
        Matrix3<Real>(kRatio2,kRatio2);

    rkB = ((Real)-2.0)*(rkA*Center);
    rfC = rkA.QForm(Center,Center) - (Real)1.0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Ellipsoid3<Real>::FromCoefficients (const Real afCoeff[10])
{
    Matrix3<Real> kA;
    Vector3<Real> kB;
    Real fC;
    Convert(afCoeff,kA,kB,fC);
    return FromCoefficients(kA,kB,fC);
}
//----------------------------------------------------------------------------
template <class Real>
bool Ellipsoid3<Real>::FromCoefficients (const Matrix3<Real>& rkA,
    const Vector3<Real>& rkB, Real fC)
{
    // Compute the center K = -A^{-1}*B/2.
    Matrix3<Real> kInvA = rkA.Inverse();
    if (kInvA == Matrix3<Real>::ZERO)
    {
        return false;
    }

    Center = ((Real)-0.5)*(kInvA*rkB);

    // Compute B^T*A^{-1}*B/4 - C = K^T*A*K - C = -K^T*B/2 - C.
    Real fRHS = -((Real)0.5)*(Center.Dot(rkB)) - fC;
    if (Math<Real>::FAbs(fRHS) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    // Compute M = A/(K^T*A*K - C).
    Real fInvRHS = ((Real)1.0)/fRHS;
    Matrix3<Real> kM = fInvRHS*rkA;

    // Factor into M = R*D*R^T.
    Eigen<Real> kES(kM);
    kES.IncrSortEigenStuff3();
    for (int i = 0; i < 3; i++)
    {
        if (kES.GetEigenvalue(i) <= (Real)0.0)
        {
            return false;
        }

        Extent[i] = Math<Real>::InvSqrt(kES.GetEigenvalue(i));
        kES.GetEigenvector(i,Axis[i]);
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
Real Ellipsoid3<Real>::Evaluate (const Vector3<Real>& rkPoint) const
{
    Vector3<Real> kDiff = rkPoint - Center;
    Real fRatio0 = Axis[0].Dot(kDiff)/Extent[0];
    Real fRatio1 = Axis[1].Dot(kDiff)/Extent[1];
    Real fRatio2 = Axis[2].Dot(kDiff)/Extent[2];
    Real fValue = fRatio0*fRatio0+fRatio1*fRatio1+fRatio2*fRatio2-(Real)1.0;
    return fValue;
}
//----------------------------------------------------------------------------
template <class Real>
bool Ellipsoid3<Real>::Contains (const Vector3<Real>& rkPoint) const
{
    return Evaluate(rkPoint) <= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipsoid3<Real>::Convert (const Real afCoeff[10], Matrix3<Real>& rkA,
    Vector3<Real>& rkB, Real& rfC)
{
    rfC = afCoeff[0];
    rkB[0] = afCoeff[1];
    rkB[1] = afCoeff[2];
    rkB[2] = afCoeff[3];
    rkA[0][0] = afCoeff[4];
    rkA[0][1] = ((Real)0.5)*afCoeff[5];
    rkA[0][2] = ((Real)0.5)*afCoeff[6];
    rkA[1][0] = rkA[0][1];
    rkA[1][1] = afCoeff[7];
    rkA[1][2] = ((Real)0.5)*afCoeff[8];
    rkA[2][0] = rkA[0][2];
    rkA[2][1] = rkA[1][2];
    rkA[2][2] = afCoeff[9];
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipsoid3<Real>::Convert (const Matrix3<Real>& rkA,
    const Vector3<Real>& rkB, Real fC, Real afCoeff[10])
{
    afCoeff[0] = fC;
    afCoeff[1] = rkB[0];
    afCoeff[2] = rkB[1];
    afCoeff[3] = rkB[2];
    afCoeff[4] = rkA[0][0];
    afCoeff[5] = ((Real)2.0)*rkA[0][1];
    afCoeff[6] = ((Real)2.0)*rkA[0][2];
    afCoeff[7] = rkA[1][1];
    afCoeff[8] = ((Real)2.0)*rkA[1][2];
    afCoeff[9] = rkA[2][2];
}
//----------------------------------------------------------------------------
