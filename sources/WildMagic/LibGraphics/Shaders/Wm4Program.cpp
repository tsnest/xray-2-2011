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
// Version: 4.0.7 (2009/02/27)

#include "Wm4GraphicsPCH.h"
#include "Wm4Program.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Program,Object);
WM4_IMPLEMENT_STREAM(Program);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Program,Object);

std::string Program::RendererType("");
char Program::CommentChar = 0;

const std::string Program::ms_kFloatStr("float");
const std::string Program::ms_kFloat1Str("float1");
const std::string Program::ms_kFloat1x1Str("float1x1");
const std::string Program::ms_kFloat1x2Str("float1x2");
const std::string Program::ms_kFloat1x3Str("float1x3");
const std::string Program::ms_kFloat1x4Str("float1x4");
const std::string Program::ms_kFloat2Str("float2");
const std::string Program::ms_kFloat2x1Str("float2x1");
const std::string Program::ms_kFloat2x2Str("float2x2");
const std::string Program::ms_kFloat2x3Str("float2x3");
const std::string Program::ms_kFloat2x4Str("float2x4");
const std::string Program::ms_kFloat3Str("float3");
const std::string Program::ms_kFloat3x1Str("float3x1");
const std::string Program::ms_kFloat3x2Str("float3x2");
const std::string Program::ms_kFloat3x3Str("float3x3");
const std::string Program::ms_kFloat3x4Str("float3x4");
const std::string Program::ms_kFloat4Str("float4");
const std::string Program::ms_kFloat4x1Str("float4x1");
const std::string Program::ms_kFloat4x2Str("float4x2");
const std::string Program::ms_kFloat4x3Str("float4x3");
const std::string Program::ms_kFloat4x4Str("float4x4");
const std::string Program::ms_kSampler1DStr("sampler1D");
const std::string Program::ms_kSampler2DStr("sampler2D");
const std::string Program::ms_kSampler3DStr("sampler3D");
const std::string Program::ms_kSamplerCubeStr("samplerCUBE");
const std::string Program::ms_kSamplerProjStr("sampler2DSHADOW");
const std::string Program::ms_kPositionStr("POSITION");
const std::string Program::ms_kBlendWeightStr("BLENDWEIGHT");
const std::string Program::ms_kNormalStr("NORMAL");
const std::string Program::ms_kColorStr("COLOR");
const std::string Program::ms_kColor0Str("COLOR0");
const std::string Program::ms_kColor1Str("COLOR1");
const std::string Program::ms_kFogCoordStr("FOGCOORD");
const std::string Program::ms_kFogStr("FOG");
const std::string Program::ms_kPSizeStr("PSIZE");
const std::string Program::ms_kBlendIndicesStr("BLENDINDICES");
const std::string Program::ms_kTexCoordStr("TEXCOORD");
const std::string Program::ms_kTangentStr("TANGENT");
const std::string Program::ms_kBinormalStr("BINORMAL");
const std::string Program::ms_kWPosStr("WPOS");
const std::string Program::ms_kInStr("in");
const std::string Program::ms_kEOL("\n");

