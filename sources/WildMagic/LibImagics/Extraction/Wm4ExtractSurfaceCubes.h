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

#ifndef WM4EXTRACTSURFACECUBES_H
#define WM4EXTRACTSURFACECUBES_H

#include "Wm4ImagicsLIB.h"
#include "Wm4Vector3.h"
#include "Wm4TriangleKey.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM ExtractSurfaceCubes
{
public:
    // The input is a 3D image with lexicographically ordered voxels in
    // (x,y,z) stored in a linear array.  Voxel (x,y,z) is stored in the
    // array at location x+xbound*(y+ybound*z).  The caller is responsible
    // for deleting aiData if it was dynamically allocated.
    ExtractSurfaceCubes (int iXBound, int iYBound, int iZBound, int* aiData);

    // The level value *must* not be exactly an integer.  This simplifies the
    // level surface construction immensely.
    void ExtractContour (float fLevel, std::vector<Vector3f>& rkVA,
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
    enum
    {
        EI_XMIN_YMIN =  0,
        EI_XMIN_YMAX =  1,
        EI_XMAX_YMIN =  2,
        EI_XMAX_YMAX =  3,
        EI_XMIN_ZMIN =  4,
        EI_XMIN_ZMAX =  5,
        EI_XMAX_ZMIN =  6,
        EI_XMAX_ZMAX =  7,
        EI_YMIN_ZMIN =  8,
        EI_YMIN_ZMAX =  9,
        EI_YMAX_ZMIN = 10,
        EI_YMAX_ZMAX = 11,
        FI_XMIN      = 12,
        FI_XMAX      = 13,
        FI_YMIN      = 14,
        FI_YMAX      = 15,
        FI_ZMIN      = 16,
        FI_ZMAX      = 17,

        EB_XMIN_YMIN = 1 << EI_XMIN_YMIN,
        EB_XMIN_YMAX = 1 << EI_XMIN_YMAX,
        EB_XMAX_YMIN = 1 << EI_XMAX_YMIN,
        EB_XMAX_YMAX = 1 << EI_XMAX_YMAX,
        EB_XMIN_ZMIN = 1 << EI_XMIN_ZMIN,
        EB_XMIN_ZMAX = 1 << EI_XMIN_ZMAX,
        EB_XMAX_ZMIN = 1 << EI_XMAX_ZMIN,
        EB_XMAX_ZMAX = 1 << EI_XMAX_ZMAX,
        EB_YMIN_ZMIN = 1 << EI_YMIN_ZMIN,
        EB_YMIN_ZMAX = 1 << EI_YMIN_ZMAX,
        EB_YMAX_ZMIN = 1 << EI_YMAX_ZMIN,
        EB_YMAX_ZMAX = 1 << EI_YMAX_ZMAX,
        FB_XMIN      = 1 << FI_XMIN,
        FB_XMAX      = 1 << FI_XMAX,
        FB_YMIN      = 1 << FI_YMIN,
        FB_YMAX      = 1 << FI_YMAX,
        FB_ZMIN      = 1 << FI_ZMIN,
        FB_ZMAX      = 1 << FI_ZMAX
    };

    // vertex-edge-triangle table to support mesh topology
    class WM4_IMAGICS_ITEM VETable
    {
    public:
        VETable ();

        bool IsValidVertex (int i) const;
        float GetX (int i) const;
        float GetY (int i) const;
        float GetZ (int i) const;
        void Insert (int i, const Vector3f& rkP);
        void Insert (int i0, int i1);
        void RemoveTriangles (std::vector<Vector3f>& rkVA,
            std::vector<TriangleKey>& rkTA);

    protected:
        void RemoveVertex (int i);
        bool Remove (TriangleKey& rkT);

        class WM4_IMAGICS_ITEM Vertex
        {
        public:
            Vertex ();

            Vector3f P;
            int AdjQuantity;
            int Adj[4];
            bool Valid;
        };

        Vertex m_akVertex[18];
    };

    int GetVertices (float fLevel, int iX, int iY, int iZ, VETable& rkTable);

    void GetXMinEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetXMaxEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetYMinEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetYMaxEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetZMinEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetZMaxEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);

    Vector3f GetGradient (Vector3f kP);

    int m_iXBound, m_iYBound, m_iZBound, m_iXYBound;
    int* m_aiData;
};

#include "Wm4ExtractSurfaceCubes.inl"

}

#endif
