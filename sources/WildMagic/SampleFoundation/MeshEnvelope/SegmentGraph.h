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

#ifndef SEGMENTGRAPH
#define SEGMENTGRAPH

#include "Rational.h"

class SegmentGraph
{
public:
    SegmentGraph ();
    ~SegmentGraph ();

    class Vertex
    {
    public:
        Vertex ();
        ~Vertex ();

        void InsertAdjacent (Vertex* pkAdjacent);
        void RemoveAdjacent (Vertex* pkAdjacent);

        enum { VG_CHUNK = 16 };
        RPoint2 Position;
        int VQuantity;
        Vertex** V;
    };

    class Edge
    {
    public:
        Edge ();
        Edge (Vertex* pkV0, Vertex* pkV1);

        void SetVertices (Vertex* pkV0, Vertex* pkV1);
        Vertex* GetVertex (int i) const;
        bool operator== (const Edge& rkEdge) const;
        bool operator< (const Edge& rkEdge) const;

    private:
        Vertex* m_apkV[2];
    };

    typedef std::map<RPoint2,Vertex*> VMap;
    typedef std::set<Edge> ESet;

    VMap& GetVertices ();
    ESet& GetEdges ();

    void InsertEdge (const RPoint2& rkPoint0, const RPoint2& rkPoint1);
    bool RemoveEdge (const RPoint2& rkPoint0, const RPoint2& rkPoint1);
    void ExtractEnvelope (std::vector<RPoint2>& rkEnvelope);

private:
    Vertex* GetVertex (const RPoint2& rkPoint);
    Vertex* InsertVertex (const RPoint2& rkPoint);

    VMap m_kVMap;
    ESet m_kESet;
};

#endif
