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
// Version: 4.0.1 (2007/08/11)

#include "Wm4GraphicsPCH.h"
#include "Wm4LightingEffect.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,LightingEffect,ShaderEffect);
WM4_IMPLEMENT_STREAM(LightingEffect);
WM4_IMPLEMENT_DEFAULT_NAME_ID(LightingEffect,ShaderEffect);

//----------------------------------------------------------------------------
LightingEffect::LightingEffect ()
    :
    ShaderEffect(1)
{
    // If no lights are attached, the Configure() call will turn the effect
    // off.
}
//----------------------------------------------------------------------------
LightingEffect::~LightingEffect ()
{
}
//----------------------------------------------------------------------------
void LightingEffect::AttachLight (Light* pkLight)
{
    assert(pkLight);

    // Check if the light is already in the list.
    for (int i = 0; i < (int)m_kLights.size(); i++)
    {
        if (m_kLights[i] == pkLight)
        {
            // The light already exists, so do nothing.
            return;
        }
    }

    // The light is not in the current list, so add it.
    m_kLights.push_back(pkLight);
}
//----------------------------------------------------------------------------
void LightingEffect::DetachLight (Light* pkLight)
{
    std::vector<LightPtr>::iterator pkIter = m_kLights.begin();
    for (/**/; pkIter != m_kLights.end(); pkIter++)
    {
        if (pkLight == *pkIter)
        {
            m_kLights.erase(pkIter);
            return;
        }
    }
}
//----------------------------------------------------------------------------
void LightingEffect::Configure ()
{
    if (m_kLights.size() == 0)
    {
        SetPassQuantity(1);
        m_kVShader[0] = WM4_NEW VertexShader("Material");
        m_kPShader[0] = WM4_NEW PixelShader("PassThrough4");
        return;
    }

    // Use a bucket sort on the lights to arrange them in the order:
    // ambient, directional, point, spot.
    std::vector<LightPtr> m_akBucket[4];
    int iLQuantity = (int)m_kLights.size();
    int i, iType;
    for (i = 0; i < iLQuantity; i++)
    {
        iType = (int)m_kLights[i]->Type;
        m_akBucket[iType].push_back(m_kLights[i]);
    }

    // For multipass rendering.  The default is to use additive blending, but
    // you can change the blending modes, if so desired, in your application
    // code.
    SetPassQuantity(iLQuantity);
    for (i = 1; i < iLQuantity; i++)
    {
        AlphaState* pkAS = m_kAlphaState[i];
        pkAS->BlendEnabled = true;
        pkAS->SrcBlend = AlphaState::SBF_ONE;
        pkAS->DstBlend = AlphaState::DBF_ONE;
    }

    const char acType[4] = { 'a', 'd', 'p', 's' };
    std::string kShaderName("L1$");
    int iLight = 0;
    for (iType = 0; iType < 4; iType++)
    {
        for (i = 0; i < (int)m_akBucket[iType].size(); i++)
        {
            kShaderName[2] = acType[iType];
            m_kVShader[iLight] = WM4_NEW VertexShader(kShaderName);
            m_kPShader[iLight] = WM4_NEW PixelShader("PassThrough4");
            iLight++;
        }
    }
}
//----------------------------------------------------------------------------
void LightingEffect::SetGlobalState (int iPass, Renderer* pkRenderer,
    bool bPrimaryEffect)
{
    ShaderEffect::SetGlobalState(iPass,pkRenderer,bPrimaryEffect);

    MaterialState* pkMS = pkRenderer->GetMaterialState();
    if (iPass == 0)
    {
        m_kSaveEmissive = pkMS->Emissive;
    }
    else
    {
        pkMS->Emissive = ColorRGB::BLACK;
    }

    pkRenderer->SetLight(0,m_kLights[iPass]);
}
//----------------------------------------------------------------------------
void LightingEffect::RestoreGlobalState (int iPass, Renderer* pkRenderer,
    bool bPrimaryEffect)
{
    ShaderEffect::RestoreGlobalState(iPass,pkRenderer,bPrimaryEffect);

    pkRenderer->SetLight(0,0);

    if (iPass == m_iPassQuantity - 1)
    {
        MaterialState* pkMS = pkRenderer->GetMaterialState();
        pkMS->Emissive = m_kSaveEmissive;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void LightingEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    ShaderEffect::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(LightingEffect);
}
//----------------------------------------------------------------------------
void LightingEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ShaderEffect::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool LightingEffect::Register (Stream& rkStream) const
{
    return ShaderEffect::Register(rkStream);
}
//----------------------------------------------------------------------------
void LightingEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    ShaderEffect::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(LightingEffect);
}
//----------------------------------------------------------------------------
int LightingEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return ShaderEffect::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* LightingEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(ShaderEffect::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
