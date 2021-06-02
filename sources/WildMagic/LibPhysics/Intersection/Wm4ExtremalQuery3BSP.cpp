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
// Version: 4.0.2 (2009/02/05)

#include "Wm4PhysicsPCH.h"
#include "Wm4ExtremalQuery3BSP.h"
#include "Wm4BasicMesh.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
ExtremalQuery3BSP<Real>::ExtremalQuery3BSP (
    const ConvexPolyhedron3<Real>& rkPolytope)
    :
    ExtremalQuery3<Real>(rkPolytope)
{
    // Create the adjacency information for the polytope.
    int iVQuantity = m_pkPolytope->GetVQuantity();
    const Vector3<Real>* akVertex = m_pkPolytope->GetVertices();
    int iTQuantity = m_pkPolytope->GetTQuantity();
    const int* aiIndex = m_pkPolytope->GetIndices();
    BasicMesh kMesh(iVQuantity,akVertex,iTQuantity,aiIndex);

    // Create the set of unique arcs which are used to create the BSP tree.
    std::multiset<SphericalArc> kArcs;
    CreateSphericalArcs(kMesh,kArcs);

    // Create the BSP tree to be used in the extremal query.
    std::vector<SphericalArc> kNodes;
    CreateBSPTree(kArcs,kNodes);

    // Copy the nodes into a single, nonresizeable array.
    m_iNodeQuantity = (int)kNodes.size();
    m_akNode = WM4_NEW SphericalArc[m_iNodeQuantity];
    size_t uiSize = m_iNodeQuantity*sizeof(SphericalArc);
    System::Memcpy(m_akNode,uiSize,&kNodes.front(),uiSize);
}
//----------------------------------------------------------------------------
template <class Real>
ExtremalQuery3BSP<Real>::~ExtremalQuery3BSP ()
{
    WM4_DELETE[] m_akNode;
}
//----------------------------------------------------------------------------
template <class Real>
void ExtremalQuery3BSP<Real>::GetExtremeVertices (
    const Vector3<Real>& rkDirection, int& riPositiveDirection,
    int& riNegativeDirection)
{
    // Do a nonrecursive depth-first search of the BSP tree to determine
    // spherical polygon contains the incoming direction D.
    int iCurrent = 0;  // the root of the BSP tree
    while (iCurrent >= 0)
    {
        SphericalArc& rkCurrent = m_akNode[iCurrent];
        int iSign = (int)Math<Real>::Sign(rkDirection.Dot(rkCurrent.Normal));
        if (iSign >= 0)
        {
            iCurrent = rkCurrent.PosChild;
            if (iCurrent == -1)
            {
                // At a leaf node.
                riPositiveDirection = rkCurrent.PosVertex;
            }
        }
        else
        {
            iCurrent = rkCurrent.NegChild;
            if (iCurrent == -1)
            {
                // At a leaf node.
                riPositiveDirection = rkCurrent.NegVertex;
            }
        }
    }

    // Do a nonrecursive depth-first search of the BSP tree to determine
    // spherical polygon contains the reverse incoming direction -D.
    iCurrent = 0;  // the root of the BSP tree
    while (iCurrent >= 0)
    {
        SphericalArc& rkCurrent = m_akNode[iCurrent];
        int iSign = (int)-Math<Real>::Sign(rkDirection.Dot(rkCurrent.Normal));
        if (iSign >= 0)
        {
            iCurrent = rkCurrent.PosChild;
            if (iCurrent == -1)
            {
                // At a leaf node.
                riNegativeDirection = rkCurrent.PosVertex;
            }
        }
        else
        {
            iCurrent = rkCurrent.NegChild;
            if (iCurrent == -1)
            {
                // At a leaf node.
                riNegativeDirection = rkCurrent.NegVertex;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
int ExtremalQuery3BSP<Real>::GetNodeQuantity () const
{
    return m_iNodeQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
int ExtremalQuery3BSP<Real>::GetTreeDepth () const
{
    return m_iTreeDepth;
}
//----------------------------------------------------------------------------
template <class Real>
void ExtremalQuery3BSP<Real>::SortVertexAdjacents (BasicMesh& rkMesh)
{
    // The typecast is to allow modifying the vertices.  As long as the
    // sorting algorithm is correct, this is a safe thing to do.
    int iVQuantity = rkMesh.GetVQuantity();
    BasicMesh::Vertex* akVertex = (BasicMesh::Vertex*)rkMesh.GetVertices();

    const BasicMesh::Triangle* akTriangle = rkMesh.GetTriangles();
    for (int i = 0; i < iVQuantity; i++)
    {
        // This copy circumvents the constness of the mesh vertices, which
        // allows the sorting of the mesh triangles shared by a mesh vertex.
        BasicMesh::Vertex& rkVertex = akVertex[i];

        // This is a consequence of the mesh being a polyhedron.
        assert(rkVertex.VQuantity == rkVertex.TQuantity);

        // Once we have the first vertex to sort and an initial triangle
        // sharing it, we can walk around the vertex following triangle
        // adjacency links.  It is safe to overwrite the vertex data.
        int iTIndex = rkVertex.T[0];
        const BasicMesh::Triangle* pkTri = &akTriangle[iTIndex];

        for (int iAdj = 0; iAdj < rkVertex.VQuantity; iAdj++)
        {
            int iPrev, iCurr;
            for (iPrev = 2, iCurr = 0; iCurr < 3; iPrev = iCurr++)
            {
                if (pkTri->V[iCurr] == i)
                {
                    rkVertex.V[iAdj] = pkTri->V[iPrev];
                    rkVertex.E[iAdj] = pkTri->E[iPrev];
                    rkVertex.T[iAdj] = iTIndex;

                    // next triangle to visit
                    iTIndex = pkTri->T[iPrev];
                    pkTri = &akTriangle[iTIndex];
                    break;
                }
            }
            assert(iCurr < 3);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void ExtremalQuery3BSP<Real>::CreateSphericalArcs (BasicMesh& rkMesh,
    std::multiset<SphericalArc>& rkArcs)
{
    int iEQuantity = rkMesh.GetEQuantity();
    const BasicMesh::Edge* akEdge = rkMesh.GetEdges();
    const BasicMesh::Triangle* akTriangle = rkMesh.GetTriangles();

    const int aiPrev[3] = { 2, 0, 1 };
    const int aiNext[3] = { 1, 2, 0 };

    for (int i = 0; i < iEQuantity; i++)
    {
        const BasicMesh::Edge& rkEdge = akEdge[i];

        SphericalArc kArc;
        kArc.N[0] = rkEdge.T[0];
        kArc.N[1] = rkEdge.T[1];
        kArc.Separation = 1;

        kArc.Normal = m_akFaceNormal[kArc.N[0]].Cross(
            m_akFaceNormal[kArc.N[1]]);

        const BasicMesh::Triangle& rkAdj = akTriangle[rkEdge.T[0]];
        int j;
        for (j = 0; j < 3; j++)
        {
            if (rkAdj.V[j] != rkEdge.V[0] && rkAdj.V[j] != rkEdge.V[1])
            {
                kArc.PosVertex = rkAdj.V[aiPrev[j]];
                kArc.NegVertex = rkAdj.V[aiNext[j]];
                break;
            }
        }
        assert(j < 3);

        rkArcs.insert(kArc);
    }

    CreateSphericalBisectors(rkMesh,rkArcs);
}
//----------------------------------------------------------------------------
template <class Real>
void ExtremalQuery3BSP<Real>::CreateSphericalBisectors (BasicMesh& rkMesh,
    std::multiset<SphericalArc>& rkArcs)
{
    // For each vertex, sort the normals into a counterclockwise spherical
    // polygon when viewed from outside the sphere.
    SortVertexAdjacents(rkMesh);

    int iVQuantity = rkMesh.GetVQuantity();
    const BasicMesh::Vertex* akVertex = rkMesh.GetVertices();
    std::queue<std::pair<int,int> > kQueue;
    for (int i = 0; i < iVQuantity; i++)
    {
        const BasicMesh::Vertex& rkVertex = akVertex[i];

        kQueue.push(std::make_pair(0,rkVertex.TQuantity));
        while (!kQueue.empty())
        {
            std::pair<int,int> kArc = kQueue.front();
            kQueue.pop();
            int i0 = kArc.first, i1 = kArc.second;
            int iSeparation = i1 - i0;
            if (iSeparation > 1 && iSeparation != rkVertex.TQuantity-1)
            {
                if (i1 < rkVertex.TQuantity)
                {
                    SphericalArc kArc;
                    kArc.N[0] = rkVertex.T[i0];
                    kArc.N[1] = rkVertex.T[i1];
                    kArc.Separation = iSeparation;

                    kArc.Normal = m_akFaceNormal[kArc.N[0]].Cross(
                        m_akFaceNormal[kArc.N[1]]);

                    kArc.PosVertex = i;
                    kArc.NegVertex = i;
                    rkArcs.insert(kArc);
                }
                int iMid = (i0 + i1 + 1)/2;
                if (iMid != i1)
                {
                    kQueue.push(std::make_pair(i0,iMid));
                    kQueue.push(std::make_pair(iMid,i1));
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void ExtremalQuery3BSP<Real>::CreateBSPTree (
    std::multiset<SphericalArc>& rkArcs, std::vector<SphericalArc>& rkNodes)
{
    // The tree has at least a root.
    m_iTreeDepth = 1;

    typename std::multiset<SphericalArc>::reverse_iterator pkIter;
    for (pkIter = rkArcs.rbegin(); pkIter != rkArcs.rend(); pkIter++)
    {
        InsertArc(*pkIter,rkNodes);
    }

    // The leaf nodes are not counted in the traversal of InsertArc.  The
    // depth must be incremented to account for leaves.
    m_iTreeDepth++;
}
//----------------------------------------------------------------------------
template <class Real>
void ExtremalQuery3BSP<Real>::InsertArc (const SphericalArc& rkArc,
    std::vector<SphericalArc>& rkNodes)
{
    // The incoming arc is stored at the end of the nodes array.
    if (rkNodes.size() > 0)
    {
        // Do a nonrecursive depth-first search of the current BSP tree to
        // place the incoming arc.
        std::stack<int> kCandidates;
        kCandidates.push(0);  // the root of the BSP tree
        while (!kCandidates.empty())
        {
            int iCurrent = kCandidates.top();
            kCandidates.pop();
            SphericalArc* pkCurrent = &rkNodes[iCurrent];

            int iSign0;
            if (rkArc.N[0] == pkCurrent->N[0]
            ||  rkArc.N[0] == pkCurrent->N[1])
            {
                iSign0 = 0;
            }
            else
            {
                iSign0 = (int)Math<Real>::Sign(
                    m_akFaceNormal[rkArc.N[0]].Dot(pkCurrent->Normal));
            }

            int iSign1;
            if (rkArc.N[1] == pkCurrent->N[0]
            ||  rkArc.N[1] == pkCurrent->N[1])
            {
                iSign1 = 0;
            }
            else
            {
                iSign1 = (int)Math<Real>::Sign(
                    m_akFaceNormal[rkArc.N[1]].Dot(pkCurrent->Normal));
            }

            int iDoTest = 0;
            if (iSign0*iSign1 < 0)
            {
                // The new arc straddles the current arc, so propagate it
                // to both child nodes.
                iDoTest = 3;
            }
            else if (iSign0 > 0 || iSign1 > 0)
            {
                // The new arc is on the positive side of the current arc.
                iDoTest = 1;
            }
            else if (iSign0 < 0 || iSign1 < 0)
            {
                // The new arc is on the negative side of the current arc.
                iDoTest = 2;
            }
            // Else iSign0 = iSign1 = 0, in which case no propagation is
            // needed since the current BSP node will handle the correct
            // partitioning of the arcs during extremal queries.

            int iDepth;

            if (iDoTest & 1)
            {
                if (pkCurrent->PosChild != -1)
                {
                    kCandidates.push(pkCurrent->PosChild);
                    iDepth = (int)kCandidates.size();
                    if (iDepth > m_iTreeDepth)
                    {
                        m_iTreeDepth = iDepth;
                    }
                }
                else
                {
                    pkCurrent->PosChild = (int)rkNodes.size();
                    rkNodes.push_back(rkArc);

                    // The push_back can cause a reallocation, so the current
                    // pointer must be refreshed.
                    pkCurrent = &rkNodes[iCurrent];
                }
            }

            if (iDoTest & 2)
            {
                if (pkCurrent->NegChild != -1)
                {
                    kCandidates.push(pkCurrent->NegChild);
                    iDepth = (int)kCandidates.size();
                    if (iDepth > m_iTreeDepth)
                    {
                        m_iTreeDepth = iDepth;
                    }
                }
                else
                {
                    pkCurrent->NegChild = (int)rkNodes.size();
                    rkNodes.push_back(rkArc);
                }
            }
        }
    }
    else
    {
        // root node
        rkNodes.push_back(rkArc);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class ExtremalQuery3BSP<float>;

template WM4_PHYSICS_ITEM
class ExtremalQuery3BSP<double>;
//----------------------------------------------------------------------------
}
