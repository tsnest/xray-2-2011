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
#include "Wm4StencilState.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,StencilState,GlobalState);
WM4_IMPLEMENT_STREAM(StencilState);
WM4_IMPLEMENT_DEFAULT_NAME_ID(StencilState,GlobalState);
WM4_IMPLEMENT_INITIALIZE(StencilState);
WM4_IMPLEMENT_TERMINATE(StencilState);

const char* StencilState::ms_aacCompare[StencilState::CF_QUANTITY] =
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

const char* StencilState::ms_aacOperation[StencilState::OT_QUANTITY] =
{
    "OT_KEEP",
    "OT_ZERO",
    "OT_REPLACE",
    "OT_INCREMENT",
    "OT_DECREMENT",
    "OT_INVERT"
};

//----------------------------------------------------------------------------
void StencilState::Initialize ()
{
    Default[STENCIL] = WM4_NEW StencilState;
}
//----------------------------------------------------------------------------
void StencilState::Terminate ()
{
    Default[STENCIL] = 0;
}
//----------------------------------------------------------------------------
StencilState::StencilState ()
{
    Enabled = false;
    Compare = CF_NEVER;
    Reference = 0;
    Mask = (unsigned int)~0;
    WriteMask = (unsigned int)~0;
    OnFail = OT_KEEP;
    OnZFail = OT_KEEP;
    OnZPass = OT_KEEP;
}
//----------------------------------------------------------------------------
StencilState::~StencilState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void StencilState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    int iTmp;
    rkStream.Read(Enabled);
    rkStream.Read(iTmp);
    Compare = (CompareFunction)iTmp;
    rkStream.Read(Reference);
    rkStream.Read(Mask);
    rkStream.Read(WriteMask);
    rkStream.Read(iTmp);
    OnFail = (OperationType)iTmp;
    rkStream.Read(iTmp);
    OnZFail = (OperationType)iTmp;
    rkStream.Read(iTmp);
    OnZPass = (OperationType)iTmp;

    WM4_END_DEBUG_STREAM_LOAD(StencilState);
}
//----------------------------------------------------------------------------
void StencilState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool StencilState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void StencilState::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(Enabled);
    rkStream.Write((int)Compare);
    rkStream.Write(Reference);
    rkStream.Write(Mask);
    rkStream.Write(WriteMask);
    rkStream.Write((int)OnFail);
    rkStream.Write((int)OnZFail);
    rkStream.Write((int)OnZPass);

    WM4_END_DEBUG_STREAM_SAVE(StencilState);
}
//----------------------------------------------------------------------------
int StencilState::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return GlobalState::GetDiskUsed(rkVersion) +
        sizeof(char) + // Enabled
        sizeof(int) +  // Compare
        sizeof(Reference) +
        sizeof(Mask) +
        sizeof(WriteMask) +
        sizeof(int) +  // OnFail
        sizeof(int) +  // OnZFail
        sizeof(int);   // OnZPass
}
//----------------------------------------------------------------------------
StringTree* StencilState::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("enabled =",Enabled));
    pkTree->Append(Format("compare =",ms_aacCompare[Compare]));
    pkTree->Append(Format("reference =",Reference));
    pkTree->Append(Format("mask =",Mask));
    pkTree->Append(Format("write mask =",WriteMask));
    pkTree->Append(Format("on fail =",ms_aacOperation[OnFail]));
    pkTree->Append(Format("on z-fail =",ms_aacOperation[OnZFail]));
    pkTree->Append(Format("on z-pass =",ms_aacOperation[OnZPass]));

    // children
    pkTree->Append(GlobalState::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
