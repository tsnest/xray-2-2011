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
inline int TerrainPage::GetSize () const
{
    return m_iSize;
}
//----------------------------------------------------------------------------
inline const unsigned short* TerrainPage::GetHeights () const
{
    return m_ausHeight;
}
//----------------------------------------------------------------------------
inline const Vector2f& TerrainPage::GetOrigin () const
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetMinElevation () const
{
    return m_fMinElevation;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetMaxElevation () const
{
    return m_fMaxElevation;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetSpacing () const
{
    return m_fSpacing;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetX (int iX) const
{
    return m_kOrigin.X() + m_fSpacing*float(iX);
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetY (int iY) const
{
    return m_kOrigin.Y() + m_fSpacing*float(iY);
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetHeight (int iIndex) const
{
    return m_fMinElevation + m_fMultiplier*float(m_ausHeight[iIndex]);
}
//----------------------------------------------------------------------------
inline float& TerrainPage::UVBias ()
{
    return m_fUVBias;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetTextureCoordinate (int iIndex) const
{
    return m_fTextureSpacing*float(iIndex);
}
//----------------------------------------------------------------------------
