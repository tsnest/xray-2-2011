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
inline int LightingEffect::GetLightQuantity () const
{
    return (int)m_kLights.size();
}
//----------------------------------------------------------------------------
inline Light* LightingEffect::GetLight (int i) const
{
    assert(0 <= i && i < (int)m_kLights.size());
    return m_kLights[i];
}
//----------------------------------------------------------------------------
inline void LightingEffect::DetachAllLights ()
{
    m_kLights.clear();
}
//----------------------------------------------------------------------------
