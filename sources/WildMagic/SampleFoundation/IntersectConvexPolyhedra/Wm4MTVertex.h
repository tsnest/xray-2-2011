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
// Version: 4.7.0 (2008/09/15)

#ifndef WM4MTVERTEX_H
#define WM4MTVERTEX_H

#include "Wm4System.h"
#include "Wm4UnorderedSet.h"

namespace Wm4
{

class MTVertex
{
public:
    MTVertex (int iLabel = -1, int iEGrow = 0, int iTGrow = 0);
    MTVertex (const MTVertex& rkV);
    virtual ~MTVertex ();

    MTVertex& operator= (const MTVertex& rkV);

    // Vertex labels are read-only since they are used for maps in the MTMesh
    // class for inverse look-up.
    int GetLabel () const;

    int GetEdgeQuantity () const;
    int GetEdge (int i) const;
    bool InsertEdge (int iE);
    bool RemoveEdge (int iE);
    bool ReplaceEdge (int iEOld, int iENew);

    int GetTriangleQuantity () const;
    int GetTriangle (int i) const;
    bool InsertTriangle (int iT);
    bool RemoveTriangle (int iT);
    bool ReplaceTriangle(int iTOld, int iTNew);

    bool operator== (const MTVertex& rkV) const;

protected:
    int m_iLabel;
    UnorderedSet<int> m_kESet, m_kTSet;
};

#include "Wm4MTVertex.inl"

}

#endif
