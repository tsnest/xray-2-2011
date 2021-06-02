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

#ifndef POLYSOLID2_H
#define POLYSOLID2_H

#include "RPoint2.h"
#include <map>
#include <set>
#include <vector>

class SegmentEdge;

// This code assumes that the bounded polysolid region is always to the
// right of each directed edge <v0,v1> in 'poly'.  The edge direction is
// v1-v0 = (a,b) and the normal pointing inwards is (b,-a).

class Polysolid2
{
public:
    // construction and destruction
    Polysolid2 ();
    Polysolid2 (const Polysolid2& rkPoly);
    ~Polysolid2 ();

    // assignment
    Polysolid2& operator= (const Polysolid2& rkPoly);

    class Vertex
    {
    public:
        Vertex ();
        ~Vertex ();

        void InsertAdjacent (Vertex* pkAdjacent);

        RPoint2 Position;
        std::vector<Vertex*> Adjacent;
    };

    class Edge
    {
    public:
        Edge ();
        Edge (Vertex* pkV0, Vertex* pkV1);

        void SetVertices (Vertex* pkV0, Vertex* pkV1);
        Vertex* GetVertex (int i) const;
        bool operator< (const Edge& rkEdge) const;

    private:
        Vertex* m_apkV[2];
    };

    typedef std::map<RPoint2,Vertex*> VMap;
    typedef std::set<Edge> ESet;

    const VMap& GetVertices () const;
    const ESet& GetEdges () const;

    void InsertEdge (const RPoint2& rkPoint0, const RPoint2& rkPoint1);

    // boolean operations

    // intersection
    Polysolid2 operator& (const Polysolid2& rkPoly) const;

    // union
    Polysolid2 operator| (const Polysolid2& rkPoly) const;

    // difference
    Polysolid2 operator- (const Polysolid2& rkPoly) const;

    // exclusive or
    Polysolid2 operator^ (const Polysolid2& rkPoly) const;

protected:
    Vertex* GetVertex (const RPoint2& rkPoint);
    Vertex* InsertVertex (const RPoint2& rkPoint);

    void SegmentAll (const Polysolid2& rkPoly, SegmentEdge akTEdge[4]) const;

    VMap m_kVMap;
    ESet m_kESet;
};

#endif
