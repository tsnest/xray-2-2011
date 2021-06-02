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
// Version: 4.4.0 (2007/06/07)

#ifndef WM4VETMESH_H
#define WM4VETMESH_H

#include "Wm4FoundationLIB.h"
#include "Wm4Math.h"
#include "Wm4TSmallUnorderedSet.h"

namespace Wm4
{

class WM4_FOUNDATION_ITEM VETMesh
{
public:
    // A vertex is <v>.
    // An edge is <v0,v1> where v0 = min(v0,v1).
    // A triangle is <v0,v1,v2> where v0 = min(v0,v1,v2).

    class WM4_FOUNDATION_ITEM Edge
    {
    public:
        Edge ();
        Edge (int iV0, int iV1);
        bool operator< (const Edge& rkE) const;
        bool operator== (const Edge& rkE) const;
        bool operator!= (const Edge& rkE) const;

        int V[2];
    };

    class WM4_FOUNDATION_ITEM Triangle
    {
    public:
        Triangle ();
        Triangle (int iV0, int iV1, int iV2);
        bool operator< (const Triangle& rkT) const;
        bool operator== (const Triangle& rkT) const;
        bool operator!= (const Triangle& rkT) const;

        int V[3];
    };

    class WM4_FOUNDATION_ITEM VertexAttribute
    {
    public:
        VertexAttribute ()
            :
            ESet(8,8),
            TSet(8,8)
        {
            Data = 0;
        }

        void* Data;
        TSmallUnorderedSet<Edge> ESet;
        TSmallUnorderedSet<Triangle> TSet;
    };

    class WM4_FOUNDATION_ITEM EdgeAttribute
    {
    public:
        EdgeAttribute ()
            :
            TSet(2,2)
        {
            Data = 0;
        }

        void* Data;
        TSmallUnorderedSet<Triangle> TSet;
    };

    class WM4_FOUNDATION_ITEM TriangleAttribute
    {
    public:
        TriangleAttribute ()
        {
            Data = 0;
        }

        void* Data;
    };

    // For readability of the code.
    typedef std::map<int,VertexAttribute> VMap;
    typedef VMap::iterator MVIter;
    typedef VMap::const_iterator MVCIter;
    typedef std::map<Edge,EdgeAttribute> EMap;
    typedef EMap::iterator MEIter;
    typedef EMap::const_iterator MECIter;
    typedef std::map<Triangle,TriangleAttribute> TMap;
    typedef TMap::iterator MTIter;
    typedef TMap::const_iterator MTCIter;


    // Construction and destruction.
    VETMesh ();
    virtual ~VETMesh ();

    // Accessors for sizes.
    int GetVertexQuantity () const;
    int GetEdgeQuantity () const;
    int GetTriangleQuantity () const;

    // Used for operations that create new meshes from the current one.  This
    // allows derived class construction within the base class operations.
    virtual VETMesh* Create () const;

    // Callbacks that are executed when vertices, edges, or triangles are
    // inserted or removed during triangle insertion, triangle removal, or
    // edge collapsing.  The default behavior for the creation is to return
    // null pointers.  A derived class may override the creation and return
    // data to be attached to the specific mesh component.  The default
    // behavior for the destruction is to do nothing.  A derived class may
    // override the destruction and handle the data that was detached from
    // the specific mesh component before its destruction.
    virtual void OnVertexInsert (int iV, bool bCreate, void*& rpvData);
    virtual void OnVertexRemove (int iV, bool bDestroy, void* pvData);
    virtual void OnEdgeInsert (const Edge& rkE, bool bCreate, void*& rpvData);
    virtual void OnEdgeRemove (const Edge& rkE, bool bDestroy, void* pvData);
    virtual void OnTriangleInsert (const Triangle& rkT, bool bCreate,
        void*& rpvData);
    virtual void OnTriangleRemove (const Triangle& rkT, bool bDestroy,
        void* pvData);

    // Insert and remove triangles.  The functions are virtual in case a
    // derived class wants to implement pre- and post-operation semantics.
    virtual void InsertTriangle (int iV0, int iV1, int iV2);
    virtual void InsertTriangle (const Triangle& rkT);
    virtual void RemoveTriangle (int iV0, int iV1, int iV2);
    virtual void RemoveTriangle (const Triangle& rkT);

    // This should be called before Mesh destruction if a derived class has
    // allocated vertex, edge, or triangle data and attached it to the mesh
    // components.  Since the creation and destruction callbacks are virtual,
    // any insert/remove operations in the base Mesh destructor will only
    // call the base virtual callbacks, not any derived-class ones.  An
    // alternative to calling this is that the derived class maintain enough
    // information to know which data objects to destroy during its own
    // destructor call.
    virtual void RemoveAllTriangles ();

