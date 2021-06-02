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
// Version: 4.0.2 (2007/06/07)

#include "Wm4GraphicsPCH.h"
#include "Wm4ClodMesh.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,ClodMesh,TriMesh);
WM4_IMPLEMENT_STREAM(ClodMesh);
WM4_IMPLEMENT_DEFAULT_NAME_ID(ClodMesh,TriMesh);

//----------------------------------------------------------------------------
ClodMesh::ClodMesh (VertexBuffer* pkVBuffer, const IndexBuffer* pkIBuffer,
    CollapseRecordArray* pkRecordArray)
    :
    TriMesh(pkVBuffer,0)
{
    assert(pkRecordArray);

    m_iTargetRecord = 0;
    m_iCurrentRecord = 0;
    m_spkRecordArray = pkRecordArray;

    // Make a copy of the indices.
    int iIQuantity = pkIBuffer->GetIndexQuantity();
    const int* aiIndex = pkIBuffer->GetData();
    IBuffer = WM4_NEW IndexBuffer(iIQuantity);
    size_t uiSize = iIQuantity*sizeof(int);
    System::Memcpy(IBuffer->GetData(),uiSize,aiIndex,uiSize);
}
//----------------------------------------------------------------------------
ClodMesh::ClodMesh ()
{
    m_iTargetRecord = 0;
    m_iCurrentRecord = 0;
}
//----------------------------------------------------------------------------
ClodMesh::~ClodMesh ()
{
}
//----------------------------------------------------------------------------
void ClodMesh::SelectLevelOfDetail ()
{
    const CollapseRecord* akRecord = m_spkRecordArray->GetData();

    // Get the target record.  The virtual function may be overridden by a
    // derived class to obtain a desired automated change in the target.
    int iTargetRecord = GetAutomatedTargetRecord();

    // Collapse the mesh, if necessary.
    int* aiIndex = IBuffer->GetData();
    int i, iC;
    while (m_iCurrentRecord < iTargetRecord)
    {
        m_iCurrentRecord++;

        // Replace indices in the connectivity array.
        const CollapseRecord& rkRecord = akRecord[m_iCurrentRecord];
        for (i = 0; i < rkRecord.IQuantity; i++)
        {
            iC = rkRecord.Index[i];
            assert(aiIndex[iC] == rkRecord.VThrow);
            aiIndex[iC] = rkRecord.VKeep;
        }

        // Reduce the vertex count; the vertices are properly ordered.
        VBuffer->SetVertexQuantity(rkRecord.VQuantity);

        // Reduce the triangle count; the triangles are properly ordered.
        IBuffer->SetIndexQuantity(3*rkRecord.TQuantity);

        // The vertices are unchanged, so only the index buffer needs
        // refreshing.
        IBuffer->Release();
    }

    // Expand the mesh, if necessary.
    while (m_iCurrentRecord > iTargetRecord)
    {
        // Restore indices in the connectivity array.
        const CollapseRecord& rkRecord = akRecord[m_iCurrentRecord];
        for (i = 0; i < rkRecord.IQuantity; i++)
        {
            iC = rkRecord.Index[i];
            assert(aiIndex[iC] == rkRecord.VKeep);
            aiIndex[iC] = rkRecord.VThrow;
        }

        m_iCurrentRecord--;
        const CollapseRecord& rkPrevRecord = akRecord[m_iCurrentRecord];

        // Increase the vertex count; the vertices are properly ordered.
        VBuffer->SetVertexQuantity(rkPrevRecord.VQuantity);

        // Increase the triangle count; the triangles are properly ordered.
        IBuffer->SetIndexQuantity(3*rkPrevRecord.TQuantity);

        // The vertices are unchanged, so only the index buffer needs
        // refreshing.
        IBuffer->Release();
    }
}
//----------------------------------------------------------------------------
void ClodMesh::GetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    SelectLevelOfDetail();
    TriMesh::GetVisibleSet(rkCuller,bNoCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ClodMesh::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iCurrentRecord);
    rkStream.Read(m_iTargetRecord);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject); // m_spkRecordArray
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(ClodMesh);
}
//----------------------------------------------------------------------------
void ClodMesh::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkRecordArray = (CollapseRecordArray*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool ClodMesh::Register (Stream& rkStream) const
{
    if (!TriMesh::Register(rkStream))
    {
        return false;
    }

    if (m_spkRecordArray)
    {
        m_spkRecordArray->Register(rkStream);
    }
    return true;
}
//----------------------------------------------------------------------------
void ClodMesh::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iCurrentRecord);
    rkStream.Write(m_iTargetRecord);

    // link data
    rkStream.Write(m_spkRecordArray);

    WM4_END_DEBUG_STREAM_SAVE(ClodMesh);
}
//----------------------------------------------------------------------------
int ClodMesh::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return TriMesh::GetDiskUsed(rkVersion) +
        sizeof(m_iCurrentRecord) +
        sizeof(m_iTargetRecord) +
        WM4_PTRSIZE(m_spkRecordArray);
}
//----------------------------------------------------------------------------
StringTree* ClodMesh::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("current record =",m_iCurrentRecord));
    pkTree->Append(Format("target record =",m_iTargetRecord));

    // children
    pkTree->Append(TriMesh::SaveStrings());
    if (m_spkRecordArray)
    {
        pkTree->Append(m_spkRecordArray->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
