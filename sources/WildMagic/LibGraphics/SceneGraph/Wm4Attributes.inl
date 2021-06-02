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
// Version: 4.0.1 (2007/06/16)

//----------------------------------------------------------------------------
inline int Attributes::GetChannelQuantity () const
{
    return m_iChannelQuantity;
}
//----------------------------------------------------------------------------
inline int Attributes::GetPositionOffset () const
{
    return m_iPositionOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetPositionChannels () const
{
    return m_iPositionChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasPosition () const
{
    return m_iPositionChannels > 0;
}
//----------------------------------------------------------------------------
inline int Attributes::GetBlendWeightOffset () const
{
    return m_iBlendWeightOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetBlendWeightChannels () const
{
    return m_iBlendWeightChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasBlendWeight () const
{
    return m_iBlendWeightChannels > 0;
}
//----------------------------------------------------------------------------
inline int Attributes::GetNormalOffset () const
{
    return m_iNormalOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetNormalChannels () const
{
    return m_iNormalChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasNormal () const
{
    return m_iNormalChannels > 0;
}
//----------------------------------------------------------------------------
inline int Attributes::GetMaxColors () const
{
    return (int)m_kColorChannels.size();
}
//----------------------------------------------------------------------------
inline int Attributes::GetFogOffset () const
{
    return m_iFogOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetFogChannels () const
{
    return m_iFogChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasFog () const
{
    return m_iFogChannels > 0;
}
//----------------------------------------------------------------------------
inline int Attributes::GetPSizeOffset () const
{
    return m_iPSizeOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetPSizeChannels () const
{
    return m_iPSizeChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasPSize() const
{
    return m_iPSizeChannels > 0;
}
//----------------------------------------------------------------------------
inline int Attributes::GetBlendIndicesOffset () const
{
    return m_iBlendIndicesOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetBlendIndicesChannels () const
{
    return m_iBlendIndicesChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasBlendIndices() const
{
    return m_iBlendIndicesChannels > 0;
}
//----------------------------------------------------------------------------
inline int Attributes::GetMaxTCoords () const
{
    return (int)m_kTCoordChannels.size();
}
//----------------------------------------------------------------------------
inline int Attributes::GetTangentOffset () const
{
    return m_iTangentOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetTangentChannels () const
{
    return m_iTangentChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasTangent () const
{
    return m_iTangentChannels > 0;
}
//----------------------------------------------------------------------------
inline int Attributes::GetBitangentOffset () const
{
    return m_iTangentOffset;
}
//----------------------------------------------------------------------------
inline int Attributes::GetBitangentChannels () const
{
    return m_iBitangentChannels;
}
//----------------------------------------------------------------------------
inline bool Attributes::HasBitangent () const
{
    return m_iBitangentChannels > 0;
}
//----------------------------------------------------------------------------
