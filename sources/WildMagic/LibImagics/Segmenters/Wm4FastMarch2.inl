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
inline int FastMarch2::GetXBound () const
{
    return m_iXBound;
}
//----------------------------------------------------------------------------
inline int FastMarch2::GetYBound () const
{
    return m_iYBound;
}
//----------------------------------------------------------------------------
inline float FastMarch2::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
inline float FastMarch2::GetYSpacing () const
{
    return m_fYSpacing;
}
//----------------------------------------------------------------------------
inline int FastMarch2::Index (int iX, int iY) const
{
    assert(0 <= iX && iX < m_iXBound && 0 <= iY && iY < m_iYBound);
    return iX + m_iXBound*iY;
}
//----------------------------------------------------------------------------
