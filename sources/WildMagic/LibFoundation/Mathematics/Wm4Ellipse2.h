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
// Version: 4.0.1 (2007/04/01)

#ifndef WM4ELLIPSE2_H
#define WM4ELLIPSE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Eigen.h"

namespace Wm4
{

template <class Real>
class Ellipse2
{
public:
    // An ellipse has center K, axis directions U[0] and U[1] (both
    // unit-length vectors), and extents e[0] and e[1] (both positive
    // numbers).  A point X = K+y[0]*U[0]+y[1]*U[1] is on the ellipse whenever
    // (y[0]/e[0])^2+(y[1]/e[1])^2 = 1.  The test for a point inside the
    // ellipse uses "<=" instead of "=" in the previous expression.  An
    // algebraic representation for the ellipse is
    //   1 = (X-K)^T * (U[0]*U[0]^T/e[0]^2 + U[1]*U[1]^T/e[1]^2) * (X-K)
    //     = (X-K)^T * M * (X-K)
    // where the superscript T denotes transpose.  Observe that U[i]*U[i]^T
    // is a matrix, not a scalar dot product.  The matrix M is symmetric.
    // The ellipse is also represented by a quadratic equation
    //   0 = a0 + a1*x[0] + a2*x[1] + a3*x[0]^2 + a4*x[0]*x[1] + a5*x[1]^2
    //     = a0 + [a1 a2]*X + X^T*[a3   a4/2]*X
    //                            [a4/2 a5  ]
    //     = C + B^T*X + X^T*A*X
    // where X = (x[0],x[1]).  This equation can be factored to the form
    // (X-K)^T*M*(X-K) = 1, where K = -A^{-1}*B/2, M = A/(B^T*A^{-1}*B/4-C).
    // To be an ellipse, M must have all positive eigenvalues.

    // Construction and destruction.
    Ellipse2 ();  // uninitialized
    Ellipse2 (const Vector2<Real>& rkCenter, const Vector2<Real>* akAxis,
        const Real* afExtent);
    Ellipse2 (const Vector2<Real>& rkCenter, const Vector2<Real>& rkAxis0,
        const Vector2<Real>& rkAxis1, Real fExtent0, Real fExtent1);
    ~Ellipse2 ();

    // Compute M = sum_{i=0}^1 U[i]*U[i]^T/e[i]^2.
    void GetM (Matrix2<Real>& rkM) const;

    // Compute M^{-1} = sum_{i=0}^1 U[i]*U[i]^T*e[i]^2.
    void GetMInverse (Matrix2<Real>& rkMInverse) const;

    // Construct the coefficients in the equation.
    void ToCoefficients (Real afCoeff[6]) const;
    void ToCoefficients (Matrix2<Real>& rkA, Vector2<Real>& rkB, Real& rfC)
        const;

    // Construct C, U[i], and e[i] from the quadratic equation.  The return
    // value is 'true' if and only if the input coefficients represent an
    // ellipse.  If the function returns 'false', the ellipse data members
    // are undefined.
    bool FromCoefficients (const Real afCoeff[6]);
    bool FromCoefficients (const Matrix2<Real>& rkA, const Vector2<Real>& rkB,
        Real fC);

    // Compute the quadratic Q(X) = (X-K)^T * M * (X-K) - 1.
    Real Evaluate (const Vector2<Real>& rkPoint) const;

    // Test whether the input point is inside or on the ellipse.
    bool Contains (const Vector2<Real>& rkPoint) const;

    Vector2<Real> Center;
    Vector2<Real> Axis[2];
    Real Extent[2];

private:
    static void Convert (const Real afCoeff[6], Matrix2<Real>& rkA,
        Vector2<Real>& rkB, Real& rfC);
    static void Convert (const Matrix2<Real>& rkA, const Vector2<Real>& rkB,
        Real fC, Real afCoeff[6]);
};

#include "Wm4Ellipse2.inl"

typedef Ellipse2<float> Ellipse2f;
typedef Ellipse2<double> Ellipse2d;

}

#endif
