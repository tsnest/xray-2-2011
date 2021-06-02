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

#ifndef WM4QUADRICSURFACE_H
#define WM4QUADRICSURFACE_H

#include "Wm4FoundationLIB.h"
#include "Wm4ImplicitSurface.h"
#include "Wm4RVector3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM QuadricSurface : public ImplicitSurface<Real>
{
public:
    // A quadric surface is defined implicitly by
    //
    //   0 = a0 + a1*x[0] + a2*x[1] + a3*x[2] + a4*x[0]^2 + a5*x[0]*x[1] +
    //       a6*x[0]*x[2] + a7*x[1]^2 + a8*x[1]*x[2] + a9*x[2]^2
    //
    //     = a0 + [a1 a2 a3]*X + X^T*[a4   a5/2 a6/2]*X
    //                               [a5/2 a7   a8/2]
    //                               [a6/2 a8/2 a9  ]
    //     = C + B^T*X + X^T*A*X
    //
    // The matrix A is symmetric.

    QuadricSurface ();  // all coefficients zero
    QuadricSurface (const Real afCoeff[10]);

    // member access
    const Real* GetCoefficients () const;
    const Matrix3<Real>& GetA () const;
    const Vector3<Real>& GetB () const;
    Real GetC () const;

    // the function
    virtual Real F (const Vector3<Real>& rkP) const;

    // first-order partial derivatives
    virtual Real FX (const Vector3<Real>& rkP) const;
    virtual Real FY (const Vector3<Real>& rkP) const;
    virtual Real FZ (const Vector3<Real>& rkP) const;

    // second-order partial derivatives
    virtual Real FXX (const Vector3<Real>& rkP) const;
    virtual Real FXY (const Vector3<Real>& rkP) const;
    virtual Real FXZ (const Vector3<Real>& rkP) const;
    virtual Real FYY (const Vector3<Real>& rkP) const;
    virtual Real FYZ (const Vector3<Real>& rkP) const;
    virtual Real FZZ (const Vector3<Real>& rkP) const;

    enum  // solution type
    {
        QT_NONE,
        QT_POINT,
        QT_LINE,
        QT_PLANE,
        QT_TWO_PLANES,
        QT_PARABOLIC_CYLINDER,
        QT_ELLIPTIC_CYLINDER,
        QT_HYPERBOLIC_CYLINDER,
        QT_ELLIPTIC_PARABOLOID,
        QT_HYPERBOLIC_PARABOLOID,
        QT_ELLIPTIC_CONE,
        QT_HYPERBOLOID_ONE_SHEET,
        QT_HYPERBOLOID_TWO_SHEETS,
        QT_ELLIPSOID
    };

    // classification of the equation using exact arithmetic
    int GetType () const;

protected:
    Real m_afCoeff[10];
    Matrix3<Real> m_kA;
    Vector3<Real> m_kB;
    Real m_fC;

private:
    typedef TRational<4*sizeof(Real)> Rational;
    typedef RVector3<4*sizeof(Real)> QSVector;

    class RReps
    {
    public:
        RReps (const Real afCoeff[10])
        {
            Rational kOneHalf(1,2);

            C = Rational(afCoeff[0]);
            B0 = Rational(afCoeff[1]);
            B1 = Rational(afCoeff[2]);
            B2 = Rational(afCoeff[3]);
            A00 = Rational(afCoeff[4]);
            A01 = kOneHalf*Rational(afCoeff[5]);
            A02 = kOneHalf*Rational(afCoeff[6]);
            A11 = Rational(afCoeff[7]);
            A12 = kOneHalf*Rational(afCoeff[8]);
            A22 = Rational(afCoeff[9]);

            Sub00 = A11*A22 - A12*A12;
            Sub01 = A01*A22 - A12*A02;
            Sub02 = A01*A12 - A02*A11;
            Sub11 = A00*A22 - A02*A02;
            Sub12 = A00*A12 - A02*A01;
            Sub22 = A00*A11 - A01*A01;
            C0 = A00*Sub00 - A01*Sub01 + A02*Sub02;
            C1 = Sub00 + Sub11 + Sub22;
            C2 = A00 + A11 + A22;
        }

        // quadratic coefficients
        Rational A00, A01, A02, A11, A12, A22, B0, B1, B2, C;

        // 2-by-2 determinants
        Rational Sub00, Sub01, Sub02, Sub11, Sub12, Sub22;

        // characteristic polynomial L^3 - C2*L^2 + C1*L - C0
        Rational C0, C1, C2;

        // for Sturm sequences
        Rational C3, C4, C5;
    };

    static void GetRootSigns (RReps& rkReps, int& riPositiveRoots,
        int& riNegativeRoots, int& riZeroRoots);
    static int GetSignChanges (int iQuantity, const Rational* akValue);
    static int ClassifyZeroRoots0 (const RReps& rkReps, int iPositiveRoots);
    static int ClassifyZeroRoots1 (const RReps& rkReps, int iPositiveRoots);
    static int ClassifyZeroRoots1 (const RReps& rkReps, int iPositiveRoots,
        const QSVector& rkP0, const QSVector& rkP1, const QSVector& rkP2);
    static int ClassifyZeroRoots2 (const RReps& rkReps, int iPositiveRoots);
    static int ClassifyZeroRoots2 (const RReps& rkReps, int iPositiveRoots,
        const QSVector& rkP0, const QSVector& rkP1, const QSVector& rkP2);
    static int ClassifyZeroRoots3 (const RReps& rkReps);
};

typedef QuadricSurface<float> QuadricSurfacef;
typedef QuadricSurface<double> QuadricSurfaced;

}

#endif
