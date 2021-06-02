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
#define RMLOOP_RETN(expression)\
for (int iRow = 0, i = 0; iRow < Rows; iRow++, \
     i += RealM<Real,Rows,Cols>::INCR) \
{ \
    for (int iCol = 0; iCol < Cols; iCol++, i++) \
    { \
        expression; \
    } \
}
//----------------------------------------------------------------------------
#define RMLOOP_RETR(expression)\
RealM<Real,Rows,Cols> kResult; \
for (int iRow = 0, i = 0; iRow < Rows; iRow++, \
     i += RealM<Real,Rows,Cols>::INCR) \
{ \
    for (int iCol = 0; iCol < Cols; iCol++, i++) \
    { \
        expression; \
    } \
} \
return kResult;
//----------------------------------------------------------------------------
#define RMLOOP_RETT(expression)\
for (int iRow = 0, i = 0; iRow < Rows; iRow++, \
     i += RealM<Real,Rows,Cols>::INCR) \
{ \
    for (int iCol = 0; iCol < Cols; iCol++, i++) \
    { \
        expression; \
    } \
} \
return *this;
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>::RealM ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>::RealM (bool bZero)
    :
    RealMBase<Real>(bZero)
{
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>::RealM (const RealM& rkMatrix)
    :
    RealMBase<Real>(rkMatrix)
{
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>::RealM (const RealMBase<Real>& rkMatrix)
    :
    RealMBase<Real>(rkMatrix)
{
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>& RealM<Real,Rows,Cols>::operator= (
    const RealM& rkMatrix)
{
    RMLOOP_RETT(m_afEntry[i] = rkMatrix.m_afEntry[i])
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>& RealM<Real,Rows,Cols>::operator= (
    const RealMBase<Real>& rkMatrix)
{
    const Real* afMEntry = (const Real*)rkMatrix;
    RMLOOP_RETT(m_afEntry[i] = afMEntry[i])
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols> RealM<Real,Rows,Cols>::operator+ (
    const RealM& rkMatrix) const
{
    RMLOOP_RETR(kResult.m_afEntry[i] = m_afEntry[i] + rkMatrix.m_afEntry[i])
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols> RealM<Real,Rows,Cols>::operator- (
    const RealM& rkMatrix) const
{
    RMLOOP_RETR(kResult.m_afEntry[i] = m_afEntry[i] - rkMatrix.m_afEntry[i])
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols> RealM<Real,Rows,Cols>::operator* (Real fScalar) const
{
    RMLOOP_RETR(kResult.m_afEntry[i] = m_afEntry[i]*fScalar)
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols> RealM<Real,Rows,Cols>::operator/ (Real fScalar) const
{
    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        RMLOOP_RETR(kResult.m_afEntry[i] = m_afEntry[i]*fInvScalar)
    }
    RMLOOP_RETR(kResult.m_afEntry[i] = Math<Real>::MAX_REAL)
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols> RealM<Real,Rows,Cols>::operator- () const
{
    RMLOOP_RETR(kResult.m_afEntry[i] = -m_afEntry[i])
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>& RealM<Real,Rows,Cols>::operator+= (
    const RealM& rkMatrix)
{
    RMLOOP_RETT(m_afEntry[i] += rkMatrix.m_afEntry[i])
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>& RealM<Real,Rows,Cols>::operator-= (
    const RealM& rkMatrix)
{
    RMLOOP_RETT(m_afEntry[i] -= rkMatrix.m_afEntry[i])
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>& RealM<Real,Rows,Cols>::operator*= (Real fScalar)
{
    RMLOOP_RETT(m_afEntry[i] *= fScalar)
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols>& RealM<Real,Rows,Cols>::operator/= (Real fScalar)
{
    if (fScalar != (Real)0.0)
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        RMLOOP_RETT(m_afEntry[i] *= fInvScalar)
    }
    RMLOOP_RETT(m_afEntry[i] = Math<Real>::MAX_REAL)
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols> operator* (Real fScalar,
    const RealM<Real,Rows,Cols>& rkMatrix)
{
    const Real* afMEntry = (const Real*)rkMatrix;
    RMLOOP_RETR(((Real*)kResult)[i] = afMEntry[i]*fScalar)
}
//----------------------------------------------------------------------------
