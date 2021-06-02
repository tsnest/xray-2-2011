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

#ifndef WM4CONTELLIPSOID3MINCR_H
#define WM4CONTELLIPSOID3MINCR_H

#include "Wm4FoundationLIB.h"
#include "Wm4Matrix3.h"

namespace Wm4
{

// The ellipsoid in general form is  X^t A X + B^t X + C = 0 where
// A is a positive definite 3x3 matrix, B is a 3x1 vector, C is a
// scalar, and X is a 3x1 vector.  Completing the square,
// (X-U)^t A (X-U) = U^t A U - C where U = -0.5 A^{-1} B.  Define
// M = A/(U^t A U - C).  The ellipsoid is (X-U)^t M (X-U) = 1.  Factor
// M = R^t D R where R is orthonormal and D is diagonal with positive
// diagonal terms.  If Y = R(X-U), then the ellipsoid is 1 = Y^t D Y =
// d1*y1^2+d2*y2^2+d3*y3^2.  For an ellipsoid (x/a)^2+(y/b)^2+(z/c)^2
// = 1, the volume is (4*pi/3)*a*b*c.  For Y^t D Y = 1, the volume is
// therefore (4*pi/3)/sqrt(d1*d2*d3).  Finally, note that det(M) =
// det(D) = d1*d2*d3, so the volume of the ellipsoid is
// (4*pi/3)/sqrt(det(M)).
//
// Compute minimal volume ellipsoid (X-C)^t R^t D R (X-C) = 1 given center
// C and orientation matrix R by finding diagonal D.  Minimal volume is
// (4*pi/3)/sqrt(D[0]*D[1]*D[2]).

template <class Real>
class WM4_FOUNDATION_ITEM ContEllipsoid3MinCR
{
public:
    ContEllipsoid3MinCR (int iQuantity, const Vector3<Real>* akPoint,
        const Vector3<Real>& rkC, const Matrix3<Real>& rkR, Real afD[3]);

private:
    void FindEdgeMax (int iQuantity, Real* afA, Real* afB, Real* afC,
        int& riPlane0, int& riPlane1, Real& rfX0, Real& rfY0, Real& rfZ0);

    void FindFacetMax (int iQuantity, Real* afA, Real* afB, Real* afC,
        int& riPlane0, Real& rfX0, Real& rfY0, Real& rfZ0);

    void MaxProduct (int iQuantity, Real* afA, Real* afB, Real* afC,
        Real& rfX, Real& rfY, Real& rfZ);
};

typedef ContEllipsoid3MinCR<float> ContEllipsoid3MinCRf;
typedef ContEllipsoid3MinCR<double> ContEllipsoid3MinCRd;

}

#endif
