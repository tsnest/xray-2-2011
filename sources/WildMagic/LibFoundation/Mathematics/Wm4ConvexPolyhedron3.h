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

#ifndef WM4CONVEXPOLYHEDRON3_H
#define WM4CONVEXPOLYHEDRON3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Polyhedron3.h"
#include "Wm4Plane3.h"

namespace Wm4
{

template <class Real>
class ConvexPolyhedron3 : public Polyhedron3<Real>
{
public:
    // Construction and destruction.  The caller is responsible for ensuring
    // that the mesh represents a convex polyhedron.  The triangular faces
    // must be ordered counterclockwise when viewed from outside the mesh.
    // To assign ConvexPolyhedron3 the ownership of the input arrays, set
    // bOwner to 'true'; otherwise, set it to 'false', in which case the
    // caller is responsible for deleting the arrays if they were dynamically
    // allocated.
    //
    // The class stores the planes associated with the faces, and the plane
    // normals are inner pointing.  The planes may be supplied to the
    // constructor, in which case the ownership flag applies.  If they are not
    // passed (akPlane is null), the class automatically generates them and
    // owns this array, even if the other arrays are not owned.
    ConvexPolyhedron3 (int iVQuantity, Vector3<Real>* akVertex,
        int iTQuantity, int* aiIndex, Plane3<Real>* akPlane, bool bOwner);

    // Copy constructor.  If the input polyhedron owns its data, then a copy
    // is made for 'this' object, in which case 'this' owns its data.  If the
    // input polyhedron does not own its data, 'this' shares that same data
    // so does not own it.
    ConvexPolyhedron3 (const ConvexPolyhedron3& rkPoly);

    virtual ~ConvexPolyhedron3 ();

    // Assignment.  If the input polyhedron owns its data, then a copy is
    // made for 'this' object, in which case 'this' owns its data.  If the
    // input polyhedron does not own its data, 'this' shares that same data
    // so does not own it.
    ConvexPolyhedron3& operator= (const ConvexPolyhedron3& rkPoly);

    // read-only member access
    const Plane3<Real>* GetPlanes () const;
    const Plane3<Real>& GetPlane (int i) const;

    // Allow vertex modification.  The caller must guarantee that the mesh
    // will remain convex.  After modifying as many vertices as you like,
    // call UpdatePlanes().  If the modifications are all done via SetVertex,
    // then the planes are updated only for those triangles affected by the
    // modifications.  If any modifications are done by accessing the vertex
    // array via GetVertices, this class has no knowledge of the changes, in
    // which case UpdatePlanes() will recompute all the planes.
    virtual void SetVertex (int i, const Vector3<Real>& rkV);
    void UpdatePlanes ();

    // Test for convexity.  This function will iterate over the faces of the
    // polyhedron and verify for each face that the polyhedron vertices are
    // all on the nonnegative side of the facet plane.  A signed distance test
    // is used, so a vertex is on the wrong side of a plane (for convexity)
    // when its signed distance satisfies d < 0.  Numerical round-off errors
    // can generate incorrect convexity tests, so a small negative threshold
    // t may be passed to this function, in which case the distance test
    // becomes d < t < 0.
    bool IsConvex (Real fThreshold = (Real)0.0) const;

    // Point-in-polyhedron test.  The threshold serves the same purpose as
    // the one in IsConvex.
    bool ContainsPoint (const Vector3<Real>& rkP, Real fThreshold = (Real)0.0)
        const;

protected:
    using Polyhedron3<Real>::m_iVQuantity;
    using Polyhedron3<Real>::m_akVertex;
    using Polyhedron3<Real>::m_iTQuantity;
    using Polyhedron3<Real>::m_aiIndex;

    Plane3<Real>* m_akPlane;
    bool m_bPlaneOwner;

    // Support for efficient updating of facet planes.  The set stores the
    // indices for those triangles affected by modifying vertices.
    void UpdatePlane (int i, const Vector3<Real>& rkAverage);
    std::set<int> m_kTModified;
};

#include "Wm4ConvexPolyhedron3.inl"

typedef ConvexPolyhedron3<float> ConvexPolyhedron3f;
typedef ConvexPolyhedron3<double> ConvexPolyhedron3d;

}

#endif
