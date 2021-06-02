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
inline int PlanarShadowEffect::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline void PlanarShadowEffect::SetPlane (int i, TriMeshPtr spkPlane)
{
    assert(0 <= i && i < m_iQuantity);
    m_aspkPlane[i] = spkPlane;
    m_aspkPlane[i]->Culling = Spatial::CULL_ALWAYS;
}
//----------------------------------------------------------------------------
inline TriMeshPtr PlanarShadowEffect::GetPlane (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_aspkPlane[i];
}
//----------------------------------------------------------------------------
inline void PlanarShadowEffect::SetProjector (int i, LightPtr spkProjector)
{
    assert(0 <= i && i < m_iQuantity);
    m_aspkProjector[i] = spkProjector;
}
//----------------------------------------------------------------------------
inline LightPtr PlanarShadowEffect::GetProjector (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_aspkProjector[i];
}
//----------------------------------------------------------------------------
inline void PlanarShadowEffect::SetShadowColor (int i,
    const ColorRGBA& rkShadowColor)
{
    assert(0 <= i && i < m_iQuantity);
    m_akShadowColor[i] = rkShadowColor;
}
//----------------------------------------------------------------------------
inline const ColorRGBA& PlanarShadowEffect::GetShadowColor (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akShadowColor[i];
}
//----------------------------------------------------------------------------
