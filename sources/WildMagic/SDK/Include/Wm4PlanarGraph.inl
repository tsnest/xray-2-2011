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

//----------------------------------------------------------------------------
template <class Point2>
PlanarGraph<Point2>::PlanarGraph ()
{
}
//----------------------------------------------------------------------------
template <class Point2>
PlanarGraph<Point2>::~PlanarGraph ()
{
    typename Vertices::iterator pkIter;
    for (pkIter = m_kVertices.begin(); pkIter != m_kVertices.end(); pkIter++)
    {
        Vertex* pkVertex = pkIter->second;
        WM4_DELETE pkVertex;
    }
}
//----------------------------------------------------------------------------
template <class Point2>
const typename PlanarGraph<Point2>::Vertices&
PlanarGraph<Point2>::GetVertices () const
{
    return m_kVertices;
}
//----------------------------------------------------------------------------
template <class Point2>
const typename PlanarGraph<Point2>::Vertex*
PlanarGraph<Point2>::GetVertex (int iIndex) const
{
    typename Vertices::const_iterator pkIter = m_kVertices.find(iIndex);
    return (pkIter != m_kVertices.end() ? pkIter->second : 0);
}
//----------------------------------------------------------------------------
template <class Point2>
bool PlanarGraph<Point2>::InsertVertex (const Point2& rkPosition, int iIndex)
{
    typename Vertices::iterator pkIter = m_kVertices.find(iIndex);
    if (pkIter != m_kVertices.end())
    {
        return false;
    }

    // Insert the vertex into the vertex set.  The adjacency array has already
    // been initialized to empty.
    Vertex* pkVertex = WM4_NEW Vertex(rkPosition,iIndex);
    m_kVertices[iIndex] = pkVertex;
    return true;
}
//----------------------------------------------------------------------------
template <class Point2>
bool PlanarGraph<Point2>::RemoveVertex (int iIndex)
{
    typename Vertices::iterator pkIter = m_kVertices.find(iIndex);
    if (pkIter != m_kVertices.end())
    {
        Vertex* pkVertex = pkIter->second;
        if (pkVertex->Adjacent.size() == 0)
        {
            m_kVertices.erase(pkIter);
            WM4_DELETE pkVertex;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Point2>
const typename PlanarGraph<Point2>::Edges&
PlanarGraph<Point2>::GetEdges () const
{
    return m_kEdges;
}
//----------------------------------------------------------------------------
template <class Point2>
bool PlanarGraph<Point2>::InsertEdge (int iIndex0, int iIndex1)
{
    // Look up the vertices.  If one or the other does not exist, there is
    // nothing to do.  The typecast supports conceptual constness from the
    // users perspective.
    Vertex* pkVertex0 = (Vertex*)GetVertex(iIndex0);
    if (!pkVertex0)
    {
        return false;
    }

    Vertex* pkVertex1 = (Vertex*)GetVertex(iIndex1);
    if (!pkVertex1)
    {
        return false;
    }

    EdgeKey kEdge(iIndex0,iIndex1);
    std::map<EdgeKey,bool>::iterator pkIter = m_kEdges.find(kEdge);
    if (pkIter == m_kEdges.end())
    {
        // The edge does not exist, insert it into the set.  The edge is
        // tagged as "not a cycle".
        m_kEdges[kEdge] = false;

        // Update the vertex-adjacency information.  The graph is undirected,
        // so each vertex must know about the other.
        pkVertex0->Insert(pkVertex1);
        pkVertex1->Insert(pkVertex0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Point2>
bool PlanarGraph<Point2>::RemoveEdge (int iIndex0, int iIndex1)
{
    // Look up the vertices.  If one or the other does not exist, there is
    // nothing to do.  The typecast supports conceptual constness from the
    // users perspective.
    Vertex* pkVertex0 = (Vertex*)GetVertex(iIndex0);
    if (!pkVertex0)
    {
        return false;
    }

    Vertex* pkVertex1 = (Vertex*)GetVertex(iIndex1);
    if (!pkVertex1)
    {
        return false;
    }

    EdgeKey kEdge(iIndex0,iIndex1);
    std::map<EdgeKey,bool>::iterator pkIter = m_kEdges.find(kEdge);
    if (pkIter != m_kEdges.end())
    {
        // The edge exists, remove it from the set.
        m_kEdges.erase(pkIter);

        // Update the vertex-adjacency information.  The graph is undirected,
        // so each vertex knows about the other.
        pkVertex0->Remove(pkVertex1);
        pkVertex1->Remove(pkVertex0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Point2>
void PlanarGraph<Point2>::ExtractPrimitives (
    std::vector<Primitive*>& rkPrimitives)
{
    // Create a heap of vertices sorted lexicographically.
    std::set<VertexPtr> kHeap;
    typename Vertices::iterator pkIter;
    for (pkIter = m_kVertices.begin(); pkIter != m_kVertices.end(); pkIter++)
    {
        kHeap.insert(pkIter->second);
    }

    while (!kHeap.empty())
    {
        // Get the vertex of minimum x-value.
        VertexPtr kVPtr = *kHeap.begin();
        Vertex* pkV0 = (Vertex*)kVPtr;

        if (pkV0->Adjacent.size() == 0)
        {
            ExtractIsolatedVertex(pkV0,kHeap,rkPrimitives);
        }
        else if (pkV0->Adjacent.size() == 1)
        {
            ExtractFilament(pkV0,pkV0->Adjacent[0],kHeap,rkPrimitives);
        }
        else
        {
            // The primitive can be a filament or a minimal cycle.
            ExtractPrimitive(pkV0,kHeap,rkPrimitives);
        }
    }
}
//----------------------------------------------------------------------------
template <class Point2>
void PlanarGraph<Point2>::SetCycleEdge (int iIndex0, int iIndex1,
    bool bCycleEdge)
{
    EdgeKey kEdge(iIndex0,iIndex1);
    typename Edges::iterator pkIter = m_kEdges.find(kEdge);
    if (pkIter != m_kEdges.end())
    {
        pkIter->second = bCycleEdge;
    }
}
//----------------------------------------------------------------------------
template <class Point2>
bool PlanarGraph<Point2>::GetCycleEdge (int iIndex0, int iIndex1) const
{
    EdgeKey kEdge(iIndex0,iIndex1);
    typename Edges::const_iterator pkIter = m_kEdges.find(kEdge);
    if (pkIter != m_kEdges.end())
    {
        return pkIter->second;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Point2>
void PlanarGraph<Point2>::ExtractIsolatedVertex (Vertex* pkV0,
    std::set<VertexPtr>& rkHeap, std::vector<Primitive*>& rkPrimitives)
{
    Primitive* pkPrimitive = WM4_NEW Primitive(PT_ISOLATED_VERTEX);

    pkPrimitive->Sequence.push_back(
        std::make_pair(pkV0->Position,pkV0->Index));
    rkHeap.erase(pkV0);
    RemoveVertex(pkV0->Index);

    rkPrimitives.push_back(pkPrimitive);
}
//----------------------------------------------------------------------------
template <class Point2>
void PlanarGraph<Point2>::ExtractFilament (Vertex* pkV0, Vertex* pkV1,
    std::set<VertexPtr>& rkHeap, std::vector<Primitive*>& rkPrimitives)
{
    // (V0,V1) is the first edge of the purported filament.
    assert(pkV0->Adjacent.size() != 2);
    if (GetCycleEdge(pkV0->Index,pkV1->Index))
    {
        // The edge is from an earlier visited minimal cycle.  Delete the
        // purported filament because it is an imposter.
        if (pkV0->Adjacent.size() >= 3)
        {
            // V0 is a branch point.  Break the connection.
            RemoveEdge(pkV0->Index,pkV1->Index);
            pkV0 = pkV1;
            if (pkV0->Adjacent.size() == 1)
            {
                pkV1 = pkV0->Adjacent[0];
            }
        }

        while (pkV0->Adjacent.size() == 1)
        {
            pkV1 = pkV0->Adjacent[0];
            if (GetCycleEdge(pkV0->Index,pkV1->Index))
            {
                rkHeap.erase(pkV0);
                RemoveEdge(pkV0->Index,pkV1->Index);
                RemoveVertex(pkV0->Index);
                pkV0 = pkV1;
            }
            else
            {
                break;
            }
        }

        if (pkV0->Adjacent.size() == 0)
        {
            rkHeap.erase(pkV0);
            RemoveVertex(pkV0->Index);
        }
    }
    else
    {
        // A real filament has been found.
        Primitive* pkPrimitive = WM4_NEW Primitive(PT_FILAMENT);

        if (pkV0->Adjacent.size() >= 3)
        {
            // V0 is a branch point.  Store it and break the connection.
            pkPrimitive->Sequence.push_back(
                std::make_pair(pkV0->Position,pkV0->Index));
            RemoveEdge(pkV0->Index,pkV1->Index);
            pkV0 = pkV1;
            if (pkV0->Adjacent.size() == 1)
            {
                pkV1 = pkV0->Adjacent[0];
            }
        }

        while (pkV0->Adjacent.size() == 1)
        {
            pkV1 = pkV0->Adjacent[0];
            pkPrimitive->Sequence.push_back(
                std::make_pair(pkV0->Position,pkV0->Index));
            rkHeap.erase(pkV0);
            RemoveEdge(pkV0->Index,pkV1->Index);
            RemoveVertex(pkV0->Index);
            pkV0 = pkV1;
        }

        pkPrimitive->Sequence.push_back(
            std::make_pair(pkV0->Position,pkV0->Index));

        if (pkV0->Adjacent.size() == 0)
        {
            rkHeap.erase(pkV0);
            RemoveVertex(pkV0->Index);
        }

        rkPrimitives.push_back(pkPrimitive);
    }
}
//----------------------------------------------------------------------------
template <class Point2>
void PlanarGraph<Point2>::ExtractPrimitive (Vertex* pkV0,
    std::set<VertexPtr>& rkHeap, std::vector<Primitive*>& rkPrimitives)
{
    std::set<Vertex*> kVisited;
    std::vector<std::pair<Point2,int> > kSequence;
    kSequence.push_back(std::make_pair(pkV0->Position,pkV0->Index));
    Vertex* pkV1 = GetClockwiseMost(0,pkV0);
    Vertex* pkVPrev = pkV0;
    Vertex* pkVCurr = pkV1;

    while (pkVCurr && pkVCurr != pkV0
        && kVisited.find(pkVCurr) == kVisited.end())
    {
        kSequence.push_back(std::make_pair(pkVCurr->Position,pkVCurr->Index));
        kVisited.insert(pkVCurr);
        Vertex* pkVNext = GetCounterclockwiseMost(pkVPrev,pkVCurr);
        pkVPrev = pkVCurr;
        pkVCurr = pkVNext;
    }

    if (!pkVCurr)
    {
        // A filament has been found.  It is not necessarily rooted at V0.
        assert(pkVPrev->Adjacent.size() == 1);
        ExtractFilament(pkVPrev,pkVPrev->Adjacent[0],rkHeap,rkPrimitives);
    }
    else if (pkVCurr == pkV0)
    {
        // A minimal cycle has been found.
        Primitive* pkPrimitive = WM4_NEW Primitive(PT_MINIMAL_CYCLE);
        pkPrimitive->Sequence = kSequence;
        rkPrimitives.push_back(pkPrimitive);

        // Mark the edges to indicate they are part of a cycle.
        int iSQuantity = (int)kSequence.size();
        for (int i0 = iSQuantity-1, i1 = 0; i1 < iSQuantity; i0 = i1++)
        {
            int iV0 = kSequence[i0].second;
            int iV1 = kSequence[i1].second;
            SetCycleEdge(iV0,iV1,true);
        }

        // Remove any vertices and edges not needed by other primitives.
        RemoveEdge(pkV0->Index,pkV1->Index);

        // Since the edges are marked, the calls to GetFilament in this block
        // will only delete more edges but not create a primitive.
        if (pkV0->Adjacent.size() == 1)
        {
            ExtractFilament(pkV0,pkV0->Adjacent[0],rkHeap,rkPrimitives);
        }

        if (pkV1->Adjacent.size() == 1)
        {
            ExtractFilament(pkV1,pkV1->Adjacent[0],rkHeap,rkPrimitives);
        }
    }
    else  // pkVCurr has been visited before
    {
        // A cycle has been found, but it is not guaranteed to be a minimal
        // cycle.  V0 is therefore part of a filament.
        
        // Find a filament starting vertex.
        while (pkV0->Adjacent.size() == 2)
        {
            if (pkV0->Adjacent[0] != pkV1)
            {
                pkV1 = pkV0;
                pkV0 = pkV0->Adjacent[0];
            }
            else
            {
                pkV1 = pkV0;
                pkV0 = pkV0->Adjacent[1];
            }
        }

        // Create the primitive.
        ExtractFilament(pkV0,pkV1,rkHeap,rkPrimitives);
    }
}
//----------------------------------------------------------------------------
template <class Point2>
typename PlanarGraph<Point2>::Vertex*
PlanarGraph<Point2>::GetClockwiseMost (Vertex* pkVPrev, Vertex* pkVCurr)
{
    Vertex* pkVNext = 0;
    Point2 kDCurr = (pkVPrev ? pkVCurr->Position - pkVPrev->Position :
        Point2(0,-1));
    Point2 kDNext;
    bool bVCurrConvex = false;

    for (int i = 0; i < (int)pkVCurr->Adjacent.size(); i++)
    {
        // Get an adjacent vertex.
        Vertex* pkVAdj = pkVCurr->Adjacent[i];

        // No backtracking allowed.
        if (pkVAdj == pkVPrev)
        {
            continue;
        }

        // The potential direction to move in.
        Point2 kDAdj = pkVAdj->Position - pkVCurr->Position;

        // Select the first candidate.
        if (!pkVNext)
        {
            pkVNext = pkVAdj;
            kDNext = kDAdj;
            bVCurrConvex = (kDNext[0]*kDCurr[1]-kDNext[1]*kDCurr[0] <= 0);
            continue;
        }

        // Update if the next candidate is clockwise of the current
        // clockwise-most vertex.
        if (bVCurrConvex)
        {
            if (kDCurr[0]*kDAdj[1]-kDCurr[1]*kDAdj[0] < 0
            ||  kDNext[0]*kDAdj[1]-kDNext[1]*kDAdj[0] < 0)
            {
                pkVNext = pkVAdj;
                kDNext = kDAdj;
                bVCurrConvex = (kDNext[0]*kDCurr[1]-kDNext[1]*kDCurr[0] <= 0);
            }
        }
        else
        {
            if (kDCurr[0]*kDAdj[1]-kDCurr[1]*kDAdj[0] < 0
            &&  kDNext[0]*kDAdj[1]-kDNext[1]*kDAdj[0] < 0)
            {
                pkVNext = pkVAdj;
                kDNext = kDAdj;
                bVCurrConvex = (kDNext[0]*kDCurr[1]-kDNext[1]*kDCurr[0] <= 0);
            }
        }
    }

    return pkVNext;
}
//----------------------------------------------------------------------------
template <class Point2>
typename PlanarGraph<Point2>::Vertex*
PlanarGraph<Point2>::GetCounterclockwiseMost (Vertex* pkVPrev,
    Vertex* pkVCurr)
{
    Vertex* pkVNext = 0;
    Point2 kDCurr = (pkVPrev ? pkVCurr->Position - pkVPrev->Position :
        Point2(0,-1));
    Point2 kDNext;
    bool bVCurrConvex = false;

    for (int i = 0; i < (int)pkVCurr->Adjacent.size(); i++)
    {
        // Get an adjacent vertex.
        Vertex* pkVAdj = pkVCurr->Adjacent[i];

        // No backtracking allowed.
        if (pkVAdj == pkVPrev)
        {
            continue;
        }

        // The potential direction to move in.
        Point2 kDAdj = pkVAdj->Position - pkVCurr->Position;

        // Select the first candidate.
        if (!pkVNext)
        {
            pkVNext = pkVAdj;
            kDNext = kDAdj;
            bVCurrConvex = (kDNext[0]*kDCurr[1]-kDNext[1]*kDCurr[0] <= 0);
            continue;
        }

        // Update if the next candidate is clockwise of the current
        // clockwise-most vertex.
        if (bVCurrConvex)
        {
            if (kDCurr[0]*kDAdj[1]-kDCurr[1]*kDAdj[0] > 0
            &&  kDNext[0]*kDAdj[1]-kDNext[1]*kDAdj[0] > 0)
            {
                pkVNext = pkVAdj;
                kDNext = kDAdj;
                bVCurrConvex = (kDNext[0]*kDCurr[1]-kDNext[1]*kDCurr[0] <= 0);
            }
        }
        else
        {
            if (kDCurr[0]*kDAdj[1]-kDCurr[1]*kDAdj[0] > 0
            ||  kDNext[0]*kDAdj[1]-kDNext[1]*kDAdj[0] > 0)
            {
                pkVNext = pkVAdj;
                kDNext = kDAdj;
                bVCurrConvex = (kDNext[0]*kDCurr[1]-kDNext[1]*kDCurr[0] <= 0);
            }
        }
    }

    return pkVNext;
}
//----------------------------------------------------------------------------
