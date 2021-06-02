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
#include "Wm4TriMesh.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,TriMesh,Triangles);
WM4_IMPLEMENT_STREAM(TriMesh);
WM4_IMPLEMENT_DEFAULT_NAME_ID(TriMesh,Triangles);

//----------------------------------------------------------------------------
TriMesh::TriMesh ()
{
    Type = GT_TRIMESH;
}
//----------------------------------------------------------------------------
TriMesh::TriMesh (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer,
    bool bUpdateModelNormals, int iTangentSpaceType)
    :
    Triangles(pkVBuffer,pkIBuffer)
{
    Type = GT_TRIMESH;

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
TriMesh::~TriMesh ()
{
}
//----------------------------------------------------------------------------
bool TriMesh::GetTriangle (int i, int& riV0, int& riV1, int& riV2) const
{
    if (0 <= i && i < GetTriangleQuantity())
    {
        const int* piIndex = &IBuffer->GetData()[3*i];
        riV0 = *piIndex++;
        riV1 = *piIndex++;
        riV2 = *piIndex;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void TriMesh::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    Triangles::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(TriMesh);
}
//----------------------------------------------------------------------------
void TriMesh::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Triangles::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool TriMesh::Register (Stream& rkStream) const
{
    return Triangles::Register(rkStream);
}
//----------------------------------------------------------------------------
void TriMesh::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    Triangles::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(TriMesh);
}
//----------------------------------------------------------------------------
int TriMesh::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Triangles::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* TriMesh::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Triangles::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
