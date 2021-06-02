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

#ifndef SEGMENTEDGE_H
#define SEGMENTEDGE_H

#include "RPoint2.h"

class Polysolid2;

class SegmentEdge
{
public:
    SegmentEdge ();
    ~SegmentEdge ();

    bool HasEdges () const;
    void AddEdge (const RPoint2& rkPosition0, const RPoint2& rkPosition1);

    // for union, intersection
    void MergeAppend (SegmentEdge& rkTEdge);
    void MergeUnique (SegmentEdge& rkTEdge);
    void MergeEqual  (SegmentEdge& rkTEdge);

    // for difference, exclusive or
    void MergeAppendReverse (SegmentEdge& rkTEdge);
    void MergeUniqueReverse (SegmentEdge& rkTEdge);

    void ConvertToPolySolid (Polysolid2& rkPoly);

protected:
    class TaggedEdge
    {
    public:
        RPoint2 Position[2];
        TaggedEdge* Next;
    };

    TaggedEdge* m_pkFirst;
};

#endif
