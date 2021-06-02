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

#ifndef WM4INTPBICUBIC2_H
#define WM4INTPBICUBIC2_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntpBicubic2
{
public:
    // Construction and destruction.  IntpBicubic2 does not accept
    // responsibility for deleting the input array.  The application must do
    // so.  The interpolator is for uniformly spaced (x,y)-values.  The
    // function values are assumed to be organized as f(x,y) = F[y][x].
    // Exact interpolation is achieved by setting bCatmullRom to 'true',
    // giving you the Catmull-Rom blending matrix.  If a smooth interpolation
    // is desired, set bCatmullRom to 'false' to obtain B-spline blending.

    IntpBicubic2 (int iXBound, int iYBound, Real fXMin, Real fXSpacing,
        Real fYMin, Real fYSpacing, Real** aafF, bool bCatmullRom);

    int GetXBound () const;
    int GetYBound () const;
    int GetQuantity () const;
    Real** GetF () const;

    Real GetXMin () const;
    Real GetXMax () const;
    Real GetXSpacing () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetYSpacing () const;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax and ymin <= y <= ymax.
    // If (x,y) is outside the extremes, the function returns MAXREAL.  The
    // first operator is for function evaluation.  The second operator is for
    // function or derivative evaluations.  The uiXOrder argument is the order
    // of the x-derivative and the uiYOrder argument is the order of the
    // y-derivative.  Both orders are zero to get the function value itself.
    Real operator() (Real fX, Real fY) const;
    Real operator() (int iXOrder, int iYOrder, Real fX, Real fY) const;

private:
    int m_iXBound, m_iYBound, m_iQuantity;
    Real m_fXMin, m_fXMax, m_fXSpacing, m_fInvXSpacing;
    Real m_fYMin, m_fYMax, m_fYSpacing, m_fInvYSpacing;
    Real** m_aafF;
    const Real (*m_aafBlend)[4];

    static const Real ms_aafCRBlend[4][4];
    static const Real ms_aafBSBlend[4][4];
};

typedef IntpBicubic2<float> IntpBicubic2f;
typedef IntpBicubic2<double> IntpBicubic2d;

}

#endif
