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
// Version: 4.1.0 (2006/11/05)

#ifndef WM4POLYNOMIALROOTSR_H
#define WM4POLYNOMIALROOTSR_H

#include "Wm4FoundationLIB.h"
#include "Wm4TRational.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM PolynomialRootsR
{
public:
    // Construction and destruction.
    PolynomialRootsR ();
    ~PolynomialRootsR ();

    // Member access.
    enum { INFINITE_QUANTITY = -1 };
    int GetQuantity () const;
    Real GetRoot (int i) const;
    int GetMultiplicity (int i) const;

    typedef TRational<16*sizeof(Real)> Rational;

    // p(x) = c0 + c1*x
    bool Linear (Real fC0, Real fC1);
    bool Linear (const Rational& rkC0, const Rational& rkC1);

    // p(x) = c0 + c1*x + c2*x^2
    // m(x) = a0 + a1*x + x^2
    bool Quadratic (Real fC0, Real fC1, Real fC2);
    bool Quadratic (const Rational& rkC0, const Rational& rkC1,
        const Rational& rkC2);
    bool Quadratic (const Rational& rkA0, const Rational& rkA1);

    // p(x) = c0 + c1*x + c2*x^2 + c3*x^3
    // m(x) = a0 + a1*x + a2*x^2 + x^3
    bool Cubic (Real fC0, Real fC1, Real fC2, Real fC3);
    bool Cubic (const Rational& rkC0, const Rational& rkC1,
        const Rational& rkC2, const Rational& rkC3);
    bool Cubic (const Rational& rkA0, const Rational& rkA1,
        const Rational& rkA2);

    // p(x) = c0 + c1*x + c2*x^2 + c3*x^3 + c4*x^4 (monic when c4 = 1)
    // m(x) = c0 + c1*x + c2*x^2 + c3*x^3 + x^4
    bool Quartic (Real fC0, Real fC1, Real fC2, Real fC3, Real fC4);
    bool Quartic (const Rational& rkC0, const Rational& rkC1,
        const Rational& rkC2, const Rational& rkC3, const Rational& rkC4);
    bool Quartic (const Rational& rkA0, const Rational& rkA1,
        const Rational& rkA2, const Rational& rkA3);

private:
    void SortRoots ();

    int m_iQuantity;
    Real m_afRoot[4];
    int m_aiMultiplicity[4];

    static const Rational ms_kZero;
    static const Rational ms_kOne;
};

typedef PolynomialRootsR<float> PolynomialRootsRf;
typedef PolynomialRootsR<double> PolynomialRootsRd;

}

#endif
