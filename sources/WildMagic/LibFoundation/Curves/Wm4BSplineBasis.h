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

#ifndef WM4BSPLINEBASIS_H
#define WM4BSPLINEBASIS_H

#include "Wm4FoundationLIB.h"
#include "Wm4Math.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM BSplineBasis
{
public:
    BSplineBasis ();

    // Open uniform or periodic uniform.  The knot array is internally
    // generated with equally spaced elements.
    BSplineBasis (int iNumCtrlPoints, int iDegree, bool bOpen);
    void Create (int iNumCtrlPoints, int iDegree, bool bOpen);

    // Open nonuniform.  The knot array must have n-d elements.  The elements
    // must be nondecreasing.  Each element must be in [0,1].  The caller is
    // responsible for deleting afKnot.  An internal copy is made, so to
    // dynamically change knots you must use the SetKnot function.
    BSplineBasis (int iNumCtrlPoints, int iDegree, const Real* afKnot);
    void Create (int iNumCtrlPoints, int iDegree, const Real* afKnot);

    virtual ~BSplineBasis ();

    int GetNumCtrlPoints () const;
    int GetDegree () const;
    bool IsOpen () const;
    bool IsUniform () const;

    // The knot values can be changed only if the basis function is nonuniform
    // and the input index is valid (0 <= i <= n-d-1).  If these conditions
    // are not satisfied, GetKnot returns MAX_REAL.
    void SetKnot (int i, Real fKnot);
    Real GetKnot (int i) const;

    // access basis functions and their derivatives
    Real GetD0 (int i) const;
    Real GetD1 (int i) const;
    Real GetD2 (int i) const;
    Real GetD3 (int i) const;

    // evaluate basis functions and their derivatives
    void Compute (Real fTime, unsigned int uiOrder, int& riMinIndex,
        int& riMaxIndex) const;

protected:
    int Initialize (int iNumCtrlPoints, int iDegree, bool bOpen);
    Real** Allocate () const;
    void Deallocate (Real** aafArray);

    // Determine knot index i for which knot[i] <= rfTime < knot[i+1].
    int GetKey (Real& rfTime) const;

    int m_iNumCtrlPoints;    // n+1
    int m_iDegree;           // d
    Real* m_afKnot;          // knot[n+d+2]
    bool m_bOpen, m_bUniform;

    // Storage for the basis functions and their derivatives first three
    // derivatives.  The basis array is always allocated by the constructor
    // calls.  A derivative basis array is allocated on the first call to a
    // derivative member function.
    Real** m_aafBD0;             // bd0[d+1][n+d+1]
    mutable Real** m_aafBD1;     // bd1[d+1][n+d+1]
    mutable Real** m_aafBD2;     // bd2[d+1][n+d+1]
    mutable Real** m_aafBD3;     // bd3[d+1][n+d+1]
};

typedef BSplineBasis<float> BSplineBasisf;
typedef BSplineBasis<double> BSplineBasisd;

}

#endif
