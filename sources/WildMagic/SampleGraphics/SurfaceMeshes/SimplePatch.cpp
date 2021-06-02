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

#include "SimplePatch.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,SimplePatch,SurfacePatch);
WM4_IMPLEMENT_STREAM(SimplePatch);
WM4_IMPLEMENT_DEFAULT_NAME_ID(SimplePatch,SurfacePatch);

//----------------------------------------------------------------------------
SimplePatch::SimplePatch ()
    :
    SurfacePatch(-2.0f,2.0f,-2.0f,2.0f,true)
{
    Amplitude = 0.0f;
}
//----------------------------------------------------------------------------
SimplePatch::~SimplePatch ()
{
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::P (float fU, float fV) const
{
    // P(u,v) = (u,v,A*u*v)
    return Vector3f(fU,fV,Amplitude*fU*fV);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PU (float fU, float fV) const
{
    // P_u = (1,0,A*v)
    return Vector3f(1.0f,0.0f,Amplitude*fV);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PV (float fU, float fV) const
{
    // P_v = (0,1,A*u)
    return Vector3f(0.0f,1.0f,Amplitude*fU);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PUU (float fU, float fV) const
{
    // P_uu = (0,0,0)
    return Vector3f(0.0f,0.0f,0.0f);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PUV (float fU, float fV) const
{
    // P_uv = (0,0,A)
    return Vector3f(0.0f,0.0f,Amplitude);
}
//----------------------------------------------------------------------------
Vector3f SimplePatch::PVV (float fU, float fV) const
{
    // P_vv = (0,0,0)
    return Vector3f(0.0f,0.0f,0.0f);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SimplePatch::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    SurfacePatch::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Amplitude);

    WM4_END_DEBUG_STREAM_LOAD(SimplePatch);
}
//----------------------------------------------------------------------------
void SimplePatch::Link (Stream& rkStream, Stream::Link* pkLink)
{
    SurfacePatch::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SimplePatch::Register (Stream& rkStream) const
{
    return SurfacePatch::Register(rkStream);
}
//----------------------------------------------------------------------------
void SimplePatch::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    SurfacePatch::Save(rkStream);

    // native data
    rkStream.Write(Amplitude);

    WM4_END_DEBUG_STREAM_SAVE(SimplePatch);
}
//----------------------------------------------------------------------------
int SimplePatch::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return SurfacePatch::GetDiskUsed(rkVersion) +
        sizeof(Amplitude);
}
//----------------------------------------------------------------------------
StringTree* SimplePatch::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("amplitude =",Amplitude));

    // children
    pkTree->Append(SurfacePatch::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
