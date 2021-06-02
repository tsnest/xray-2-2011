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
// Version: 4.0.3 (2006/08/10)

#include "Wm4GraphicsPCH.h"
#include "Wm4CreateClodMesh.h"
using namespace Wm4;

//----------------------------------------------------------------------------
CreateClodMesh::CreateClodMesh (VertexBuffer* pkVBuffer,
    IndexBuffer* pkIBuffer, int& riCQuantity, CollapseRecord*& rakCRecord)
{
    // Hang onto these to avoid having to pass them through member function
    // calls.
    m_iVQuantity = pkVBuffer->GetVertexQuantity();
    m_pkVBuffer = pkVBuffer;
    m_iTQuantity = pkIBuffer->GetIndexQuantity()/3;
    m_aiConnect = pkIBuffer->GetData();

    // This is for reordering vertices and triangles.
    m_iVCurrent = m_iVQuantity-1;
    m_iTCurrent = m_iTQuantity-1;
    m_aiVOrdered = WM4_NEW int[m_iVQuantity];
    m_aiVPermute = WM4_NEW int[m_iVQuantity];
    m_aiNewConnect = WM4_NEW int[3*m_iTQuantity];

    // Insert the triangles into the mesh.  The triangle indices are attached
    // as extra data.
    m_bCollapsing = false;
    for (int i = 0; i < m_iTQuantity; i++)
    {
        int iV0 = m_aiConnect[3*i];
        int iV1 = m_aiConnect[3*i+1];
        int iV2 = m_aiConnect[3*i+2];
        assert(iV0 != iV1 && iV0 != iV2 && iV1 != iV2);
        Triangle kT(iV0,iV1,iV2);
        InsertTriangle(kT);
        SetData(kT,i);
    }

    assert((int)(m_kVMap.size()) == m_iVQuantity);
    assert((int)(m_kTMap.size()) == m_iTQuantity);

    InitializeHeap();

    m_bCollapsing = true;
    while (m_iHQuantity > 0)
    {
        if (m_apkHeap[0]->m_fMetric == Mathf::MAX_REAL)
        {
            // All remaining heap elements have infinite weight.
            FlushVertices();
            FlushTriangles();
            break;
        }

        DoCollapse();

        assert((int)(m_kVMap.size()) == m_iVCurrent+1);
        assert((int)(m_kTMap.size()) == m_iTCurrent+1);
    }
    m_bCollapsing = false;

    // Permute the vertices and triangle connectivity so that the last
    // vertex/triangle in the array is the first vertex/triangle to be
    // removed.
    Reorder();

    // The collapse records store the incremental changes that are used for
    // dynamic LOD changes in the caller of this constructor.
    ComputeRecords(riCQuantity,rakCRecord);
}
//----------------------------------------------------------------------------
CreateClodMesh::~CreateClodMesh ()
{
    // Remove all triangles.  The use of a temporary object
    // Triangle(iV0,iV1,iV2) is necessary because the removal is occurring
    // simultaneously with the map traversal, and the traversal relies on
    // the iterator to be correct.
    MTIter pkT = m_kTMap.begin();
    while (!m_kTMap.empty() && pkT != m_kTMap.end())
    {
        int iV0 = pkT->first.m_aiV[0];
        int iV1 = pkT->first.m_aiV[1];
        int iV2 = pkT->first.m_aiV[2];
        pkT++;

        RemoveTriangle(Triangle(iV0,iV1,iV2));
    }

    WM4_DELETE[] m_apkHeap;
    WM4_DELETE[] m_aiVOrdered;
    WM4_DELETE[] m_aiVPermute;
    WM4_DELETE[] m_aiNewConnect;
}
//----------------------------------------------------------------------------
void CreateClodMesh::InsertTriangle (const Triangle& rkT)
{
    int iV0 = rkT.m_aiV[0];
    int iV1 = rkT.m_aiV[1];
    int iV2 = rkT.m_aiV[2];

    Triangle kT(iV0,iV1,iV2);
    Edge kE0(iV0,iV1), kE1(iV1,iV2), kE2(iV2,iV0);

    // insert triangle
    std::pair<MTIter,bool> kRT = m_kTMap.insert(
        std::make_pair(kT,TriangleAttribute()));

    // insert vertices
    std::pair<MVIter,bool> kRV0 = m_kVMap.insert(
        std::make_pair(iV0,VertexAttribute()));
    kRV0.first->second.m_kESet.Insert(kE0);
    kRV0.first->second.m_kESet.Insert(kE2);
    kRV0.first->second.m_kTSet.Insert(kT);

    std::pair<MVIter,bool> kRV1 = m_kVMap.insert(
        std::make_pair(iV1,VertexAttribute()));
    kRV1.first->second.m_kESet.Insert(kE0);
    kRV1.first->second.m_kESet.Insert(kE1);
    kRV1.first->second.m_kTSet.Insert(kT);

    std::pair<MVIter,bool> kRV2 = m_kVMap.insert(
        std::make_pair(iV2,VertexAttribute()));
    kRV2.first->second.m_kESet.Insert(kE1);
    kRV2.first->second.m_kESet.Insert(kE2);
    kRV2.first->second.m_kTSet.Insert(kT);

    // insert edges
    std::pair<MEIter,bool> kRE0 = m_kEMap.insert(
        std::make_pair(kE0,EdgeAttribute()));
    kRE0.first->second.m_kTSet.Insert(kT);

    std::pair<MEIter,bool> kRE1 = m_kEMap.insert(
        std::make_pair(kE1,EdgeAttribute()));
    kRE1.first->second.m_kTSet.Insert(kT);

    std::pair<MEIter,bool> kRE2 = m_kEMap.insert(
        std::make_pair(kE2,EdgeAttribute()));
    kRE2.first->second.m_kTSet.Insert(kT);

    // It is possible that a 'keep' vertex was removed because the triangles
    // sharing the collapse edge were removed first, but then the insertion
    // of a modified triangle reinserts the 'keep' vertex.
    if (m_bCollapsing)
    {
        if (kRV0.second)
        {
            m_kVDelete.Remove(iV0);
        }

        if (kRV1.second)
        {
            m_kVDelete.Remove(iV1);
        }

        if (kRV2.second)
        {
            m_kVDelete.Remove(iV2);
        }
    }

    OnEdgeInsert(kE0,kRE0.second,kRE0.first->second.Record);
    OnEdgeInsert(kE1,kRE1.second,kRE1.first->second.Record);
    OnEdgeInsert(kE2,kRE2.second,kRE2.first->second.Record);
}
//----------------------------------------------------------------------------
void CreateClodMesh::RemoveTriangle (const Triangle& rkT)
{
    int iV0 = rkT.m_aiV[0];
    int iV1 = rkT.m_aiV[1];
    int iV2 = rkT.m_aiV[2];

    // Remove the triangle.
    Triangle kT(iV0,iV1,iV2);
    MTIter pkT = m_kTMap.find(kT);
    if (pkT == m_kTMap.end())
    {
        // The triangle does not exist, so there is .othing to do
        return;
    }

    // Update the edges.
    Edge kE0(iV0,iV1), kE1(iV1,iV2), kE2(iV2,iV0);

    MEIter pkE0 = m_kEMap.find(kE0);
    assert(pkE0 != m_kEMap.end());
    pkE0->second.m_kTSet.Remove(kT);

    MEIter pkE1 = m_kEMap.find(kE1);
    assert(pkE1 != m_kEMap.end());
    pkE1->second.m_kTSet.Remove(kT);

    MEIter pkE2 = m_kEMap.find(kE2);
    assert(pkE2 != m_kEMap.end());
    pkE2->second.m_kTSet.Remove(kT);

    // Update the vertices.
    MVIter pkV0 = m_kVMap.find(iV0);
    assert(pkV0 != m_kVMap.end());
    pkV0->second.m_kTSet.Remove(kT);

    MVIter pkV1 = m_kVMap.find(iV1);
    assert(pkV1 != m_kVMap.end());
    pkV1->second.m_kTSet.Remove(kT);

    MVIter pkV2 = m_kVMap.find(iV2);
    assert(pkV2 != m_kVMap.end());
    pkV2->second.m_kTSet.Remove(kT);

    if (pkE0->second.m_kTSet.GetQuantity() == 0)
    {
        pkV0->second.m_kESet.Remove(kE0);
        pkV1->second.m_kESet.Remove(kE0);
    }

    if (pkE1->second.m_kTSet.GetQuantity() == 0)
    {
        pkV1->second.m_kESet.Remove(kE1);
        pkV2->second.m_kESet.Remove(kE1);
    }

    if (pkE2->second.m_kTSet.GetQuantity() == 0)
    {
        pkV0->second.m_kESet.Remove(kE2);
        pkV2->second.m_kESet.Remove(kE2);
    }

    bool bDestroy = (pkV0->second.m_kESet.GetQuantity() == 0 &&
        pkV0->second.m_kTSet.GetQuantity() == 0);
    if (bDestroy)
    {
        if (m_bCollapsing)
        {
            // Keep track of vertices removed during the edge collapse.
            m_kVDelete.Insert(iV0);
        }
        m_kVMap.erase(iV0);
    }

    bDestroy = (pkV1->second.m_kESet.GetQuantity() == 0 &&
        pkV1->second.m_kTSet.GetQuantity() == 0);
    if (bDestroy)
    {
        if (m_bCollapsing)
        {
            // Keep track of vertices removed during the edge collapse.
            m_kVDelete.Insert(iV1);
        }
        m_kVMap.erase(iV1);
    }

    bDestroy = (pkV2->second.m_kESet.GetQuantity() == 0 &&
        pkV2->second.m_kTSet.GetQuantity() == 0);
    if (bDestroy)
    {
        if (m_bCollapsing)
        {
            // Keep track of vertices removed during the edge collapse.
            m_kVDelete.Insert(iV2);
        }
        m_kVMap.erase(iV2);
    }

    bDestroy = (pkE0->second.m_kTSet.GetQuantity() == 0);
    OnEdgeRemove(kE0,bDestroy,pkE0->second.Record);
    if (bDestroy)
    {
        m_kEMap.erase(kE0);
    }

    bDestroy = (pkE1->second.m_kTSet.GetQuantity() == 0);
    OnEdgeRemove(kE1,bDestroy,pkE1->second.Record);
    if (bDestroy)
    {
        m_kEMap.erase(kE1);
    }

    bDestroy = (pkE2->second.m_kTSet.GetQuantity() == 0);
    OnEdgeRemove(kE2,bDestroy,pkE2->second.Record);
    if (bDestroy)
    {
        m_kEMap.erase(kE2);
    }

    m_kTMap.erase(kT);
}
//----------------------------------------------------------------------------
void CreateClodMesh::SetData (const Triangle& rkT, int iIndex)
{
    MTIter pkT = m_kTMap.find(
        Triangle(rkT.m_aiV[0],rkT.m_aiV[1],rkT.m_aiV[2]));
    if (pkT != m_kTMap.end())
    {
        pkT->second.Index = iIndex;
    }
}
//----------------------------------------------------------------------------
int CreateClodMesh::GetData (const Triangle& rkT)
{
    MTIter pkT = m_kTMap.find(
        Triangle(rkT.m_aiV[0],rkT.m_aiV[1],rkT.m_aiV[2]));
    return (pkT != m_kTMap.end() ? pkT->second.Index : -1);
}
//----------------------------------------------------------------------------
void CreateClodMesh::DoCollapse ()
{
    // Define a 2-edge to be an edge that has exactly two triangles sharing
    // it.  An edge is collapsible if it is a 2-edge and has at least one end
    // point whose sharing edges are all 2-edges.  In this case, such an end
    // point will be the 'throw' vertex.  This keeps the boundary and junction
    // edges from changing geometry and helps preserve the shape of the mesh.
    // The topology is always guaranteed not to change.

    // When this function is called, the metric has already been calculated
    // and is finite (so exactly two triangles must be sharing this edge).
    assert(m_apkHeap[0]->m_fMetric < Mathf::MAX_REAL);
    Edge kEdge = m_apkHeap[0]->m_kEdge;

    // Test the endpoints to see if either has only 2-edges sharing it.
    int i;
    for (i = 0; i < 2; i++)
    {
        MVCIter pkV = m_kVMap.find(kEdge.m_aiV[i]);
        assert(pkV != m_kVMap.end());
        const TSmallUnorderedSet<Edge>* pkESet = &pkV->second.m_kESet;

        int j;
        for (j = 0; j < pkESet->GetQuantity(); j++)
        {
            MECIter pkEM = m_kEMap.find((*pkESet)[j]);
            assert(pkEM != m_kEMap.end());
            if (pkEM->second.m_kTSet.GetQuantity() != 2)
            {
                break;
            }
        }

        if (j == pkESet->GetQuantity())
        {
            // All edges sharing this end point are 2-edges.
            break;
        }
    }

    if (i < 2)
    {
        int iVThrow = kEdge.m_aiV[i];
        int iVKeep = kEdge.m_aiV[1-i];
        if (!CollapseCausesFolding(iVKeep,iVThrow))
        {
            Remove();
            CollapseEdge(iVKeep,iVThrow);
            return;
        }
    }

    // The edge is not collapsible, assign it infinite weight and update the
    // heap.
    Update(0,Mathf::MAX_REAL);
}
//----------------------------------------------------------------------------
bool CreateClodMesh::CollapseCausesFolding (int iVKeep, int iVThrow)
{
    MVIter pkVT = m_kVMap.find(iVThrow);
    assert(pkVT != m_kVMap.end());

    Edge kCollapse(iVKeep,iVThrow);
    for (int j = 0; j < pkVT->second.m_kTSet.GetQuantity(); j++)
    {
        Triangle kT = pkVT->second.m_kTSet[j];
        if (kCollapse == Edge(kT.m_aiV[0],kT.m_aiV[1])
        ||  kCollapse == Edge(kT.m_aiV[1],kT.m_aiV[2])
        ||  kCollapse == Edge(kT.m_aiV[2],kT.m_aiV[0]))
        {
            // This triangle would be removed in a collapse, so it does not
            // contribute to any folding.
            continue;
        }

        for (int i = 0; i < 3; i++)
        {
            if (kT.m_aiV[i] == iVThrow)
            {
                // Test if potential replacement triangle (either ordering)
                // is in the mesh.
                int iV0 = iVKeep;
                int iV1 = kT.m_aiV[(i+1)%3];
                int iV2 = kT.m_aiV[(i+2)%3];

                if (m_kTMap.find(Triangle(iV0,iV1,iV2)) != m_kTMap.end()
                ||  m_kTMap.find(Triangle(iV0,iV2,iV1)) != m_kTMap.end())
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------
float CreateClodMesh::GetMetric (MEIter pkE)
{
    const float fLengthWeight = 10.0f;
    const float fAngleWeight = 1.0f;

    // Compute the metric for the edge.  Only manifold edges (exactly two
    // triangles sharing the edge) are allowed to collapse.
    if (pkE->second.m_kTSet.GetQuantity() == 2)
    {
        // length contribution
        Vector3f& rkEnd0 = m_pkVBuffer->Position3(pkE->first.m_aiV[0]);
        Vector3f& rkEnd1 = m_pkVBuffer->Position3(pkE->first.m_aiV[1]);
        Vector3f kDiff = rkEnd1 - rkEnd0;
        float fMetric = fLengthWeight*kDiff.Length();

        // angle/area contribution
        Triangle kT = pkE->second.m_kTSet[0];
        Vector3f kV0 = m_pkVBuffer->Position3(kT.m_aiV[0]);
        Vector3f kV1 = m_pkVBuffer->Position3(kT.m_aiV[1]);
        Vector3f kV2 = m_pkVBuffer->Position3(kT.m_aiV[2]);
        Vector3f kE0 = kV1 - kV0;
        Vector3f kE1 = kV2 - kV0;
        Vector3f kN0 = kE0.Cross(kE1);

        kT = pkE->second.m_kTSet[1];
        kV0 = m_pkVBuffer->Position3(kT.m_aiV[0]);
        kV1 = m_pkVBuffer->Position3(kT.m_aiV[1]);
        kV2 = m_pkVBuffer->Position3(kT.m_aiV[2]);
        kE0 = kV1 - kV0;
        kE1 = kV2 - kV0;
        Vector3f kN1 = kE0.Cross(kE1);

        Vector3f kCross = kN0.Cross(kN1);
        fMetric += fAngleWeight*kCross.Length();

        return fMetric;
    }

    // Boundary edges (one triangle containing edge) and junction edges
    // (3 or more triangles sharing edge) are not allowed to collapse.
    return Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
void CreateClodMesh::RemoveTriangleDuringCollapse (const Triangle& rkT)
{
    // If the triangle is an original one, reorder the connectivity array so
    // that the triangle occurs at the end.
    int iTIndex = GetData(rkT);
    if (iTIndex >= 0)
    {
        assert(m_iTCurrent >= 0);
        m_aiNewConnect[3*m_iTCurrent] = m_aiConnect[3*iTIndex];
        m_aiNewConnect[3*m_iTCurrent+1] = m_aiConnect[3*iTIndex+1];
        m_aiNewConnect[3*m_iTCurrent+2] = m_aiConnect[3*iTIndex+2];
        m_iTCurrent--;
    }

    RemoveTriangle(rkT);
}
//----------------------------------------------------------------------------
void CreateClodMesh::ModifyTriangle (Triangle& rkT, int iVKeep, int iVThrow)
{
#ifdef _DEBUG
    int iTStart = (int)m_kTMap.size();
#endif

    // Get the index of the pre-modified triangle, then remove the triangle
    // from the mesh.
    int iTIndex = GetData(rkT);
    RemoveTriangle(rkT);

    // replace 'throw' by 'keep'
    for (int i = 0; i < 3; i++)
    {
        if (rkT.m_aiV[i] == iVThrow)
        {
            rkT.m_aiV[i] = iVKeep;
            break;
        }
    }

    // Indices on modified triangles are the same as the indices on the
    // pre-modified triangles.
    InsertTriangle(rkT);
    SetData(rkT,iTIndex);

#ifdef _DEBUG
    int iTFinal = (int)m_kTMap.size();
    assert(iTFinal == iTStart);
#endif
}
//----------------------------------------------------------------------------
void CreateClodMesh::CollapseEdge (int iVKeep, int iVThrow)
{
#ifdef _DEBUG
    int iVStart = (int)m_kVMap.size();
    int iTStart = (int)m_kTMap.size();
    assert(iVStart > 0 && iTStart > 0);
#endif

    // Find the edge to collapse/
    Edge kCollapse(iVKeep,iVThrow);
    MEIter pkEM = m_kEMap.find(kCollapse);
    assert(pkEM != m_kEMap.end());

    // Keep track of vertices that are deleted in the collapse.
    m_kVDelete.Clear();

    // Remove the collapse-edge-shared triangles.  Using a copy of the
    // triangle set from the collapse edge is required since removal of the
    // last triangle sharing the collapse edge will remove that edge from
    // the edge map, thereby invalidating any iterator that points to data
    // in the collapse edge.
    TSmallUnorderedSet<Triangle> kTSet = pkEM->second.m_kTSet;
    int iTDeletions = kTSet.GetQuantity();
    assert(iTDeletions > 0);
    int j;
    for (j = 0; j < kTSet.GetQuantity(); j++)
    {
        RemoveTriangleDuringCollapse(kTSet[j]);
    }

    // Replace 'throw' vertices by 'keep' vertices in the remaining triangles
    // at the 'throw' vertex.  The old triangles are removed and the modified
    // triangles are inserted.
    Triangle kT;
    MVIter pkVM = m_kVMap.find(iVThrow);
    if (pkVM != m_kVMap.end())
    {
        kTSet = pkVM->second.m_kTSet;
        for (j = 0; j < kTSet.GetQuantity(); j++)
        {
            kT = kTSet[j];
            ModifyTriangle(kT,iVKeep,iVThrow);
        }
    }

    // The set of potentially modified edges consists of all those edges that
    // are shared by the triangles containing the 'keep' vertex.  Modify these
    // metrics and update the heap.
    TSmallUnorderedSet<Edge> kModified;

    MVCIter pkV = m_kVMap.find(iVKeep);
    const TSmallUnorderedSet<Triangle>* pkTSet =
        (pkV != m_kVMap.end() ? &pkV->second.m_kTSet : 0);

    if (pkTSet)
    {
        kTSet = *pkTSet;
        for (j = 0; j < kTSet.GetQuantity(); j++)
        {
            kT = kTSet[j];
            kModified.Insert(Edge(kT.m_aiV[0],kT.m_aiV[1]));
            kModified.Insert(Edge(kT.m_aiV[1],kT.m_aiV[2]));
            kModified.Insert(Edge(kT.m_aiV[2],kT.m_aiV[0]));
        }

        for (j = 0; j < kModified.GetQuantity(); j++)
        {
            MEIter pkEM = m_kEMap.find(kModified[j]);
            HeapRecord* pkRecord = pkEM->second.Record;
            float fMetric = GetMetric(pkEM);
            Update(pkRecord->m_iHIndex,fMetric);
        }
    }

#ifdef _DEBUG
    int iVFinal = (int)m_kVMap.size();
    int iVDiff = iVStart - iVFinal;
    int iTFinal = (int)m_kTMap.size();
    int iTDiff = iTStart - iTFinal;
    assert(iVDiff == m_kVDelete.GetQuantity() && iTDiff == iTDeletions);
#endif

    // Save vertex reordering information.
    for (j = 0; j < m_kVDelete.GetQuantity(); j++)
    {
        assert(0 <= m_iVCurrent && m_iVCurrent < m_iVQuantity);

        int iV = m_kVDelete[j];
        assert(0 <= iV && iV < m_iVQuantity);

        m_aiVOrdered[m_iVCurrent] = iV;
        m_aiVPermute[iV] = m_iVCurrent;
        m_iVCurrent--;
    }

    // Save the collapse information for use in constructing the final
    // collapse records for the caller of the constructor of this class.
    CollapseRecord kCR(iVKeep,iVThrow,m_kVDelete.GetQuantity(),iTDeletions);
    m_kEDelete.push_back(kCR);
}
//----------------------------------------------------------------------------
void CreateClodMesh::FlushVertices ()
{
    for (MVCIter pkV = m_kVMap.begin(); pkV != m_kVMap.end(); pkV++)
    {
        assert(0 <= m_iVCurrent && m_iVCurrent < m_iVQuantity);

        int iV = pkV->first;
        assert(0 <= iV && iV < m_iVQuantity);

        m_aiVOrdered[m_iVCurrent] = iV;
        m_aiVPermute[iV] = m_iVCurrent;
        m_iVCurrent--;
    }

    assert( m_iVCurrent == -1 );
}
//----------------------------------------------------------------------------
void CreateClodMesh::FlushTriangles ()
{
    for (MTCIter pkT = m_kTMap.begin(); pkT != m_kTMap.end(); pkT++)
    {
        int iTIndex = pkT->second.Index;
        if (iTIndex >= 0)
        {
            assert(m_iTCurrent >= 0);
            m_aiNewConnect[3*m_iTCurrent] = m_aiConnect[3*iTIndex];
            m_aiNewConnect[3*m_iTCurrent+1] = m_aiConnect[3*iTIndex+1];
            m_aiNewConnect[3*m_iTCurrent+2] = m_aiConnect[3*iTIndex+2];
            m_iTCurrent--;
        }
    }

    assert(m_iTCurrent == -1);
}
//----------------------------------------------------------------------------
void CreateClodMesh::Reorder ()
{
    // Permute the vertices.
    VertexBuffer* pkNewVBuffer = WM4_NEW VertexBuffer(
        m_pkVBuffer->GetAttributes(),m_pkVBuffer->GetVertexQuantity());

    size_t uiSize = m_pkVBuffer->GetVertexSize()*sizeof(float);
    float* afSrc;
    float* afTrg;
    int i;
    for (i = 0; i < m_iVQuantity; i++)
    {
        afSrc = m_pkVBuffer->PositionTuple(m_aiVOrdered[i]);
        afTrg = pkNewVBuffer->PositionTuple(i);
        System::Memcpy(afTrg,uiSize,afSrc,uiSize);
    }

    // Copy the permuted vertices back to the original vertex buffer.
    uiSize = m_pkVBuffer->GetChannelQuantity()*sizeof(float);
    System::Memcpy(m_pkVBuffer->GetData(),uiSize,pkNewVBuffer->GetData(),
        uiSize);
    WM4_DELETE pkNewVBuffer;

    // Permute the connectivity array and copy to the original array.
    for (i = 0; i < 3*m_iTQuantity; i++)
    {
        m_aiConnect[i] = m_aiVPermute[m_aiNewConnect[i]];
    }

    // Permute the keep/throw pairs.
    for (i = 0; i < (int)m_kEDelete.size(); i++)
    {
        CollapseRecord& rkCR = m_kEDelete[i];
        rkCR.VKeep = m_aiVPermute[rkCR.VKeep];
        rkCR.VThrow = m_aiVPermute[rkCR.VThrow];
    }
}
//----------------------------------------------------------------------------
void CreateClodMesh::ComputeRecords (int& riCQuantity,
    CollapseRecord*& rakCRecord)
{
    // An array for temporary storage of indices.
    int iIQuantity = 3*m_iTQuantity;
    int* aiIndex = (iIQuantity > 0 ? WM4_NEW int[iIQuantity] : 0);

    // Build the collapse records for the caller.
    riCQuantity = (int)m_kEDelete.size() + 1;
    rakCRecord = WM4_NEW CollapseRecord[riCQuantity];

    // Initial record only stores the initial vertex and triangle quantities.
    rakCRecord[0].VQuantity = m_iVQuantity;
    rakCRecord[0].TQuantity = m_iTQuantity;

    // Construct a multimap of the connectivity information.
    typedef std::multimap<int,int> CMap;
    CMap kCMap;
    int i;
    for (i = 0; i < iIQuantity; i++)
    {
        kCMap.insert(CMap::value_type(m_aiConnect[i],i));
    }

    // Construct the replacement arrays.
    int iVQuantity = m_iVQuantity, iTQuantity = m_iTQuantity;
    int iR;
    for (iR = 0; iR < (int)m_kEDelete.size(); iR++)
    {
        CollapseRecord& rkERecord = m_kEDelete[iR];
        CollapseRecord& rkRecord = rakCRecord[iR+1];

        iVQuantity -= rkERecord.VQuantity;
        iTQuantity -= rkERecord.TQuantity;

        rkRecord.VKeep = rkERecord.VKeep;
        rkRecord.VThrow = rkERecord.VThrow;
        rkRecord.VQuantity = iVQuantity;
        rkRecord.TQuantity = iTQuantity;
        rkRecord.IQuantity = 0;

        if (iTQuantity > 0)
        {
            int iMax = 3*iTQuantity;

            std::pair<CMap::iterator,CMap::iterator> kIterPair =
                kCMap.equal_range(rkRecord.VThrow);
            CMap::iterator pkIter = kIterPair.first;
            while (pkIter != kIterPair.second)
            {
                CMap::iterator pkTmpIter = pkIter++;
                int j = pkTmpIter->second;
                if (j < iMax)
                {
                    kCMap.erase(pkTmpIter);
                    kCMap.insert(CMap::value_type(rkRecord.VKeep,j));
                    aiIndex[rkRecord.IQuantity++] = j;
                }
            }

            if (rkRecord.IQuantity > 0)
            {
                rkRecord.Index = WM4_NEW int[rkRecord.IQuantity];
                size_t uiSize = rkRecord.IQuantity*sizeof(int);
                System::Memcpy(rkRecord.Index,uiSize,aiIndex,uiSize);
            }
        }
        else
        {
            rkRecord.Index = 0;
        }
    }

    // Fill in the connectivity array.
    CMap::iterator pkIter;
    for (pkIter = kCMap.begin(); pkIter != kCMap.end(); pkIter++)
    {
        m_aiConnect[pkIter->second] = pkIter->first;
    }

    // Expand mesh back to original.
    for (iR = riCQuantity - 1; iR > 0; iR--)
    {
        // Restore indices in connectivity array.
        CollapseRecord& rkRecord = rakCRecord[iR];
        for (i = 0; i < rkRecord.IQuantity; i++)
        {
            int iC = rkRecord.Index[i];
            assert(m_aiConnect[iC] == rkRecord.VKeep);
            m_aiConnect[iC] = rkRecord.VThrow;
        }
    }

    WM4_DELETE[] aiIndex;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// heap operations
//----------------------------------------------------------------------------
void CreateClodMesh::InitializeHeap ()
{
    // It is possible that during an edge collapse, the number of *temporary*
    // edges is larger than the original number of edges in the mesh.  To
    // make sure there is enough heap space, allocate two times the number of
    // original edges.
    m_iHQuantity = (int)m_kEMap.size();
    m_apkHeap = WM4_NEW HeapRecord*[2*m_iHQuantity];

    int iHIndex = 0;
    for (MEIter pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++, iHIndex++)
    {
        m_apkHeap[iHIndex] = pkE->second.Record;
        m_apkHeap[iHIndex]->m_kEdge = pkE->first;
        m_apkHeap[iHIndex]->m_iHIndex = iHIndex;
        m_apkHeap[iHIndex]->m_fMetric = GetMetric(pkE);
    }

    Sort();
}
//----------------------------------------------------------------------------
void CreateClodMesh::Sort ()
{
    int iLast = m_iHQuantity - 1;
    for (int iLeft = iLast/2; iLeft >= 0; iLeft--)
    {
        HeapRecord* pkRecord = m_apkHeap[iLeft];
        int iPa = iLeft, iCh = 2*iLeft + 1;
        while (iCh <= iLast)
        {
            if (iCh < iLast)
            {
                int iChP = iCh+1;
                if (m_apkHeap[iCh]->m_fMetric > m_apkHeap[iChP]->m_fMetric)
                {
                    iCh = iChP;
                }
            }

            if (m_apkHeap[iCh]->m_fMetric >= pkRecord->m_fMetric)
            {
                break;
            }

            m_apkHeap[iCh]->m_iHIndex = iPa;
            m_apkHeap[iPa] = m_apkHeap[iCh];
            iPa = iCh;
            iCh = 2*iCh + 1;
        }

        pkRecord->m_iHIndex = iPa;
        m_apkHeap[iPa] = pkRecord;
    }
}
//----------------------------------------------------------------------------
void CreateClodMesh::Add (float fMetric)
{
    // Under normal heap operations, you would have to make sure that the
    // heap storage grows if necessary.  Increased storage demand will not
    // happen in this application.  The creation of the heap record itself is
    // done in OnEdgeCreate.
    m_iHQuantity++;

    int iCh = m_iHQuantity - 1;
    HeapRecord* pkRecord = m_apkHeap[iCh];
    pkRecord->m_fMetric = fMetric;
    while (iCh > 0)
    {
        int iPa = (iCh-1)/2;
        if (m_apkHeap[iPa]->m_fMetric <= fMetric)
        {
            break;
        }

        m_apkHeap[iPa]->m_iHIndex = iCh;
        m_apkHeap[iCh] = m_apkHeap[iPa];
        pkRecord->m_iHIndex = iPa;
        m_apkHeap[iPa] = pkRecord;
        iCh = iPa;
    }
}
//----------------------------------------------------------------------------
void CreateClodMesh::Remove ()
{
    HeapRecord* pkRoot = m_apkHeap[0];

    int iLast = m_iHQuantity - 1;
    HeapRecord* pkRecord = m_apkHeap[iLast];
    int iPa = 0, iCh = 1;
    while (iCh <= iLast)
    {
        if (iCh < iLast)
        {
            int iChP = iCh+1;
            if (m_apkHeap[iCh]->m_fMetric > m_apkHeap[iChP]->m_fMetric)
            {
                iCh = iChP;
            }
        }

        if (m_apkHeap[iCh]->m_fMetric >= pkRecord->m_fMetric)
        {
            break;
        }

        m_apkHeap[iCh]->m_iHIndex = iPa;
        m_apkHeap[iPa] = m_apkHeap[iCh];
        iPa = iCh;
        iCh = 2*iCh + 1;
    }

    pkRecord->m_iHIndex = iPa;
    m_apkHeap[iPa] = pkRecord;
    m_iHQuantity--;

    // To notify OnEdgeDestroy that this edge was already removed from the
    // heap, but the object must be deleted by that callback.
    pkRoot->m_iHIndex = -1;
}
//----------------------------------------------------------------------------
void CreateClodMesh::Update (int iHIndex, float fMetric)
{
    HeapRecord* pkRecord = m_apkHeap[iHIndex];
    int iPa, iCh, iChP, iMaxCh;

    if (fMetric > pkRecord->m_fMetric)
    {
        pkRecord->m_fMetric = fMetric;

        // The new weight is larger than the old weight.  Propagate it
        // towards the leaves.
        iPa = iHIndex;
        iCh = 2*iPa+1;
        while (iCh < m_iHQuantity)
        {
            // At least one child exists.
            if (iCh < m_iHQuantity-1)
            {
                // Two children exist.
                iChP = iCh+1;
                if (m_apkHeap[iCh]->m_fMetric <= m_apkHeap[iChP]->m_fMetric)
                {
                    iMaxCh = iCh;
                }
                else
                {
                    iMaxCh = iChP;
                }
            }
            else
            {
                // One child exists.
                iMaxCh = iCh;
            }

            if (m_apkHeap[iMaxCh]->m_fMetric >= fMetric)
            {
                break;
            }

            m_apkHeap[iMaxCh]->m_iHIndex = iPa;
            m_apkHeap[iPa] = m_apkHeap[iMaxCh];
            pkRecord->m_iHIndex = iMaxCh;
            m_apkHeap[iMaxCh] = pkRecord;
            iPa = iMaxCh;
            iCh = 2*iPa+1;
        }
    }
    else if (fMetric < pkRecord->m_fMetric)
    {
        pkRecord->m_fMetric = fMetric;

        // The new weight is smaller than the old weight.  Propagate it
        // towards the root.
        iCh = iHIndex;
        while (iCh > 0)
        {
            // A parent exists.
            iPa = (iCh-1)/2;

            if (m_apkHeap[iPa]->m_fMetric <= fMetric)
            {
                break;
            }

            m_apkHeap[iPa]->m_iHIndex = iCh;
            m_apkHeap[iCh] = m_apkHeap[iPa];
            pkRecord->m_iHIndex = iPa;
            m_apkHeap[iPa] = pkRecord;
            iCh = iPa;
        }
    }
}
//----------------------------------------------------------------------------
bool CreateClodMesh::IsValidHeap (int iStart, int iFinal)
{
    for (int iC = iStart; iC <= iFinal; iC++)
    {
        int iP = (iC-1)/2;
        if (iP > iStart)
        {
            if (m_apkHeap[iP]->m_fMetric > m_apkHeap[iC]->m_fMetric)
            {
                return false;
            }

            if (m_apkHeap[iP]->m_iHIndex != iP)
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool CreateClodMesh::IsValidHeap ()
{
    return IsValidHeap(0,m_iHQuantity-1);
}
//----------------------------------------------------------------------------
void CreateClodMesh::PrintHeap (const char* acFilename)
{
    std::ofstream kOStr(acFilename);
    for (int i = 0; i < m_iHQuantity; i++)
    {
        HeapRecord* pkRecord = m_apkHeap[i];
        kOStr << pkRecord->m_iHIndex << "= <" << pkRecord->m_kEdge.m_aiV[0]
              << ',' << pkRecord->m_kEdge.m_aiV[1] << "> "
              << pkRecord->m_fMetric << std::endl;

        int iValue = i+2;
        int iBits = 0;
        while (iValue != 0)
        {
            if (iValue & 1)
            {
                iBits++;
            }
            iValue >>= 1;
        }
        if (iBits == 1)
        {
            kOStr << std::endl;
        }
    }
}
//----------------------------------------------------------------------------
void CreateClodMesh::Print (const char* acFilename) const
{
    std::ofstream kOStr(acFilename);
    int i;

    // Print the vertices.
    kOStr << "vertex quantity = " << (int)m_kVMap.size() << std::endl;
    for (MVCIter pkVM = m_kVMap.begin(); pkVM != m_kVMap.end(); pkVM++)
    {
        kOStr << "v<" << pkVM->first << "> : e ";

        const TSmallUnorderedSet<Edge>& rkESet = pkVM->second.m_kESet;
        for (i = 0; i < rkESet.GetQuantity(); i++)
        {
            kOStr << '<' << rkESet[i].m_aiV[0]
                  << ',' << rkESet[i].m_aiV[1]
                  << "> ";
        }

        kOStr << ": t ";
        const TSmallUnorderedSet<Triangle>& rkTSet = pkVM->second.m_kTSet;
        for (i = 0; i < rkTSet.GetQuantity(); i++)
        {
            kOStr << '<' << rkTSet[i].m_aiV[0]
                  << ',' << rkTSet[i].m_aiV[1]
                  << ',' << rkTSet[i].m_aiV[2]
                  << "> ";
        }
        kOStr << std::endl;
    }
    kOStr << std::endl;

    // Print the edges.
    kOStr << "edge quantity = " << (int)m_kEMap.size() << std::endl;
    for (MECIter pkEM = m_kEMap.begin(); pkEM != m_kEMap.end(); pkEM++)
    {
        kOStr << "e<" << pkEM->first.m_aiV[0] << ',' << pkEM->first.m_aiV[1];
        kOStr << "> : t ";
        const TSmallUnorderedSet<Triangle>& rkTSet = pkEM->second.m_kTSet;
        for (i = 0; i < rkTSet.GetQuantity(); i++)
        {
            kOStr << '<' << rkTSet[i].m_aiV[0]
                  << ',' << rkTSet[i].m_aiV[1]
                  << ',' << rkTSet[i].m_aiV[2]
                  << "> ";
        }
        kOStr << std::endl;
    }
    kOStr << std::endl;

    // Print the triangles.
    kOStr << "triangle quantity = " << (int)m_kTMap.size() << std::endl;
    for (MTCIter pkTM = m_kTMap.begin(); pkTM != m_kTMap.end(); pkTM++)
    {
        kOStr << "t<" << pkTM->first.m_aiV[0] << ',' << pkTM->first.m_aiV[1];
        kOStr << ',' << pkTM->first.m_aiV[2]  << ">" << std::endl;
    }
    kOStr << std::endl;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mesh insert/remove callbacks
//----------------------------------------------------------------------------
void CreateClodMesh::OnEdgeInsert (const Edge& rkE, bool bCreate,
    HeapRecord*& rpkRecord)
{
    if (bCreate)
    {
        rpkRecord = WM4_NEW HeapRecord;
        if (m_bCollapsing)
        {
            m_apkHeap[m_iHQuantity] = rpkRecord;
            m_apkHeap[m_iHQuantity]->m_kEdge = rkE;
            m_apkHeap[m_iHQuantity]->m_iHIndex = m_iHQuantity;
            Add(GetMetric(m_kEMap.find(rkE)));
        }
    }
    else
    {
        if (m_bCollapsing)
        {
            HeapRecord* pkRecord = rpkRecord;
            assert(pkRecord->m_kEdge == rkE);
            if (pkRecord->m_iHIndex >= 0)
            {
                Update(pkRecord->m_iHIndex,GetMetric(m_kEMap.find(rkE)));
            }
            else
            {
                assert(pkRecord->m_iHIndex == -1);
                pkRecord->m_iHIndex = m_iHQuantity;
                Add(GetMetric(m_kEMap.find(rkE)));
            }
        }
    }
}
//----------------------------------------------------------------------------
void CreateClodMesh::OnEdgeRemove (const Edge&, bool bDestroy,
    HeapRecord* pkRecord)
{
    // Remove the edge from the heap.  The metric of the edge is set to
    // -INFINITY so that it has the minimum value of all edges.  The update
    // call bubbles the edge to the root of the heap.  The edge is then
    // removed from the root.

    if (bDestroy)
    {
        if (pkRecord->m_iHIndex >= 0)
        {
            Update(pkRecord->m_iHIndex,-Mathf::MAX_REAL);
            Remove();
        }
        WM4_DELETE pkRecord;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// CreateClodMesh::Edge
//----------------------------------------------------------------------------
CreateClodMesh::Edge::Edge ()
{
}
//----------------------------------------------------------------------------
CreateClodMesh::Edge::Edge (int iV0, int iV1)
{
    if (iV0 < iV1)
    {
        // v0 is minimum
        m_aiV[0] = iV0;
        m_aiV[1] = iV1;
    }
    else
    {
        // v1 is minimum
        m_aiV[0] = iV1;
        m_aiV[1] = iV0;
    }
}
//----------------------------------------------------------------------------
bool CreateClodMesh::Edge::operator< (const Edge& rkE) const
{
    if (m_aiV[1] < rkE.m_aiV[1])
    {
        return true;
    }

    if (m_aiV[1] == rkE.m_aiV[1])
    {
        return m_aiV[0] < rkE.m_aiV[0];
    }

    return false;
}
//----------------------------------------------------------------------------
bool CreateClodMesh::Edge::operator== (const Edge& rkE) const
{
    return m_aiV[0] == rkE.m_aiV[0] && m_aiV[1] == rkE.m_aiV[1];
}
//----------------------------------------------------------------------------
bool CreateClodMesh::Edge::operator!= (const Edge& rkE) const
{
    return !operator==(rkE);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// CreateClodMesh::Triangle
//----------------------------------------------------------------------------
CreateClodMesh::Triangle::Triangle ()
{
}
//----------------------------------------------------------------------------
CreateClodMesh::Triangle::Triangle (int iV0, int iV1, int iV2)
{
    if (iV0 < iV1)
    {
        if (iV0 < iV2)
        {
            // V0 is minimum.
            m_aiV[0] = iV0;
            m_aiV[1] = iV1;
            m_aiV[2] = iV2;
        }
        else
        {
            // V2 is minimum.
            m_aiV[0] = iV2;
            m_aiV[1] = iV0;
            m_aiV[2] = iV1;
        }
    }
    else
    {
        if (iV1 < iV2)
        {
            // V1 is minimum.
            m_aiV[0] = iV1;
            m_aiV[1] = iV2;
            m_aiV[2] = iV0;
        }
        else
        {
            // V2 is minimum.
            m_aiV[0] = iV2;
            m_aiV[1] = iV0;
            m_aiV[2] = iV1;
        }
    }
}
//----------------------------------------------------------------------------
bool CreateClodMesh::Triangle::operator< (const Triangle& rkT) const
{
    if (m_aiV[2] < rkT.m_aiV[2])
    {
        return true;
    }

    if (m_aiV[2] == rkT.m_aiV[2])
    {
        if (m_aiV[1] < rkT.m_aiV[1])
        {
            return true;
        }

        if (m_aiV[1] == rkT.m_aiV[1])
        {
            return m_aiV[0] < rkT.m_aiV[0];
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool CreateClodMesh::Triangle::operator== (const Triangle& rkT) const
{
    return (m_aiV[0] == rkT.m_aiV[0]) &&
          ((m_aiV[1] == rkT.m_aiV[1] && m_aiV[2] == rkT.m_aiV[2]) ||
           (m_aiV[1] == rkT.m_aiV[2] && m_aiV[2] == rkT.m_aiV[1]));
}
//----------------------------------------------------------------------------
bool CreateClodMesh::Triangle::operator!= (const Triangle& rkT) const
{
    return !operator==(rkT);
}
//----------------------------------------------------------------------------
