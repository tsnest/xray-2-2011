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

#ifndef WM4BSPLINEFITBASIS_H
#define WM4BSPLINEFITBASIS_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM BSplineFitBasis
{
public:
    // Construction and destruction.  This class is only for open uniform
    // B-spline basis functions.  The input is the number of control points
    // for a B-spline curve using this basis and the degree of that curve.
    BSplineFitBasis (int iQuantity, int iDegree);
    ~BSplineFitBasis ();

    // Data member access.
    int GetQuantity () const;
    int GetDegree () const;

    // Evaluate the basis functions.  This function fills in the values
    // returned by GetValue(i) for 0 <= i <= degree.  The return indices iMin
    // and iMax are relative to the array of control points.  The GetValue(i)
    // are the coefficients for the control points ctrl[iMin] throught
    // ctrl[iMax] in the curve evaluation (i.e. the curve has local control).
    void Compute (Real fT, int& iMin, int& iMax) const;
    Real GetValue (int i) const;

private:
    // The number of control points and degree for the curve.
    int m_iQuantity, m_iDegree;

    // The storage for knots and basis evaluation.
    mutable Real* m_afValue;  // m_afValue[0..degree]
    mutable Real* m_afKnot;   // m_afKnot[2*degree]
};

typedef BSplineFitBasis<float> BSplineFitBasisf;
typedef BSplineFitBasis<double> BSplineFitBasisd;

}

#endif
