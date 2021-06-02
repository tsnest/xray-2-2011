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
// Version: 4.6.0 (2007/09/23)

#include "Wm4GraphicsPCH.h"
#include "Wm4DynamicTexture.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,DynamicTexture,Texture);
WM4_IMPLEMENT_STREAM(DynamicTexture);
WM4_IMPLEMENT_DEFAULT_NAME_ID(DynamicTexture,Texture);

//----------------------------------------------------------------------------
DynamicTexture::DynamicTexture ()
{
}
//----------------------------------------------------------------------------
DynamicTexture::DynamicTexture (const char* acTextureName, Image* pkImage)
    :
    Texture(acTextureName,pkImage)
{
}
//----------------------------------------------------------------------------
DynamicTexture::~DynamicTexture ()
{
}
//----------------------------------------------------------------------------
void DynamicTexture::SetImage (Image*)
{
    // You may not override m_spkImage that was set by the constructor.
}
//----------------------------------------------------------------------------
void DynamicTexture::SetFilterType (FilterType eFType)
{
    if (eFType == NEAREST_NEAREST || eFType == NEAREST_LINEAR)
    {
        eFType = NEAREST;
    }
    else if (eFType == LINEAR_NEAREST || eFType == LINEAR_LINEAR)
    {
        eFType = LINEAR;
    }

    Texture::SetFilterType(eFType);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void DynamicTexture::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    Texture::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(DynamicTexture);
}
//----------------------------------------------------------------------------
void DynamicTexture::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Texture::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool DynamicTexture::Register (Stream& rkStream) const
{
    return Texture::Register(rkStream);
}
//----------------------------------------------------------------------------
void DynamicTexture::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    Texture::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(DynamicTexture);
}
//----------------------------------------------------------------------------
int DynamicTexture::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Texture::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* DynamicTexture::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Texture::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
