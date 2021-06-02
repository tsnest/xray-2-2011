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
#include "Wm4PolygonOffsetState.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,PolygonOffsetState,GlobalState);
WM4_IMPLEMENT_STREAM(PolygonOffsetState);
WM4_IMPLEMENT_DEFAULT_NAME_ID(PolygonOffsetState,GlobalState);
WM4_IMPLEMENT_INITIALIZE(PolygonOffsetState);
WM4_IMPLEMENT_TERMINATE(PolygonOffsetState);

//----------------------------------------------------------------------------
void PolygonOffsetState::Initialize ()
{
    Default[POLYGONOFFSET] = WM4_NEW PolygonOffsetState;
}
//----------------------------------------------------------------------------
void PolygonOffsetState::Terminate ()
{
    Default[POLYGONOFFSET] = 0;
}
//----------------------------------------------------------------------------
PolygonOffsetState::PolygonOffsetState ()
{
    FillEnabled = false;
    LineEnabled = false;
    PointEnabled = false;
    Scale = 0.0f;
    Bias = 0.0f;
}
//----------------------------------------------------------------------------
PolygonOffsetState::~PolygonOffsetState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void PolygonOffsetState::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    GlobalState::Load(rkStream,pkLink);

    // native data
    rkStream.Read(FillEnabled);
    rkStream.Read(LineEnabled);
    rkStream.Read(PointEnabled);
    rkStream.Read(Scale);
    rkStream.Read(Bias);

    WM4_END_DEBUG_STREAM_LOAD(PolygonOffsetState);
}
//----------------------------------------------------------------------------
void PolygonOffsetState::Link (Stream& rkStream, Stream::Link* pkLink)
{
    GlobalState::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool PolygonOffsetState::Register (Stream& rkStream) const
{
    return GlobalState::Register(rkStream);
}
//----------------------------------------------------------------------------
void PolygonOffsetState::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    GlobalState::Save(rkStream);

    // native data
    rkStream.Write(FillEnabled);
    rkStream.Write(LineEnabled);
    rkStream.Write(PointEnabled);
    rkStream.Write(Scale);
    rkStream.Write(Bias);

    WM4_END_DEBUG_STREAM_SAVE(PolygonOffsetState);
}
//----------------------------------------------------------------------------
int PolygonOffsetState::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return GlobalState::GetDiskUsed(rkVersion) +
        sizeof(char) + // FillEnabled
        sizeof(char) + // LineEnabled
        sizeof(char) + // PointEnabled
        sizeof(Scale) +
        sizeof(Bias);
}
//----------------------------------------------------------------------------
StringTree* PolygonOffsetState::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("fill enabled =",FillEnabled));
    pkTree->Append(Format("line enabled =",LineEnabled));
    pkTree->Append(Format("point enabled =",PointEnabled));
    pkTree->Append(Format("scale =",Scale));
    pkTree->Append(Format("bias =",Bias));

    // children
    pkTree->Append(GlobalState::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
