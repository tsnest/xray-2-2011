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

#ifndef WM4BSPLINEVOLUME_H
#define WM4BSPLINEVOLUME_H

#include "Wm4FoundationLIB.h"
#include "Wm4BSplineBasis.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM BSplineVolume
{
public:
    // Construction and destruction of an open uniform B-spline volume.  The
    // class will allocate space for the control points.  The caller is
    // responsible for setting the values with the member function
    // ControlPoint.

    BSplineVolume (int iNumUCtrlPoints, int iNumVCtrlPoints,
        int iNumWCtrlPoints, int iUDegree, int iVDegree, int iWDegree);

    ~BSplineVolume ();

    int GetNumCtrlPoints (int iDim) const;
    int GetDegree (int iDim) const;

    // Control points may be changed at any time.  If any input index is
    // invalid, the returned point is a vector whose components are all
    // MAX_REAL.
    void SetControlPoint (int iUIndex, int iVIndex, int iWIndex,
        const Vector3<Real>& rkCtrlPoint);
    Vector3<Real> GetControlPoint (int iUIndex, int iVIndex, int iWIndex)
        const;

    // The spline is defined for 0 <= u <= 1, 0 <= v <= 1, and 0 <= w <= 1.
    // The input values should be in this domain.  Any inputs smaller than 0
    // are clamped to 0.  Any inputs larger than 1 are clamped to 1.
    Vector3<Real> GetPosition (Real fU, Real fV, Real fW) const;
    Vector3<Real> GetDerivativeU (Real fU, Real fV, Real fW) const;
    Vector3<Real> GetDerivativeV (Real fU, Real fV, Real fW) const;
    Vector3<Real> GetDerivativeW (Real fU, Real fV, Real fW) const;

    // for array indexing:  i = 0 for u, i = 1 for v, i = 2 for w
    Vector3<Real> GetPosition (Real afP[3]) const;
    Vector3<Real> GetDerivative (int i, Real afP[3]) const;

private:
    Vector3<Real>*** m_aaakCtrlPoint;  // ctrl[unum][vnum][wnum]
    BSplineBasis<Real> m_akBasis[3];
};

typedef BSplineVolume<float> BSplineVolumef;
typedef BSplineVolume<double> BSplineVolumed;

}

#endif
