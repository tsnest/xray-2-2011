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
// Version: 4.0.2 (2007/08/01)

#include "Wm4GraphicsPCH.h"
#include "Wm4VertexBuffer.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,VertexBuffer,Object);
WM4_IMPLEMENT_STREAM(VertexBuffer);
WM4_IMPLEMENT_DEFAULT_NAME_ID(VertexBuffer,Object);

//----------------------------------------------------------------------------
VertexBuffer::VertexBuffer (const Attributes& rkAttributes,
    int iVertexQuantity)
    :
    m_kAttributes(rkAttributes)
{
    assert(iVertexQuantity > 0);
    m_iVertexQuantity = iVertexQuantity;
    m_iVertexSize = m_kAttributes.GetChannelQuantity();
    m_iChannelQuantity = m_iVertexQuantity*m_iVertexSize;
    m_afChannel = WM4_NEW float[m_iChannelQuantity];
    memset(m_afChannel,0,m_iChannelQuantity*sizeof(float));
}
//----------------------------------------------------------------------------
VertexBuffer::VertexBuffer (const VertexBuffer* pkVBuffer)
{
    assert(pkVBuffer);
    m_kAttributes = pkVBuffer->m_kAttributes;
    m_iVertexQuantity = pkVBuffer->m_iVertexQuantity;
    m_iVertexSize = m_kAttributes.GetChannelQuantity();
    m_iChannelQuantity = m_iVertexQuantity*m_iVertexSize;
    m_afChannel = WM4_NEW float[m_iChannelQuantity];
    size_t uiSize = m_iChannelQuantity*sizeof(float);
    System::Memcpy(m_afChannel,uiSize,pkVBuffer->m_afChannel,uiSize);
}
//----------------------------------------------------------------------------
VertexBuffer::VertexBuffer ()
{
    m_iVertexQuantity = 0;
    m_iVertexSize = 0;
    m_iChannelQuantity = 0;
    m_afChannel = 0;
}
//----------------------------------------------------------------------------
VertexBuffer::~VertexBuffer ()
{
    // Inform all renderers using this vertex buffer that it is being
    // destroyed.  This allows the renderer to free up any associated
    // resources.
    Release();

    WM4_DELETE[] m_afChannel;
}
//----------------------------------------------------------------------------
float* VertexBuffer::PositionTuple (int i)
{
    if (m_kAttributes.HasPosition() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::PositionTuple (int i) const
{
    if (m_kAttributes.HasPosition() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::BlendWeightTuple (int i)
{
    if (m_kAttributes.HasBlendWeight() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::BlendWeightTuple (int i) const
{
    if (m_kAttributes.HasBlendWeight() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::NormalTuple (int i)
{
    if (m_kAttributes.HasNormal() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::NormalTuple (int i) const
{
    if (m_kAttributes.HasNormal() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::ColorTuple (int iUnit, int i)
{
    if (m_kAttributes.HasColor(iUnit) && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::ColorTuple (int iUnit, int i) const
{
    if (m_kAttributes.HasColor(iUnit) && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::FogTuple (int i)
{
    if (m_kAttributes.HasFog() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetFogOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::FogTuple (int i) const
{
    if (m_kAttributes.HasFog() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetFogOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::PSizeTuple (int i)
{
    if (m_kAttributes.HasPSize() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetPSizeOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::PSizeTuple (int i) const
{
    if (m_kAttributes.HasPSize() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetPSizeOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::BlendIndicesTuple (int i)
{
    if (m_kAttributes.HasBlendIndices() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::BlendIndicesTuple (int i) const
{
    if (m_kAttributes.HasBlendIndices() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::TCoordTuple (int iUnit, int i)
{
    if (m_kAttributes.HasTCoord(iUnit) && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::TCoordTuple (int iUnit, int i) const
{
    if (m_kAttributes.HasTCoord(iUnit) && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::TangentTuple (int i)
{
    if (m_kAttributes.HasTangent() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::TangentTuple (int i) const
{
    if (m_kAttributes.HasTangent() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float* VertexBuffer::BitangentTuple (int i)
{
    if (m_kAttributes.HasBitangent() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
const float* VertexBuffer::BitangentTuple (int i) const
{
    if (m_kAttributes.HasBitangent() && 0 <= i && i < m_iVertexQuantity)
    {
        int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
        return m_afChannel + iIndex;
    }
    return 0;
}
//----------------------------------------------------------------------------
float& VertexBuffer::Position1 (int i)
{
    assert(m_kAttributes.GetPositionChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::Position1 (int i) const
{
    assert(m_kAttributes.GetPositionChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::Position2 (int i)
{
    assert(m_kAttributes.GetPositionChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::Position2 (int i) const
{
    assert(m_kAttributes.GetPositionChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f& VertexBuffer::Position3 (int i)
{
    assert(m_kAttributes.GetPositionChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f VertexBuffer::Position3 (int i) const
{
    assert(m_kAttributes.GetPositionChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f& VertexBuffer::Position4 (int i)
{
    assert(m_kAttributes.GetPositionChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f VertexBuffer::Position4 (int i) const
{
    assert(m_kAttributes.GetPositionChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPositionOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::BlendWeight1 (int i)
{
    assert(m_kAttributes.GetBlendWeightChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::BlendWeight1 (int i) const
{
    assert(m_kAttributes.GetBlendWeightChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::BlendWeight2 (int i)
{
    assert(m_kAttributes.GetBlendWeightChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::BlendWeight2 (int i) const
{
    assert(m_kAttributes.GetBlendWeightChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f& VertexBuffer::BlendWeight3 (int i)
{
    assert(m_kAttributes.GetBlendWeightChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f VertexBuffer::BlendWeight3 (int i) const
{
    assert(m_kAttributes.GetBlendWeightChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f& VertexBuffer::BlendWeight4 (int i)
{
    assert(m_kAttributes.GetBlendWeightChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f VertexBuffer::BlendWeight4 (int i) const
{
    assert(m_kAttributes.GetBlendWeightChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendWeightOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::Normal1 (int i)
{
    assert(m_kAttributes.GetNormalChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::Normal1 (int i) const
{
    assert(m_kAttributes.GetNormalChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::Normal2 (int i)
{
    assert(m_kAttributes.GetNormalChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::Normal2 (int i) const
{
    assert(m_kAttributes.GetNormalChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f& VertexBuffer::Normal3 (int i)
{
    assert(m_kAttributes.GetNormalChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f VertexBuffer::Normal3 (int i) const
{
    assert(m_kAttributes.GetNormalChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f& VertexBuffer::Normal4 (int i)
{
    assert(m_kAttributes.GetNormalChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f VertexBuffer::Normal4 (int i) const
{
    assert(m_kAttributes.GetNormalChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetNormalOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::Color1 (int iUnit, int i)
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::Color1 (int iUnit, int i) const
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::Color2 (int iUnit, int i)
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::Color2 (int iUnit, int i) const
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
ColorRGB& VertexBuffer::Color3 (int iUnit, int i)
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(ColorRGB*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
ColorRGB VertexBuffer::Color3 (int iUnit, int i) const
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(ColorRGB*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
ColorRGBA& VertexBuffer::Color4 (int iUnit, int i)
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(ColorRGBA*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
ColorRGBA VertexBuffer::Color4 (int iUnit, int i) const
{
    assert(m_kAttributes.GetColorChannels(iUnit) == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetColorOffset(iUnit);
    return *(ColorRGBA*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::Fog1 (int i)
{
    assert(m_kAttributes.GetFogChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetFogOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::Fog1 (int i) const
{
    assert(m_kAttributes.GetFogChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetFogOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::PSize1 (int i)
{
    assert(m_kAttributes.GetPSizeChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPSizeOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::PSize1 (int i) const
{
    assert(m_kAttributes.GetPSizeChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetPSizeOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::BlendIndices1 (int i)
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::BlendIndices1 (int i) const
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::BlendIndices2 (int i)
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::BlendIndices2 (int i) const
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f& VertexBuffer::BlendIndices3 (int i)
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f VertexBuffer::BlendIndices3 (int i) const
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f& VertexBuffer::BlendIndices4 (int i)
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f VertexBuffer::BlendIndices4 (int i) const
{
    assert(m_kAttributes.GetBlendIndicesChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBlendIndicesOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::TCoord1 (int iUnit, int i)
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::TCoord1 (int iUnit, int i) const
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::TCoord2 (int iUnit, int i)
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::TCoord2 (int iUnit, int i) const
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f& VertexBuffer::TCoord3 (int iUnit, int i)
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f VertexBuffer::TCoord3 (int iUnit, int i) const
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f& VertexBuffer::TCoord4 (int iUnit, int i)
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f VertexBuffer::TCoord4 (int iUnit, int i) const
{
    assert(m_kAttributes.GetTCoordChannels(iUnit) == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTCoordOffset(iUnit);
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::Tangent1 (int i)
{
    assert(m_kAttributes.GetTangentChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::Tangent1 (int i) const
{
    assert(m_kAttributes.GetTangentChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::Tangent2 (int i)
{
    assert(m_kAttributes.GetTangentChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::Tangent2 (int i) const
{
    assert(m_kAttributes.GetTangentChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f& VertexBuffer::Tangent3 (int i)
{
    assert(m_kAttributes.GetTangentChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f VertexBuffer::Tangent3 (int i) const
{
    assert(m_kAttributes.GetTangentChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f& VertexBuffer::Tangent4 (int i)
{
    assert(m_kAttributes.GetTangentChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f VertexBuffer::Tangent4 (int i) const
{
    assert(m_kAttributes.GetTangentChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetTangentOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float& VertexBuffer::Bitangent1 (int i)
{
    assert(m_kAttributes.GetBitangentChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
float VertexBuffer::Bitangent1 (int i) const
{
    assert(m_kAttributes.GetBitangentChannels() == 1);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f& VertexBuffer::Bitangent2 (int i)
{
    assert(m_kAttributes.GetBitangentChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector2f VertexBuffer::Bitangent2 (int i) const
{
    assert(m_kAttributes.GetBitangentChannels() == 2);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(Vector2f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f& VertexBuffer::Bitangent3 (int i)
{
    assert(m_kAttributes.GetBitangentChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector3f VertexBuffer::Bitangent3 (int i) const
{
    assert(m_kAttributes.GetBitangentChannels() == 3);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(Vector3f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f& VertexBuffer::Bitangent4 (int i)
{
    assert(m_kAttributes.GetBitangentChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
Vector4f VertexBuffer::Bitangent4 (int i) const
{
    assert(m_kAttributes.GetBitangentChannels() == 4);
    int iIndex = m_iVertexSize*i + m_kAttributes.GetBitangentOffset();
    return *(Vector4f*)(m_afChannel + iIndex);
}
//----------------------------------------------------------------------------
void VertexBuffer::BuildCompatibleArray (const Attributes& rkIAttr,
    bool bPackARGB, int& riChannels, float*& rafCompatible) const
{
    // The use of "unsigned int" is to allow storage of "float" channels and
    // of the ARGB-formatted colors, which are "unsigned int".  Typecasting
    // "float" pointers to "unsigned int" pointers and then dereferencing
    // them works as expected.  The alternative is to use a vector of "float"
    // and typecast "unsigned int" pointers to "float" pointers.  However,
    // dereferencing to a "float" to allow push_back has problems.  The
    // memory pattern for an "unsigned int" might correspond to an invalid
    // "float".  The floating-point unit actually makes adjustments to these
    // values, changing what it is you started with.
    std::vector<unsigned int> kCompatible;    
    const unsigned int* puiData;
    float fOne = 1.0f;
    unsigned int* puiOne = (unsigned int*)&fOne;
    int iUnit, iIChannels, iVBChannels;

    for (int i = 0, j; i < m_iVertexQuantity; i++)
    {
        if (rkIAttr.HasPosition())
        {
            iIChannels = rkIAttr.GetPositionChannels();
            iVBChannels = m_kAttributes.GetPositionChannels();
            puiData = (unsigned int*)PositionTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    // Fill with 1 so that the w-component is compatible with
                    // a homogeneous point.
                    kCompatible.push_back(*puiOne);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }

        if (rkIAttr.HasBlendWeight())
        {
            iIChannels = rkIAttr.GetBlendWeightChannels();
            iVBChannels = m_kAttributes.GetBlendWeightChannels();
            puiData = (unsigned int*)BlendWeightTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    kCompatible.push_back(0);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }

        if (rkIAttr.HasNormal())
        {
            iIChannels = rkIAttr.GetNormalChannels();
            iVBChannels = m_kAttributes.GetNormalChannels();
            puiData = (unsigned int*)NormalTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    // Fill with 0 so that the w-component is compatible with
                    // a homogeneous vector.
                    kCompatible.push_back(0);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }

        for (iUnit = 0; iUnit < (int)rkIAttr.GetMaxColors(); iUnit++)
        {
            if (rkIAttr.HasColor(iUnit))
            {
                unsigned int auiColor[4], uiPackColor, uiValue;
                float fValue;

                iIChannels = rkIAttr.GetColorChannels(iUnit);
                iVBChannels = m_kAttributes.GetColorChannels(iUnit);
                puiData = (unsigned int*)ColorTuple(iUnit,i);
                if (iVBChannels < iIChannels)
                {
                    for (j = 0; j < iVBChannels; j++)
                    {
                        kCompatible.push_back(*puiData++);
                    }
                    for (j = iVBChannels; j < iIChannels; j++)
                    {
                        // Fill with 1 so that the a-component is compatible
                        // with an opaque color.
                        kCompatible.push_back(*puiOne);
                    }
                    if (bPackARGB)
                    {
                        for (j = iIChannels; j < 4; j++)
                        {
                            // Fill with 1 so that the a-component is
                            // compatible with an opaque color.
                            kCompatible.push_back(*puiOne);
                        }

                        // Map from [0,1] to [0,255].
                        for (j = 3; j >= 0; j--)
                        {
                            uiValue = kCompatible.back();
                            fValue = *(float*)&uiValue;
                            auiColor[j] = (unsigned int)(255.0f*fValue);
                            kCompatible.pop_back();
                        }

                        uiPackColor =
                            (auiColor[2]      ) |  // blue
                            (auiColor[1] <<  8) |  // green
                            (auiColor[0] << 16) |  // red
                            (auiColor[3] << 24);   // alpha

                        kCompatible.push_back(uiPackColor);
                    }
                }
                else
                {
                    for (j = 0; j < iIChannels; j++)
                    {
                        kCompatible.push_back(*puiData++);
                    }
                    if (bPackARGB)
                    {
                        for (j = iIChannels; j < 4; j++)
                        {
                            // Fill with 1 so that the a-component is
                            // compatible with an opaque color.
                            kCompatible.push_back(*puiOne);
                        }

                        // Map from [0,1] to [0,255].
                        for (j = 3; j >= 0; j--)
                        {
                            uiValue = kCompatible.back();
                            fValue = *(float*)&uiValue;
                            auiColor[j] = (unsigned int)(255.0f*fValue);
                            kCompatible.pop_back();
                        }

                        uiPackColor =
                            (auiColor[2]      ) |  // blue
                            (auiColor[1] <<  8) |  // green
                            (auiColor[0] << 16) |  // red
                            (auiColor[3] << 24);   // alpha

                        kCompatible.push_back(uiPackColor);
                    }
                }
            }
        }

        if (rkIAttr.HasFog())
        {
            iIChannels = rkIAttr.GetFogChannels();
            iVBChannels = m_kAttributes.GetFogChannels();
            puiData = (unsigned int*)FogTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    kCompatible.push_back(0);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }

        if (rkIAttr.HasPSize())
        {
            iIChannels = rkIAttr.GetPSizeChannels();
            iVBChannels = m_kAttributes.GetPSizeChannels();
            puiData = (unsigned int*)PSizeTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    kCompatible.push_back(0);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }

        if (rkIAttr.HasBlendIndices())
        {
            iIChannels = rkIAttr.GetBlendIndicesChannels();
            iVBChannels = m_kAttributes.GetBlendIndicesChannels();
            puiData = (unsigned int*)BlendIndicesTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    kCompatible.push_back(0);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }

        for (iUnit = 0; iUnit < (int)rkIAttr.GetMaxTCoords(); iUnit++)
        {
            if (rkIAttr.HasTCoord(iUnit))
            {
                iIChannels = rkIAttr.GetTCoordChannels(iUnit);
                iVBChannels = m_kAttributes.GetTCoordChannels(iUnit);
                puiData = (unsigned int*)TCoordTuple(iUnit,i);
                if (iVBChannels < iIChannels)
                {
                    for (j = 0; j < iVBChannels; j++)
                    {
                        kCompatible.push_back(*puiData++);
                    }
                    for (j = iVBChannels; j < iIChannels; j++)
                    {
                        // Fill with 0 so that the components are compatible
                        // with a higher-dimensional image embedded in a
                        // lower-dimensional one.
                        kCompatible.push_back(0);
                    }
                }
                else
                {
                    for (j = 0; j < iIChannels; j++)
                    {
                        kCompatible.push_back(*puiData++);
                    }
                }
            }
        }

        if (rkIAttr.HasTangent())
        {
            iIChannels = rkIAttr.GetTangentChannels();
            iVBChannels = m_kAttributes.GetTangentChannels();
            puiData = (unsigned int*)TangentTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    // Fill with 0 so that the w-component is compatible with
                    // a homogeneous vector.
                    kCompatible.push_back(0);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }

        if (rkIAttr.HasBitangent())
        {
            iIChannels = rkIAttr.GetBitangentChannels();
            iVBChannels = m_kAttributes.GetBitangentChannels();
            puiData = (unsigned int*)BitangentTuple(i);
            if (iVBChannels < iIChannels)
            {
                for (j = 0; j < iVBChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
                for (j = iVBChannels; j < iIChannels; j++)
                {
                    // Fill with 0 so that the w-component is compatible with
                    // a homogeneous vector.
                    kCompatible.push_back(0);
                }
            }
            else
            {
                for (j = 0; j < iIChannels; j++)
                {
                    kCompatible.push_back(*puiData++);
                }
            }
        }
    }

    riChannels = (int)kCompatible.size();
    if (!rafCompatible)
    {
        rafCompatible = WM4_NEW float[riChannels];
    }
    size_t uiSize = riChannels*sizeof(float);
    System::Memcpy(rafCompatible,uiSize,&kCompatible.front(),uiSize);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void VertexBuffer::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    rkStream.Read(m_iVertexSize);
    rkStream.Read(m_iVertexQuantity);
    rkStream.Read(m_iChannelQuantity);
    m_afChannel = WM4_NEW float[m_iChannelQuantity];
    rkStream.Read(m_iChannelQuantity,m_afChannel);

    if (rkStream.GetVersion() <= StreamVersion(4,3))
    {
        int iPositionChannels;
        rkStream.Read(iPositionChannels);
        m_kAttributes.SetPositionChannels(iPositionChannels);

        int iNormalChannels;
        rkStream.Read(iNormalChannels);
        m_kAttributes.SetNormalChannels(iNormalChannels);

        int iMaxColors;
        rkStream.Read(iMaxColors);
        int i;
        for (i = 0; i < iMaxColors; i++)
        {
            int iColorChannels;
            rkStream.Read(iColorChannels);
            m_kAttributes.SetColorChannels(i,iColorChannels);
        }

        int iMaxTCoords;
        rkStream.Read(iMaxTCoords);
        for (i = 0; i < iMaxTCoords; i++)
        {
            int iTCoordChannels;
            rkStream.Read(iTCoordChannels);
            m_kAttributes.SetTCoordChannels(i,iTCoordChannels);
        }
    }
    else
    {
        int iPositionChannels;
        rkStream.Read(iPositionChannels);
        m_kAttributes.SetPositionChannels(iPositionChannels);

        int iBlendWeightChannels;
        rkStream.Read(iBlendWeightChannels);
        m_kAttributes.SetBlendWeightChannels(iBlendWeightChannels);

        int iNormalChannels;
        rkStream.Read(iNormalChannels);
        m_kAttributes.SetNormalChannels(iNormalChannels);

        int iMaxColors;
        rkStream.Read(iMaxColors);
        int i;
        for (i = 0; i < iMaxColors; i++)
        {
            int iColorChannels;
            rkStream.Read(iColorChannels);
            m_kAttributes.SetColorChannels(i,iColorChannels);
        }

        int iFogChannels;
        rkStream.Read(iFogChannels);
        m_kAttributes.SetFogChannels(iFogChannels);

        int iPSizeChannels;
        rkStream.Read(iPSizeChannels);
        m_kAttributes.SetPSizeChannels(iPSizeChannels);

        int iBlendIndicesChannels;
        rkStream.Read(iBlendIndicesChannels);
        m_kAttributes.SetBlendIndicesChannels(iBlendIndicesChannels);

        int iMaxTCoords;
        rkStream.Read(iMaxTCoords);
        for (i = 0; i < iMaxTCoords; i++)
        {
            int iTCoordChannels;
            rkStream.Read(iTCoordChannels);
            m_kAttributes.SetTCoordChannels(i,iTCoordChannels);
        }

        int iTangentChannels;
        rkStream.Read(iTangentChannels);
        m_kAttributes.SetTangentChannels(iTangentChannels);

        int iBitangentChannels;
        rkStream.Read(iBitangentChannels);
        m_kAttributes.SetBitangentChannels(iBitangentChannels);
    }

    WM4_END_DEBUG_STREAM_LOAD(VertexBuffer);
}
//----------------------------------------------------------------------------
void VertexBuffer::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool VertexBuffer::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void VertexBuffer::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);
    rkStream.Write(m_iVertexSize);
    rkStream.Write(m_iVertexQuantity);
    rkStream.Write(m_iChannelQuantity);
    rkStream.Write(m_iChannelQuantity,m_afChannel);

    rkStream.Write(m_kAttributes.GetPositionChannels());
    rkStream.Write(m_kAttributes.GetBlendWeightChannels());
    rkStream.Write(m_kAttributes.GetNormalChannels());

    rkStream.Write(m_kAttributes.GetMaxColors());
    int i;
    for (i = 0; i < m_kAttributes.GetMaxColors(); i++)
    {
        rkStream.Write(m_kAttributes.GetColorChannels(i));
    }
    rkStream.Write(m_kAttributes.GetFogChannels());
    rkStream.Write(m_kAttributes.GetPSizeChannels());
    rkStream.Write(m_kAttributes.GetBlendIndicesChannels());

    rkStream.Write(m_kAttributes.GetMaxTCoords());
    for (i = 0; i < m_kAttributes.GetMaxTCoords(); i++)
    {
        rkStream.Write(m_kAttributes.GetTCoordChannels(i));
    }

    rkStream.Write(m_kAttributes.GetTangentChannels());
    rkStream.Write(m_kAttributes.GetBitangentChannels());

    WM4_END_DEBUG_STREAM_SAVE(VertexBuffer);
}
//----------------------------------------------------------------------------
int VertexBuffer::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Object::GetDiskUsed(rkVersion) +
        sizeof(m_iVertexSize) +
        sizeof(m_iVertexQuantity) +
        sizeof(m_iChannelQuantity) +
        m_iChannelQuantity*sizeof(m_afChannel[0]);

    if (rkVersion <= StreamVersion(4,3))
    {
        // numPositionChannels + numNormalChannels + numMaxColors +
        // numMaxTCoords
        iSize += 4*sizeof(int);
    }
    else
    {
        // numPositionChannels + numBlendWeightChannels + numNormalChannels +
        // numMaxColors + numFogChannels + numPSizeChannels +
        // numBlendIndicesChannels + numMaxTCoords + numTangentChannels +
        // numBitangentChannels
        iSize += 10*sizeof(int);
    }

    iSize += sizeof(int)*m_kAttributes.GetMaxColors();
    iSize += sizeof(int)*m_kAttributes.GetMaxTCoords();

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* VertexBuffer::SaveStrings (const char* acTitle)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("vertex quantity = ",m_iVertexQuantity));
    pkTree->Append(Format("vertex size = ",m_iVertexSize));

    pkTree->Append(Format("position channels =",
        m_kAttributes.GetPositionChannels()));
    pkTree->Append(Format("blendweight channels =",
        m_kAttributes.GetBlendWeightChannels()));
    pkTree->Append(Format("normal channels =",
        m_kAttributes.GetNormalChannels()));

    pkTree->Append(Format("color units =",m_kAttributes.GetMaxColors()));
    const size_t uiSubtitleSize = 64;
    char acSubtitle[uiSubtitleSize];
    int i;
    for (i = 0; i < m_kAttributes.GetMaxColors(); i++)
    {
        System::Sprintf(acSubtitle,uiSubtitleSize,"color[%d] channels =",i);
        pkTree->Append(Format(acSubtitle,m_kAttributes.GetColorChannels(i)));
    }

    pkTree->Append(Format("fog channels =",
        m_kAttributes.GetFogChannels()));

    pkTree->Append(Format("psize channels =",
        m_kAttributes.GetPSizeChannels()));

    pkTree->Append(Format("blendindices channels =",
        m_kAttributes.GetBlendIndicesChannels()));

    pkTree->Append(Format("tcoord units =",m_kAttributes.GetMaxTCoords()));
    for (i = 0; i < m_kAttributes.GetMaxTCoords(); i++)
    {
        System::Sprintf(acSubtitle,uiSubtitleSize,"tcoord[%d] channels =",i);
        pkTree->Append(Format(acSubtitle,m_kAttributes.GetTCoordChannels(i)));
    }

    pkTree->Append(Format("tangent channels =",
        m_kAttributes.GetTangentChannels()));

    pkTree->Append(Format("bitangent channels =",
        m_kAttributes.GetBitangentChannels()));

    // children
    pkTree->Append(Object::SaveStrings());
    pkTree->Append(Format(acTitle,m_iChannelQuantity,m_afChannel));

    return pkTree;
}
//----------------------------------------------------------------------------
