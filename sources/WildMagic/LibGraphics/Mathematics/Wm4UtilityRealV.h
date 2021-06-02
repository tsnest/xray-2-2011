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

#ifndef WM4UTILITYREALV_H
#define WM4UTILITYREALV_H

#include "Wm4GraphicsLIB.h"
#include "Wm4RealV.h"

namespace Wm4
{
// The input is V = (x,y).  The function returns the length of the vector.
template <typename Real, int N>
Real Length (const RealV<Real,N>& rkV);

// The input is V = (x,y).  The function returns the squared-length of the
// vector.
template <typename Real, int N>
Real SquaredLength (const RealV<Real,N>& rkV);

// The inputs are V0 and V1 = (x1,y1).  The function returns the dot product
// of V0 and V1.
template <typename Real, int N>
Real Dot (const RealV<Real,N>& rkV0, const RealV<Real,N>& rkV1);

// The input is V.  If V has positive length, the function normalizes the
// the vector to V/|V| and returns the length |V|.  If V is the zero vector
// (or nearly zero when using floating-point arithmetic), the function sets
// V to the zero vector and returns a length of exactly zero.
template <typename Real, int N>
Real Normalize (RealV<Real,N>& rkV);

// Compute the extreme values of the input array of vectors.
template <typename Real, int N>
void ComputeExtremes (int iQuantity, const RealV<Real,N>* akVector,
    RealV<Real,N>& rkMin, RealV<Real,N>& rkMax);

#include "Wm4UtilityRealV.inl"
}

#endif
