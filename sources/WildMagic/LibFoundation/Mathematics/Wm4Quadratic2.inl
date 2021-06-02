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
Quadratic2<Real>::Quadratic2 ()
{
    memset(m_afCoeff,0,6*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic2<Real>::Quadratic2 (Real fA0, Real fA1, Real fA2, Real fA3,
    Real fA4, Real fA5)
{
    m_afCoeff[0] = fA0;
    m_afCoeff[1] = fA1;
    m_afCoeff[2] = fA2;
    m_afCoeff[3] = fA3;
    m_afCoeff[4] = fA4;
    m_afCoeff[5] = fA5;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic2<Real>::Quadratic2 (const Real afA[6])
{
    for (int i = 0; i < 6; i++)
    {
        m_afCoeff[i] = afA[i];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic2<Real>::operator const Real* () const
{
    return m_afCoeff;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic2<Real>::operator Real* ()
{
    return m_afCoeff;
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::operator[] (int i) const
{
    assert(0 <= i && i < 6);
    return m_afCoeff[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic2<Real>::operator[] (int i)
{
    assert(0 <= i && i < 6);
    return m_afCoeff[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::Constant() const
{
    return m_afCoeff[0];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic2<Real>::Constant()
{
    return m_afCoeff[0];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::X() const
{
    return m_afCoeff[1];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic2<Real>::X()
{
    return m_afCoeff[1];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::Y() const
{
    return m_afCoeff[2];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic2<Real>::Y()
{
    return m_afCoeff[2];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::XX() const
{
    return m_afCoeff[3];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic2<Real>::XX()
{
    return m_afCoeff[3];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::XY() const
{
    return m_afCoeff[4];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic2<Real>::XY()
{
    return m_afCoeff[4];
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::YY() const
{
    return m_afCoeff[5];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Quadratic2<Real>::YY()
{
    return m_afCoeff[5];
}
//----------------------------------------------------------------------------
template <class Real>
void Quadratic2<Real>::Set (int iXOrder, int iYOrder, Real fCoeff)
{
    if (iXOrder >= 0 && iYOrder >= 0)
    {
        int iSum = iXOrder + iYOrder;
        if (iSum <= 2)
        {
            int i = iSum*(1+iSum)/2 + iYOrder;
            assert(0 <= i && i < 6);
            m_afCoeff[i] = fCoeff;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::Get (int iXOrder, int iYOrder) const
{
    if (iXOrder >= 0 && iYOrder >= 0)
    {
        int iSum = iXOrder + iYOrder;
        if (iSum <= 2)
        {
            int i = iSum*(1+iSum)/2 + iYOrder;
            assert(0 <= i && i < 6);
            return m_afCoeff[i];
        }
    }

    return (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::operator() (Real fX, Real fY) const
{
    Real fResult = m_afCoeff[0] + fX*(m_afCoeff[1] + fX*m_afCoeff[3]) +
        fY*(m_afCoeff[2] + fX*m_afCoeff[4] + fY*m_afCoeff[5]);

    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Quadratic2<Real>::operator() (const Vector2<Real>& rkP) const
{
    return (*this)(rkP.X(),rkP.Y());
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic2<Real> Quadratic2<Real>::Translate (const Vector2<Real>& rkTrn)
    const
{
    Quadratic2<Real> kResult;

    Real fX = -rkTrn.X(), fY = -rkTrn.Y();
    kResult.m_afCoeff[0] = (*this)(fX,fY);
    kResult.m_afCoeff[1] = m_afCoeff[1] + ((Real)2.0)*fX*m_afCoeff[3] +
        fY*m_afCoeff[4];
    kResult.m_afCoeff[2] = m_afCoeff[2] + ((Real)2.0)*fY*m_afCoeff[5] +
        fX*m_afCoeff[4];
    kResult.m_afCoeff[3] = m_afCoeff[3];
    kResult.m_afCoeff[4] = m_afCoeff[4];
    kResult.m_afCoeff[5] = m_afCoeff[5];

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic2<Real> Quadratic2<Real>::Rotate (const Matrix2<Real>& rkRot) const
{
    Quadratic2<Real> kResult;

    Real fCos = rkRot[0][0], fSin = rkRot[0][1];
    Real fCos2 = fCos*fCos, fSin2 = fSin*fSin, fSinCos = fSin*fCos;
    kResult.m_afCoeff[0] = m_afCoeff[0];
    kResult.m_afCoeff[1] = m_afCoeff[1]*fCos + m_afCoeff[2]*fSin;
    kResult.m_afCoeff[2] = m_afCoeff[2]*fCos - m_afCoeff[1]*fSin;
    kResult.m_afCoeff[3] = m_afCoeff[3]*fCos2 + m_afCoeff[4]*fSinCos +
        m_afCoeff[5]*fSin2;
    kResult.m_afCoeff[4] = m_afCoeff[4]*(fCos2 - fSin2) +
        ((Real)2.0)*fSinCos*(m_afCoeff[5] - m_afCoeff[3]);
    kResult.m_afCoeff[5] = m_afCoeff[3]*fSin2 - m_afCoeff[4]*fSinCos +
        m_afCoeff[5]*fCos2;

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Quadratic2<Real> Quadratic2<Real>::Scale (const Vector2<Real>& rkScale) const
{
    Quadratic2<Real> kResult;

    Real fInvSX = ((Real)1.0)/rkScale.X();
    Real fInvSY = ((Real)1.0)/rkScale.Y();
    kResult.m_afCoeff[0] = m_afCoeff[0];
    kResult.m_afCoeff[1] = m_afCoeff[1]*fInvSX;
    kResult.m_afCoeff[2] = m_afCoeff[2]*fInvSY;
    kResult.m_afCoeff[3] = m_afCoeff[3]*fInvSX*fInvSX;
    kResult.m_afCoeff[4] = m_afCoeff[4]*fInvSX*fInvSY;
    kResult.m_afCoeff[5] = m_afCoeff[5]*fInvSY*fInvSY;

    return kResult;
}
//----------------------------------------------------------------------------
