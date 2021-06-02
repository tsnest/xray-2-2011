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
// Version: 4.0.8 (2007/09/24)

#include "Wm4GraphicsPCH.h"
#include "Wm4Shader.h"
#include "Wm4Catalog.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Shader,Object);
WM4_IMPLEMENT_ABSTRACT_STREAM(Shader);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Shader,Object);

//----------------------------------------------------------------------------
Shader::Shader ()
{
}
//----------------------------------------------------------------------------
Shader::Shader (const std::string& rkShaderName)
    :
    m_kShaderName(rkShaderName)
{
}
//----------------------------------------------------------------------------
Shader::~Shader ()
{
}
//----------------------------------------------------------------------------
bool Shader::SetTexture (int i, Texture* pkTexture)
{
    if (pkTexture)
    {
        if (0 <= i && i < (int)m_kTextures.size())
        {
            m_kTextures[i] = pkTexture;
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
bool Shader::SetTexture (int i, const std::string& rkName)
{
    if (0 <= i && i < (int)m_kTextures.size())
    {
        Texture* pkTexture = Catalog<Texture>::GetActive()->Find(rkName);
        if (pkTexture)
        {
            m_kTextures[i] = pkTexture;
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
Texture* Shader::GetTexture (int i)
{
    if (0 <= i && i < (int)m_kTextures.size())
    {
        return m_kTextures[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
const Texture* Shader::GetTexture (int i) const
{
    if (0 <= i && i < (int)m_kTextures.size())
    {
        return m_kTextures[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
Texture* Shader::GetTexture (const std::string& rkName)
{
    for (int i = 0; i < (int)m_kTextures.size(); i++)
    {
        if (m_kTextures[i] && m_kTextures[i]->GetName() == rkName)
        {
            return m_kTextures[i];
        }
    }
    return 0;
}
//----------------------------------------------------------------------------
const Texture* Shader::GetTexture (const std::string& rkName) const
{
    for (int i = 0; i < (int)m_kTextures.size(); i++)
    {
        if (m_kTextures[i] && m_kTextures[i]->GetName() == rkName)
        {
            return m_kTextures[i];
        }
    }
    return 0;
}
//----------------------------------------------------------------------------
void Shader::OnLoadProgram ()
{
    // The data sources must be set for the user constants.  Determine how
    // many float channels are needed for the storage.
    int iUCQuantity = m_spkProgram->GetUserConstantQuantity();
    int i, iChannels;
    UserConstant* pkUC;
    for (i = 0, iChannels = 0; i < iUCQuantity; i++)
    {
        pkUC = m_spkProgram->GetUserConstant(i);
        assert(pkUC);
        iChannels += 4*pkUC->GetRegisterQuantity();
    }
    m_kUserData.resize(iChannels);

    // Set the data sources for the user constants.
    for (i = 0, iChannels = 0; i < iUCQuantity; i++)
    {
        pkUC = m_spkProgram->GetUserConstant(i);
        assert(pkUC);
        pkUC->SetDataSource(&m_kUserData[iChannels]);
        iChannels += 4*pkUC->GetRegisterQuantity();
    }

    // The number of textures is the number of samplers required by the
    // program.
    int iSIQuantity = m_spkProgram->GetSamplerInformationQuantity();
    if (iSIQuantity > 0)
    {
        m_kTextures.resize(iSIQuantity);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Shader::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kShaderName);

    // TO DO.  The image names are removed in streamversion 4.6, but to avoid
    // having to re-export *.wmof files, keep the names around.  Remove them
    // at a later date.
    int iQuantity, i;
    if (rkStream.GetVersion() < StreamVersion(4,6))
    {
        rkStream.Read(iQuantity);
        m_kImageNames.resize(iQuantity);
        for (i = 0; i < iQuantity; i++)
        {
            rkStream.Read(m_kImageNames[i]);
        }
    }

    // link data
    Object* pkObject;
    if (rkStream.GetVersion() >= StreamVersion(4,6))
    {
        rkStream.Read(pkObject);  // m_spkProgram
        pkLink->Add(pkObject);
    }

    rkStream.Read(iQuantity);
    m_kTextures.resize(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Read(pkObject);  // m_kTextures[i]
        pkLink->Add(pkObject);
    }

    // The data member m_kUserData is set during loading at program runtime.

    WM4_END_DEBUG_STREAM_LOAD(Shader);
}
//----------------------------------------------------------------------------
void Shader::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID;
    if (rkStream.GetVersion() >= StreamVersion(4,6))
    {
        pkLinkID = pkLink->GetLinkID();
        m_spkProgram = (Program*)rkStream.GetFromMap(pkLinkID);
    }

    for (int i = 0; i < (int)m_kTextures.size(); i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_kTextures[i] = (Texture*)rkStream.GetFromMap(pkLinkID);

        Image* pkImage;
        if (m_kImageNames.size() > 0)
        {
            // TO DO.  The pre-streamversion-4.6 files have Textures with no
            // names.  Set these names to the image names.  Use the 'assert' in
            // case a WM user has set the Texture name for other reasons.
            assert(m_kTextures[i]->GetName() == ""
                || m_kTextures[i]->GetName() == m_kImageNames[i]);

            m_kTextures[i]->SetName(m_kImageNames[i]);
            pkImage = Catalog<Image>::GetActive()->Find(m_kImageNames[i]);
        }
        else
        {
            assert(m_kTextures[i]->GetName() != "");
            pkImage =
                Catalog<Image>::GetActive()->Find(m_kTextures[i]->GetName());
        }
        m_kTextures[i]->SetImage(pkImage);
    }
}
//----------------------------------------------------------------------------
bool Shader::Register (Stream& rkStream) const
{
    if (!Object::Register(rkStream))
    {
        return false;
    }

    if (m_spkProgram)
    {
        m_spkProgram->Register(rkStream);
    }

    for (int i = 0; i < (int)m_kTextures.size(); i++)
    {
        if (m_kTextures[i])
        {
            m_kTextures[i]->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Shader::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(m_kShaderName);

    // link data
    rkStream.Write(m_spkProgram);

    int iQuantity = (int)m_kTextures.size();
    rkStream.Write(iQuantity);
    for (int i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kTextures[i]);
    }

    // The data member m_kUserData is set during loading at program runtime.

    WM4_END_DEBUG_STREAM_SAVE(Shader);
}
//----------------------------------------------------------------------------
int Shader::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Object::GetDiskUsed(rkVersion) +
        sizeof(int) + (int)m_kShaderName.length();

    int iQuantity;
    if (rkVersion < StreamVersion(4,6))
    {
        iQuantity = (int)m_kImageNames.size();
        iSize += sizeof(int);
        for (int i = 0; i < iQuantity; i++)
        {
            iSize += sizeof(int) + (int)m_kImageNames[i].length();
        }
    }
    else
    {
        iSize += WM4_PTRSIZE(m_spkProgram);
    }

    iQuantity = (int)m_kTextures.size();
    iSize += sizeof(int) + iQuantity*WM4_PTRSIZE(m_kTextures[0]);

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* Shader::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("shader name =",m_kShaderName.c_str()));

    // children
    pkTree->Append(Object::SaveStrings());

    if (m_spkProgram)
    {
        pkTree->Append(m_spkProgram->SaveStrings());
    }

    for (int i = 0; i < (int)m_kTextures.size(); i++)
    {
        pkTree->Append(m_kTextures[i]->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
