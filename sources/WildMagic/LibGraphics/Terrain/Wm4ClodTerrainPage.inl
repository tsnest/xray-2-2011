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
inline int ClodTerrainPage::GetSize () const
{
    return m_iSize;
}
//----------------------------------------------------------------------------
inline const unsigned short* ClodTerrainPage::GetHeights () const
{
    return m_ausHeight;
}
//----------------------------------------------------------------------------
inline const Vector2f& ClodTerrainPage::GetOrigin () const
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetMinElevation () const
{
    return m_fMinElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetMaxElevation () const
{
    return m_fMaxElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetSpacing () const
{
    return m_fSpacing;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetPixelTolerance () const
{
    return m_fPixelTolerance;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetX (int iX) const
{
    return m_kOrigin.X() + m_fSpacing*(float)iX;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetY (int iY) const
{
    return m_kOrigin.Y() + m_fSpacing*(float)iY;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetHeight (int iIndex) const
{
    return m_fMinElevation + m_fMultiplier*(float)m_ausHeight[iIndex];
}
//----------------------------------------------------------------------------
inline float& ClodTerrainPage::UVBias ()
{
    return m_fUVBias;
}
//----------------------------------------------------------------------------
inline float ClodTerrainPage::GetTextureCoordinate (int iIndex) const
{
    return m_fUVBias + m_fTextureSpacing*(1.0f-2.0f*m_fUVBias) *
        (float)iIndex;
}
//----------------------------------------------------------------------------
