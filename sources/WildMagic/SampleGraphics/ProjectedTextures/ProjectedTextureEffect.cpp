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
// Version: 4.0.2 (2007/09/24)

#include "ProjectedTextureEffect.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,ProjectedTextureEffect,ShaderEffect);
WM4_IMPLEMENT_STREAM(ProjectedTextureEffect);

//----------------------------------------------------------------------------
ProjectedTextureEffect::ProjectedTextureEffect (Camera* pkProjector,
    const char* acProjectorImage, Light* pkLight)
    :
    ShaderEffect(1),
    m_spkProjector(pkProjector),
    m_spkLight(pkLight)
{
    m_kVShader[0] = WM4_NEW VertexShader("ProjectedTexture");
    m_kPShader[0] = WM4_NEW PixelShader("ProjectedTexture");

    m_kPShader[0]->SetTexture(0,acProjectorImage);
}
//----------------------------------------------------------------------------
ProjectedTextureEffect::ProjectedTextureEffect ()
{
}
//----------------------------------------------------------------------------
ProjectedTextureEffect::~ProjectedTextureEffect ()
{
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::SetGlobalState (int iPass, Renderer* pkRenderer,
    bool bPrimaryEffect)
{
    ShaderEffect::SetGlobalState(iPass,pkRenderer,bPrimaryEffect);

    pkRenderer->SetProjector(m_spkProjector);
    pkRenderer->SetLight(0,m_spkLight);
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::RestoreGlobalState (int iPass,
    Renderer* pkRenderer, bool bPrimaryEffect)
{
    ShaderEffect::RestoreGlobalState(iPass,pkRenderer,bPrimaryEffect);

    pkRenderer->SetProjector(0);
    pkRenderer->SetLight(0,0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* ProjectedTextureEffect::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = ShaderEffect::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkProjector)
    {
        pkFound = m_spkProjector->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_spkLight)
    {
        pkFound = m_spkLight->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    ShaderEffect::GetAllObjectsByName(rkName,rkObjects);

    if (m_spkProjector)
    {
        m_spkProjector->GetAllObjectsByName(rkName,rkObjects);
    }

    if (m_spkLight)
    {
        m_spkLight->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* ProjectedTextureEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = ShaderEffect::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkProjector)
    {
        pkFound = m_spkProjector->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_spkLight)
    {
        pkFound = m_spkLight->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ProjectedTextureEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    ShaderEffect::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkProjector
    pkLink->Add(pkObject);
    rkStream.Read(pkObject);  // m_spkLight
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(ProjectedTextureEffect);
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ShaderEffect::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkProjector = (Camera*)rkStream.GetFromMap(pkLinkID);
    pkLinkID = pkLink->GetLinkID();
    m_spkLight = (Light*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool ProjectedTextureEffect::Register (Stream& rkStream) const
{
    if (!ShaderEffect::Register(rkStream))
    {
        return false;
    }

    if (m_spkProjector)
    {
        m_spkProjector->Register(rkStream);
    }

    if (m_spkLight)
    {
        m_spkLight->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void ProjectedTextureEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    ShaderEffect::Save(rkStream);

    // link data
    rkStream.Write(m_spkProjector);
    rkStream.Write(m_spkLight);

    WM4_END_DEBUG_STREAM_SAVE(ProjectedTextureEffect);
}
//----------------------------------------------------------------------------
int ProjectedTextureEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return ShaderEffect::GetDiskUsed(rkVersion) +
        WM4_PTRSIZE(m_spkProjector) +
        WM4_PTRSIZE(m_spkLight);
}
//----------------------------------------------------------------------------
StringTree* ProjectedTextureEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    // children
    pkTree->Append(ShaderEffect::SaveStrings());

    if (m_spkProjector)
    {
        pkTree->Append(m_spkProjector->SaveStrings());
    }

    if (m_spkLight)
    {
        pkTree->Append(m_spkLight->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
