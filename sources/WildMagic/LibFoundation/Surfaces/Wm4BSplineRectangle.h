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

#ifndef WM4BSPLINERECTANGLE_H
#define WM4BSPLINERECTANGLE_H

#include "Wm4FoundationLIB.h"
#include "Wm4ParametricSurface.h"
#include "Wm4BSplineBasis.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM BSplineRectangle : public ParametricSurface<Real>
{
public:
    // Construction and destruction.   The caller is responsible for deleting
    // the input arrays if they were dynamically allocated.  Internal copies
    // of the arrays are made, so to dynamically change control points or
    // knots you must use the 'SetControlPoint', 'GetControlPoint', and
    // 'Knot' member functions.

    // Spline types for curves are
    //   open uniform (OU)
    //   periodic uniform (PU)
    //   open nonuniform (ON)
    // For tensor product surfaces, you have to choose a type for each of two
    // dimensions, leading to nine possible spline types for surfaces.  The
    // constructors below represent these choices.

    // (OU,OU), (OU,PU), (PU,OU), or (PU,PU)
    BSplineRectangle (int iNumUCtrlPoints, int iNumVCtrlPoints,
        Vector3<Real>** aakCtrlPoint, int iUDegree, int iVDegree, bool bULoop,
        bool bVLoop, bool bUOpen, bool bVOpen);

    // (OU,ON) or (PU,ON)
    BSplineRectangle (int iNumUCtrlPoints, int iNumVCtrlPoints,
        Vector3<Real>** aakCtrlPoint, int iUDegree, int iVDegree, bool bULoop,
        bool bVLoop, bool bUOpen, Real* afVKnot);

    // (ON,OU) or (ON,PU)
    BSplineRectangle (int iNumUCtrlPoints, int iNumVCtrlPoints,
        Vector3<Real>** aakCtrlPoint, int iUDegree, int iVDegree, bool bULoop,
        bool bVLoop, Real* afUKnot, bool bVOpen);

    // (ON,ON)
    BSplineRectangle (int iNumUCtrlPoints, int iNumVCtrlPoints,
        Vector3<Real>** aakCtrlPoint, int iUDegree, int iVDegree, bool bULoop,
        bool bVLoop, Real* afUKnot, Real* afVKnot);

    virtual ~BSplineRectangle ();

    int GetNumCtrlPoints (int iDim) const;
    int GetDegree (int iDim) const;
    bool IsOpen (int iDim) const;
    bool IsUniform (int iDim) const;
    bool IsLoop (int iDim) const;

    // Control points may be changed at any time.  If either input index is
    // invalid, GetControlPoint returns a vector whose components are all
    // MAX_REAL.
    void SetControlPoint (int iUIndex, int iVIndex,
        const Vector3<Real>& rkCtrl);
    Vector3<Real> GetControlPoint (int iUIndex, int iVIndex) const;

    // The knot values can be changed only if the surface is nonuniform in the
    // selected dimension and only if the input index is valid.  If these
    // conditions are not satisfied, GetKnot returns MAX_REAL.
    void SetKnot (int iDim, int i, Real fKnot);
    Real GetKnot (int iDim, int i) const;

    // The spline is defined for 0 <= u <= 1 and 0 <= v <= 1.  The input
    // values should be in this domain.  Any inputs smaller than 0 are clamped
    // to 0.  Any inputs larger than 1 are clamped to 1.
    virtual Vector3<Real> P (Real fU, Real fV) const;
    virtual Vector3<Real> PU (Real fU, Real fV) const;
    virtual Vector3<Real> PV (Real fU, Real fV) const;
    virtual Vector3<Real> PUU (Real fU, Real fV) const;
    virtual Vector3<Real> PUV (Real fU, Real fV) const;
    virtual Vector3<Real> PVV (Real fU, Real fV) const;

    // If you need position and derivatives at the same time, it is more
    // efficient to call these functions.  Pass the addresses of those
    // quantities whose values you want.  You may pass 0 in any argument
    // whose value you do not want.
    void Get (Real fU, Real fV, Vector3<Real>* pkP, Vector3<Real>* pkPU,
        Vector3<Real>* pkPV, Vector3<Real>* pkPUU, Vector3<Real>* pkPUV,
        Vector3<Real>* pkPVV) const;

protected:
    // Replicate the necessary number of control points when the Create
    // function has bLoop equal to true, in which case the spline surface
    // must be a closed surface in the corresponding dimension.
    void CreateControl (Vector3<Real>** aakCtrlPoint);

    int m_iNumUCtrlPoints, m_iNumVCtrlPoints;
    Vector3<Real>** m_aakCtrlPoint;  // ctrl[unum][vnum]
    bool m_abLoop[2];
    BSplineBasis<Real> m_akBasis[2];
    int m_iUReplicate, m_iVReplicate;
};

typedef BSplineRectangle<float> BSplineRectanglef;
typedef BSplineRectangle<double> BSplineRectangled;

}

#endif
