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
inline void Spatial::SetParent (Spatial* pkParent)
{
    m_pkParent = pkParent;
}
//----------------------------------------------------------------------------
inline Spatial* Spatial::GetParent ()
{
    return m_pkParent;
}
//----------------------------------------------------------------------------
inline int Spatial::GetGlobalStateQuantity () const
{
    return (int)m_kGlobalStates.size();
}
//----------------------------------------------------------------------------
inline GlobalState* Spatial::GetGlobalState (int i) const
{
    assert(0 <= i && i < (int)m_kGlobalStates.size());
    return m_kGlobalStates[i];
}
//----------------------------------------------------------------------------
inline void Spatial::DetachAllGlobalStates ()
{
    m_kGlobalStates.clear();
}
//----------------------------------------------------------------------------
inline int Spatial::GetLightQuantity () const
{
    return (int)m_kLights.size();
}
//----------------------------------------------------------------------------
inline Light* Spatial::GetLight (int i) const
{
    assert(0 <= i && i < (int)m_kLights.size());
    return StaticCast<Light>(m_kLights[i]);
}
//----------------------------------------------------------------------------
inline void Spatial::DetachAllLights ()
{
    m_kLights.clear();
}
//----------------------------------------------------------------------------
inline int Spatial::GetEffectQuantity () const
{
    return (int)m_kEffects.size();
}
//----------------------------------------------------------------------------
inline Effect* Spatial::GetEffect (int i) const
{
    assert(0 <= i && i < (int)m_kEffects.size());
    return StaticCast<Effect>(m_kEffects[i]);
}
//----------------------------------------------------------------------------
inline void Spatial::DetachAllEffects ()
{
    m_kEffects.clear();
}
//----------------------------------------------------------------------------
inline void Spatial::SetStartEffect (int i)
{
    assert(0 <= i && i < (int)m_kEffects.size());
    m_iStartEffect = i;
}
//----------------------------------------------------------------------------
inline int Spatial::GetStartEffect () const
{
    return m_iStartEffect;
}
//----------------------------------------------------------------------------
