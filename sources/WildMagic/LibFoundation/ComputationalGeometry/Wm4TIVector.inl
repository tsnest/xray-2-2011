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
template <int VSIZE>
TIVector<VSIZE>::TIVector ()
{
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>::TIVector (const TIVector& rkV)
{
    for (int i = 0; i < VSIZE; i++)
    {
        m_aiTuple[i] = rkV.m_aiTuple[i];
    }
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>::operator const Integer64* () const
{
    return m_aiTuple;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>::operator Integer64* ()
{
    return m_aiTuple;
}
//----------------------------------------------------------------------------
template <int VSIZE>
Integer64 TIVector<VSIZE>::operator[] (int i) const
{
    assert(0 <= i && i < VSIZE);
    return m_aiTuple[i];
}
//----------------------------------------------------------------------------
template <int VSIZE>
Integer64& TIVector<VSIZE>::operator[] (int i)
{
    assert(0 <= i && i < VSIZE);
    return m_aiTuple[i];
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>& TIVector<VSIZE>::operator= (const TIVector& rkV)
{
    for (int i = 0; i < VSIZE; i++)
    {
        m_aiTuple[i] = rkV.m_aiTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool TIVector<VSIZE>::operator== (const TIVector& rkV) const
{
    for (int i = 0; i < VSIZE; i++)
    {
        if (m_aiTuple[i] != rkV.m_aiTuple[i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool TIVector<VSIZE>::operator!= (const TIVector& rkV) const
{
    return !operator==(rkV);
}
//----------------------------------------------------------------------------
template <int VSIZE>
int TIVector<VSIZE>::CompareArrays (const TIVector& rkV) const
{
    for (int i = 0; i < VSIZE; i++)
    {
        if (m_aiTuple[i] < rkV.m_aiTuple[i])
        {
            return -1;
        }
        if (m_aiTuple[i] > rkV.m_aiTuple[i])
        {
            return +1;
        }
    }
    return 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool TIVector<VSIZE>::operator< (const TIVector& rkV) const
{
    return CompareArrays(rkV) < 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool TIVector<VSIZE>::operator<= (const TIVector& rkV) const
{
    return CompareArrays(rkV) <= 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool TIVector<VSIZE>::operator> (const TIVector& rkV) const
{
    return CompareArrays(rkV) > 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool TIVector<VSIZE>::operator>= (const TIVector& rkV) const
{
    return CompareArrays(rkV) >= 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE> TIVector<VSIZE>::operator+ (const TIVector& rkV) const
{
    TIVector<VSIZE> iSum;
    for (int i = 0; i < VSIZE; i++)
    {
        iSum.m_aiTuple[i] = m_aiTuple[i] + rkV.m_aiTuple[i];
    }
    return iSum;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE> TIVector<VSIZE>::operator- (const TIVector& rkV) const
{
    TIVector<VSIZE> iDiff;
    for (int i = 0; i < VSIZE; i++)
    {
        iDiff.m_aiTuple[i] = m_aiTuple[i] - rkV.m_aiTuple[i];
    }
    return iDiff;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE> TIVector<VSIZE>::operator* (const Integer64& riI) const
{
    TIVector<VSIZE> iProd;
    for (int i = 0; i < VSIZE; i++)
    {
        iProd.m_aiTuple[i] = riI*m_aiTuple[i];
    }
    return iProd;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE> TIVector<VSIZE>::operator/ (const Integer64& riI) const
{
    assert(riI != 0);

    TIVector<VSIZE> iProd;
    for (int i = 0; i < VSIZE; i++)
    {
        iProd.m_aiTuple[i] = m_aiTuple[i]/riI;
    }

    return iProd;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE> TIVector<VSIZE>::operator- () const
{
    TIVector<VSIZE> iNeg;
    for (int i = 0; i < VSIZE; i++)
    {
        iNeg.m_aiTuple[i] = -m_aiTuple[i];
    }
    return iNeg;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE> operator* (const Integer64& riI, const TIVector<VSIZE>& rkV)
{
    TIVector<VSIZE> iProd;
    for (int i = 0; i < VSIZE; i++)
    {
        iProd[i] = riI*rkV[i];
    }
    return iProd;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>& TIVector<VSIZE>::operator+= (const TIVector& rkV)
{
    for (int i = 0; i < VSIZE; i++)
    {
        m_aiTuple[i] += rkV.m_aiTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>& TIVector<VSIZE>::operator-= (const TIVector& rkV)
{
    for (int i = 0; i < VSIZE; i++)
    {
        m_aiTuple[i] -= rkV.m_aiTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>& TIVector<VSIZE>::operator*= (const Integer64& riI)
{
    for (int i = 0; i < VSIZE; i++)
    {
        m_aiTuple[i] *= riI;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
TIVector<VSIZE>& TIVector<VSIZE>::operator/= (const Integer64& riI)
{
    assert(riI != 0);
    for (int i = 0; i < VSIZE; i++)
    {
        m_aiTuple[i] /= riI;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
Integer64 TIVector<VSIZE>::SquaredLength () const
{
    Integer64 iSqrLen = 0;
    for (int i = 0; i < VSIZE; i++)
    {
        iSqrLen += m_aiTuple[i]*m_aiTuple[i];
    }
    return iSqrLen;
}
//----------------------------------------------------------------------------
template <int VSIZE>
Integer64 TIVector<VSIZE>::Dot (const TIVector& rkV) const
{
    Integer64 iDot = 0;
    for (int i = 0; i < VSIZE; i++)
    {
        iDot += m_aiTuple[i]*rkV.m_aiTuple[i];
    }
    return iDot;
}
//----------------------------------------------------------------------------
