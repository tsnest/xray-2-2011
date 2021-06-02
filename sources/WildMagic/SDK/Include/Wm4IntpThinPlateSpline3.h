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

#ifndef WM4INTPTHINPLATESPLINE3_H
#define WM4INTPTHINPLATESPLINE3_H

// WARNING.  This code maps the inputs (x,y,z) to the unit cube.  The thin
// plate spline function evaluation maps the input to the unit cube and
// performs the interpolation in that space.  The idea is to keep the floating
// point numbers to order 1 for numerical stability of the algorithm. Some
// folks are not excited about this preprocessing step as it is not part of
// the classical thin plate spline algorithm.  It is easy enough for you to
// remove the remappings from the code.

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntpThinPlateSpline3
{
public:
    // Construction and destruction.  Data points are of form
    // (x,y,z,f(x,y,z)).  The smoothing parameter must be nonnegative.  If you
    // want IntpThinPlateSpline3 to delete the input arrays during
    // destruction, set bOwner to 'true'.  Otherwise, you own the arrays and
    // must delete them yourself.
    IntpThinPlateSpline3 (int iQuantity, Real* afX, Real* afY, Real* afZ,
        Real* afF, Real fSmooth, bool bOwner);

    ~IntpThinPlateSpline3 ();

    // Check this after the constructor call to see if the thin plate spline
    // coefficients were successfully computed.  If so, then calls to
    // operator()(Real,Real,Real) will work properly.
    bool IsInitialized () const;

    // Evaluate the thin plate spline interpolator.  If IsInitialized()
    // returns 'false', this operator will always return MAX_REAL.
    Real operator() (Real fX, Real fY, Real fZ);

private:
    static Real Kernel (Real fT);

    bool m_bInitialized;
    int m_iQuantity;

    // input data mapped to unit cube
    Real* m_afX;
    Real* m_afY;
    Real* m_afZ;

    // thin plate spline coefficients
    Real* m_afA;
    Real m_afB[4];

    // extent of input data
    Real m_fXMin, m_fXMax, m_fXInvRange;
    Real m_fYMin, m_fYMax, m_fYInvRange;
    Real m_fZMin, m_fZMax, m_fZInvRange;
};

typedef IntpThinPlateSpline3<float> IntpThinPlateSpline3f;
typedef IntpThinPlateSpline3<double> IntpThinPlateSpline3d;

}

#endif
