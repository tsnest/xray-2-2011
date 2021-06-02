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

#ifndef WM4CREATECLODMESH_H
#define WM4CREATECLODMESH_H

#include "Wm4GraphicsLIB.h"
#include "Wm4CollapseRecord.h"
#include "Wm4ColorRGB.h"
#include "Wm4Vector2.h"
#include "Wm4Vector3.h"
#include "Wm4VertexBuffer.h"
#include "Wm4IndexBuffer.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM CreateClodMesh
{
public:
    CreateClodMesh (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer,
        int& riCQuantity, CollapseRecord*& rakCRecord);

    ~CreateClodMesh ();

private:
    // vertex is <v>
    // edge is <v0,v1> where v0 = min(v0,v1)
    // triangle is <v0,v1,v2> where v0 = min(v0,v1,v2)

    class WM4_GRAPHICS_ITEM Edge
    {
    public:
        Edge ();
        Edge (int iV0, int iV1);
        bool operator< (const Edge& rkE) const;
        bool operator== (const Edge& rkE) const;
        bool operator!= (const Edge& rkE) const;

        int m_aiV[2];
    };

    class WM4_GRAPHICS_ITEM Triangle
    {
    public:
        Triangle ();
        Triangle (int iV0, int iV1, int iV2);
        bool operator< (const Triangle& rkT) const;
        bool operator== (const Triangle& rkT) const;
        bool operator!= (const Triangle& rkT) const;

        int m_aiV[3];
    };

    class WM4_GRAPHICS_ITEM HeapRecord
    {
    public:
        HeapRecord () : m_kEdge(-1,-1)
        {
            m_iHIndex = -1;
            m_fMetric = -1.0f;
        }

        Edge m_kEdge;
        int m_iHIndex;
        float m_fMetric;
    };

    class WM4_GRAPHICS_ITEM VertexAttribute
    {
    public:
        VertexAttribute ()
            :
            m_kESet(8,8),
            m_kTSet(8,8)
        {
        }

        TSmallUnorderedSet<Edge> m_kESet;
        TSmallUnorderedSet<Triangle> m_kTSet;
    };

    class WM4_GRAPHICS_ITEM EdgeAttribute
    {
    public:
        EdgeAttribute ()
            :
            m_kTSet(2,2)
        {
            Record = 0;
        }

        HeapRecord* Record;
        TSmallUnorderedSet<Triangle> m_kTSet;
    };

    class WM4_GRAPHICS_ITEM TriangleAttribute
    {
    public:
        TriangleAttribute ()
        {
            Index = -1;
        }

        int Index;
    };

    // for readability of the code
    typedef std::map<int,VertexAttribute> VMap;
    typedef VMap::iterator MVIter;
    typedef VMap::const_iterator MVCIter;
    typedef std::map<Edge,EdgeAttribute> EMap;
    typedef EMap::iterator MEIter;
    typedef EMap::const_iterator MECIter;
    typedef std::map<Triangle,TriangleAttribute> TMap;
    typedef TMap::iterator MTIter;
    typedef TMap::const_iterator MTCIter;

    // insert and remove triangles
    void InsertTriangle (const Triangle& rkT);
    void RemoveTriangle (const Triangle& rkT);

    // Callbacks that are executed when vertices, edges, or triangles are
    // inserted or removed during triangle insertion, triangle removal, or
    // edge collapsing.
    void OnEdgeInsert (const Edge& rkE, bool bCreate, HeapRecord*& rpkRecord);
    void OnEdgeRemove (const Edge& rkE, bool bDestroy, HeapRecord* pkRecord);

    // triangle attributes
    void SetData (const Triangle& rkT, int iIndex);
    int GetData (const Triangle& rkT);

    void RemoveTriangleDuringCollapse (const Triangle& rkT);
    void ModifyTriangle (Triangle& rkT, int iVKeep, int iVThrow);
    void CollapseEdge (int iVKeep, int iVThrow);
    void FlushVertices ();
    void FlushTriangles ();

    void DoCollapse ();
    bool CollapseCausesFolding (int iVKeep, int iVThrow);
    float GetMetric (MEIter pkE);
    void Reorder ();
    void ComputeRecords (int& riCQuantity, CollapseRecord*& rakCRecord);

    void InitializeHeap ();
    void Sort ();
    void Add (float fMetric);
    void Remove ();
    void Update (int iHIndex, float fMetric);
    bool IsValidHeap (int iStart, int iFinal);
    bool IsValidHeap ();

    // write the heap to an ASCII file (for debugging)
    void PrintHeap (const char* acFilename);

    // write the mesh to an ASCII file (for debugging)
    void Print (const char* acFilename) const;

    // triangle mesh to be decimated
    int m_iVQuantity;
    VertexBuffer* m_pkVBuffer;
    int m_iTQuantity;
    int* m_aiConnect;

    int m_iHQuantity;
    HeapRecord** m_apkHeap;
    bool m_bCollapsing;

    // for reordering vertices and triangles
    TSmallUnorderedSet<int> m_kVDelete;
    int m_iVCurrent, m_iTCurrent;
    int* m_aiVOrdered;
    int* m_aiVPermute;
    int* m_aiNewConnect;
    std::vector<CollapseRecord> m_kEDelete;

    // vertex-edge-triangle data structure
    std::map<int,VertexAttribute> m_kVMap;
    std::map<Edge,EdgeAttribute> m_kEMap;
    std::map<Triangle,TriangleAttribute> m_kTMap;
};

}

#endif
