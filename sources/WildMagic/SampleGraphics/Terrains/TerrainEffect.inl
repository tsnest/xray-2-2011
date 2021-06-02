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
inline void TerrainEffect::SetFogColor (const ColorRGB& rkFogColor)
{
    m_afFogColorDensity[0] = rkFogColor[0];
    m_afFogColorDensity[1] = rkFogColor[1];
    m_afFogColorDensity[2] = rkFogColor[2];
}
//----------------------------------------------------------------------------
inline ColorRGB TerrainEffect::GetFogColor () const
{
    return ColorRGB(m_afFogColorDensity[0],m_afFogColorDensity[1],
        m_afFogColorDensity[2]);
}
//----------------------------------------------------------------------------
inline void TerrainEffect::SetFogDensity (float fFogDensity)
{
    m_afFogColorDensity[3] = fFogDensity;
}
//----------------------------------------------------------------------------
inline float TerrainEffect::GetFogDensity () const
{
    return m_afFogColorDensity[3];
}
//----------------------------------------------------------------------------
