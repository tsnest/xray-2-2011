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
inline int FastMarch3::GetXBound () const
{
    return m_iXBound;
}
//----------------------------------------------------------------------------
inline int FastMarch3::GetYBound () const
{
    return m_iYBound;
}
//----------------------------------------------------------------------------
inline int FastMarch3::GetZBound () const
{
    return m_iZBound;
}
//----------------------------------------------------------------------------
inline float FastMarch3::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
inline float FastMarch3::GetYSpacing () const
{
    return m_fYSpacing;
}
//----------------------------------------------------------------------------
inline float FastMarch3::GetZSpacing () const
{
    return m_fZSpacing;
}
//----------------------------------------------------------------------------
inline int FastMarch3::Index (int iX, int iY, int iZ) const
{
    assert(0 <= iX && iX < m_iXBound
        && 0 <= iY && iY < m_iYBound
        && 0 <= iZ && iZ < m_iZBound);

    return iX + m_iXBound*(iY + m_iYBound*iZ);
}
//----------------------------------------------------------------------------
