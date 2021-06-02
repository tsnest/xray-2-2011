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

#include "SegmentEdge.h"
#include "Polysolid2.h"

//----------------------------------------------------------------------------
SegmentEdge::SegmentEdge ()
{
    m_pkFirst = 0;
}
//----------------------------------------------------------------------------
SegmentEdge::~SegmentEdge ()
{
    while (m_pkFirst)
    {
        TaggedEdge* pkSave = m_pkFirst->Next;
        WM4_DELETE m_pkFirst;
        m_pkFirst = pkSave;
    }
}
//----------------------------------------------------------------------------
bool SegmentEdge::HasEdges () const
{
    return m_pkFirst != 0;
}
//----------------------------------------------------------------------------
void SegmentEdge::AddEdge (const RPoint2& rkPosition0,
    const RPoint2& rkPosition1)
{
    assert(rkPosition0 != rkPosition1);

    // check if edge is already in list
    TaggedEdge* pkEdge;
    for (pkEdge = m_pkFirst; pkEdge; pkEdge = pkEdge->Next)
    {
        if (rkPosition0 == pkEdge->Position[0]
        &&  rkPosition1 == pkEdge->Position[1])
        {
            return;
        }
    }

    pkEdge = WM4_NEW TaggedEdge;
    pkEdge->Position[0] = rkPosition0;
    pkEdge->Position[1] = rkPosition1;
    pkEdge->Next = m_pkFirst;
    m_pkFirst = pkEdge;
}
//----------------------------------------------------------------------------
void SegmentEdge::MergeAppend (SegmentEdge& rkTEdge)
{
    if (rkTEdge.m_pkFirst == 0)
    {
        return;
    }

    // search for end of list
    TaggedEdge* pkPrev = 0;
    for (TaggedEdge* pkEdge = m_pkFirst; pkEdge; pkEdge = pkEdge->Next)
    {
        pkPrev = pkEdge;
    }

    // append rkTEdge list to 'this' list
    if (pkPrev)
    {
        pkPrev->Next = rkTEdge.m_pkFirst;
    }
    else
    {
        // 'this' list was empty, appended list is that of rkTEdge
        m_pkFirst = rkTEdge.m_pkFirst;
    }

    rkTEdge.m_pkFirst = 0;
}
//----------------------------------------------------------------------------
void SegmentEdge::MergeUnique (SegmentEdge& rkTEdge)
{
    TaggedEdge* pkUnique = 0;

    for (TaggedEdge* pkTEL = rkTEdge.m_pkFirst; pkTEL; pkTEL = pkTEL->Next)
    {
        // check if edge is already in 'this' list
        TaggedEdge* pkTER;
        for (pkTER = m_pkFirst; pkTER; pkTER = pkTER->Next)
        {
            if (pkTER->Position[0] == pkTEL->Position[0]
            &&  pkTER->Position[1] == pkTEL->Position[1])
            {
                break;
            }
        }

        if (pkTER == 0)
        {
            // pTEL is not in 'this' list, add to unique list
            TaggedEdge* pkEdge = WM4_NEW TaggedEdge;
            pkEdge->Position[0] = pkTEL->Position[0];
            pkEdge->Position[1] = pkTEL->Position[1];
            pkEdge->Next = pkUnique;
            pkUnique = pkEdge;
        }
    }

    if (pkUnique)
    {
        // search for end of list
        TaggedEdge* pkPrev = 0;
        for (TaggedEdge* pkEdge = m_pkFirst; pkEdge; pkEdge = pkEdge->Next)
        {
            pkPrev = pkEdge;
        }
        
        // append rkTEdge list to 'this' list
        pkPrev = pkUnique;
    }
}
//----------------------------------------------------------------------------
void SegmentEdge::MergeEqual (SegmentEdge& rkTEdge)
{
    TaggedEdge* pkEqual = 0;

    for (TaggedEdge* pkTER = m_pkFirst; pkTER; pkTER = pkTER->Next)
    {
        TaggedEdge* pkTEL;
        for (pkTEL = rkTEdge.m_pkFirst; pkTEL; pkTEL = pkTEL->Next)
        {
            if (pkTER->Position[0] == pkTEL->Position[0]
            &&  pkTER->Position[1] == pkTEL->Position[1])
            {
                // pkTER and pkTEL are equal edges, add to equal list
                TaggedEdge* pkEdge = WM4_NEW TaggedEdge;
                pkEdge->Position[0] = pkTEL->Position[0];
                pkEdge->Position[1] = pkTEL->Position[1];
                pkEdge->Next = pkEqual;
                pkEqual = pkEdge;
                break;
            }
        }
    }

    // replace 'this' list by equal list
    while (m_pkFirst)
    {
        TaggedEdge* pkSave = m_pkFirst->Next;
        WM4_DELETE m_pkFirst;
        m_pkFirst = pkSave;
    }

    m_pkFirst = pkEqual;
}
//----------------------------------------------------------------------------
void SegmentEdge::MergeAppendReverse (SegmentEdge& rkTEdge)
{
    if (rkTEdge.m_pkFirst == 0)
    {
        return;
    }

    // reverse order of edge endpoints
    TaggedEdge* pkEdge;
    for (pkEdge = rkTEdge.m_pkFirst; pkEdge; pkEdge = pkEdge->Next)
    {
        RPoint2 kSave = pkEdge->Position[0];
        pkEdge->Position[0] = pkEdge->Position[1];
        pkEdge->Position[1] = kSave;
    }

    // search for end of list
    TaggedEdge* pkPrev = 0;
    for (pkEdge = m_pkFirst; pkEdge; pkEdge = pkEdge->Next)
    {
        pkPrev = pkEdge;
    }

    // append rkTEdge list to 'this' list
    pkPrev->Next = rkTEdge.m_pkFirst;
    rkTEdge.m_pkFirst = 0;
}
//----------------------------------------------------------------------------
void SegmentEdge::MergeUniqueReverse (SegmentEdge& rkTEdge)
{
    TaggedEdge* pkUnique = 0;
    TaggedEdge* pkEdge;

    for (TaggedEdge* pkTEL = rkTEdge.m_pkFirst; pkTEL; pkTEL = pkTEL->Next)
    {
        // check if edge is already in 'this' list
        TaggedEdge* pkTER;
        for (pkTER = m_pkFirst; pkTER; pkTER = pkTER->Next)
        {
            if (pkTER->Position[0] == pkTEL->Position[0]
            &&  pkTER->Position[1] == pkTEL->Position[1])
            {
                break;
            }
        }

        if (pkTER == 0)
        {
            // pkTEL is not in 'this' list, add to unique list (reversed)
            pkEdge = WM4_NEW TaggedEdge;
            pkEdge->Position[0] = pkTEL->Position[1];
            pkEdge->Position[1] = pkTEL->Position[0];
            pkEdge->Next = pkUnique;
            pkUnique = pkEdge;
        }
    }

    if (pkUnique)
    {
        // search for end of list
        TaggedEdge* pkPrev = 0;
        for (pkEdge = m_pkFirst; pkEdge; pkEdge = pkEdge->Next)
        {
            pkPrev = pkEdge;
        }
        
        // append rkTEdge list to 'this' list
        pkPrev = pkUnique;
    }
}
//----------------------------------------------------------------------------
void SegmentEdge::ConvertToPolySolid (Polysolid2& rkPoly)
{
    for (TaggedEdge* pkEdge = m_pkFirst; pkEdge; pkEdge = pkEdge->Next)
    {
        rkPoly.InsertEdge(pkEdge->Position[0],pkEdge->Position[1]);
    }
}
//----------------------------------------------------------------------------
