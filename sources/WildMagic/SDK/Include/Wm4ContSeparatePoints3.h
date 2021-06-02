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

#ifndef WM4CONTSEPARATEPOINTS3_H
#define WM4CONTSEPARATEPOINTS3_H

// Separate two point sets, if possible, by computing a plane for which the
// point sets lie on opposite sides.  The algorithm computes the convex hull
// of the point sets, then uses the method of separating axes to determine if
// the two convex polyhedra are disjoint.  The convex hull calculation is
// O(n*log(n)).  There is a randomized linear approach that takes O(n), but
// I have not yet implemented it.
//
// The return value of the function is 'true' if and only if there is a
// separation.  If 'true', the returned plane is a separating plane.

#include "Wm4FoundationLIB.h"
#include "Wm4Plane3.h"

namespace Wm4
{

// Assumes that both sets have at least 4 noncoplanar points.
template <class Real>
class WM4_FOUNDATION_ITEM SeparatePoints3
{
public:
    SeparatePoints3 (int iQuantity0, const Vector3<Real>* akVertex0,
        int iQuantity1, const Vector3<Real>* akVertex1,
        Plane3<Real>& rkSeprPlane);

    // Return the result of the constructor call.  If 'true', the output
    // plane rkSeprPlane is valid.
    operator bool ();

private:
    static int OnSameSide (const Plane3<Real>& rkPlane, int iTriangleQuantity,
        const int* aiIndex, const Vector3<Real>* akPoint);

    static int WhichSide (const Plane3<Real>& rkPlane, int iTriangleQuantity,
        const int* aiIndex, const Vector3<Real>* akPoint);

    bool m_bSeparated;
};

typedef SeparatePoints3<float> SeparatePoints3f;
typedef SeparatePoints3<double> SeparatePoints3d;

}

#endif