//----------------------------------------------------------------------------
bool Program::Load (const std::string& rkFilename, Program* pkProgram)
{
    const char* acDecorated = System::GetPath(rkFilename.c_str(),
        System::SM_READ);
    if (!acDecorated)
    {
        return false;
    }

    std::ifstream kIStr(acDecorated);
    if (!kIStr)
    {
        return false;
    }

    std::string& rkProgramText = pkProgram->m_kProgramText;
    Attributes& rkIAttr = pkProgram->m_kInputAttributes;
    Attributes& rkOAttr = pkProgram->m_kOutputAttributes;
    std::vector<RendererConstant>& rkRCs = pkProgram->m_kRendererConstants;
    std::vector<NumericalConstant>& rkNCs = pkProgram->m_kNumericalConstants;
    std::vector<UserConstant>& rkUCs = pkProgram->m_kUserConstants;
    std::vector<SamplerInformation>& rkSIs = pkProgram->m_kSamplerInformation;

    std::string kLine, kToken, kVarType, kVarName, kVarIO, kVarSemantic;
    std::string kRegister, kData, kUnit;
    std::string::size_type uiBegin, uiEnd, uiSave;
    int iNumFloats, iUnit, iBaseRegister, iRegisterQuantity;
    SamplerInformation::Type eSType;
    RendererConstant::Type eRCType;

    while (!kIStr.eof())
    {
#if defined(WM4_USING_VC6) || defined(WM4_USING_VC70)
        const int iBufferSize = 1024;
        char acBuffer[iBufferSize];
        kIStr.getline(acBuffer,iBufferSize);
        kLine = std::string(acBuffer);
#else
        getline(kIStr,kLine);

        // Depending on the platform that *.wmsp files are compiled on, the
        // newline may consist of a line feed '\n' (10), a carriage return
        // '\r' (13), or a carriage return followed by a line feed.
        if (kLine.length() > 0)
        {
            if (10 == (int)kLine[kLine.length()-1])
            {
                if (kLine.length() > 1 && 13 == (int)kLine[kLine.length()-2])
                {
                    kLine.resize(kLine.length()-2);
                }
                else
                {
                    kLine.resize(kLine.length()-1);
                }
            }
            else if (13 == (int)kLine[kLine.length()-1])
            {
                kLine.resize(kLine.length()-1);
            }
        }
#endif

        // The information needed by Program is contained in the shader
        // program comment lines.  All other lines are assumed to be needed
        // by the graphics API.
        if (kLine[0] != CommentChar)
        {
            rkProgramText += kLine + ms_kEOL;
            continue;
        }

        uiBegin = kLine.find("var",1);
        if (uiBegin != std::string::npos)
        {
            // Test the last field of the string.  If this variable is used,
            // the value is 1.  If this variable is not used, the value is 0.
            // When not used, we can just skip the line.
            char cUsed = kLine[kLine.length()-1];
            if (cUsed == '0')
            {
                continue;
            }

            // Skip over "var".
            uiBegin += 3;

            // Skip over white space.
            uiBegin = kLine.find_first_not_of(" ",uiBegin);
            if (uiBegin == std::string::npos)
            {
                assert(false);
                return false;
            }

            // Get the variable's data type.
            iNumFloats = 0;
            iRegisterQuantity = 0;
            eSType = SamplerInformation::MAX_SAMPLER_TYPES;
            uiEnd = kLine.find(" ",uiBegin);
            kVarType = kLine.substr(uiBegin,uiEnd-uiBegin);
            if (kVarType == ms_kFloatStr
            ||  kVarType == ms_kFloat1Str
            ||  kVarType == ms_kFloat1x1Str)
            {
                iNumFloats = 1;
                iRegisterQuantity = 1;
            }
            else if (kVarType == ms_kFloat1x2Str)
            {
                iRegisterQuantity = 1;
                iNumFloats = 2;
            }
            else if (kVarType == ms_kFloat1x3Str)
            {
                iRegisterQuantity = 1;
                iNumFloats = 3;
            }
            else if (kVarType == ms_kFloat1x4Str)
            {
                iRegisterQuantity = 1;
                iNumFloats = 4;
            }
            else if (kVarType == ms_kFloat2Str)
            {
                iNumFloats = 2;
                iRegisterQuantity = 1;
            }
            else if (kVarType == ms_kFloat2x1Str)
            {
                iRegisterQuantity = 2;
                iNumFloats = 2;
            }
            else if (kVarType == ms_kFloat2x2Str)
            {
                iRegisterQuantity = 2;
                iNumFloats = 4;
            }
            else if (kVarType == ms_kFloat2x3Str)
            {
                iRegisterQuantity = 2;
                iNumFloats = 6;
            }
            else if (kVarType == ms_kFloat2x4Str)
            {
                iRegisterQuantity = 2;
                iNumFloats = 8;
            }
            else if (kVarType == ms_kFloat3Str)
            {
                iNumFloats = 3;
                iRegisterQuantity = 1;
            }
            else if (kVarType == ms_kFloat3x1Str)
            {
                iRegisterQuantity = 3;
                iNumFloats = 3;
            }
            else if (kVarType == ms_kFloat3x2Str)
            {
                iRegisterQuantity = 3;
                iNumFloats = 6;
            }
            else if (kVarType == ms_kFloat3x3Str)
            {
                iRegisterQuantity = 3;
                iNumFloats = 9;
            }
            else if (kVarType == ms_kFloat3x4Str)
            {
                iRegisterQuantity = 3;
                iNumFloats = 12;
            }
            else if (kVarType == ms_kFloat4Str)
            {
                iNumFloats = 4;
                iRegisterQuantity = 1;
            }
            else if (kVarType == ms_kFloat4x1Str)
            {
                iRegisterQuantity = 4;
                iNumFloats = 4;
            }
            else if (kVarType == ms_kFloat4x2Str)
            {
                iRegisterQuantity = 4;
                iNumFloats = 8;
            }
            else if (kVarType == ms_kFloat4x3Str)
            {
                iRegisterQuantity = 4;
                iNumFloats = 12;
            }
            else if (kVarType == ms_kFloat4x4Str)
            {
                iRegisterQuantity = 4;
                iNumFloats = 16;
            }
            else if (kVarType == ms_kSampler1DStr)
            {
                eSType = SamplerInformation::SAMPLER_1D;
            }
            else if (kVarType == ms_kSampler2DStr)
            {
                eSType = SamplerInformation::SAMPLER_2D;
            }
            else if (kVarType == ms_kSampler3DStr)
            {
                eSType = SamplerInformation::SAMPLER_3D;
            }
            else if (kVarType == ms_kSamplerCubeStr)
            {
                eSType = SamplerInformation::SAMPLER_CUBE;
            }
            else if (kVarType == ms_kSamplerProjStr)
            {
                eSType = SamplerInformation::SAMPLER_PROJ;
            }
            else
            {
                // The data type is not supported by Wild Magic.
                assert(false);
                return false;
            }

            // Skip over white space.
            uiBegin = kLine.find_first_not_of(" ",uiEnd);
            if (uiBegin == std::string::npos)
            {
                assert(false);
                return false;
            }

            // Get the variable's name.
            uiEnd = kLine.find(" ",uiBegin);
            kVarName = kLine.substr(uiBegin,uiEnd-uiBegin);

            // Skip over white space.
            uiBegin = kLine.find_first_not_of(" ",uiEnd);
            if (uiBegin == std::string::npos)
            {
                assert(false);
                return false;
            }

            // Get sampler information (if relevant).
            if (eSType != SamplerInformation::MAX_SAMPLER_TYPES)
            {
                // Skip over "texunit".
                uiBegin = kLine.find("texunit",uiBegin);
                if (uiBegin == std::string::npos)
                {
                    assert(false);
                    return false;
                }
                uiBegin += 7;
                if (kLine[uiBegin] != ' ')
                {
                    assert(false);
                    return false;
                }

                // Get the texture unit.
                uiBegin = kLine.find_first_of("0123456789",uiBegin);
                uiEnd = kLine.find_first_not_of("0123456789",uiBegin);
                kUnit = kLine.substr(uiBegin,uiEnd-uiBegin);
                iUnit = (int)atoi(kUnit.c_str());
                if (iUnit < 0 || iUnit >= 16)  // Supports 16 samplers.
                {
                    assert(false);
                    return false;
                }

                SamplerInformation kSU(kVarName,eSType,iUnit);
                rkSIs.push_back(kSU);
                continue;
            }

            // Get the variable's I/O status.
            uiSave = uiBegin;
            uiBegin = kLine.find("$",uiSave);
            if (uiBegin != std::string::npos)
            {
                // The variable is either an input or output variable.
                uiBegin += 2;
                uiEnd = kLine.find_first_of(".",uiBegin);
                kVarIO = kLine.substr(uiBegin,uiEnd-uiBegin);

                // Get the variable's semantic.
                uiBegin = uiEnd+1;
                uiEnd = kLine.find_first_of(" ",uiBegin);
                kVarSemantic = kLine.substr(uiBegin,uiEnd-uiBegin);

                if (kVarIO == ms_kInStr)
                {
                    if (kVarSemantic == ms_kPositionStr)
                    {
                        rkIAttr.SetPositionChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kBlendWeightStr)
                    {
                        rkIAttr.SetBlendWeightChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kNormalStr)
                    {
                        rkIAttr.SetNormalChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kColorStr
                    || kVarSemantic == ms_kColor0Str)
                    {
                        rkIAttr.SetColorChannels(0,iNumFloats);
                    }
                    else if (kVarSemantic == ms_kColor1Str)
                    {
                        rkIAttr.SetColorChannels(1,iNumFloats);
                    }
                    else if (kVarSemantic == ms_kFogCoordStr
                    || kVarSemantic == ms_kFogStr)
                    {
                        rkIAttr.SetFogChannels(1);
                    }
                    else if (kVarSemantic == ms_kPSizeStr)
                    {
                        rkIAttr.SetPSizeChannels(1);
                    }
                    else if (kVarSemantic == ms_kBlendIndicesStr)
                    {
                        rkIAttr.SetBlendIndicesChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kTangentStr)
                    {
                        rkIAttr.SetTangentChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kBinormalStr)
                    {
                        rkIAttr.SetBitangentChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kWPosStr)
                    {
                        // Nothing to do.  An input of WPOS in a pixel shader
                        // is not an output of a vertex shader.
                    }
                    else // texture coordinate
                    {
                        if (kVarSemantic.substr(0,8) != ms_kTexCoordStr)
                        {
                            assert(false);
                            return false;
                        }
                        iUnit = (int)kVarSemantic[8] - '0';
                        rkIAttr.SetTCoordChannels(iUnit,iNumFloats);
                    }
                }
                else  // kVarIO == std::string("out")
                {
                    if (kVarSemantic == ms_kPositionStr)
                    {
                        rkOAttr.SetPositionChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kNormalStr)
                    {
                        rkOAttr.SetNormalChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kColorStr
                    || kVarSemantic == ms_kColor0Str)
                    {
                        rkOAttr.SetColorChannels(0,iNumFloats);
                    }
                    else if (kVarSemantic == ms_kColor1Str)
                    {
                        rkOAttr.SetColorChannels(1,iNumFloats);
                    }
                    else if (kVarSemantic == ms_kFogStr)
                    {
                        rkOAttr.SetFogChannels(1);
                    }
                    else if (kVarSemantic == ms_kPSizeStr)
                    {
                        rkOAttr.SetPSizeChannels(1);
                    }
                    else if (kVarSemantic == ms_kTangentStr)
                    {
                        rkOAttr.SetTangentChannels(iNumFloats);
                    }
                    else if (kVarSemantic == ms_kBinormalStr)
                    {
                        rkOAttr.SetBitangentChannels(iNumFloats);
                    }
                    else // texture coordinate
                    {
                        if (kVarSemantic.substr(0,8) != ms_kTexCoordStr)
                        {
                            assert(false);
                            return false;
                        }
                        iUnit = (int)kVarSemantic[8] - '0';
                        rkOAttr.SetTCoordChannels(iUnit,iNumFloats);
                    }
                }
            }
            else
            {
                // The variable is stored in a constant register.
                uiBegin = kLine.find("c[",uiSave);
                if (uiBegin == std::string::npos)
                {
                    assert(false);
                    return false;
                }
                uiEnd = kLine.find_first_of("]",uiBegin);
                uiBegin += 2;
                kRegister = kLine.substr(uiBegin,uiEnd-uiBegin);
                iBaseRegister = atoi(kRegister.c_str());

                // The variable is either a render state or user-defined.
                eRCType = RendererConstant::GetType(kVarName);
                if (eRCType != RendererConstant::MAX_TYPES)
                {
                    // renderer constant
                    RendererConstant kRC(eRCType,iBaseRegister,
                        iRegisterQuantity);
                    rkRCs.push_back(kRC);
                }
                else
                {
                    // user-defined constant
                    UserConstant kUC(kVarName,iBaseRegister,
                        iRegisterQuantity);
                    rkUCs.push_back(kUC);
                }
            }

            continue;
        }

        uiBegin = kLine.find("const",1);
        if (uiBegin != std::string::npos)
        {
            // A numerical constant register has been found.
            uiBegin = kLine.find("c[");
            if (uiBegin == std::string::npos)
            {
                assert(false);
                return false;
            }
            uiEnd = kLine.find_first_of("]",uiBegin);
            uiBegin += 2;
            kRegister = kLine.substr(uiBegin,uiEnd-uiBegin);
            iBaseRegister = atoi(kRegister.c_str());

            // Get the constant's data, which occurs after the equality.
            float afData[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            uiBegin = kLine.find("=");
            if (uiBegin == std::string::npos)
            {
                assert(false);
                return false;
            }
            uiBegin++;
            if (uiBegin == std::string::npos)
            {
                assert(false);
                return false;
            }
            uiBegin = kLine.find_first_not_of(" ",uiBegin);
            if (uiBegin == std::string::npos)
            {
                assert(false);
                return false;
            }

            for (int i = 0; i < 4; i++)
            {
                uiEnd = kLine.find_first_of(" ",uiBegin);
                if (uiEnd == std::string::npos)
                {
                    kData = kLine.substr(uiBegin);
                    afData[i] = (float)atof(kData.c_str());
                    break;
                }

                kData = kLine.substr(uiBegin,uiEnd-uiBegin);
                afData[i] = (float)atof(kData.c_str());
                uiBegin = kLine.find_first_not_of(" ",uiEnd);
                if (uiBegin == std::string::npos)
                {
                    assert(false);
                    return false;
                }
            }

            NumericalConstant kNC(iBaseRegister,afData);
            rkNCs.push_back(kNC);
        }
    }

    kIStr.close();
    return true;
}
//----------------------------------------------------------------------------
Program::Program ()
{
}
//----------------------------------------------------------------------------
Program::~Program ()
{
    Release();
}
//----------------------------------------------------------------------------
RendererConstant* Program::GetRendererConstant (int i)
{
    if (0 <= i && i < (int)m_kRendererConstants.size())
    {
        return &m_kRendererConstants[i];
    }

    assert(false);
    return 0;
}
//----------------------------------------------------------------------------
RendererConstant* Program::GetRendererConstant (RendererConstant::Type eType)
{
    for (int i = 0; i < (int)m_kRendererConstants.size(); i++)
    {
        if (eType == m_kRendererConstants[i].GetType())
        {
            return &m_kRendererConstants[i];
        }
    }

    assert(false);
    return 0;
}
//----------------------------------------------------------------------------
NumericalConstant* Program::GetNumericalConstant (int i)
{
    if (0 <= i && i < (int)m_kNumericalConstants.size())
    {
        return &m_kNumericalConstants[i];
    }

    assert(false);
    return 0;
}
//----------------------------------------------------------------------------
UserConstant* Program::GetUserConstant (int i)
{
    if (0 <= i && i < (int)m_kUserConstants.size())
    {
        return &m_kUserConstants[i];
    }

    assert(false);
    return 0;
}
//----------------------------------------------------------------------------
UserConstant* Program::GetUserConstant (const std::string& rkName)
{
    for (int i = 0; i < (int)m_kUserConstants.size(); i++)
    {
        if (rkName == m_kUserConstants[i].GetName())
        {
            return &m_kUserConstants[i];
        }
    }

    assert(false);
    return 0;
}
//----------------------------------------------------------------------------
SamplerInformation* Program::GetSamplerInformation (int i)
{
    if (0 <= i && i < (int)m_kSamplerInformation.size())
    {
        return &m_kSamplerInformation[i];
    }

    assert(false);
    return 0;
}
//----------------------------------------------------------------------------
SamplerInformation* Program::GetSamplerInformation (const std::string& rkName)
{
    for (int i = 0; i < (int)m_kSamplerInformation.size(); i++)
    {
        if (rkName == m_kSamplerInformation[i].GetName())
        {
            return &m_kSamplerInformation[i];
        }
    }

    assert(false);
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Program::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    Object::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(Program);
}
//----------------------------------------------------------------------------
void Program::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Program::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void Program::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    Object::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(Program);
}
//----------------------------------------------------------------------------
int Program::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* Program::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Object::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
