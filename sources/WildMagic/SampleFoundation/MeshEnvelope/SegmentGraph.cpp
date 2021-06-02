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
// Version: 4.0.1 (2007/08/02)

#include "SegmentGraph.h"

//----------------------------------------------------------------------------
SegmentGraph::SegmentGraph ()
{
}
//----------------------------------------------------------------------------
SegmentGraph::~SegmentGraph ()
{
    VMap::iterator pkIter;
    for (pkIter = m_kVMap.begin(); pkIter != m_kVMap.end(); pkIter++)
    {
        Vertex* pkVertex = pkIter->second;
        WM4_DELETE pkVertex;
    }
}
//----------------------------------------------------------------------------
SegmentGraph::VMap& SegmentGraph::GetVertices ()
{
    return m_kVMap;
}
//----------------------------------------------------------------------------
SegmentGraph::ESet& SegmentGraph::GetEdges ()
{
    return m_kESet;
}
//----------------------------------------------------------------------------
void SegmentGraph::InsertEdge (const RPoint2& rkPoint0,
    const RPoint2& rkPoint1)
{
    // Insert vertices into the vertex set.  If the points already exist,
    // the function call is just a lookup; otherwise, new vertices are
    // allocated.
    Vertex* pkVertex0 = InsertVertex(rkPoint0);
    Vertex* pkVertex1 = InsertVertex(rkPoint1);

    Edge kEdge(pkVertex0,pkVertex1);
    std::set<Edge>::iterator pkIter = m_kESet.find(kEdge);
    if (pkIter == m_kESet.end())
    {
        // The edge does not exist, insert it into the set.
        m_kESet.insert(kEdge);

        // Update the vertex-adjacency information.
        pkVertex0->InsertAdjacent(pkVertex1);
        pkVertex1->InsertAdjacent(pkVertex0);
    }
}
//----------------------------------------------------------------------------
bool SegmentGraph::RemoveEdge (const RPoint2& rkPoint0,
    const RPoint2& rkPoint1)
{
    // Look up the vertices.  If one or the other does not exist, there is
    // nothing to do.
    Vertex* pkVertex0 = GetVertex(rkPoint0);
    if (!pkVertex0)
    {
        return false;
    }

    Vertex* pkVertex1 = GetVertex(rkPoint1);
    if (!pkVertex1)
    {
        return false;
    }

    Edge kEdge(pkVertex0,pkVertex1);
    std::set<Edge>::iterator pkIter = m_kESet.find(kEdge);
    if (pkIter != m_kESet.end())
    {
        // The edge exists, remove it from the set.
        m_kESet.erase(kEdge);

        // Update the vertex-adjacency information.
        pkVertex0->RemoveAdjacent(pkVertex1);
        pkVertex1->RemoveAdjacent(pkVertex0);

        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SegmentGraph::ExtractEnvelope (std::vector<RPoint2>& rkEnvelope)
{
    // Get the left-most (min x) and bottom-most (min y) vertex of the graph.
    VMap::iterator pkIter = m_kVMap.begin();
    Vertex* pkVMin = pkIter->second;
    for (++pkIter; pkIter != m_kVMap.end(); pkIter++)
    {
        Vertex* pkVertex = pkIter->second;
        if (pkVertex->Position.X() < pkVMin->Position.X())
        {
            pkVMin = pkVertex;
        }
        else if (pkVertex->Position.X() == pkVMin->Position.X())
        {
            if (pkVertex->Position.Y() < pkVMin->Position.Y())
            {
                pkVMin = pkVertex;
            }
        }
    }

    // Traverse the envelope.  If Vcurr is the current vertex and Vprev is
    // its predecessor in the envelope, then the successor Vnext is selected
    // to be a vertex adjacent to Vcurr according to the following.  The
    // current edge has direction Dcurr = Vcurr - Vprev.  Vnext is an adjacent
    // vertex of Vcurr, selected so that the direction Dnext = Vnext - Vcurr
    // has the property that Dnext forms the largest interior angle at Vcurr
    // compared to -Dcurr, the angle measured by rotating -Dcurr clockwise
    // about Vcurr.  The starting vertex Vmin does not have a predecessor, but
    // the construction of Vmin implies a starting direction Dcurr = (0,-1).
    Vertex* pkVCurr = pkVMin;
    Vertex* pkVPrev = 0;
    Vertex* pkVNext = 0;
    RPoint2 kDCurr(0,-1), kDNext;
    bool bVCurrConvex;

    // The loop could be a 'while' loop, terminating when the Vnext becomes
    // Vmin, but to trap potential errors that cause an infinite loop, a for
    // loop is used to guarantee bounded iterations.  The upper bound of the
    // loop is the worst-case behavior (each vertex connected to all other
    // vertices).
    int i, iVQuantity = (int)m_kVMap.size();
    const int iMaxVQuantity = iVQuantity*(iVQuantity - 1)/2;
    for (i = 0; i < iMaxVQuantity; i++)
    {
        rkEnvelope.push_back(pkVCurr->Position);

        // Search the adjacent vertices for Vnext.
        for (int j = 0; j < pkVCurr->VQuantity; j++)
        {
            Vertex* pkVertex = pkVCurr->V[j];
            if (pkVertex == pkVPrev)
            {
                continue;
            }

            RPoint2 kD = pkVertex->Position - pkVCurr->Position;
            if (!pkVNext)
            {
                pkVNext = pkVertex;
                kDNext = kD;
                bVCurrConvex = (kDNext.DotPerp(kDCurr) <= 0);
                continue;
            }

            if (bVCurrConvex)
            {
                if (kDCurr.DotPerp(kD) < 0
                ||  kDNext.DotPerp(kD) < 0)
                {
                    pkVNext = pkVertex;
                    kDNext = kD;
                    bVCurrConvex = (kDNext.DotPerp(kDCurr) <= 0);
                }
            }
            else
            {
                if (kDCurr.DotPerp(kD) < 0
                &&  kDNext.DotPerp(kD) < 0)
                {
                    pkVNext = pkVertex;
                    kDNext = kD;
                    bVCurrConvex = (kDNext.DotPerp(kDCurr) <= 0);
                }
            }
        }

        assert(pkVNext);
        pkVPrev = pkVCurr;
        pkVCurr = pkVNext;
        pkVNext = 0;
        kDCurr = kDNext;

        if (pkVCurr == pkVMin)
        {
            break;
        }
    }
    assert(i < iVQuantity);
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex* SegmentGraph::GetVertex (const RPoint2& rkPoint)
{
    VMap::iterator pkIter = m_kVMap.find(rkPoint);
    return (pkIter != m_kVMap.end() ? pkIter->second : 0);
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex* SegmentGraph::InsertVertex (const RPoint2& rkPoint)
{
    VMap::iterator pkIter = m_kVMap.find(rkPoint);
    if (pkIter != m_kVMap.end())
    {
        return pkIter->second;
    }

    // Insert the vertex into the vertex set.  The adjacency array has already
    // been initialized to empty.
    Vertex* pkVertex = WM4_NEW Vertex;
    pkVertex->Position = rkPoint;
    m_kVMap[rkPoint] = pkVertex;
    return pkVertex;
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex::Vertex ()
{
    VQuantity = 0;
    V = 0;
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex::~Vertex ()
{
    WM4_DELETE[] V;
}
//----------------------------------------------------------------------------
void SegmentGraph::Vertex::InsertAdjacent (Vertex* pkAdjacent)
{
    // Grow the vertex array if necessary.
    if ((VQuantity % VG_CHUNK) == 0)
    {
        size_t uiSize = VQuantity*sizeof(Vertex*);

        Vertex** apkSave = V;
        V = WM4_NEW Vertex*[VQuantity + VG_CHUNK];
        if (apkSave)
        {
            memcpy(V,apkSave,uiSize);
            WM4_DELETE[] apkSave;
        }
    }

    V[VQuantity] = pkAdjacent;
    VQuantity++;
}
//----------------------------------------------------------------------------
void SegmentGraph::Vertex::RemoveAdjacent (Vertex* pkAdjacent)
{
    for (int i = 0; i < VQuantity; i++)
    {
        if (pkAdjacent == V[i])
        {
            // Maintain a compact array.
            VQuantity--;
            if (i < VQuantity)
            {
                V[i] = V[VQuantity];
            }
            V[VQuantity] = 0;
            return;
        }
    }
}
//----------------------------------------------------------------------------
SegmentGraph::Edge::Edge ()
{
    m_apkV[0] = 0;
    m_apkV[1] = 0;
}
//----------------------------------------------------------------------------
SegmentGraph::Edge::Edge (Vertex* pkV0, Vertex* pkV1)
{
    SetVertices(pkV0,pkV1);
}
//----------------------------------------------------------------------------
void SegmentGraph::Edge::SetVertices (Vertex* pkV0, Vertex* pkV1)
{
    if (pkV0 < pkV1)
    {
        // V0 is minimum.
        m_apkV[0] = pkV0;
        m_apkV[1] = pkV1;
    }
    else
    {
        // V1 is minimum.
        m_apkV[0] = pkV1;
        m_apkV[1] = pkV0;
    }
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex* SegmentGraph::Edge::GetVertex (int i) const
{
    return m_apkV[i];
}
//----------------------------------------------------------------------------
bool SegmentGraph::Edge::operator== (const Edge& rkEdge) const
{
    return m_apkV[0] == rkEdge.m_apkV[0] && m_apkV[1] == rkEdge.m_apkV[1];
}
//----------------------------------------------------------------------------
bool SegmentGraph::Edge::operator< (const Edge& rkEdge) const
{
    if (m_apkV[1] < rkEdge.m_apkV[1])
    {
        return true;
    }

    if (m_apkV[1] > rkEdge.m_apkV[1])
    {
        return false;
    }

    return m_apkV[0] < rkEdge.m_apkV[0];
}
//----------------------------------------------------------------------------
