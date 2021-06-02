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

#include "Wm4GraphicsPCH.h"
#include "Wm4Attributes.h"
using namespace Wm4;

//----------------------------------------------------------------------------
Attributes::Attributes ()
{
    m_iChannelQuantity = 0;
    m_iPositionOffset = -1;
    m_iPositionChannels = 0;
    m_iNormalOffset = -1;
    m_iNormalChannels = 0;
    m_iBlendWeightOffset = -1;
    m_iBlendWeightChannels = 0;
    m_iFogOffset = -1;
    m_iFogChannels = 0;
    m_iPSizeOffset = -1;
    m_iPSizeChannels = 0;
    m_iBlendIndicesOffset = -1;
    m_iBlendIndicesChannels = 0;
    m_iTangentOffset = -1;
    m_iTangentChannels = 0;
    m_iBitangentOffset = -1;
    m_iBitangentChannels = 0;
}
//----------------------------------------------------------------------------
Attributes::~Attributes ()
{
}
//----------------------------------------------------------------------------
void Attributes::SetPositionChannels (int iPositionChannels)
{
    assert(0 <= iPositionChannels && iPositionChannels <= 4);

    m_iPositionChannels = iPositionChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetBlendWeightChannels (int iBlendWeightChannels)
{
    assert(0 <= iBlendWeightChannels && iBlendWeightChannels <= 4);

    m_iBlendWeightChannels = iBlendWeightChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetNormalChannels (int iNormalChannels)
{
    assert(0 <= iNormalChannels && iNormalChannels <= 4);

    m_iNormalChannels = iNormalChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetColorChannels (int iUnit, int iColorChannels)
{
    assert(0 <= iUnit && 0 <= iColorChannels && iColorChannels <= 4);

    int iMaxUnits = (int)m_kColorChannels.size();
    if (iUnit >= iMaxUnits)
    {
        m_kColorChannels.resize(iUnit+1);
    }

    m_kColorChannels[iUnit] = iColorChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetFogChannels (int iFogChannels)
{
    assert(0 <= iFogChannels && iFogChannels <= 1);

    m_iFogChannels = iFogChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetPSizeChannels (int iPSizeChannels)
{
    assert(0 <= iPSizeChannels && iPSizeChannels <= 1);

    m_iPSizeChannels = iPSizeChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetBlendIndicesChannels (int iBlendIndicesChannels)
{
    assert(0 <= iBlendIndicesChannels && iBlendIndicesChannels <= 4);

    m_iBlendIndicesChannels = iBlendIndicesChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetTCoordChannels (int iUnit, int iTCoordChannels)
{
    assert(0 <= iUnit && 0 <= iTCoordChannels && iTCoordChannels <= 4);

    int iMaxUnits = (int)m_kTCoordChannels.size();
    if (iUnit >= iMaxUnits)
    {
        m_kTCoordChannels.resize(iUnit+1);
    }

    m_kTCoordChannels[iUnit] = iTCoordChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetTangentChannels (int iTangentChannels)
{
    assert(0 <= iTangentChannels && iTangentChannels <= 4);

    m_iTangentChannels = iTangentChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
void Attributes::SetBitangentChannels (int iBitangentChannels)
{
    assert(0 <= iBitangentChannels && iBitangentChannels <= 4);

    m_iBitangentChannels = iBitangentChannels;
    UpdateOffsets();
}
//----------------------------------------------------------------------------
int Attributes::GetColorOffset (int iUnit) const
{
    if (0 <= iUnit && iUnit < (int)m_kColorOffset.size())
    {
        return m_kColorOffset[iUnit];
    }
    return -1;
}
//----------------------------------------------------------------------------
int Attributes::GetColorChannels (int iUnit) const
{
    if (0 <= iUnit && iUnit < (int)m_kColorChannels.size())
    {
        return m_kColorChannels[iUnit];
    }
    return 0;
}
//----------------------------------------------------------------------------
bool Attributes::HasColor (int iUnit) const
{
    if (0 <= iUnit && iUnit < (int)m_kColorChannels.size())
    {
        return m_kColorChannels[iUnit] > 0;
    }
    return false;
}
//----------------------------------------------------------------------------
int Attributes::GetTCoordOffset (int iUnit) const
{
    if (0 <= iUnit && iUnit < (int)m_kTCoordOffset.size())
    {
        return m_kTCoordOffset[iUnit];
    }
    return -1;
}
//----------------------------------------------------------------------------
int Attributes::GetTCoordChannels (int iUnit) const
{
    if (0 <= iUnit && iUnit < (int)m_kTCoordChannels.size())
    {
        return m_kTCoordChannels[iUnit];
    }
    return 0;
}
//----------------------------------------------------------------------------
bool Attributes::HasTCoord (int iUnit) const
{
    if (0 <= iUnit && iUnit < (int)m_kTCoordChannels.size())
    {
        return m_kTCoordChannels[iUnit] > 0;
    }
    return false;
}
//----------------------------------------------------------------------------
bool Attributes::Matches (const Attributes& rkAttr, bool bIncludePosition,
    bool bIncludeBlendWeight, bool bIncludeNormal, bool bIncludeColor,
    bool bIncludeFog, bool bIncludePSize, bool bIncludeBlendIndices,
    bool bIncludeTCoord, bool bIncludeTangent, bool bIncludeBitangent) const
{
    int i;

    if (bIncludePosition)
    {
        if (m_iPositionChannels != rkAttr.m_iPositionChannels)
        {
            return false;
        }
    }

    if (bIncludeBlendWeight)
    {
        if (m_iBlendWeightChannels != rkAttr.m_iBlendWeightChannels)
        {
            return false;
        }
    }

    if (bIncludeNormal)
    {
        if (m_iNormalChannels != rkAttr.m_iNormalChannels)
        {
            return false;
        }
    }

    if (bIncludeColor)
    {
        if (m_kColorChannels.size() != rkAttr.m_kColorChannels.size())
        {
            return false;
        }
        for (i = 0; i < (int)m_kColorChannels.size(); i++)
        {
            if (m_kColorChannels[i] != rkAttr.m_kColorChannels[i])
            {
                return false;
            }
        }
    }

    if (bIncludeFog)
    {
        if (m_iFogChannels != rkAttr.m_iFogChannels)
        {
            return false;
        }
    }

    if (bIncludePSize)
    {
        if (m_iPSizeChannels != rkAttr.m_iPSizeChannels)
        {
            return false;
        }
    }

    if (bIncludeBlendIndices)
    {
        if (m_iBlendIndicesChannels != rkAttr.m_iBlendIndicesChannels)
        {
            return false;
        }
    }

    if (bIncludeTCoord)
    {
        if (m_kTCoordChannels.size() != rkAttr.m_kTCoordChannels.size())
        {
            return false;
        }
        for (i = 0; i < (int)m_kTCoordChannels.size(); i++)
        {
            if (m_kTCoordChannels[i] != rkAttr.m_kTCoordChannels[i])
            {
                return false;
            }
        }
    }

    if (bIncludeTangent)
    {
        if (m_iTangentChannels != rkAttr.m_iTangentChannels)
        {
            return false;
        }
    }

    if (bIncludeBitangent)
    {
        if (m_iBitangentChannels != rkAttr.m_iBitangentChannels)
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Attributes::operator== (const Attributes& rkAttr) const
{
    // We need self.value == rkAttr.value for all values.
    int i;

    if (rkAttr.m_iPositionChannels != m_iPositionChannels)
    {
        return false;
    }

    if (rkAttr.m_iBlendWeightChannels != m_iBlendWeightChannels)
    {
        return false;
    }

    if (rkAttr.m_iNormalChannels != m_iNormalChannels)
    {
        return false;
    }

    if (rkAttr.m_kColorChannels.size() != m_kColorChannels.size())
    {
        return false;
    }
    for (i = 0; i < (int)m_kColorChannels.size(); i++)
    {
        if (rkAttr.m_kColorChannels[i] != m_kColorChannels[i])
        {
            return false;
        }
    }

    if (rkAttr.m_iFogChannels != m_iFogChannels)
    {
        return false;
    }

    if (rkAttr.m_iPSizeChannels != m_iPSizeChannels)
    {
        return false;
    }

    if (rkAttr.m_iBlendIndicesChannels != m_iBlendIndicesChannels)
    {
        return false;
    }

    if (rkAttr.m_kTCoordChannels.size() != m_kTCoordChannels.size())
    {
        return false;
    }
    for (i = 0; i < (int)m_kTCoordChannels.size(); i++)
    {
        if (rkAttr.m_kTCoordChannels[i] != m_kTCoordChannels[i])
        {
            return false;
        }
    }

    if (rkAttr.m_iTangentChannels != m_iTangentChannels)
    {
        return false;
    }

    if (rkAttr.m_iBitangentChannels != m_iBitangentChannels)
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool Attributes::operator!= (const Attributes& rkAttr) const
{
    return !operator==(rkAttr);
}
//----------------------------------------------------------------------------
void Attributes::UpdateOffsets ()
{
    int i;

    m_iChannelQuantity = 0;
    m_iPositionOffset = -1;
    m_iBlendWeightOffset = -1;
    m_iNormalOffset = -1;
    m_kColorOffset.resize(m_kColorChannels.size());
    for (i = 0; i < (int)m_kColorChannels.size(); i++)
    {
        m_kColorOffset[i] = -1;
    }
    m_iFogOffset = -1;
    m_iPSizeOffset = -1;
    m_iBlendIndicesOffset = -1;
    m_kTCoordOffset.resize(m_kTCoordChannels.size());
    for (i = 0; i < (int)m_kTCoordChannels.size(); i++)
    {
        m_kTCoordOffset[i] = -1;
    }
    m_iTangentOffset = -1;
    m_iBitangentOffset = -1;

    if (m_iPositionChannels > 0)
    {
        m_iPositionOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iPositionChannels;
    }

    if (m_iBlendWeightChannels > 0)
    {
        m_iBlendWeightOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iBlendWeightChannels;
    }

    if (m_iNormalChannels > 0)
    {
        m_iNormalOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iNormalChannels;
    }

    for (i = 0; i < (int)m_kColorChannels.size(); i++)
    {
        if (m_kColorChannels[i] > 0)
        {
            m_kColorOffset[i] = m_iChannelQuantity;
            m_iChannelQuantity += m_kColorChannels[i];
        }
    }

    if (m_iFogChannels > 0)
    {
        m_iFogOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iFogChannels;
    }

    if (m_iPSizeChannels > 0)
    {
        m_iPSizeOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iPSizeChannels;
    }

    if (m_iBlendIndicesChannels > 0)
    {
        m_iBlendIndicesOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iBlendIndicesChannels;
    }

    for (i = 0; i < (int)m_kTCoordChannels.size(); i++)
    {
        if (m_kTCoordChannels[i] > 0)
        {
            m_kTCoordOffset[i] = m_iChannelQuantity;
            m_iChannelQuantity += m_kTCoordChannels[i];
        }
    }

    if (m_iTangentChannels > 0)
    {
        m_iTangentOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iTangentChannels;
    }

    if (m_iBitangentChannels > 0)
    {
        m_iBitangentOffset = m_iChannelQuantity;
        m_iChannelQuantity += m_iBitangentChannels;
    }
}
//----------------------------------------------------------------------------
