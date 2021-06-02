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
// Version: 4.6.0 (2007/08/20)

#ifndef WM4CONTPOINTINPOLYHEDRON3_H
#define WM4CONTPOINTINPOLYHEDRON3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Plane3.h"
#include "Wm4Ray3.h"
#include "Wm4Vector2.h"

// This class contains various implementations for point-in-polyhedron
// queries.  The planes stored with the faces are used in all cases to
// reject ray-face intersection tests, a quick culling operation.
//
// The algorithm is to cast a ray from the input point P and test for
// intersection against each face of the polyhedron.  If the ray only
// intersects faces at interior points (not vertices, not edge points),
// then the point is inside when the number of intersections is odd and
// the point is outside when the number of intersections is even.  If the
// ray intersects an edge or a vertex, then the counting must be handled
// differently.  The details are tedious.  As an alternative, the approach
// here is to allow you to specify 2*N+1 rays, where N >= 0.  You should
// choose these rays randomly.  Each ray reports "inside" or "outside".
// Whichever result occurs N+1 or more times is the "winner".  The input
// rayQuantity is 2*N+1.  The input array Direction must have rayQuantity
// elements.  If you are feeling lucky, choose rayQuantity to be 1.
//
// TO DO.  Add a Contains function that uses exact arithmetic and that
// casts one ray, keeping track of the parity correctly when the ray
// intersects a vertex or an edge.  If the faces are always triangles,
// the use of barycentric coordinates gives us a "normalized" measurement
// of how close to a vertex or an edge the intersection point is.  This,
// in turn, might make it easy to use filtered predicates, which will be
// faster than always using exact arithmetic.  If the faces are not
// triangles, any triangulation introduces edges that are not face
// boundary edges.  These should be ignored by the special-case handling
// of boundary edges.  In the most general case of simple polygon faces
// without triangulation, it is not immediately clear how to compute a
// "normalized" measurement that will allow us to use filtered predicates
// in an easy manner.

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM PointInPolyhedron3
{
public:
    // For simple polyhedra with triangle faces.
    class TriangleFace
    {
    public:
        // When you view the face from outside, the vertices are
        // counterclockwise ordered.  The Indices array stores the indices
        // into the vertex array.
        int Indices[3];

        // The normal vector is unit length and points to the outside of the
        // polyhedron.
        Plane3<Real> Plane;
    };

    // The Contains query will use ray-triangle intersection queries.
    PointInPolyhedron3 (int iVQuantity, const Vector3<Real>* akVertex,
        int iFQuantity, const TriangleFace* akFace, int iRayQuantity,
        const Vector3<Real>* akDirection);

    // For simple polyhedra with convex polygon faces.
    class ConvexFace
    {
    public:
        // When you view the face from outside, the vertices are
        // counterclockwise ordered.  The Indices array stores the indices
        // into the vertex array.
        std::vector<int> Indices;

        // The normal vector is unit length and points to the outside of the
        // polyhedron.
        Plane3<Real> Plane;
    };

    // The Contains query will use ray-convexpolygon intersection queries.  A
    // ray-convexpolygon intersection query can be implemented in many ways.
    // In this context, uiMethod is one of three value:
    //   0 : Use a triangle fan and perform a ray-triangle intersection query
    //       for each triangle.
    //   1 : Find the point of intersection of ray and plane of polygon.  Test
    //       whether that point is inside the convex polygon using an O(N)
    //       test.
    //   2 : Find the point of intersection of ray and plane of polygon.  Test
    //       whether that point is inside the convex polygon using an O(log N)
    //       test.
    PointInPolyhedron3 (int iVQuantity, const Vector3<Real>* akVertex,
        int iFQuantity, const ConvexFace* akFace, int iRayQuantity,
        const Vector3<Real>* akDirection, unsigned int uiMethod);

    // For simple polyhedra with simple polygon faces that are generally
    // not all convex.
    class SimpleFace
    {
    public:
        // When you view the face from outside, the vertices are
        // counterclockwise ordered.  The Indices array stores the indices
        // into the vertex array.
        std::vector<int> Indices;

        // The normal vector is unit length and points to the outside of the
        // polyhedron.
        Plane3<Real> Plane;

        // Each simple face may be triangulated.  The indices are relative to
        // the vertex array.  Each triple of indices represents a triangle in
        // the triangulation.
        std::vector<int> Triangles;
    };

    // The Contains query will use ray-simplepolygon intersection queries.  A
    // ray-simplepolygon intersection query can be implemented in a couple of
    // ways.  In this context, uiMethod is one of two value:
    //   0 : Iterate over the triangles of each face and perform a
    //       ray-triangle intersection query for each triangle.  This requires
    //       that the SimpleFace::Triangles array be initialized for each
    //       face.
    //   1 : Find the point of intersection of ray and plane of polygon.  Test
    //       whether that point is inside the polygon using an O(N) test.  The
    //       SimpleFace::Triangles array is not used for this method, so it
    //       does not have to be initialized for each face.
    PointInPolyhedron3 (int iVQuantity, const Vector3<Real>* akVertex,
        int iFQuantity, const SimpleFace* akFace, int iRayQuantity,
        const Vector3<Real>* akDirection, unsigned int uiMethod);

    // This function will select the actual algorithm based on which
    // constructor you used for this class.
    bool Contains (const Vector3<Real>& rkP) const;

private:
    // For all types of faces.  The ray origin is the test point.  The ray
    // direction is one of those passed to the constructors.  The plane origin
    // is a point on the plane of the face.  The plane normal is a unit-length
    // normal to the face and that points outside the polyhedron.
    static bool FastNoIntersect (const Ray3<Real>& rkRay,
        const Plane3<Real>& rkPlane);

    // For triangle faces.
    bool ContainsT0 (const Vector3<Real>& rkP) const;

    // For convex faces.
    bool ContainsC0 (const Vector3<Real>& rkP) const;
    bool ContainsC1C2 (const Vector3<Real>& rkP, unsigned int uiMethod) const;

    // For simple faces.
    bool ContainsS0 (const Vector3<Real>& rkP) const;
    bool ContainsS1 (const Vector3<Real>& rkP) const;

    int m_iVQuantity;
    const Vector3<Real>* m_akVertex;

    int m_iFQuantity;
    const TriangleFace* m_akTFace;
    const ConvexFace* m_akCFace;
    const SimpleFace* m_akSFace;

    unsigned int m_uiMethod;
    int m_iRayQuantity;
    const Vector3<Real>* m_akDirection;

    // Temporary storage for those methods that reduce the problem to 2D
    // point-in-polygon queries.  The array stores the projections of
    // face vertices onto the plane of the face.  It is resized as needed.
    mutable std::vector<Vector2<Real> > m_kProjVertices;
};

typedef PointInPolyhedron3<float> PointInPolyhedron3f;
typedef PointInPolyhedron3<double> PointInPolyhedron3d;

}

#endif
