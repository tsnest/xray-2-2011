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

#ifndef WM4ELLIPSOID3_H
#define WM4ELLIPSOID3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Eigen.h"

namespace Wm4
{

template <class Real>
class Ellipsoid3
{
public:
    // An ellipsoid has center K, axis directions U[0], U[1], and U[2] (all
    // unit-length vectors), and extents e[0], e[1], and e[2] (all positive
    // numbers).  A point X = K+y[0]*U[0]+y[1]*U[1]+y[2]*U[2] is on the
    // ellipsoid whenever (y[0]/e[0])^2+(y[1]/e[1])^2+(y[2]/e[2])^2 = 1.  The
    // test for a point inside the ellipsoid uses "<=" instead of "=" in the
    // previous expression.  An algebraic representation for the ellipsoid is
    //   1 = (X-K)^T * (U[0]*U[0]^T/e[0]^2 + U[1]*U[1]^T/e[1]^2 +
    //                  U[2]*U[2]^T/e[2]^2) * (X-K)
    //     = (X-K)^T * M * (X-K)
    // where the superscript T denotes transpose.  Observe that U[i]*U[i]^T
    // is a matrix, not a scalar dot product.  The matrix M is symmetric.
    // The ellipse is also represented by a quadratic equation
    //   0 = a0 + a1*x[0] + a2*x[1] + a3*x[2] + a4*x[0]^2 + a5*x[0]*x[1] +
    //       a6*x[0]*x[2] + a7*x[1]^2 + a8*x[1]*x[2] + a9*x[2]^2
    //     = a0 + [a1 a2 a3]*X + X^T*[a4   a5/2 a6/2]*X
    //                               [a5/2 a7   a8/2]
    //                               [a6/2 a8/2 a9  ]
    //     = C + B^T*X + X^T*A*X
    // where X = (x[0],x[1],x[2]).  This equation can be factored to the form
    // (X-K)^T*M*(X-K) = 1, where K = -A^{-1}*B/2, M = A/(B^T*A^{-1}*B/4-C).
    // To be an ellipsoid, M must have all positive eigenvalues.

    // Construction and destruction.
    Ellipsoid3 ();  // uninitialized
    Ellipsoid3 (const Vector3<Real>& rkCenter, const Vector3<Real>* akAxis,
        const Real* afExtent);
    Ellipsoid3 (const Vector3<Real>& rkCenter, const Vector3<Real>& rkAxis0,
        const Vector3<Real>& rkAxis1, const Vector3<Real>& rkAxis2,
        Real fExtent0, Real fExtent1, Real fExtent2);
    ~Ellipsoid3 ();

    // Compute M = sum_{i=0}^2 U[i]*U[i]^T/e[i]^2.
    void GetM (Matrix3<Real>& rkM) const;

    // Compute M^{-1} = sum_{i=0}^2 U[i]*U[i]^T*e[i]^2.
    void GetMInverse (Matrix3<Real>& rkMInverse) const;

    // Construct the coefficients in the equation.
    void ToCoefficients (Real afCoeff[10]) const;
    void ToCoefficients (Matrix3<Real>& rkA, Vector3<Real>& rkB, Real& rfC)
        const;

    // Construct C, U[i], and e[i] from the equation.  The return value is
    // 'true' if and only if the input coefficients represent an ellipsoid.
    // If the function returns 'false', the ellipsoid data members are
    // undefined.
    bool FromCoefficients (const Real afCoeff[10]);
    bool FromCoefficients (const Matrix3<Real>& rkA, const Vector3<Real>& rkB,
        Real fC);

    // Compute the quadratic Q(X) = (X-K)^T * M * (X-K) - 1.
    Real Evaluate (const Vector3<Real>& rkPoint) const;

    // Test whether the input point is inside or on the ellipsoid.
    bool Contains (const Vector3<Real>& rkPoint) const;

    Vector3<Real> Center;
    Vector3<Real> Axis[3];
    Real Extent[3];

private:
    static void Convert (const Real afCoeff[10], Matrix3<Real>& rkA,
        Vector3<Real>& rkB, Real& rfC);
    static void Convert (const Matrix3<Real>& rkA, const Vector3<Real>& rkB,
        Real fC, Real afCoeff[10]);
};

#include "Wm4Ellipsoid3.inl"

typedef Ellipsoid3<float> Ellipsoid3f;
typedef Ellipsoid3<double> Ellipsoid3d;

}

#endif
