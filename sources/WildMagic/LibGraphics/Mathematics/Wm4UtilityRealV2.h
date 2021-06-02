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
// Version: 4.0.0 (2006/09/06)

#ifndef WM4UTILITYREALV2_H
#define WM4UTILITYREALV2_H

#include "Wm4GraphicsLIB.h"
#include "Wm4UtilityRealV.h"

namespace Wm4
{
// The input is V = (x,y).  The function returns (y,-x).
template <typename Real>
RealV<Real,2> Perp (const RealV<Real,2>& rkV);

// The input is V = (x,y).  The function returns (y,-x)/sqrt(x*x + y*y).
template <typename Real>
RealV<Real,2> UnitPerp (const RealV<Real,2>& rkV);

// The inputs are U = (x0,y0) and V = (x1,y1).  The function returns
// x0*y1 - x1*y0.
template <typename Real>
Real DotPerp (const RealV<Real,2>& rkU, const RealV<Real,2>& rkV);

// Compute the barycentric coordinates of the point P with respect to the
// triangle <V0,V1,V2>, P = b0*V0 + b1*V1 + b2*V2, where b0 + b1 + b2 = 1.
template <typename Real>
void GetBarycentrics (const RealV<Real,2>& rkP, const RealV<Real,2>& rkV0,
    const RealV<Real,2>& rkV1, const RealV<Real,2>& rkV2, Real afBary[3]);

// Gram-Schmidt orthonormalization.  Take linearly independent vectors V0 and
// V1 and compute an orthonormal set (unit length, mutually perpendicular).
// The return value is 'true' iff the construction is successful (V0 and V1
// are linearly independent).
template <typename Real>
bool Orthonormalize (RealV<Real,2>& rkV0, RealV<Real,2>& rkV1);

template <typename Real>
bool Orthonormalize (RealV<Real,2> akV[2]);

// The input V1 must be a nonzero vector.  The input V0 need not be
// initialized.  The output is an orthonormal basis {U0,U1}.  The input V1 is
// normalized by this function to produce U1.  If you know V1 is already unit
// length, use U1 = V1 and U0 = Perp(U1).  The return value is 'true' iff the
// construction is successful (V1 is a nonzero vector).
template <typename Real>
bool GenerateOrthonormalBasis (RealV<Real,2>& rkV0, RealV<Real,2>& rkV1);

#include "Wm4UtilityRealV2.inl"
}

#endif
