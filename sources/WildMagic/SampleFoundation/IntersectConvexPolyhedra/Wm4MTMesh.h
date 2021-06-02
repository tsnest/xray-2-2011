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

#ifndef WM4MTMESH_H
#define WM4MTMESH_H

// Manifold Triangle Mesh.  Each edge has 1 or 2 triangles sharing it.

#include "Wm4MTIVertex.h"
#include "Wm4MTIEdge.h"
#include "Wm4MTITriangle.h"
#include "Wm4MTVertex.h"
#include "Wm4MTEdge.h"
#include "Wm4MTTriangle.h"

namespace Wm4
{

class MTMesh
{
public:
    MTMesh (int iVQuantity = 0, int iEQuantity = 0, int iTQuantity = 0);
    MTMesh (const MTMesh& rkMesh);
    virtual ~MTMesh ();

    void Reset (int iVQuantity = 0, int iEQuantity = 0, int iTQuantity = 0);
    MTMesh& operator= (const MTMesh& rkMesh);

    int GetVQuantity () const;
    int V (int iLabel) const;
    const MTVertex& GetVertex (int iVIndex) const;
    int GetVLabel (int iVIndex) const;

    int GetEQuantity () const;
    int E (int iLabel0, int iLabel1) const;
    const MTEdge& GetEdge (int iEIndex) const;
    int GetELabel (int iEIndex) const;
    int& ELabel (int iEIndex);

    int GetTQuantity () const;
    int T (int iLabel0, int iLabel1, int iLabel2) const;
    const MTTriangle& GetTriangle (int iTIndex) const;
    int GetTLabel (int iTIndex) const;
    int& TLabel (int iTIndex);

    int& InitialELabel ();
    int& InitialTLabel ();

    bool Insert (int iLabel0, int iLabel1, int iLabel2);
    bool Remove (int iLabel0, int iLabel1, int iLabel2);

    bool SubdivideCentroid (int iLabel0, int iLabel1, int iLabel2,
        int& riNextLabel);
    bool SubdivideCentroidAll (int& riNextLabel);

    bool SubdivideEdge (int iLabel0, int iLabel1, int& riNextLabel);

    virtual void Print (std::ofstream& rkOStr) const;
    virtual bool Print (const char* acFilename) const;

protected:
    void AttachTriangleToEdge (int iT, MTTriangle& rkT, int i, int iE,
        MTEdge& rkE);
    int InsertVertex (int iLabel);
    int InsertEdge (int iLabel0, int iLabel1);
    int InsertTriangle (int iLabel0, int iLabel1, int iLabel2);

    void DetachTriangleFromEdge (int iT, MTTriangle& rkT, int i, int iE,
        MTEdge& rkE);
    void RemoveVertex (int iLabel);
    void RemoveEdge (int iLabel0, int iLabel1);
    void RemoveTriangle (int iLabel0, int iLabel1, int iLabel2);

    UnorderedSet<MTVertex> m_akVertex;
    UnorderedSet<MTEdge> m_akEdge;
    UnorderedSet<MTTriangle> m_akTriangle;

    int m_iInitialELabel;
    int m_iInitialTLabel;

    typedef std::map<MTIVertex,int> VMap;
    typedef std::map<MTIEdge,int> EMap;
    typedef std::map<MTITriangle,int> TMap;
    typedef VMap::iterator VIter;
    typedef EMap::iterator EIter;
    typedef TMap::iterator TIter;
    typedef VMap::const_iterator VCIter;
    typedef EMap::const_iterator ECIter;
    typedef TMap::const_iterator TCIter;

    VMap m_kVMap;
    EMap m_kEMap;
    TMap m_kTMap;
};

#include "Wm4MTMesh.inl"

}

#endif
