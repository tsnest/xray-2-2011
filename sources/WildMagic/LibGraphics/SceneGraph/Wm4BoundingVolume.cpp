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
#include "Wm4BoundingVolume.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,BoundingVolume,Object);
WM4_IMPLEMENT_ABSTRACT_STREAM(BoundingVolume);
WM4_IMPLEMENT_DEFAULT_NAME_ID(BoundingVolume,Object);

//----------------------------------------------------------------------------
BoundingVolume::BoundingVolume ()
{
}
//----------------------------------------------------------------------------
BoundingVolume::~BoundingVolume ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BoundingVolume::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    Object::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(BoundingVolume);
}
//----------------------------------------------------------------------------
void BoundingVolume::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BoundingVolume::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void BoundingVolume::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    Object::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(BoundingVolume);
}
//----------------------------------------------------------------------------
int BoundingVolume::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* BoundingVolume::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Object::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
