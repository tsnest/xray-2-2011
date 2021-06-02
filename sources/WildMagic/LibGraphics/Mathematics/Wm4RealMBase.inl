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
RealMBase<Real>::RealMBase ()
{
    // The data is uninitialized for performance in array construction.
}
//----------------------------------------------------------------------------
template <typename Real>
RealMBase<Real>::RealMBase (bool bZero)
{
    int i;
    if (bZero)
    {
        for (i = 0; i < 16; i++)
        {
            m_afEntry[i] = (Real)0.0;
        }
    }
    else
    {
        for (i = 0; i < 16; i++)
        {
            m_afEntry[i] = Math<Real>::MAX_REAL;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
RealMBase<Real>::RealMBase (const RealMBase& rkMatrix)
{
    for (int i = 0; i < 16; i++)
    {
        m_afEntry[i] = rkMatrix.m_afEntry[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
RealMBase<Real>::~RealMBase ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
RealMBase<Real>& RealMBase<Real>::operator= (const RealMBase& rkMatrix)
{
    for (int i = 0; i < 16; i++)
    {
        m_afEntry[i] = rkMatrix.m_afEntry[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
RealMBase<Real>::operator const Real* () const
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <typename Real>
RealMBase<Real>::operator Real* ()
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* RealMBase<Real>::operator[] (int iRow) const
{
    // Assert:  0 <= iRow < N, where N in {1,2,3,4} depending on which
    // derived class object is making the call.
    return &m_afEntry[iRow << 2];
}
//----------------------------------------------------------------------------
template <typename Real>
Real* RealMBase<Real>::operator[] (int iRow)
{
    // Assert:  0 <= iRow < N, where N in {1,2,3,4} depending on which
    // derived class object is making the call.
    return &m_afEntry[iRow << 2];
}
//----------------------------------------------------------------------------
template <typename Real>
Real RealMBase<Real>::operator() (int iRow, int iCol) const
{
    // Assert:  0 <= iRow < Rows and 0 <= iCol < Cols, depending on which
    // derived class object is making the call.
    return m_afEntry[iCol + (iRow << 2)];
}
//----------------------------------------------------------------------------
template <typename Real>
Real& RealMBase<Real>::operator() (int iRow, int iCol)
{
    // Assert:  0 <= iRow < Rows and 0 <= iCol < Cols, depending on which
    // derived class object is making the call.
    return m_afEntry[iCol + (iRow << 2)];
}
//----------------------------------------------------------------------------
