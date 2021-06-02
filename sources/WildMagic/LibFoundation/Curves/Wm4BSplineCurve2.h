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

#ifndef WM4BSPLINECURVE2_H
#define WM4BSPLINECURVE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4SingleCurve2.h"
#include "Wm4BSplineBasis.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM BSplineCurve2 : public SingleCurve2<Real>
{
public:
    // Construction and destruction.  The caller is responsible for deleting
    // the input arrays if they were dynamically allocated.  Internal copies
    // of the arrays are made, so to dynamically change control points or
    // knots you must use the 'SetControlPoint', 'GetControlPoint', and
    // 'Knot' member functions.

    // Uniform spline.  The number of control points is n+1 >= 2.  The degree
    // of the B-spline is d and must satisfy 1 <= d <= n.  The knots are
    // implicitly calculated in [0,1].  If bOpen is 'true', the spline is
    // open and the knots are
    //   t[i] = 0,               0 <= i <= d
    //          (i-d)/(n+1-d),   d+1 <= i <= n
    //          1,               n+1 <= i <= n+d+1
    // If bOpen is 'false', the spline is periodic and the knots are
    //   t[i] = (i-d)/(n+1-d),   0 <= i <= n+d+1
    // If bLoop is 'true', extra control points are added to generate a closed
    // curve.  For an open spline, the control point array is reallocated and
    // one extra control point is added, set to the first control point
    // C[n+1] = C[0].  For a periodic spline, the control point array is
    // reallocated and the first d points are replicated.  In either case the
    // knot array is calculated accordingly.

    BSplineCurve2 (int iNumCtrlPoints, const Vector2<Real>* akCtrlPoint,
        int iDegree, bool bLoop, bool bOpen);

    // Open, nonuniform spline.  The knot array must have n-d elements.  The
    // elements must be nondecreasing.  Each element must be in [0,1].
    BSplineCurve2 (int iNumCtrlPoints, const Vector2<Real>* akCtrlPoint,
        int iDegree, bool bLoop, const Real* afKnot);

    virtual ~BSplineCurve2 ();

    int GetNumCtrlPoints () const;
    int GetDegree () const;
    bool IsOpen () const;
    bool IsUniform () const;
    bool IsLoop () const;

    // Control points may be changed at any time.  The input index should be
    // valid (0 <= i <= n).  If it is invalid, GetControlPoint returns a
    // vector whose components are all MAX_REAL.
    void SetControlPoint (int i, const Vector2<Real>& rkCtrl);
    Vector2<Real> GetControlPoint (int i) const;

    // The knot values can be changed only if the basis function is nonuniform
    // and the input index is valid (0 <= i <= n-d-1).  If these conditions
    // are not satisfied, GetKnot returns MAX_REAL.
    void SetKnot (int i, Real fKnot);
    Real GetKnot (int i) const;

    // The spline is defined for 0 <= t <= 1.  If a t-value is outside [0,1],
    // an open spline clamps t to [0,1].  That is, if t > 1, t is set to 1;
    // if t < 0, t is set to 0.  A periodic spline wraps to to [0,1].  That
    // is, if t is outside [0,1], then t is set to t-floor(t).
    virtual Vector2<Real> GetPosition (Real fTime) const;
    virtual Vector2<Real> GetFirstDerivative (Real fTime) const;
    virtual Vector2<Real> GetSecondDerivative (Real fTime) const;
    virtual Vector2<Real> GetThirdDerivative (Real fTime) const;

    // If you need position and derivatives at the same time, it is more
    // efficient to call these functions.  Pass the addresses of those
    // quantities whose values you want.  You may pass 0 in any argument
    // whose value you do not want.
    void Get (Real fTime, Vector2<Real>* pkPos, Vector2<Real>* pkDer1,
        Vector2<Real>* pkDer2, Vector2<Real>* pkDer3) const;

    // Access the basis function to compute it without control points.  This
    // is useful for least squares fitting of curves.
    BSplineBasis<Real>& GetBasis ();

    // TO DO.  This is not yet implemented.
    virtual Real GetVariation (Real fT0, Real fT1,
        const Vector2<Real>* pkP0 = 0, const Vector2<Real>* pkP1 = 0) const;

protected:
    // Replicate the necessary number of control points when the Create
    // function has bLoop equal to true, in which case the spline curve must
    // be a closed curve.
    void CreateControl (const Vector2<Real>* akCtrlPoint);

    int m_iNumCtrlPoints;
    Vector2<Real>* m_akCtrlPoint;  // ctrl[n+1]
    bool m_bLoop;
    BSplineBasis<Real> m_kBasis;
    int m_iReplicate;  // the number of replicated control points
};

typedef BSplineCurve2<float> BSplineCurve2f;
typedef BSplineCurve2<double> BSplineCurve2d;

}

#endif
