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
// Version: 4.0.1 (2006/09/21)

#ifndef WM4EXTRACTSURFACETETRA_H
#define WM4EXTRACTSURFACETETRA_H

#include "Wm4ImagicsLIB.h"
#include "Wm4Vector3.h"
#include "Wm4EdgeKey.h"
#include "Wm4TriangleKey.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM ExtractSurfaceTetra
{
public:
    // The input is a 3D image with lexicographically ordered voxels in
    // (x,y,z) stored in a linear array.  Voxel (x,y,z) is stored in the
    // array at location x+xbound*(y+ybound*z).  The caller is responsible
    // for deleting aiData if it was dynamically allocated.
    ExtractSurfaceTetra (int iXBound, int iYBound, int iZBound, int* aiData);

    // The extraction assumes linear interpolation (decomposition of image
    // domain into tetrahedra).  The triangle ordering is selected so that
    // the triangle is counterclockwise with respect to the triangle normal
    // chosen to form an acute angle with the negative gradient of the
    // image at the centroid of the triangle.
    void ExtractContour (int iLevel, std::vector<Vector3f>& rkVA,
        std::vector<TriangleKey>& rkTA);

    // The extraction has duplicate vertices on edges shared by voxels.  This
    // function will eliminate the duplication.
    void MakeUnique (std::vector<Vector3f>& rkVA,
        std::vector<TriangleKey>& rkTA);

    // The extraction does not use any topological information about the level
    // surface.  The triangles can be a mixture of clockwise-ordered and
    // counterclockwise-ordered.  This function is an attempt to give the
    // triangles a consistent ordering by selecting a normal in approximately
    // the same direction as the average gradient at the vertices (when
    // bSameDir is true), or in the opposite direction (when bSameDir is
    // false).  This might not always produce a consistent order, but is
    // fast.  A consistent order can be computed if you build a table of
    // vertex, edge, and face adjacencies, but the resulting data structure
    // is very expensive to process to reorient triangles.
    void OrientTriangles (std::vector<Vector3f>& rkVA,
        std::vector<TriangleKey>& rkTA, bool bSameDir);

    // In case you want vertex normals for dynamic lighting of the mesh.
    void ComputeNormals (const std::vector<Vector3f>& rkVA,
        const std::vector<TriangleKey>& rkTA, std::vector<Vector3f>& rkNA);

protected:
    class WM4_IMAGICS_ITEM Vertex
    {
    public:
        Vertex (int iXNumer = 0, int iXDenom = 0, int iYNumer = 0,
            int iYDenom = 0, int iZNumer = 0, int iZDenom = 0);

        // for STL map
        bool operator< (const Vertex& rkVertex) const;
        //bool operator== (const Vertex& rkVertex) const;
        //bool operator!= (const Vertex& rkVertex) const;

        void GetTriple (Vector3f& rkMeshVertex) const;

        // vertices stored as rational numbers
        //   (xnumer/xdenom,ynumer/ydenom,znumer/zdenom)
        int m_iXNumer, m_iXDenom, m_iYNumer, m_iYDenom, m_iZNumer, m_iZDenom;
    };

    // support for extraction of level sets
    typedef std::map<Vertex,int> VtxMap;
    typedef VtxMap::iterator VtxMapIterator;
    typedef std::set<EdgeKey> ESet;
    typedef ESet::iterator ESetIterator;
    typedef std::set<TriangleKey> TriSet;
    typedef TriSet::iterator TSetIterator;

    int AddVertex (VtxMap& rkVMap, int iXNumer, int iXDenom, int iYNumer,
        int iYDenom, int iZNumer, int iZDenom);

    void AddEdge (VtxMap& rkVMap, ESet& rkESet, int iXNumer0,
        int iXDenom0, int iYNumer0, int iYDenom0, int iZNumer0, int iZDenom0,
        int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1, int iZNumer1,
        int iZDenom1);

    void AddTriangle (VtxMap& rkVMap, ESet& rkESet, TriSet& rkTSet,
        int iXNumer0, int iXDenom0, int iYNumer0, int iYDenom0, int iZNumer0,
        int iZDenom0, int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1,
        int iZNumer1, int iZDenom1, int iXNumer2, int iXDenom2, int iYNumer2,
        int iYDenom2, int iZNumer2, int iZDenom2);

    // support for extraction with linear interpolation
    void ProcessTetrahedron (VtxMap& rkVM, ESet& rkES, TriSet& rkTS,
        int iX0, int iY0, int iZ0, int iF0, int iX1, int iY1, int iZ1,
        int iF1, int iX2, int iY2, int iZ2, int iF2, int iX3, int iY3,
        int iZ3, int iF3);

    // function and gradient evaluated using trilinear interpolation
    float GetFunction (const Vector3f& rkP) const;
    Vector3f GetGradient (const Vector3f& rkP) const;

    // for unique indexing of vertices
    int m_iNextIndex;

    int m_iXBound, m_iYBound, m_iZBound, m_iXYBound, m_iXYZBound;
    int* m_aiData;
};

}

#endif
