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

#ifndef WM4APPRPOLYFIT2_H
#define WM4APPRPOLYFIT2_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

// The samples are (x[i],w[i]) for 0 <= i < S.  Think of w as a function of
// x, say w = f(x).  The function fits the samples with a polynomial of
// degree d, say w = sum_{i=0}^d c[i]*x^i.  The method is a least-squares
// fitting algorithm.  The returned array stores the coefficients c[i] for
// 0 <= i <= d.  The caller is responsible for deleting the input arrays if
// they were dynamically allocated.  The caller is also responsible for
// deleting the returned array.
//
// WARNING.  The fitting algorithm for polynomial terms
//   (1,x,x^2,...,x^d)
// is known to be nonrobust for large degrees and for large magnitude data.
// One alternative is to use orthogonal polynomials
//   (f[0](x),...,f[d](x))
// and apply the least-squares algorithm to these.  Another alternative is to
// transform
//   (x',w') = ((x-xcen)/rng, w/rng)
// where xmin = min(x[i]), xmax = max(x[i]), xcen = (xmin+xmax)/2, and
// rng = xmax-xmin.  Fit the (x',w') points,
//   w' = sum_{i=0}^d c'[i]*(x')^i.
// The original polynomial is evaluated as
//   w = rng*sum_{i=0}^d c'[i]*((x-xcen)/rng)^i

template <class Real> WM4_FOUNDATION_ITEM
Real* PolyFit2 (int iSamples, const Real* afX, const Real* afW, int iXDegree);

}

#endif
