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
// Version: 4.0.5 (2008/02/18)

#include "TerrainEffect.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,TerrainEffect,ShaderEffect);
WM4_IMPLEMENT_STREAM(TerrainEffect);
WM4_IMPLEMENT_DEFAULT_NAME_ID(TerrainEffect,ShaderEffect);

//----------------------------------------------------------------------------
TerrainEffect::TerrainEffect (const char* acBaseName,
    const char* acDetailName, const ColorRGB& rkFogColor, float fFogDensity)
    :
    ShaderEffect(1)
{
    m_kVShader[0] = WM4_NEW VertexShader("BaseMulDetailFogExpSqr");
    m_kPShader[0] = WM4_NEW PixelShader("BaseMulDetailFogExpSqr");

    m_kPShader[0]->SetTexture(0,acBaseName);
    m_kPShader[0]->SetTexture(1,acDetailName);

    Texture* pkBase = m_kPShader[0]->GetTexture(0);
    pkBase->SetFilterType(Texture::LINEAR_LINEAR);

    Texture* pkDetail = m_kPShader[0]->GetTexture(1);
    pkDetail->SetFilterType(Texture::LINEAR_LINEAR);

    SetFogColor(rkFogColor);
    SetFogDensity(fFogDensity);

    ConnectVShaderConstant(0,"FogColorDensity",m_afFogColorDensity);
    ConnectPShaderConstant(0,"FogColorDensity",m_afFogColorDensity);
}
//----------------------------------------------------------------------------
TerrainEffect::TerrainEffect ()
{
}
//----------------------------------------------------------------------------
TerrainEffect::~TerrainEffect ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void TerrainEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    ShaderEffect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(4,m_afFogColorDensity);

    WM4_END_DEBUG_STREAM_LOAD(TerrainEffect);
}
//----------------------------------------------------------------------------
void TerrainEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ShaderEffect::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool TerrainEffect::PostLink ()
{
    ConnectVShaderConstant(0,"FogColorDensity",m_afFogColorDensity);
    ConnectPShaderConstant(0,"FogColorDensity",m_afFogColorDensity);
    return GetVConstant(0,0) && GetPConstant(0,0);
}
//----------------------------------------------------------------------------
bool TerrainEffect::Register (Stream& rkStream) const
{
    return ShaderEffect::Register(rkStream);
}
//----------------------------------------------------------------------------
void TerrainEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    ShaderEffect::Save(rkStream);

    // native data
    rkStream.Write(4,m_afFogColorDensity);

    WM4_END_DEBUG_STREAM_SAVE(TerrainEffect);
}
//----------------------------------------------------------------------------
int TerrainEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return ShaderEffect::GetDiskUsed(rkVersion) +
        4*sizeof(m_afFogColorDensity[0]);
}
//----------------------------------------------------------------------------
StringTree* TerrainEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    ColorRGB kColor(m_afFogColorDensity[0],m_afFogColorDensity[1],
        m_afFogColorDensity[2]);
    pkTree->Append(Format("fog color =",kColor));
    pkTree->Append(Format("fog density =",m_afFogColorDensity[3]));

    // children
    pkTree->Append(ShaderEffect::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
