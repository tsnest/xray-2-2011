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

// #define _DEBUG_TEST

#include "Wm4GraphicsPCH.h"
#include "Wm4BoundingVolumeTree.h"
using namespace Wm4;

BoundingVolumeTree::CreatorM
BoundingVolumeTree::ms_aoCreateModelBound[BoundingVolume::BV_QUANTITY];

BoundingVolumeTree::CreatorW
BoundingVolumeTree::ms_aoCreateWorldBound[BoundingVolume::BV_QUANTITY];

//----------------------------------------------------------------------------
BoundingVolumeTree::BoundingVolumeTree (const TriMesh* pkMesh)
{
    m_pkMesh = pkMesh;
    m_pkLChild = 0;
    m_pkRChild = 0;
    m_iTriangleQuantity = 0;
    m_aiTriangle = 0;
}
//----------------------------------------------------------------------------
BoundingVolumeTree::BoundingVolumeTree (int eBVType, const TriMesh* pkMesh,
    int iMaxTrisPerLeaf, bool bStoreInteriorTris)
    :
    m_pkMesh(pkMesh)
{
    // Centroids of triangles are used for splitting a mesh.  The centroids
    // are projected onto a splitting axis and sorted.  The split is based
    // on the median of the projections.
    int iTQuantity = m_pkMesh->IBuffer->GetIndexQuantity()/3;
    const int* aiIndex = m_pkMesh->IBuffer->GetData();
    Vector3f* akCentroid = WM4_NEW Vector3f[iTQuantity];
    const float fOneThird = 1.0f/3.0f;
    int iT, i;
    for (iT = 0, i = 0; iT < iTQuantity; iT++)
    {
        int i0 = aiIndex[i++];
        int i1 = aiIndex[i++];
        int i2 = aiIndex[i++];
        akCentroid[iT] = fOneThird*(
            pkMesh->VBuffer->Position3(i0) +
            pkMesh->VBuffer->Position3(i1) +
            pkMesh->VBuffer->Position3(i2));
    }

    // Initialize binary-tree arrays for storing triangle indices.  These
    // are used to store the indices when the mesh is split.
    int* aiISplit = WM4_NEW int[iTQuantity];
    int* aiOSplit = WM4_NEW int[iTQuantity];
    for (iT = 0; iT < iTQuantity; iT++)
    {
        aiISplit[iT] = iT;
    }

    BuildTree(eBVType,iMaxTrisPerLeaf,bStoreInteriorTris,akCentroid,0,
        iTQuantity-1,aiISplit,aiOSplit);

    WM4_DELETE[] akCentroid;
    WM4_DELETE[] aiISplit;
    WM4_DELETE[] aiOSplit;

#ifdef _DEBUG_TEST
    if (bStoreInteriorTris)
    {
        const float fEpsilon = 1e-05f;
        bool bSuccess = ContainsLeafData(pkMesh->VBuffer,aiIndex,fEpsilon);
        assert( bSuccess );
    }
#endif
}
//----------------------------------------------------------------------------
BoundingVolumeTree::~BoundingVolumeTree ()
{
    WM4_DELETE[] m_aiTriangle;
    WM4_DELETE m_pkLChild;
    WM4_DELETE m_pkRChild;
}
//----------------------------------------------------------------------------
void BoundingVolumeTree::BuildTree (int eBVType, int iMaxTrisPerLeaf,
    bool bStoreInteriorTris, const Vector3f* akCentroid, int i0, int i1,
    int* aiISplit, int* aiOSplit)
{
    assert(i0 <= i1);

    Line3f kLine;
    m_spkModelBound = ms_aoCreateModelBound[eBVType](m_pkMesh,i0,i1,aiISplit,
        kLine);
    m_spkWorldBound = ms_aoCreateWorldBound[eBVType]();

    if (i1 - i0 < iMaxTrisPerLeaf)
    {
        // leaf node
        m_iTriangleQuantity = i1 - i0 + 1;
        m_aiTriangle = WM4_NEW int[m_iTriangleQuantity];
        size_t uiSize = m_iTriangleQuantity*sizeof(int);
        System::Memcpy(m_aiTriangle,uiSize,&aiISplit[i0],uiSize);

        m_pkLChild = 0;
        m_pkRChild = 0;
    }
    else
    {
        // interior node
        if (bStoreInteriorTris)
        {
            m_iTriangleQuantity = i1 - i0 + 1;
            m_aiTriangle = WM4_NEW int[m_iTriangleQuantity];
            size_t uiSize = m_iTriangleQuantity*sizeof(int);
            System::Memcpy(m_aiTriangle,uiSize,&aiISplit[i0],uiSize);
        }
        else
        {
            m_iTriangleQuantity = 0;
            m_aiTriangle = 0;
        }

        int j0, j1;
        SplitTriangles(akCentroid,i0,i1,aiISplit,j0,j1,aiOSplit,kLine);

        m_pkLChild = WM4_NEW BoundingVolumeTree(m_pkMesh);
        m_pkLChild->BuildTree(eBVType,iMaxTrisPerLeaf,bStoreInteriorTris,
            akCentroid,i0,j0,aiOSplit,aiISplit);

        m_pkRChild = WM4_NEW BoundingVolumeTree(m_pkMesh);
        m_pkRChild->BuildTree(eBVType,iMaxTrisPerLeaf,bStoreInteriorTris,
            akCentroid,j1,i1,aiOSplit,aiISplit);
    }
}
//----------------------------------------------------------------------------
int BoundingVolumeTree::Compare (const void* pvElement0,
    const void* pvElement1)
{
    const ProjectionInfo* pInfo0 = (const ProjectionInfo*) pvElement0;
    const ProjectionInfo* pInfo1 = (const ProjectionInfo*) pvElement1;

    if (pInfo0->m_fProjection < pInfo1->m_fProjection)
    {
        return -1;
    }

    if (pInfo0->m_fProjection > pInfo1->m_fProjection)
    {
        return +1;
    }

    return 0;
}
//----------------------------------------------------------------------------
void BoundingVolumeTree::SplitTriangles (const Vector3f* akCentroid,
    int i0, int i1, int* aiISplit, int& rj0, int& rj1, int* aiOSplit,
    const Line3f& rkLine)
{
    // project onto specified line
    int iQuantity = i1 - i0 + 1;
    ProjectionInfo* akInfo = WM4_NEW ProjectionInfo[iQuantity];
    int i, j;
    for (i = i0, j = 0; i <= i1; i++, j++)
    {
        int iTriangle = aiISplit[i];
        Vector3f kDiff = akCentroid[iTriangle] - rkLine.Origin;
        akInfo[j].m_iTriangle = iTriangle;
        akInfo[j].m_fProjection = rkLine.Direction.Dot(kDiff);
    }

    // find median of projections by sorting
    qsort(akInfo,iQuantity,sizeof(ProjectionInfo),Compare);
    int iMedian = (iQuantity-1)/2;

    // partition the triangles by the median
    for (j = 0, rj0 = i0-1; j <= iMedian; j++)
    {
        aiOSplit[++rj0] = akInfo[j].m_iTriangle;
    }
    for (rj1 = i1+1; j < iQuantity; j++)
    {
        aiOSplit[--rj1] = akInfo[j].m_iTriangle;
    }

    WM4_DELETE[] akInfo;
}
//----------------------------------------------------------------------------
void BoundingVolumeTree::UpdateWorldBound ()
{
    m_spkModelBound->TransformBy(m_pkMesh->World,m_spkWorldBound);
}
//----------------------------------------------------------------------------

#ifdef _DEBUG_TEST

bool BoundingVolumeTree::ContainsLeafData (const VertexBuffer* pkVBuffer,
    const int* aiIndex, float fEpsilon) const
{
    if (m_pkLChild)
    {
        if (!m_pkLChild->ContainsLeafData(pkVBuffer,aiIndex,fEpsilon))
        {
            return false;
        }
    }

    if (m_pkRChild)
    {
        if (!m_pkRChild->ContainsLeafData(pkVBuffer,aiIndex,fEpsilon))
        {
            return false;
        }
    }

    for (int iT = 0; iT < m_iTriangleQuantity; iT++)
    {
        int j = 3*m_aiTriangle[iT];
        for (int i = 0; i < 3; i++)
        {
            Vector3f kPoint = pkVBuffer->Position3(aiIndex[j++]);
            if (!m_spkModelBound->Contains(kPoint))
            {
                return false;
            }
        }
    }

    return true;
}

#endif
