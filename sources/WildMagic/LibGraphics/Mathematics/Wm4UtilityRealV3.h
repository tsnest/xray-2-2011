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

#ifndef WM4UTILITYREALV3_H
#define WM4UTILITYREALV3_H

#include "Wm4GraphicsLIB.h"
#include "Wm4UtilityRealV.h"

namespace Wm4
{
// The inputs are V0 = (x0,y0,z0) and V1 = (x1,y1,z1).  The function returns
// the cross product, Cross(V0,V1) = (y0*z1-y1*z0,z0*x1-z1*x0,x0*y1-x1*y0).
template <typename Real>
RealV<Real,3> Cross (const RealV<Real,3>& rkV0, const RealV<Real,3>& rkV1);

// The inputs are V0 = (x0,y0,z0) and V1 = (x1,y1,z1).  The function returns
// the unit-length cross product, Cross(V0,V1)/|Length(V0,V1)|, when V0 and
// V1 are linearly independent.  If V0 and V1 are linearly dependent, then
// the function returns the zero vector.
template <typename Real>
RealV<Real,3> UnitCross (const RealV<Real,3>& rkV0,
    const RealV<Real,3>& rkV1);

// Compute the barycentric coordinates of the point P with respect to the
// tetrahedron <V0,V1,V2,V3>, P = b0*V0 + b1*V1 + b2*V2 + b3*V3, where
// b0 + b1 + b2 + b3 = 1.
template <typename Real>
void GetBarycentrics (const RealV<Real,3>& rkP, const RealV<Real,3>& rkV0,
    const RealV<Real,3>& rkV1, const RealV<Real,3>& rkV2,
    const RealV<Real,3>& rkV3, Real afBary[4]);

// Gram-Schmidt orthonormalization.  Take linearly independent vectors V0,
// V1, and V2 and compute an orthonormal set (unit length, mutually
// perpendicular).  The return value is 'true' iff the construction is
// successful (V0, V1, and V2 are linearly independent).
template <typename Real>
bool Orthonormalize (RealV<Real,3>& rkV0, RealV<Real,3>& rkV1,
    RealV<Real,3>& rkV2);

template <typename Real>
bool Orthonormalize (RealV<Real,3> akV[3]);

// The input V2 must be a nonzero vector.  The inputs V0 and V1 need not be
// initialized.  The output is an orthonormal basis {U0,U1,U2}.  The input V2
// is normalized by this function to produce U2.  If you know Vw is already
// unit length, use GenerateComplementBasis(V0,V1,V2) to compute V0 and V1.
// The return value is 'true' iff the construction is successful (V2 is a
// nonzero vector).
template <typename Real>
bool GenerateOrthonormalBasis (RealV<Real,3>& rkV0, RealV<Real,3>& rkV1,
    RealV<Real,3>& rkV2);

// The input V2 must be a unit-length vector.  The output vectors {V0,V1} are
// unit length and mutually perpendicular, and {V0,V1,V2} is an orthonormal
// set.
template <typename Real>
void GenerateComplementBasis (RealV<Real,3>& rkV0, RealV<Real,3>& rkV1,
    const RealV<Real,3>& rkV2);

#include "Wm4UtilityRealV3.inl"
}

#endif
