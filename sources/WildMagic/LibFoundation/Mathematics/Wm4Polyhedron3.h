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

#ifndef WM4POLYHEDRON3_H
#define WM4POLYHEDRON3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class Polyhedron3
{
public:
    // Construction and destruction.  The caller is responsible for ensuring
    // that the mesh represents a simple polyhedron (2-manifold with the
    // topology of a sphere, non-self-intersecting).  The triangular faces
    // must be orderd counterclockwise when viewed from outside the mesh.
    // To assign Polyhedron3 the ownership of the input arrays, set bOwner to
    // 'true'; otherwise, set it to 'false', in which case the caller is
    // responsible for deleting the arrays if they were dynamically allocated.
    Polyhedron3 (int iVQuantity, Vector3<Real>* akVertex, int iTQuantity,
        int* aiIndex, bool bOwner);

    // Copy constructor.  If the input polyhedron owns its data, then a copy
    // is made for 'this' object, in which case 'this' owns its data.  If the
    // input polyhedron does not own its data, 'this' shares that same data
    // so does not own it.
    Polyhedron3 (const Polyhedron3& rkPoly);

    virtual ~Polyhedron3 ();

    // Assignment.  If the input polyhedron owns its data, then a copy is
    // made for 'this' object, in which case 'this' owns its data.  If the
    // input polyhedron does not own its data, 'this' shares that same data
    // so does not own it.
    Polyhedron3& operator= (const Polyhedron3& rkPoly);

    // read-only member access
    int GetVQuantity () const;
    const Vector3<Real>* GetVertices () const;
    const Vector3<Real>& GetVertex (int i) const;
    int GetTQuantity () const;
    const int* GetIndices () const;
    const int* GetTriangle (int i) const;

    // Allow vertex modification.  No guarantee is made that the mesh will
    // remain non-self-intersecting.
    virtual void SetVertex (int i, const Vector3<Real>& rkV);
    Vector3<Real>* GetVertices ();

    // Compute various information about the polyhedron.
    Vector3<Real> ComputeVertexAverage () const;
    Real ComputeSurfaceArea () const;
    Real ComputeVolume () const;

protected:
    int m_iVQuantity;
    Vector3<Real>* m_akVertex;
    int m_iTQuantity;
    int* m_aiIndex;
    bool m_bOwner;
};

#include "Wm4Polyhedron3.inl"

typedef Polyhedron3<float> Polyhedron3f;
typedef Polyhedron3<double> Polyhedron3d;

}

#endif
