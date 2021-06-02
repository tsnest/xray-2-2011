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

#include "Wm4GraphicsPCH.h"
#include "Wm4RendererConstant.h"
using namespace Wm4;

WM4_IMPLEMENT_INITIALIZE(RendererConstant);
WM4_IMPLEMENT_TERMINATE(RendererConstant);

std::string RendererConstant::ms_kStringMap[RendererConstant::MAX_TYPES+1] =
{
    "WMatrix",
    "VMatrix",
    "PMatrix",
    "WVMatrix",
    "VPMatrix",
    "WVPMatrix",
    "WMatrixT",
    "VMatrixT",
    "PMatrixT",
    "WVMatrixT",
    "VPMatrixT",
    "WVPMatrixT",
    "WMatrixI",
    "VMatrixI",
    "PMatrixI",
    "WVMatrixI",
    "VPMatrixI",
    "WVPMatrixI",
    "WMatrixIT",
    "VMatrixIT",
    "PMatrixIT",
    "WVMatrixIT",
    "VPMatrixIT",
    "WVPMatrixIT",

    "MaterialEmissive",
    "MaterialAmbient",
    "MaterialDiffuse",
    "MaterialSpecular",

    "CameraModelPosition",
    "CameraModelDirection",
    "CameraModelUp",
    "CameraModelRight",
    "CameraWorldPosition",
    "CameraWorldDirection",
    "CameraWorldUp",
    "CameraWorldRight",

    "ProjectorModelPosition",
    "ProjectorModelDirection",
    "ProjectorModelUp",
    "ProjectorModelRight",
    "ProjectorWorldPosition",
    "ProjectorWorldDirection",
    "ProjectorWorldUp",
    "ProjectorWorldRight",
    "ProjectorMatrix",

    "Light0ModelPosition",
    "Light0ModelDirection",
    "Light0WorldPosition",
    "Light0WorldDirection",
    "Light0Ambient",
    "Light0Diffuse",
    "Light0Specular",
    "Light0SpotCutoff",
    "Light0Attenuation",

    "Light1ModelPosition",
    "Light1ModelDirection",
    "Light1WorldPosition",
    "Light1WorldDirection",
    "Light1Ambient",
    "Light1Diffuse",
    "Light1Specular",
    "Light1SpotCutoff",
    "Light1Attenuation",

    "Light2ModelPosition",
    "Light2ModelDirection",
    "Light2WorldPosition",
    "Light2WorldDirection",
    "Light2Ambient",
    "Light2Diffuse",
    "Light2Specular",
    "Light2SpotCutoff",
    "Light2Attenuation",

    "Light3ModelPosition",
    "Light3ModelDirection",
    "Light3WorldPosition",
    "Light3WorldDirection",
    "Light3Ambient",
    "Light3Diffuse",
    "Light3Specular",
    "Light3SpotCutoff",
    "Light3Attenuation",

    "Light4ModelPosition",
    "Light4ModelDirection",
    "Light4WorldPosition",
    "Light4WorldDirection",
    "Light4Ambient",
    "Light4Diffuse",
    "Light4Specular",
    "Light4SpotCutoff",
    "Light4Attenuation",

    "Light5ModelPosition",
    "Light5ModelDirection",
    "Light5WorldPosition",
    "Light5WorldDirection",
    "Light5Ambient",
    "Light5Diffuse",
    "Light5Specular",
    "Light5SpotCutoff",
    "Light5Attenuation",

    "Light6ModelPosition",
    "Light6ModelDirection",
    "Light6WorldPosition",
    "Light6WorldDirection",
    "Light6Ambient",
    "Light6Diffuse",
    "Light6Specular",
    "Light6SpotCutoff",
    "Light6Attenuation",

    "Light7ModelPosition",
    "Light7ModelDirection",
    "Light7WorldPosition",
    "Light7WorldDirection",
    "Light7Ambient",
    "Light7Diffuse",
    "Light7Specular",
    "Light7SpotCutoff",
    "Light7Attenuation",

    "MAX_TYPES"
};

std::map<std::string,RendererConstant::Type>*
RendererConstant::ms_pkTypeMap = 0;

//----------------------------------------------------------------------------
void RendererConstant::Initialize ()
{
    assert(ms_pkTypeMap == 0);
    ms_pkTypeMap = WM4_NEW std::map<std::string,RendererConstant::Type>;
    for (int i = 0; i < (int)MAX_TYPES; i++)
    {
        ms_pkTypeMap->insert(std::make_pair(ms_kStringMap[i],(Type)i));
    }
}
//----------------------------------------------------------------------------
void RendererConstant::Terminate ()
{
    WM4_DELETE ms_pkTypeMap;
}
//----------------------------------------------------------------------------
RendererConstant::RendererConstant (Type eType, int iBaseRegister,
    int iRegisterQuantity)
{
    assert(iBaseRegister >= 0);
    assert(1 <= iRegisterQuantity && iRegisterQuantity <= 4);

    m_eType = eType;
    m_iBaseRegister = iBaseRegister;
    m_iRegisterQuantity = iRegisterQuantity;

    // To be set at run time when the special effect is applied to an object.
    memset(m_afData,0,16*sizeof(float));
}
//----------------------------------------------------------------------------
RendererConstant::~RendererConstant ()
{
}
//----------------------------------------------------------------------------
const std::string& RendererConstant::GetName (Type eType)
{
    return ms_kStringMap[eType];
}
//----------------------------------------------------------------------------
RendererConstant::Type RendererConstant::GetType (const std::string& rkName)
{
    std::map<std::string,Type>::iterator pkIter = ms_pkTypeMap->find(rkName);
    if (pkIter != ms_pkTypeMap->end())
    {
        return pkIter->second;
    }
    return MAX_TYPES;
}
//----------------------------------------------------------------------------
