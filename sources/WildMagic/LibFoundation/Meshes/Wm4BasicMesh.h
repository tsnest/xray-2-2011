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

#ifndef WM4BASICMESH_H
#define WM4BASICMESH_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

class WM4_FOUNDATION_ITEM BasicMesh
{
public:
    // The number of vertices in the input is iVQuantity.  The vertex array
    // is usually passed as akPoint, but this input can be any data type you
    // prefer (points+attributes).  The number of triangles is iTQuantity.
    // The triangles are represented as triples of indices into the vertex
    // array.  These triples are stored in aiIndex.  The caller is responsible
    // for deleting the input arrays.

    BasicMesh (int iVQuantity, const void* akPoint, int iTQuantity,
        const int* aiIndex);

    virtual ~BasicMesh ();

    // The input must correspond to a mesh whose edges are shared by no more
    // than two triangles.  If an edge with three (or more) edges is
    // encountered in the construction, IsValid returns 'false'.  Otherwise
    // it returns 'true'.
    bool IsValid () const;

    class WM4_FOUNDATION_ITEM Vertex
    {
    public:
        Vertex ();
        ~Vertex ();

        enum { MV_CHUNK = 8 };

        void InsertEdge (int iV, int iE);
        void InsertTriangle (int iT);

        int VQuantity;
        int* V;
        int* E;
        int TQuantity;
        int* T;
    };

    class WM4_FOUNDATION_ITEM Edge
    {
    public:
        Edge ();

        int V[2];
        int T[2];
    };

    class WM4_FOUNDATION_ITEM Triangle
    {
    public:
        Triangle ();

        int V[3];
        int E[3];
        int T[3];
    };

    // member access
    int GetVQuantity () const;
    int GetEQuantity () const;
    int GetTQuantity () const;
    const void* GetPoints () const;
    const int* GetIndices () const;
    const Vertex* GetVertices () const;
    const Edge* GetEdges () const;
    const Triangle* GetTriangles () const;

protected:
    int m_iVQuantity, m_iEQuantity, m_iTQuantity;
    const void* m_akPoint;
    const int* m_aiIndex;
    Vertex* m_akVertex;
    Edge* m_akEdge;
    Triangle* m_akTriangle;
    bool m_bIsValid;
};

#include "Wm4BasicMesh.inl"

}

#endif
