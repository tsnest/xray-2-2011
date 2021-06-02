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

//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real>::Quadratic3 ()
{
    memset(m_afCoeff,0,10*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real>::Quadratic3 (Real fA0, Real fA1, Real fA2, Real fA3,
    Real fA4, Real fA5, Real fA6, Real fA7, Real fA8, Real fA9)
{
    m_afCoeff[0] = fA0;
    m_afCoeff[1] = fA1;
    m_afCoeff[2] = fA2;
    m_afCoeff[3] = fA3;
    m_afCoeff[4] = fA4;
    m_afCoeff[5] = fA5;
    m_afCoeff[6] = fA6;
    m_afCoeff[7] = fA7;
    m_afCoeff[8] = fA8;
    m_afCoeff[9] = fA9;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real>::Quadratic3 (const Real afA[10])
{
    for (int i = 0; i < 10; i++)
    {
        m_afCoeff[i] = afA[i];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real>::operator const Real* () const
{
    return m_afCoeff;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real>::operator Real* ()
{
    return m_afCoeff;
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::operator[] (int i) const
{
    assert(0 <= i && i < 10);
    return m_afCoeff[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::operator[] (int i)
{
    assert(0 <= i && i < 10);
    return m_afCoeff[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::Constant() const
{
    return m_afCoeff[0];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::Constant()
{
    return m_afCoeff[0];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::X() const
{
    return m_afCoeff[1];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::X()
{
    return m_afCoeff[1];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::Y() const
{
    return m_afCoeff[2];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::Y()
{
    return m_afCoeff[2];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::Z() const
{
    return m_afCoeff[3];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::Z()
{
    return m_afCoeff[3];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::XX() const
{
    return m_afCoeff[4];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::XX()
{
    return m_afCoeff[4];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::XY() const
{
    return m_afCoeff[5];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::XY()
{
    return m_afCoeff[5];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::XZ() const
{
    return m_afCoeff[6];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::XZ()
{
    return m_afCoeff[6];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::YY() const
{
    return m_afCoeff[7];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::YY()
{
    return m_afCoeff[7];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::YZ() const
{
    return m_afCoeff[8];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::YZ()
{
    return m_afCoeff[8];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::ZZ() const
{
    return m_afCoeff[9];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic3<Real>::ZZ()
{
    return m_afCoeff[9];
}
//----------------------------------------------------------------------------
template <class Real>
void Quadratic3<Real>::Set (int iXOrder, int iYOrder, int iZOrder,
    Real fCoeff)
{
    if (iXOrder >= 0 && iYOrder >= 0 && iZOrder >= 0)
    {
        int iSumYZ = iYOrder + iZOrder;
        int iSumXYZ = iXOrder + iSumYZ;
        if (iSumXYZ <= 3)
        {
            int i = iSumYZ*(1+4*iXOrder+3*iSumYZ)/2+iXOrder*iXOrder+iZOrder;
            assert(0 <= i && i < 10);
            m_afCoeff[i] = fCoeff;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::Get (int iXOrder, int iYOrder, int iZOrder) const
{
    if (iXOrder >= 0 && iYOrder >= 0 && iZOrder >= 0)
    {
        int iSumYZ = iYOrder + iZOrder;
        int iSumXYZ = iXOrder + iSumYZ;
        if (iSumXYZ <= 3)
        {
            int i = iSumYZ*(1+4*iXOrder+3*iSumYZ)/2+iXOrder*iXOrder+iZOrder;
            assert(0 <= i && i < 10);
            return m_afCoeff[i];
        }
    }

    return (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::operator() (Real fX, Real fY, Real fZ) const
{
    Real fResult = m_afCoeff[0] + fX*(m_afCoeff[1] + fX*m_afCoeff[4]) +
        fY*(m_afCoeff[2] + fX*m_afCoeff[5] + fY*m_afCoeff[7]) + fZ *
        (m_afCoeff[3] + fX*m_afCoeff[6] + fY*m_afCoeff[8] + fZ*m_afCoeff[9]);

    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic3<Real>::operator() (const Vector3<Real>& rkP) const
{
    return (*this)(rkP.X(),rkP.Y(),rkP.Z());
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real> Quadratic3<Real>::Translate (const Vector3<Real>& rkTrn)
    const
{
    Quadratic3<Real> kResult;

    Real fX = -rkTrn.X(), fY = -rkTrn.Y(), fZ = -rkTrn.Z();
    kResult.m_afCoeff[0] = (*this)(fX,fY,fZ);
    kResult.m_afCoeff[1] = m_afCoeff[1] + ((Real)2.0)*fX*m_afCoeff[4] +
        fY*m_afCoeff[5] + fZ*m_afCoeff[6];
    kResult.m_afCoeff[2] = m_afCoeff[2] + ((Real)2.0)*fY*m_afCoeff[7] +
        fX*m_afCoeff[5] + fZ*m_afCoeff[8];
    kResult.m_afCoeff[3] = m_afCoeff[3] + ((Real)2.0)*fZ*m_afCoeff[9] +
        fX*m_afCoeff[6] + fY*m_afCoeff[8];
    kResult.m_afCoeff[4] = m_afCoeff[4];
    kResult.m_afCoeff[5] = m_afCoeff[5];
    kResult.m_afCoeff[6] = m_afCoeff[6];
    kResult.m_afCoeff[7] = m_afCoeff[7];
    kResult.m_afCoeff[8] = m_afCoeff[8];
    kResult.m_afCoeff[9] = m_afCoeff[9];

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real> Quadratic3<Real>::Rotate (const Matrix3<Real>& rkRot) const
{
    Quadratic3<Real> kResult;

    // The function in matrix-vector form is
    //   Q(X) = Transpose(X)*A*X + Transpose(B)*X + C
    // The transformed function with X = Tranpose(R)*Y is
    //   P(Y) = Y*R*A*Transpose(R)*Y + Transpose(R*B)*Y + C

    Matrix3<Real> kA;
    kA[0][0] = m_afCoeff[4];              // x*x
    kA[0][1] = ((Real)0.5)*m_afCoeff[5];  // x*y/2
    kA[0][2] = ((Real)0.5)*m_afCoeff[6];  // x*z/2
    kA[1][0] = kA[0][1];                  // x*y/2
    kA[1][1] = m_afCoeff[7];              // y*y
    kA[1][2] = ((Real)0.5)*m_afCoeff[8];  // y*z/2
    kA[2][0] = kA[0][2];                  // x*z/2
    kA[2][1] = kA[1][2];                  // y*z/2
    kA[2][2] = m_afCoeff[9];              // z*z

    Vector3<Real> kB;
    kB[0] = m_afCoeff[1];                 // x
    kB[1] = m_afCoeff[2];                 // y
    kB[2] = m_afCoeff[3];                 // z

    Matrix3<Real> kNewA = rkRot*kA.TimesTranspose(rkRot);
    Vector3<Real> kNewB = rkRot*kB;

    kResult.m_afCoeff[0] = m_afCoeff[0];
    kResult.m_afCoeff[1] = kNewB[0];
    kResult.m_afCoeff[2] = kNewB[1];
    kResult.m_afCoeff[3] = kNewB[2];
    kResult.m_afCoeff[4] = kNewA[0][0];
    kResult.m_afCoeff[5] = ((Real)2.0)*kNewA[0][1];
    kResult.m_afCoeff[6] = ((Real)2.0)*kNewA[0][2];
    kResult.m_afCoeff[7] = kNewA[1][1];
    kResult.m_afCoeff[8] = ((Real)2.0)*kNewA[1][2];
    kResult.m_afCoeff[9] = kNewA[2][2];

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic3<Real> Quadratic3<Real>::Scale (const Vector3<Real>& rkScale) const
{
    Quadratic3<Real> kResult;

    Real fInvSX = ((Real)1.0)/rkScale.X();
    Real fInvSY = ((Real)1.0)/rkScale.Y();
    Real fInvSZ = ((Real)1.0)/rkScale.Z();
    kResult.m_afCoeff[0] = m_afCoeff[0];
    kResult.m_afCoeff[1] = m_afCoeff[1]*fInvSX;
    kResult.m_afCoeff[2] = m_afCoeff[2]*fInvSY;
    kResult.m_afCoeff[3] = m_afCoeff[3]*fInvSZ;
    kResult.m_afCoeff[4] = m_afCoeff[4]*fInvSX*fInvSX;
    kResult.m_afCoeff[5] = m_afCoeff[5]*fInvSX*fInvSY;
    kResult.m_afCoeff[6] = m_afCoeff[6]*fInvSX*fInvSZ;
    kResult.m_afCoeff[7] = m_afCoeff[7]*fInvSY*fInvSY;
    kResult.m_afCoeff[8] = m_afCoeff[8]*fInvSY*fInvSZ;
    kResult.m_afCoeff[9] = m_afCoeff[9]*fInvSZ*fInvSZ;

    return kResult;
}
//----------------------------------------------------------------------------
