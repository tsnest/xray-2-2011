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
// Version: 4.0.4 (2008/01/20)

#include "Wm4GraphicsPCH.h"
#include "Wm4TriFan.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,TriFan,Triangles);
WM4_IMPLEMENT_STREAM(TriFan);
WM4_IMPLEMENT_DEFAULT_NAME_ID(TriFan,Triangles);

//----------------------------------------------------------------------------
TriFan::TriFan ()
{
    Type = GT_TRIFAN;
}
//----------------------------------------------------------------------------
TriFan::TriFan (VertexBuffer* pkVBuffer, bool bUpdateModelNormals,
    int iTangentSpaceType)
    :
    Triangles(pkVBuffer,0)
{
    Type = GT_TRIFAN;

    int iIQuantity = pkVBuffer->GetVertexQuantity();
    IBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* piIndex = IBuffer->GetData();
    for (int i = 0; i < iIQuantity; i++)
    {
        *piIndex++ = i;
    }

    if (bUpdateModelNormals)
    {
        UpdateModelNormals();
    }

    if (iTangentSpaceType > (int)GU_TANGENT_SPACE_NONE)
    {
        UpdateModelTangentSpace(iTangentSpaceType);
    }
}
//----------------------------------------------------------------------------
TriFan::TriFan (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer,
    bool bUpdateModelNormals, int iTangentSpaceType)
    :
    Triangles(pkVBuffer,pkIBuffer)
{
    Type = GT_TRIFAN;

    if (bUpdateModelNormals)
    {
        UpdateModelNormals();
    }

    if (iTangentSpaceType > (int)GU_TANGENT_SPACE_NONE)
    {
        UpdateModelTangentSpace(iTangentSpaceType);
    }
}
//----------------------------------------------------------------------------
TriFan::~TriFan ()
{
}
//----------------------------------------------------------------------------
bool TriFan::GetTriangle (int i, int& riV0, int& riV1, int& riV2) const
{
    if (0 <= i && i < GetTriangleQuantity())
    {
        const int* aiIndex = IBuffer->GetData();
        riV0 = aiIndex[0];
        riV1 = aiIndex[i+1];
        riV2 = aiIndex[i+2];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void TriFan::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    Triangles::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(TriFan);
}
//----------------------------------------------------------------------------
void TriFan::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Triangles::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool TriFan::Register (Stream& rkStream) const
{
    return Triangles::Register(rkStream);
}
//----------------------------------------------------------------------------
void TriFan::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    Triangles::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(TriFan);
}
//----------------------------------------------------------------------------
int TriFan::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Triangles::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* TriFan::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Triangles::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
