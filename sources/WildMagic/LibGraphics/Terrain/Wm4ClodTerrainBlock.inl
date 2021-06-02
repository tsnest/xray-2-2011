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
inline int ClodTerrainBlock::GetX () const
{
    return (int)m_ucX;
}
//----------------------------------------------------------------------------
inline int ClodTerrainBlock::GetY () const
{
    return (int)m_ucY;
}
//----------------------------------------------------------------------------
inline int ClodTerrainBlock::GetStride () const
{
    return (int)m_ucStride;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetEven (bool bSet)
{
    if (bSet)
    {
        m_ucFlags |= EVEN_PARITY;
    }
    else
    {
        m_ucFlags &= ~EVEN_PARITY;
    }
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetEven () const
{
    return (m_ucFlags & EVEN_PARITY) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetProcessed (bool bSet)
{
    if (bSet)
    {
        m_ucFlags |= PROCESSED;
    }
    else
    {
        m_ucFlags &= ~PROCESSED;
    }
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetProcessed () const
{
    return (m_ucFlags & PROCESSED) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetVisible (bool bSet)
{
    if (bSet)
    {
        m_ucFlags |= VISIBLE;
    }
    else
    {
        m_ucFlags &= ~VISIBLE;
    }
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetVisible () const
{
    return (m_ucFlags & VISIBLE) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::SetVisibilityTested (bool bSet)
{
    if (bSet)
    {
        m_ucFlags |= VISIBILITY_TESTED;
    }
    else
    {
        m_ucFlags &= ~VISIBILITY_TESTED;
    }
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::GetVisibilityTested () const
{
    return (m_ucFlags & VISIBILITY_TESTED) != 0;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::BitsSet () const
{
    return (m_ucFlags & BITS_MASK) != 0;
}
//----------------------------------------------------------------------------
inline void ClodTerrainBlock::ClearBits ()
{
    // retain parity bit, all others zeroed out
    m_ucFlags &= EVEN_PARITY;
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDelta (int i) const
{
    // assert(0 <= i && i <= 4);
    return m_fDelta[i];
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDeltaMax () const
{
    return m_fDeltaMax;
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDeltaL () const
{
    return m_fDeltaL;
}
//----------------------------------------------------------------------------
inline float ClodTerrainBlock::GetDeltaH () const
{
    return m_fDeltaH;
}
//----------------------------------------------------------------------------
inline const Vector3f& ClodTerrainBlock::GetMin () const
{
    return m_kMin;
}
//----------------------------------------------------------------------------
inline const Vector3f& ClodTerrainBlock::GetMax () const
{
    return m_kMax;
}
//----------------------------------------------------------------------------
inline int ClodTerrainBlock::GetParentIndex (int iChild)
{
    // p = (c-1)/4
    return (iChild - 1) >> 2;
}
//----------------------------------------------------------------------------
inline int ClodTerrainBlock::GetChildIndex (int iParent, int iIndex)
{
    // c = 4*p+i
    return (iParent << 2) + iIndex;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::IsFirstChild (int iIndex)
{
    return iIndex > 0 && ((iIndex-1) % 4) == 0;
}
//----------------------------------------------------------------------------
inline bool ClodTerrainBlock::IsSibling (int iIndex, int iTest)
{
    // assert(iIndex > 0 && iTest > 0);
    return (iIndex-1) >> 2 == (iTest-1) >> 2;
}
//----------------------------------------------------------------------------
