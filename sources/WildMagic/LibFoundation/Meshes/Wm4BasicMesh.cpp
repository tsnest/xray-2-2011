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

#include "Wm4FoundationPCH.h"
#include "Wm4BasicMesh.h"
#include "Wm4EdgeKey.h"
using namespace Wm4;

//----------------------------------------------------------------------------
BasicMesh::BasicMesh (int iVQuantity, const void* akPoint, int iTQuantity,
    const int* aiIndex)
{
    assert(iVQuantity > 0 && iTQuantity > 0 && aiIndex);
    if (iVQuantity <= 0 || iTQuantity <= 0 || !aiIndex)
    {
        m_iVQuantity = 0;
        m_iEQuantity = 0;
        m_iTQuantity = 0;
        m_akVertex = 0;
        m_akEdge = 0;
        m_akTriangle = 0;
        m_akPoint = 0;
        m_aiIndex = 0;
        m_bIsValid = false;
        return;
    }

    m_iVQuantity = iVQuantity;
    m_iEQuantity = 0;
    m_iTQuantity = iTQuantity;
    m_akPoint = akPoint;
    m_aiIndex = aiIndex;
    m_bIsValid = true;

    // dynamically construct triangle mesh from input
    m_akVertex = WM4_NEW Vertex[m_iVQuantity];
    m_akEdge = WM4_NEW Edge[3*m_iTQuantity];
    m_akTriangle = WM4_NEW Triangle[m_iTQuantity];
    std::map<EdgeKey,int> kEMap;
    for (int iT = 0; iT < m_iTQuantity; iT++)
    {
        // update triangle
        Triangle& rkT = m_akTriangle[iT];
        rkT.V[0] = *aiIndex++;
        rkT.V[1] = *aiIndex++;
        rkT.V[2] = *aiIndex++;

        // add edges to mesh
        for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            // update vertices
            m_akVertex[rkT.V[i1]].InsertTriangle(iT);

            EdgeKey kKey(rkT.V[i0],rkT.V[i1]);
            std::map<EdgeKey,int>::iterator kEIter = kEMap.find(kKey);
            if (kEIter == kEMap.end())
            {
                // first time edge encountered
                kEMap[kKey] = m_iEQuantity;

                // update edge
                Edge& rkE = m_akEdge[m_iEQuantity];
                rkE.V[0] = rkT.V[i0];
                rkE.V[1] = rkT.V[i1];
                rkE.T[0] = iT;

                // update vertices
                m_akVertex[rkE.V[0]].InsertEdge(rkE.V[1],m_iEQuantity);
                m_akVertex[rkE.V[1]].InsertEdge(rkE.V[0],m_iEQuantity);

                // update triangle
                rkT.E[i0] = m_iEQuantity;

                m_iEQuantity++;
            }
            else
            {
                // second time edge encountered
                int iE = kEIter->second;
                Edge& rkE = m_akEdge[iE];

                // update edge
                assert(rkE.T[1] == -1);  // mesh must be manifold
                if (rkE.T[1] != -1)
                {
                    WM4_DELETE[] m_akVertex;
                    WM4_DELETE[] m_akEdge;
                    WM4_DELETE[] m_akTriangle;
                    m_iVQuantity = 0;
                    m_iEQuantity = 0;
                    m_iTQuantity = 0;
                    m_akVertex = 0;
                    m_akEdge = 0;
                    m_akTriangle = 0;
                    m_akPoint = 0;
                    m_aiIndex = 0;
                    m_bIsValid = false;
                }
                rkE.T[1] = iT;

                // update triangles
                int iAdj = rkE.T[0];
                Triangle& rkAdj = m_akTriangle[iAdj];
                for (int j = 0; j < 3; j++)
                {
                    if (rkAdj.E[j] == iE)
                    {
                        rkAdj.T[j] = iT;
                        break;
                    }
                }
                rkT.E[i0] = iE;
                rkT.T[i0] = iAdj;
            }
        }
    }
}
//----------------------------------------------------------------------------
BasicMesh::~BasicMesh ()
{
    WM4_DELETE[] m_akVertex;
    WM4_DELETE[] m_akEdge;
    WM4_DELETE[] m_akTriangle;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BasicMesh::Vertex
//----------------------------------------------------------------------------
BasicMesh::Vertex::Vertex ()
{
    VQuantity = 0;
    V = 0;
    E = 0;
    TQuantity = 0;
    T = 0;
}
//----------------------------------------------------------------------------
BasicMesh::Vertex::~Vertex ()
{
    WM4_DELETE[] V;
    WM4_DELETE[] E;
    WM4_DELETE[] T;
}
//----------------------------------------------------------------------------
void BasicMesh::Vertex::InsertEdge (int iV, int iE)
{
    // check if vertex/edge in adjacency array (nothing to do if in array)
    for (int i = 0; i < VQuantity; i++)
    {
        if (iV == V[i])
        {
            return;
        }
    }

    if ((VQuantity % MV_CHUNK) == 0)
    {
        size_t uiDstSize = (VQuantity+MV_CHUNK)*sizeof(int);
        size_t uiSrcSize = VQuantity*sizeof(int);

        int* aiSave = V;
        V = WM4_NEW int[VQuantity+MV_CHUNK];
        if (aiSave)
        {
            System::Memcpy(V,uiDstSize,aiSave,uiSrcSize);
            WM4_DELETE[] aiSave;
        }

        aiSave = E;
        E = WM4_NEW int[VQuantity+MV_CHUNK];
        if (aiSave)
        {
            System::Memcpy(E,uiDstSize,aiSave,uiSrcSize);
            WM4_DELETE[] aiSave;
        }
    }

    V[VQuantity] = iV;
    E[VQuantity] = iE;
    VQuantity++;
}
//----------------------------------------------------------------------------
void BasicMesh::Vertex::InsertTriangle (int iT)
{
    // check if triangle in adjacency array (nothing to do if in array)
    for (int i = 0; i < TQuantity; i++)
    {
        if (iT == T[i])
        {
            return;
        }
    }

    if ((TQuantity % MV_CHUNK) == 0)
    {
        int* aiSave = T;
        T = WM4_NEW int[TQuantity+MV_CHUNK];
        if (aiSave)
        {
            size_t uiDstSize = (TQuantity+MV_CHUNK)*sizeof(int);
            size_t uiSrcSize = TQuantity*sizeof(int);
            System::Memcpy(T,uiDstSize,aiSave,uiSrcSize);
            WM4_DELETE[] aiSave;
        }
    }

    T[TQuantity] = iT;
    TQuantity++;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BasicMesh::Edge
//----------------------------------------------------------------------------
BasicMesh::Edge::Edge ()
{
    for (int i = 0; i < 2; i++)
    {
        V[i] = -1;
        T[i] = -1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BasicMesh::Triangle
//----------------------------------------------------------------------------
BasicMesh::Triangle::Triangle ()
{
    for (int i = 0; i < 3; i++)
    {
        V[i] = -1;
        E[i] = -1;
        T[i] = -1;
    }
}
//----------------------------------------------------------------------------
