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
// Version: 4.0.2 (2007/08/11)

#include "RipplingOceanEffect.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,RipplingOceanEffect,ShaderEffect);
WM4_IMPLEMENT_STREAM(RipplingOceanEffect);
WM4_IMPLEMENT_DEFAULT_NAME_ID(RipplingOceanEffect,ShaderEffect);

//----------------------------------------------------------------------------
RipplingOceanEffect::RipplingOceanEffect (const char* acBumpName,
    const char* acWaterName, const char* acEnvName)
    :
    ShaderEffect(1)
{
    m_kVShader[0] = WM4_NEW VertexShader("RipplingOcean");
    m_kPShader[0] = WM4_NEW PixelShader("RipplingOcean");

    m_kPShader[0]->SetTexture(0,acBumpName);
    m_kPShader[0]->SetTexture(1,acWaterName);
    m_kPShader[0]->SetTexture(2,acEnvName);

    Texture* pkBump = m_kPShader[0]->GetTexture(0);
    pkBump->SetFilterType(Texture::LINEAR);
    pkBump->SetWrapType(0,Texture::REPEAT);
    pkBump->SetWrapType(1,Texture::REPEAT);

    Texture* pkWater = m_kPShader[0]->GetTexture(1);
    pkWater->SetFilterType(Texture::LINEAR);
    pkWater->SetWrapType(0,Texture::CLAMP);
    pkWater->SetWrapType(1,Texture::CLAMP);

    Texture* pkEnv = m_kPShader[0]->GetTexture(2);
    pkEnv->SetFilterType(Texture::LINEAR);

    VertexProgram* pkVProgram = m_kVShader[0]->GetProgram();
    pkVProgram->GetUserConstant("LightDir")->SetDataSource(m_afLightDir);
    pkVProgram->GetUserConstant("WaveDirX")->SetDataSource(m_afWaveDirX);
    pkVProgram->GetUserConstant("WaveDirY")->SetDataSource(m_afWaveDirY);
    pkVProgram->GetUserConstant("WaveSpeed")->SetDataSource(m_afWaveSpeed);
    pkVProgram->GetUserConstant("WaveOffset")->SetDataSource(m_afWaveOffset);
    pkVProgram->GetUserConstant("WaveHeight")->SetDataSource(m_afWaveHeight);
    pkVProgram->GetUserConstant("BumpSpeed")->SetDataSource(m_afBumpSpeed);
    pkVProgram->GetUserConstant("Constants")->SetDataSource(m_afConstants);
}
//----------------------------------------------------------------------------
RipplingOceanEffect::RipplingOceanEffect ()
{
}
//----------------------------------------------------------------------------
RipplingOceanEffect::~RipplingOceanEffect ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void RipplingOceanEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    ShaderEffect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(3,m_afLightDir);
    rkStream.Read(4,m_afWaveDirX);
    rkStream.Read(4,m_afWaveDirY);
    rkStream.Read(4,m_afWaveSpeed);
    rkStream.Read(4,m_afWaveOffset);
    rkStream.Read(4,m_afWaveHeight);
    rkStream.Read(4,m_afBumpSpeed);
    rkStream.Read(4,m_afConstants);

    WM4_END_DEBUG_STREAM_LOAD(RipplingOceanEffect);
}
//----------------------------------------------------------------------------
void RipplingOceanEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ShaderEffect::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool RipplingOceanEffect::Register (Stream& rkStream) const
{
    return ShaderEffect::Register(rkStream);
}
//----------------------------------------------------------------------------
void RipplingOceanEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    ShaderEffect::Save(rkStream);

    // native data
    rkStream.Write(3,m_afLightDir);
    rkStream.Write(4,m_afWaveDirX);
    rkStream.Write(4,m_afWaveDirY);
    rkStream.Write(4,m_afWaveSpeed);
    rkStream.Write(4,m_afWaveOffset);
    rkStream.Write(4,m_afWaveHeight);
    rkStream.Write(4,m_afBumpSpeed);
    rkStream.Write(4,m_afConstants);

    WM4_END_DEBUG_STREAM_SAVE(RipplingOceanEffect);
}
//----------------------------------------------------------------------------
int RipplingOceanEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return ShaderEffect::GetDiskUsed(rkVersion) +
        3*sizeof(m_afLightDir[0]) +
        4*sizeof(m_afWaveDirX[0]) +
        4*sizeof(m_afWaveDirY[0]) +
        4*sizeof(m_afWaveSpeed[0]) +
        4*sizeof(m_afWaveOffset[0]) +
        4*sizeof(m_afWaveHeight[0]) +
        4*sizeof(m_afBumpSpeed[0]) +
        4*sizeof(m_afConstants[0]);
}
//----------------------------------------------------------------------------
StringTree* RipplingOceanEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    Vector3f kDirection(m_afLightDir[0],m_afLightDir[1],m_afLightDir[2]);
    pkTree->Append(Format("light direction =",kDirection));
    pkTree->Append(Format("wave0 x-direction =",m_afWaveDirX[0]));
    pkTree->Append(Format("wave1 x-direction =",m_afWaveDirX[1]));
    pkTree->Append(Format("wave2 x-direction =",m_afWaveDirX[2]));
    pkTree->Append(Format("wave3 x-direction =",m_afWaveDirX[3]));
    pkTree->Append(Format("wave0 y-direction =",m_afWaveDirY[0]));
    pkTree->Append(Format("wave1 y-direction =",m_afWaveDirY[1]));
    pkTree->Append(Format("wave2 y-direction =",m_afWaveDirY[2]));
    pkTree->Append(Format("wave3 y-direction =",m_afWaveDirY[3]));
    pkTree->Append(Format("wave0 speed =",m_afWaveSpeed[0]));
    pkTree->Append(Format("wave1 speed =",m_afWaveSpeed[1]));
    pkTree->Append(Format("wave2 speed =",m_afWaveSpeed[2]));
    pkTree->Append(Format("wave3 speed =",m_afWaveSpeed[3]));
    pkTree->Append(Format("wave0 offset =",m_afWaveOffset[0]));
    pkTree->Append(Format("wave1 offset =",m_afWaveOffset[1]));
    pkTree->Append(Format("wave2 offset =",m_afWaveOffset[2]));
    pkTree->Append(Format("wave3 offset =",m_afWaveOffset[3]));
    pkTree->Append(Format("wave0 bump speed =",m_afBumpSpeed[0]));
    pkTree->Append(Format("wave1 bump speed =",m_afBumpSpeed[1]));
    pkTree->Append(Format("wave2 bump speed =",m_afBumpSpeed[2]));
    pkTree->Append(Format("wave3 bump speed =",m_afBumpSpeed[3]));
    pkTree->Append(Format("average DuDxDvDy =",m_afConstants[0]));
    pkTree->Append(Format("ambient =",m_afConstants[1]));
    pkTree->Append(Format("texture repeat =",m_afConstants[2]));
    pkTree->Append(Format("time =",m_afConstants[3]));

    // children
    pkTree->Append(ShaderEffect::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
