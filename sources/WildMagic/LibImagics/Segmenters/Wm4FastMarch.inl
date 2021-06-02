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
// Version: 4.0.0 (2006/09/21)

//----------------------------------------------------------------------------
inline void FastMarch::SetTime (int i, float fTime)
{
    assert(0 <= i && i < m_iQuantity);
    m_afTime[i] = fTime;
}
//----------------------------------------------------------------------------
inline float FastMarch::GetTime (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_afTime[i];
}
//----------------------------------------------------------------------------
inline int FastMarch::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsValid (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return 0.0f <= m_afTime[i] && m_afTime[i] < Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsTrial (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_apkTrial[i] != 0;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsFar (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_afTime[i] == Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsZeroSpeed (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_afTime[i] == -Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsInterior (int i) const
{
    return IsValid(i) && !IsTrial(i);
}
//----------------------------------------------------------------------------
