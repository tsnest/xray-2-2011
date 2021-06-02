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
// Version: 4.0.6 (2008/02/18)

#include "Wm4GraphicsPCH.h"
#include "Wm4ShaderEffect.h"
#include "Wm4Catalog.h"
#include "Wm4Renderer.h"
#include "Wm4Geometry.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,ShaderEffect,Effect);
WM4_IMPLEMENT_STREAM(ShaderEffect);

//----------------------------------------------------------------------------
ShaderEffect::ShaderEffect (int iPassQuantity)
{
    assert(iPassQuantity > 0);

    SetPassQuantity(iPassQuantity);
}
//----------------------------------------------------------------------------
ShaderEffect::ShaderEffect ()
{
    m_iPassQuantity = 0;
}
//----------------------------------------------------------------------------
ShaderEffect::~ShaderEffect ()
{
}
//----------------------------------------------------------------------------
void ShaderEffect::SetPassQuantity (int iPassQuantity)
{
    assert(iPassQuantity > 0);
    m_iPassQuantity = iPassQuantity;

    m_kVShader.resize(m_iPassQuantity);
    m_kPShader.resize(m_iPassQuantity);
    m_kAlphaState.resize(m_iPassQuantity);
    m_kVConnect.resize(m_iPassQuantity);
    m_kPConnect.resize(m_iPassQuantity);
    SetDefaultAlphaState();
}
//----------------------------------------------------------------------------
void ShaderEffect::SetGlobalState (int iPass, Renderer* pkRenderer,
    bool bPrimaryEffect)
{
    assert(0 <= iPass && iPass < m_iPassQuantity && pkRenderer);

    // Enable all passes after the first one to blend with the previous
    // passes.
    if (!bPrimaryEffect || iPass > 0)
    {
        m_kAlphaState[iPass]->BlendEnabled = true;

        AlphaStatePtr spkSave = pkRenderer->GetAlphaState();
        pkRenderer->SetAlphaState(m_kAlphaState[iPass]);
        m_kAlphaState[iPass] = spkSave;
    }
}
//----------------------------------------------------------------------------
void ShaderEffect::RestoreGlobalState (int iPass, Renderer* pkRenderer,
    bool bPrimaryEffect)
{
    assert(0 <= iPass && iPass < m_iPassQuantity && pkRenderer);

    if (!bPrimaryEffect || iPass > 0)
    {
        AlphaStatePtr spkSave = pkRenderer->GetAlphaState();
        pkRenderer->SetAlphaState(m_kAlphaState[iPass]);
        m_kAlphaState[iPass] = spkSave;
    }
}
//----------------------------------------------------------------------------
void ShaderEffect::ConnectVShaderConstant (int iPass,
    const std::string& rkName, float* pfSource)
{
    UserConstant* pkUC = GetVConstant(iPass, rkName);
    m_kVConnect[iPass].push_back(std::make_pair(pkUC,pfSource));
}
//----------------------------------------------------------------------------
void ShaderEffect::ConnectPShaderConstant (int iPass,
    const std::string& rkName, float* pfSource)
{
    UserConstant* pkUC = GetPConstant(iPass, rkName);
    m_kPConnect[iPass].push_back(std::make_pair(pkUC,pfSource));
}
//----------------------------------------------------------------------------
void ShaderEffect::ConnectVShaderConstants (int iPass)
{
    if (0 <= iPass && iPass < (int)m_kVConnect.size())
    {
        for (int i = 0; i < (int)m_kVConnect[iPass].size(); i++)
        {
            UserConstant* pkUC = m_kVConnect[iPass][i].first;
            float* pfSource = m_kVConnect[iPass][i].second;
            pkUC->SetDataSource(pfSource);
        }
    }
}
//----------------------------------------------------------------------------
void ShaderEffect::ConnectPShaderConstants (int iPass)
{
    if (0 <= iPass && iPass < (int)m_kPConnect.size())
    {
        for (int i = 0; i < (int)m_kPConnect[iPass].size(); i++)
        {
            UserConstant* pkUC = m_kPConnect[iPass][i].first;
            float* pfSource = m_kPConnect[iPass][i].second;
            pkUC->SetDataSource(pfSource);
        }
    }
}
//----------------------------------------------------------------------------
bool ShaderEffect::SetVShader (int iPass, VertexShader* pkVShader)
{
    assert(0 <= iPass && iPass < m_iPassQuantity);
    m_kVShader[iPass] = pkVShader;

    if (m_kVShader[iPass] && m_kPShader[iPass])
    {
        VertexProgram* pkVProgram = m_kVShader[iPass]->GetProgram();
        PixelProgram* pkPProgram = m_kPShader[iPass]->GetProgram();
        return AreCompatible(pkVProgram,pkPProgram);
    }

    return true;
}
//----------------------------------------------------------------------------
bool ShaderEffect::SetPShader (int iPass, PixelShader* pkPShader)
{
    assert(0 <= iPass && iPass < m_iPassQuantity);
    m_kPShader[iPass] = pkPShader;

    if (m_kVShader[iPass] && m_kPShader[iPass])
    {
        VertexProgram* pkVProgram = m_kVShader[iPass]->GetProgram();
        PixelProgram* pkPProgram = m_kPShader[iPass]->GetProgram();
        return AreCompatible(pkVProgram,pkPProgram);
    }

    return true;
}
//----------------------------------------------------------------------------
void ShaderEffect::SetDefaultAlphaState ()
{
    // Create default alpha states. The Renderer enables this on a multieffect
    // drawing operation.  The first pass uses the default alpha state
    // (SBF_SRC_ALPHA, DBF_ONE_MINUS_SRC_ALPHA).  All other passes use
    // modulation and all are enabled.  These may be overridden by your
    // application code by accessing the state via effect->GetBlending(pass).
    m_kAlphaState[0] = WM4_NEW AlphaState;
    m_kAlphaState[0]->BlendEnabled = true;
    for (int i = 1; i < (int)m_kAlphaState.size(); i++)
    {
        m_kAlphaState[i] = WM4_NEW AlphaState;
        m_kAlphaState[i]->BlendEnabled = true;
        m_kAlphaState[i]->SrcBlend = AlphaState::SBF_DST_COLOR;
        m_kAlphaState[i]->DstBlend = AlphaState::DBF_ZERO;
    }
}
//----------------------------------------------------------------------------
bool ShaderEffect::AreCompatible (const VertexProgram* pkVProgram,
    const PixelProgram* pkPProgram) const
{
    if ((pkVProgram && !pkPProgram) || (!pkVProgram && pkPProgram))
    {
        return true;
    }

    // Ensure that the output of the vertex program and the input of the
    // pixel program are compatible.  Each vertex program always has a clip
    // position output.  This is not relevant to the compatibility check.
    const Attributes& rkVOAttr = pkVProgram->GetOutputAttributes();
    const Attributes& rkPIAttr = pkPProgram->GetInputAttributes();
    return rkVOAttr.Matches(rkPIAttr,false,true,true,true,true,true,true,
        true,true,true);
}
//----------------------------------------------------------------------------
void ShaderEffect::LoadResources (Renderer* pkRenderer, Geometry* pkGeometry)
{
    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        // Load the programs into video memory.
        VertexProgram* pkVProgram = 0;
        if (m_kVShader[iPass])
        {
            pkVProgram = m_kVShader[iPass]->GetProgram();
            pkRenderer->LoadVProgram(pkVProgram);
        }

        if (m_kPShader[iPass])
        {
            pkRenderer->LoadPProgram(m_kPShader[iPass]->GetProgram());
        }

        // Load the textures into video memory.
        const int iPTQuantity = GetPTextureQuantity(iPass);
        for (int i = 0; i < iPTQuantity; i++)
        {
            pkRenderer->LoadTexture(m_kPShader[iPass]->GetTexture(i));
        }

        if (pkGeometry && pkVProgram)
        {
            // Load the vertex buffer into video memory.
            const Attributes& rkIAttr = pkVProgram->GetInputAttributes();
            const Attributes& rkOAttr = pkVProgram->GetOutputAttributes();
            pkRenderer->LoadVBuffer(rkIAttr,rkOAttr,pkGeometry->VBuffer);

            // Load the index buffer into video memory.
            pkRenderer->LoadIBuffer(pkGeometry->IBuffer);
        }
    }
}
//----------------------------------------------------------------------------
void ShaderEffect::ReleaseResources (Renderer* pkRenderer,
    Geometry* pkGeometry)
{
    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        // Release the programs from video memory.
        if (m_kVShader[iPass])
        {
            pkRenderer->ReleaseVProgram(m_kVShader[iPass]->GetProgram());
        }

        if (m_kPShader[iPass])
        {
            pkRenderer->ReleasePProgram(m_kPShader[iPass]->GetProgram());
        }

        // Release the textures from video memory.
        const int iPTQuantity = GetPTextureQuantity(iPass);
        for (int i = 0; i < iPTQuantity; i++)
        {
            pkRenderer->ReleaseTexture(m_kPShader[iPass]->GetTexture(i));
        }

        if (pkGeometry)
        {
            // Release the vertex buffer from video memory.
            pkRenderer->ReleaseVBuffer(pkGeometry->VBuffer);

            // Release the index buffer from video memory.
            pkRenderer->ReleaseIBuffer(pkGeometry->IBuffer);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* ShaderEffect::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        if (m_kVShader[iPass])
        {
            pkFound = m_kVShader[iPass]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }

        if (m_kPShader[iPass])
        {
            pkFound = m_kPShader[iPass]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void ShaderEffect::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        if (m_kVShader[iPass])
        {
            m_kVShader[iPass]->GetAllObjectsByName(rkName,rkObjects);
        }

        if (m_kPShader[iPass])
        {
            m_kPShader[iPass]->GetAllObjectsByName(rkName,rkObjects);
        }
    }
}
//----------------------------------------------------------------------------
Object* ShaderEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        if (m_kVShader[iPass])
        {
            pkFound = m_kVShader[iPass]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }

        if (m_kPShader[iPass])
        {
            pkFound = m_kPShader[iPass]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ShaderEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iPassQuantity);
    m_kVShader.resize(m_iPassQuantity);
    m_kPShader.resize(m_iPassQuantity);
    m_kVConnect.resize(m_iPassQuantity);
    m_kPConnect.resize(m_iPassQuantity);

    // link data
    Object* pkObject;
    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        rkStream.Read(pkObject);  // m_kVShader[iPass]
        pkLink->Add(pkObject);
        rkStream.Read(pkObject);  // m_kPShader[iPass]
        pkLink->Add(pkObject);
    }

    int iQuantity;
    rkStream.Read(iQuantity);
    m_kAlphaState.resize(iQuantity);

    for (int i = 0; i < iQuantity; i++)
    {
        rkStream.Read(pkObject);  // m_kAlphaState[i]
        pkLink->Add(pkObject);
    }

    WM4_END_DEBUG_STREAM_LOAD(ShaderEffect);
}
//----------------------------------------------------------------------------
void ShaderEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    Object* pkLinkID;
    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_kVShader[iPass] = (VertexShader*)rkStream.GetFromMap(pkLinkID);
        pkLinkID = pkLink->GetLinkID();
        m_kPShader[iPass] = (PixelShader*)rkStream.GetFromMap(pkLinkID);
    }

    int iQuantity = (int)m_kAlphaState.size();
    for (int i = 0; i < iQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_kAlphaState[i] = (AlphaState*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool ShaderEffect::Register (Stream& rkStream) const
{
    if (!Effect::Register(rkStream))
    {
        return false;
    }

    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        if (m_kVShader[iPass])
        {
            m_kVShader[iPass]->Register(rkStream);
        }

        if (m_kPShader[iPass])
        {
            m_kPShader[iPass]->Register(rkStream);
        }
    }

    for (int i = 0; i < (int)m_kAlphaState.size(); i++)
    {
        if (m_kAlphaState[i])
        {
            m_kAlphaState[i]->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void ShaderEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // native data
    rkStream.Write(m_iPassQuantity);

    // link data
    for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
    {
        rkStream.Write(m_kVShader[iPass]);
        rkStream.Write(m_kPShader[iPass]);
    }

    int iQuantity = (int)m_kAlphaState.size();
    rkStream.Write(iQuantity);
    for (int i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kAlphaState[i]);
    }

    WM4_END_DEBUG_STREAM_SAVE(ShaderEffect);
}
//----------------------------------------------------------------------------
int ShaderEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Effect::GetDiskUsed(rkVersion) +
        sizeof(m_iPassQuantity) +
        m_iPassQuantity*WM4_PTRSIZE(m_kVShader[0]) +
        m_iPassQuantity*WM4_PTRSIZE(m_kPShader[0]) +
        sizeof(int) +
        ((int)m_kAlphaState.size())*WM4_PTRSIZE(m_kAlphaState[0]);
}
//----------------------------------------------------------------------------
StringTree* ShaderEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("pass quantity =",m_iPassQuantity));

    // children
    pkTree->Append(Effect::SaveStrings());

    if (m_iPassQuantity > 0)
    {
        StringTree* pkCTree = WM4_NEW StringTree;
        pkCTree->Append(Format("shaders"));
        for (int iPass = 0; iPass < m_iPassQuantity; iPass++)
        {
            pkCTree->Append(m_kVShader[iPass]->SaveStrings());
            pkCTree->Append(m_kPShader[iPass]->SaveStrings());
        }
        pkTree->Append(pkCTree);
    }

    int iQuantity = (int)m_kAlphaState.size();
    if (iQuantity > 0)
    {
        StringTree* pkCTree = WM4_NEW StringTree;
        pkCTree->Append(Format("blending states"));
        for (int i = 0; i < iQuantity; i++)
        {
            pkCTree->Append(m_kAlphaState[i]->SaveStrings());
        }
        pkTree->Append(pkCTree);
    }

    return pkTree;
}
//----------------------------------------------------------------------------