    // Write the mesh to an ASCII file.
    //void Print (const char* acFilename) const;

    // Vertex attributes.
    const VMap& GetVertexMap () const;
    void GetVertices (std::set<int>& rkVSet) const;
    void* GetData (int iV);
    const TSmallUnorderedSet<Edge>* GetEdges (int iV) const;
    const TSmallUnorderedSet<Triangle>* GetTriangles (int iV) const;

    // Edge attributes.
    const EMap& GetEdgeMap () const;
    void GetEdges (std::set<Edge>& rkESet) const;
    void* GetData (int iV0, int iV1);
    void* GetData (const Edge& rkE);
    const TSmallUnorderedSet<Triangle>* GetTriangles (int iV0, int iV1) const;

    // Triangle attributes.
    const TMap& GetTriangleMap () const;
    void GetTriangles (std::set<Triangle>& rkTSet) const;
    void* GetData (int iV0, int iV1, int iV2);
    void* GetData (const Triangle& rkT);

    // The mesh is manifold if each edge has at most two adjacent triangles.
    // It is possible that the mesh has multiple connected components.
    bool IsManifold () const;

    // The mesh is closed if each edge has exactly two adjacent triangles.
    // It is possible that the mesh has multiple connected components.
    bool IsClosed () const;

    // The mesh is connected if each triangle can be reached from any other
    // triangle by a traversal.
    bool IsConnected () const;

    // Extract the connected components from the mesh.  For large data sets,
    // the array of VETMesh can use a lot of memory.  Instead use the
    // second form that just stores a sorted connectivity array.  Let N be
    // the number of components.  The value Index[i] indicates the starting
    // index for component i with 0 <= i < N, so it is always the case that
    // Index[0] = 0.  The value Index[N] is the total number of indices in
    // the raiConnect array.  The quantity of indices for component i is
    // Q(i) = Index[i+1]-Index[i] for 0 <= i < N.  The application is
    // responsible for deleting raiConnect.
    void GetComponents (std::vector<VETMesh*>& rkComponents);
    void GetComponents (std::vector<int>& rkIndex, int*& raiConnect);

    // Extract a connected component from the mesh and remove all the
    // triangles of the component from the mesh.  This is useful for computing
    // the components in a very large mesh that uses a lot of memory.  The
    // intention is that the function is called until all components are
    // found.  The typical code is
    //
    //     VETMesh kMesh = <some mesh>;
    //     int iITotalQuantity = 3*kMesh.GetTriangleQuantity();
    //     int* aiConnect = new int[iITotalQuantity];
    //     for (int iIQuantity = 0; iIQuantity < iITotalQuantity; /**/ )
    //     {
    //         int iCurrentIQuantity;
    //         int* aiCurrentConnect = aiConnect + iIQuantity;
    //         kMesh.RemoveComponent(iCurrentIQuantity,aiCurrentConnect);
    //         iIQuantity += iCurrentIQuantity;
    //     }
    void RemoveComponent (int& riIQuantity, int* aiConnect);

    // Extract the connected components from the mesh, but each component has
    // a consistent ordering across all triangles of that component.  The
    // mesh must be manifold.  The return value is 'true' if and only if the
    // mesh is manifold.  If the mesh has multiple components, each component
    // will have a consistent ordering.  However, the mesh knows nothing about
    // the mesh geometry, so it is possible that ordering across components is
    // not consistent.  For example, if the mesh has two disjoint closed
    // manifold components, one of them could have an ordering that implies
    // outward pointing normals and the other inward pointing normals.
    //
    // NOTE.  It is possible to create a nonorientable mesh such as a Moebius
    // strip.  In this case, GetConsistentComponents will return connected
    // components, but in fact the triangles will not (and can not) be
    // consistently ordered.
    bool GetConsistentComponents (std::vector<VETMesh*>& rkComponents);

    // Reverse the ordering of all triangles in the mesh.
    VETMesh* GetReversedOrderMesh () const;

    // Statistics about the mesh.
    void GetStatistics (int& riVQuantity, int& riEQuantity, int& riTQuantity,
        float& rfAverageEdgesPerVertex, float& rfAverageTrianglesPerVertex,
        float& rfAverageTrianglesPerEdge, int& riMaximumEdgesPerVertex,
        int& riMaximumTrianglesPerVertex, int& riMaximumTrianglesPerEdge);

protected:
    std::map<int,VertexAttribute> m_kVMap;
    std::map<Edge,EdgeAttribute> m_kEMap;
    std::map<Triangle,TriangleAttribute> m_kTMap;
};

#include "Wm4VETMesh.inl"

}

#endif
