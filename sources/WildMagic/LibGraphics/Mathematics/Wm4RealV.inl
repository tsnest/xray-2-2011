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
template <typename Real, int N>
RealV<Real,N>::RealV ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>::RealV (Real fV0, Real fV1, Real fV2, Real fV3)
    :
    RealVBase<Real>(fV0,fV1,fV2,fV3)
{
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>::RealV (const RealV& rkVector)
    :
    RealVBase<Real>(rkVector)
{
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>::RealV (const RealVBase<Real>& rkVector)
    :
    RealVBase<Real>(rkVector)
{
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>& RealV<Real,N>::operator= (const RealV& rkVector)
{
    for (int i = 0; i < N; i++)
    {
        m_afTuple[i] = rkVector.m_afTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>& RealV<Real,N>::operator= (const RealVBase<Real>& rkVector)
{
    for (int i = 0; i < N; i++)
    {
        m_afTuple[i] = rkVector[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Bool<N> RealV<Real,N>::operator== (const RealV& rkVector) const
{
    Bool<N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult[i] = (m_afTuple[i] == rkVector.m_afTuple[i]);
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Bool<N> RealV<Real,N>::operator!= (const RealV& rkVector) const
{
    Bool<N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult[i] = (m_afTuple[i] != rkVector.m_afTuple[i]);
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Bool<N> RealV<Real,N>::operator<  (const RealV& rkVector) const
{
    Bool<N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult[i] = (m_afTuple[i] < rkVector.m_afTuple[i]);
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Bool<N> RealV<Real,N>::operator<= (const RealV& rkVector) const
{
    Bool<N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult[i] = (m_afTuple[i] <= rkVector.m_afTuple[i]);
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Bool<N> RealV<Real,N>::operator>  (const RealV& rkVector) const
{
    Bool<N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult[i] = (m_afTuple[i] > rkVector.m_afTuple[i]);
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Bool<N> RealV<Real,N>::operator>= (const RealV& rkVector) const
{
    Bool<N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult[i] = (m_afTuple[i] >= rkVector.m_afTuple[i]);
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N> RealV<Real,N>::operator+ (const RealV& rkVector) const
{
    RealV<Real,N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult.m_afTuple[i] = m_afTuple[i] + rkVector.m_afTuple[i];
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N> RealV<Real,N>::operator- (const RealV& rkVector) const
{
    RealV<Real,N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult.m_afTuple[i] = m_afTuple[i] - rkVector.m_afTuple[i];
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N> RealV<Real,N>::operator* (Real fScalar) const
{
    RealV<Real,N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult.m_afTuple[i] = m_afTuple[i]*fScalar;
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N> RealV<Real,N>::operator/ (Real fScalar) const
{
    RealV<Real,N> kResult;
    int i;

    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1)/fScalar;
        for (i = 0; i < N; i++)
        {
            kResult.m_afTuple[i] = m_afTuple[i]*fInvScalar;
        }
    }
    else
    {
        for (i = 0; i < N; i++)
        {
            kResult.m_afTuple[i] = Math<Real>::MAX_REAL;
        }
    }

    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N> RealV<Real,N>::operator- () const
{
    RealV<Real,N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult.m_afTuple[i] = -m_afTuple[i];
    }
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>& RealV<Real,N>::operator+= (const RealV& rkVector)
{
    for (int i = 0; i < N; i++)
    {
        m_afTuple[i] += rkVector.m_afTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>& RealV<Real,N>::operator-= (const RealV& rkVector)
{
    for (int i = 0; i < N; i++)
    {
        m_afTuple[i] -= rkVector.m_afTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>& RealV<Real,N>::operator*= (Real fScalar)
{
    for (int i = 0; i < N; i++)
    {
        m_afTuple[i] *= fScalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N>& RealV<Real,N>::operator/= (Real fScalar)
{
    if (fScalar != (Real)0)
    {
        Real fInvScalar = ((Real)1)/fScalar;
        for (int i = 0; i < N; i++)
        {
            m_afTuple[i] *= fInvScalar;
        }
    }
    else
    {
        for (int i = 0; i < N; i++)
        {
            m_afTuple[i] = Math<Real>::MAX_REAL;
        }
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealV<Real,N> operator* (Real fScalar, const RealV<Real,N>& rkVector)
{
    RealV<Real,N> kResult;
    for (int i = 0; i < N; i++)
    {
        kResult[i] = rkVector[i]*fScalar;
    }
    return kResult;
}
//----------------------------------------------------------------------------
