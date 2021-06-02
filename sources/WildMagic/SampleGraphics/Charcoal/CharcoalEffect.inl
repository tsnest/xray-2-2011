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
inline void CharcoalEffect::SetAmbientIntensity (float fAmbientIntensity)
{
    m_afAmbientContrast[0] = fAmbientIntensity;
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetAmbientIntensity () const
{
    return m_afAmbientContrast[0];
}
//----------------------------------------------------------------------------
inline void CharcoalEffect::SetContrastExponent (float fContrastExponent)
{
    m_afAmbientContrast[1] = fContrastExponent;
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetContrastExponent () const
{
    return m_afAmbientContrast[1];
}
//----------------------------------------------------------------------------
inline void CharcoalEffect::SetSmudgeFactor (float fSmudgeFactor)
{
    m_afSmudgePaper[0] = fSmudgeFactor;
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetSmudgeFactor () const
{
    return m_afSmudgePaper[0];
}
//----------------------------------------------------------------------------
inline void CharcoalEffect::SetPaperFactor (float fPaperFactor)
{
    m_afSmudgePaper[1] = fPaperFactor;
}
//----------------------------------------------------------------------------
inline float CharcoalEffect::GetPaperFactor () const
{
    return m_afSmudgePaper[1];
}
//----------------------------------------------------------------------------
