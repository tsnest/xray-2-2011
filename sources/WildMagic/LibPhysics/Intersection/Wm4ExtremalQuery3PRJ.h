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

#ifndef WM4EXTREMALQUERY3PRJ_H
#define WM4EXTREMALQUERY3PRJ_H

#include "Wm4PhysicsLIB.h"
#include "Wm4ExtremalQuery3.h"

namespace Wm4
{

template <class Real>
class WM4_PHYSICS_ITEM ExtremalQuery3PRJ : public ExtremalQuery3<Real>
{
public:
    ExtremalQuery3PRJ (const ConvexPolyhedron3<Real>& rkPolytope);
    virtual ~ExtremalQuery3PRJ ();

    // Compute the extreme vertices in the specified direction and return the
    // indices of the vertices in the polyhedron vertex array.
    virtual void GetExtremeVertices (const Vector3<Real>& rkDirection,
        int& riPositiveDirection, int& riNegativeDirection);

private:
    using ExtremalQuery3<Real>::m_pkPolytope;

    Vector3<Real> m_kCentroid;
};

typedef ExtremalQuery3PRJ<float> ExtremalQuery3PRJf;
typedef ExtremalQuery3PRJ<double> ExtremalQuery3PRJd;

}

#endif
