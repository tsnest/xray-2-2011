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
inline int ClodTerrain::GetRowQuantity () const
{
    return m_iRows;
}
//----------------------------------------------------------------------------
inline int ClodTerrain::GetColQuantity () const
{
    return m_iCols;
}
//----------------------------------------------------------------------------
inline int ClodTerrain::GetSize () const
{
    return m_iSize;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetMinElevation () const
{
    return m_fMinElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetMaxElevation () const
{
    return m_fMaxElevation;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetSpacing () const
{
    return m_fSpacing;
}
//----------------------------------------------------------------------------
inline float ClodTerrain::GetPixelTolerance () const
{
    return m_fPixelTolerance;
}
//----------------------------------------------------------------------------
inline void ClodTerrain::SetRenderer (Renderer* pkRenderer)
{
    m_pkRenderer = pkRenderer;
}
//----------------------------------------------------------------------------
inline bool& ClodTerrain::CloseAssumption ()
{
    return m_bCloseAssumption;
}
//----------------------------------------------------------------------------
inline float& ClodTerrain::UVBias ()
{
    return m_fUVBias;
}
//----------------------------------------------------------------------------
inline ColorRGBA& ClodTerrain::BorderColor ()
{
    return m_kBorderColor;
}
//----------------------------------------------------------------------------
