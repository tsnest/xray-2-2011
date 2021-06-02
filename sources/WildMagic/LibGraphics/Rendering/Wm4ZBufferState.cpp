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
#include "Wm4ZBufferState.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,ZBufferState,GlobalState);
WM4_IMPLEMENT_STREAM(ZBufferState);
WM4_IMPLEMENT_DEFAULT_NAME_ID(ZBufferState,GlobalState);
WM4_IMPLEMENT_INITIALIZE(ZBufferState);
WM4_IMPLEMENT_TERMINATE(ZBufferState);

const char* ZBufferState::ms_aacCompareMode[CF_QUANTITY] =
{
    "CF_NEVER",
    "CF_LESS",
    "CF_EQUAL",
    "CF_LEQUAL",
    "CF_GREATER",
    "CF_NOTEQUAL",
    "CF_GEQUAL",
    "CF_ALWAYS"
};

//----------------------------------------------------------------------------
void ZBufferState::Initialize ()
{
    Default[ZBUFFER] = WM4_NEW ZBufferState;
}
//----------------------------------------------------------------------------
void ZBufferState::Terminate ()
{
    Default[ZBUFFER] = 0;
}
//----------------------------------------------------------------------------
ZBufferState::ZBufferState ()
{
    Enabled = true;
    Writable = true;
    Compare = CF_LEQUAL;
}
//----------------------------------------------------------------------------
ZBufferState::~ZBufferState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ZBufferState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    int iTmp;
    rkStream.Read(Enabled);
    rkStream.Read(Writable);
    rkStream.Read(iTmp);
    Compare = (CompareMode)iTmp;

    WM4_END_DEBUG_STREAM_LOAD(ZBufferState);
}
//----------------------------------------------------------------------------
void ZBufferState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ZBufferState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void ZBufferState::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(Enabled);
    rkStream.Write(Writable);
    rkStream.Write((int)Compare);

    WM4_END_DEBUG_STREAM_SAVE(ZBufferState);
}
//----------------------------------------------------------------------------
int ZBufferState::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return GlobalState::GetDiskUsed(rkVersion) +
        sizeof(char) + // Enabled
        sizeof(char) + // Writable
        sizeof(int);   // Compare
}
//----------------------------------------------------------------------------
StringTree* ZBufferState::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("enabled =",Enabled));
    pkTree->Append(Format("writable = ",Writable));
    pkTree->Append(Format("compare = ",ms_aacCompareMode[Compare]));

    // children
    pkTree->Append(GlobalState::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
