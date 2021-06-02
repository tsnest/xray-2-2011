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
// Version: 4.0.1 (2007/05/06)

#ifndef WM4EXTREMALQUERY3BSP_H
#define WM4EXTREMALQUERY3BSP_H

#include "Wm4PhysicsLIB.h"
#include "Wm4ExtremalQuery3.h"

namespace Wm4
{

class BasicMesh;

template <class Real>
class WM4_PHYSICS_ITEM ExtremalQuery3BSP : public ExtremalQuery3<Real>
{
public:
    ExtremalQuery3BSP (const ConvexPolyhedron3<Real>& rkPolytope);
    virtual ~ExtremalQuery3BSP ();

    // Compute the extreme vertices in the specified direction and return the
    // indices of the vertices in the polyhedron vertex array.
    virtual void GetExtremeVertices (const Vector3<Real>& rkDirection,
        int& riPositiveDirection, int& riNegativeDirection);

    // tree statistics
    int GetNodeQuantity () const;
    int GetTreeDepth () const;

private:
    using ExtremalQuery3<Real>::m_pkPolytope;
    using ExtremalQuery3<Real>::m_akFaceNormal;

    class WM4_PHYSICS_ITEM SphericalArc
    {
    public:
        SphericalArc ()
        {
            PosChild = -1;
            NegChild = -1;
        }

        // Indices into the face normal array for the end points of the arc.
        int N[2];

        // The number of arcs in the path from normal N[0] to normal N[1].
        // For spherical polygon edges, the number is 1.  The number is 2 or
        // larger for bisector arcs of the spherical polygon.
        int Separation;

        // The normal is Cross(FaceNormal[N[0]],FaceNormal[N[1]]).
        Vector3<Real> Normal;

        // Indices into the vertex array for the extremal points for the
        // two regions sharing the arc.  As the arc is traversed from normal
        // N[0] to normal N[1], PosVertex is the index for the extreme vertex
        // to the left of the arc and NegVertex is the index for the extreme
        // vertex to the right of the arc.
        int PosVertex, NegVertex;

        // Support for BSP trees stored as contiguous nodes in an array.
        int PosChild, NegChild;

        // The arcs are stored in a multiset ordered by increasing separation.
        // The multiset will be traversed in reverse order.  This heuristic is
        // designed to create BSP trees whose top-most nodes can eliminate as
        // many arcs as possible during an extremal query.
        bool operator< (const SphericalArc& rkArc) const
        {
            return Separation < rkArc.Separation;
        }
    };

    void SortVertexAdjacents (BasicMesh& rkMesh);
    void CreateSphericalArcs (BasicMesh& rkMesh,
        std::multiset<SphericalArc>& rkArcs);
    void CreateSphericalBisectors (BasicMesh& rkMesh,
        std::multiset<SphericalArc>& rkArcs);

    void CreateBSPTree (std::multiset<SphericalArc>& rkArcs,
        std::vector<SphericalArc>& rkNodes);
    void InsertArc (const SphericalArc& rkArc,
        std::vector<SphericalArc>& rkNodes);

    // Fixed-size storage for the BSP nodes.
    int m_iNodeQuantity;
    SphericalArc* m_akNode;
    int m_iTreeDepth;
};

typedef ExtremalQuery3BSP<float> ExtremalQuery3BSPf;
typedef ExtremalQuery3BSP<double> ExtremalQuery3BSPd;

}

#endif
