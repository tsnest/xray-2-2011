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

#ifndef WM4POLYHEDRALMASSPROPERTIES_H
#define WM4POLYHEDRALMASSPROPERTIES_H

#include "Wm4PhysicsLIB.h"
#include "Wm4Matrix3.h"

namespace Wm4
{

// The input triangle mesh must represent a polyhedron.  The triangles are
// represented as triples of indices <V0,V1,V2> into the vertex array.
// The connectivity array has iTQuantity such triples.  The Boolean value
// bBodyCoords is 'true' if you want the inertia tensor to be relative to
// body coordinates, 'false' if you want it in world coordinates.

template <class Real> WM4_PHYSICS_ITEM
void ComputeMassProperties (const Vector3<Real>* akVertex, int iTQuantity,
    const int* aiIndex, bool bBodyCoords, Real& rfMass,
    Vector3<Real>& rkCenter, Matrix3<Real>& rkInertia);

}

#endif
