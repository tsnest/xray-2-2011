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

#ifndef POLYLINE3_H
#define POLYLINE3_H

#include "Wm4Vector3.h"
using namespace Wm4;

class Polyline3
{
public:
    // Constructor and destructor.  The quantity Q must be 2 or larger.
    // Polyline3 accepts responsibility for deleting the input array.  The
    // vertices are assumed to be ordered.  For an open polyline, the segments
    // are <V[i],V[i+1]> for 0 <= i <= Q-2.  If the polyline is closed, an
    // additional segment is <V[Q-1],V[0]>.
    Polyline3 (int iVQuantity, Vector3f* akVertex, bool bClosed);
    ~Polyline3 ();

    // accessors to vertex data
    int GetVertexQuantity () const;
    const Vector3f* GetVertices () const;
    bool GetClosed () const;

    // accessors to edge data
    int GetEdgeQuantity () const;
    const int* GetEdges () const;

    // accessors to level of detail  (MinLOD <= LOD <= MaxLOD is required)
    int GetMinLevelOfDetail () const;
    int GetMaxLevelOfDetail () const;
    void SetLevelOfDetail (int iLOD);
    int GetLevelOfDetail () const;

protected:
    // the polyline vertices
    int m_iVQuantity;
    Vector3f* m_akVertex;
    bool m_bClosed;

    // the polyline edges
    int m_iEQuantity;
    int* m_aiEdge;

    // the level of detail information
    int m_iVMin, m_iVMax;
    int* m_aiMap;
};

#include "Polyline3.inl"

#endif
