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

#include "Wm4FoundationPCH.h"
#include "Wm4PolynomialRootsR.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
PolynomialRootsR<Real>::PolynomialRootsR ()
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
PolynomialRootsR<Real>::~PolynomialRootsR ()
{
}
//----------------------------------------------------------------------------
template <class Real>
int PolynomialRootsR<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
Real PolynomialRootsR<Real>::GetRoot (int i) const
{
    if (0 <= i && i < m_iQuantity)
    {
        return m_afRoot[i];
    }
    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <class Real>
int PolynomialRootsR<Real>::GetMultiplicity (int i) const
{
    if (0 <= i && i < m_iQuantity)
    {
        return m_aiMultiplicity[i];
    }
    return 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Linear (Real fC0, Real fC1)
{
    if (fC1 != (Real)0)
    {
        // The equation is c1*x + c0 = 0, where c1 is not zero.
        Rational kRoot = Rational(-fC0)/Rational(fC1);
        m_iQuantity = 1;
        kRoot.ConvertTo(m_afRoot[0]);
        m_aiMultiplicity[0] = 1;
        return true;
    }

    if (fC0 != (Real)0)
    {
        // The equation is c0 = 0, where c0 is not zero, so there are no
        // solutions.
        m_iQuantity = 0;
        return false;
    }

    // The polynomial equation is a tautology, 0 = 0, so there are
    // infinitely many solutions.
    m_iQuantity = INFINITE_QUANTITY;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Linear (const Rational& rkC0,
    const Rational& rkC1)
{
    if (rkC1 != ms_kZero)
    {
        // The equation is c1*x + c0 = 0, where c1 is not zero.
        Rational kRoot = rkC0/rkC1;
        m_iQuantity = 1;
        kRoot.ConvertTo(m_afRoot[0]);
        m_afRoot[0] = -m_afRoot[0];
        m_aiMultiplicity[0] = 1;
        return true;
    }

    if (rkC0 != ms_kZero)
    {
        // The equation is c0 = 0, where c0 is not zero, so there are no
        // solutions.
        m_iQuantity = 0;
        return false;
    }

    // The polynomial equation is a tautology, 0 = 0, so there are
    // infinitely many solutions.
    m_iQuantity = INFINITE_QUANTITY;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Quadratic (Real fC0, Real fC1, Real fC2)
{
    if (fC2 == (Real)0)
    {
        return Linear(fC0,fC1);
    }

    // The equation is c2*x^2 + c1*x + c0 = 0, where c2 is not zero.
    Rational kC0(fC0), kC1(fC1), kC2(fC2);

    // Create a monic polynomial, x^2 + a1*x + a0 = 0.
    Rational kInvC2 = ms_kOne/kC2;
    Rational kA1 = kC1*kInvC2;
    Rational kA0 = kC0*kInvC2;

    // Solve the equation.
    return Quadratic(kA0,kA1);
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Quadratic (const Rational& rkC0,
    const Rational& rkC1, const Rational& rkC2)
{
    if (rkC2 == ms_kZero)
    {
        return Linear(rkC0,rkC1);
    }

    // The equation is c2*x^2 + c1*x + c0 = 0, where c2 is not zero.  Create
    // a monic polynomial, x^2 + a1*x + a0 = 0.
    Rational kInvC2 = ms_kOne/rkC2;
    Rational kA1 = rkC1*kInvC2;
    Rational kA0 = rkC0*kInvC2;

    // Solve the equation.
    return Quadratic(kA0,kA1);
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Quadratic (const Rational& rkA0,
    const Rational& rkA1)
{
    Rational kMHalf(-1,2);
    Rational kMA1Div2 = kMHalf*rkA1;
    Rational kDiscr = kMA1Div2*kMA1Div2 - rkA0;
    if (kDiscr > ms_kZero)
    {
        // Two distinct real-valued roots.
        m_iQuantity = 2;

        // Estimate the discriminant.
        double dDiscr;
        kDiscr.ConvertTo(dDiscr);
        assert(dDiscr > 0.0);
        dDiscr = Mathd::Sqrt(dDiscr);

        kDiscr = Rational(dDiscr);
        Rational kRoot0 = kMA1Div2 - kDiscr;
        Rational kRoot1 = kMA1Div2 + kDiscr;
        kRoot0.ConvertTo(m_afRoot[0]);
        kRoot1.ConvertTo(m_afRoot[1]);
        m_aiMultiplicity[0] = 1;
        m_aiMultiplicity[1] = 1;
    }
    else if (kDiscr == ms_kZero)
    {
        // One repeated real-valued root.
        m_iQuantity = 1;
        kMA1Div2.ConvertTo(m_afRoot[0]);
        m_aiMultiplicity[0] = 2;
    }
    else
    {
        // No real-valued roots.
        m_iQuantity = 0;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Cubic (Real fC0, Real fC1, Real fC2, Real fC3)
{
    if (fC3 == (Real)0)
    {
        return Quadratic(fC0,fC1,fC2);
    }

    // The equation is c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is not zero.
    Rational kC0(fC0), kC1(fC1), kC2(fC2), kC3(fC3);

    // Create a monic polynomial, x^3 + a2*x^2 + a1*x + a0 = 0.
    Rational kInvC3 = ms_kOne/kC3;
    Rational kA2 = kC2*kInvC3;
    Rational kA1 = kC1*kInvC3;
    Rational kA0 = kC0*kInvC3;

    // Solve the equation.
    return Cubic(kA0,kA1,kA2);
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Cubic (const Rational& rkC0,
    const Rational& rkC1, const Rational& rkC2, const Rational& rkC3)
{
    if (rkC3 == ms_kZero)
    {
        return Quadratic(rkC0,rkC1,rkC2);
    }

    // The equation is c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is not zero.
    // Create a monic polynomial, x^3 + a2*x^2 + a1*x + a0 = 0.
    Rational kInvC3 = ms_kOne/rkC3;
    Rational kA2 = rkC2*kInvC3;
    Rational kA1 = rkC1*kInvC3;
    Rational kA0 = rkC0*kInvC3;

    // Solve the equation.
    return Cubic(kA0,kA1,kA2);
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Cubic (const Rational& rkA0,
    const Rational& rkA1, const Rational& rkA2)
{
    // Reduce the equation to y^3 + b1*y + b0 = 0.
    Rational kHalf(1,2), kThird(1,3), kTwo(2);
    Rational kA2Div3 = kThird*rkA2;
    Rational kA2Div3Sqr = kA2Div3*kA2Div3;
    Rational kA2Div3Cube = kA2Div3*kA2Div3Sqr;
    Rational kB1 = rkA1 - kA2Div3*rkA2;
    Rational kB0 = rkA0 - rkA1*kA2Div3 + kTwo*kA2Div3Cube;

    // Solve the equation.
    Rational kQ = kThird*kB1, kR = kHalf*kB0;
    Rational kDiscr = kR*kR + kQ*kQ*kQ;
    if (kDiscr > ms_kZero)
    {
        // One real-valued root, two complex-valued conjugate roots.
        m_iQuantity = 1;

        // Estimate the discriminant.
        double dDiscr;
        kDiscr.ConvertTo(dDiscr);
        assert(dDiscr > 0.0);
        dDiscr = Mathd::Sqrt(dDiscr);

        const double dThird = 1.0/3.0;
        kDiscr = Rational(dDiscr);

        Rational kSum0 = kR + kDiscr;
        double dSum0;
        kSum0.ConvertTo(dSum0);
        if (dSum0 >= 0.0)
        {
            dSum0 = Mathd::Pow(dSum0,dThird);
        }
        else
        {
            dSum0 = -Mathd::Pow(-dSum0,dThird);
        }
        kSum0 = Rational(dSum0);

        Rational kSum1 = kR - kDiscr;
        double dSum1;
        kSum1.ConvertTo(dSum1);
        if (dSum1 >= 0.0)
        {
            dSum1 = Mathd::Pow(dSum1,dThird);
        }
        else
        {
            dSum1 = -Mathd::Pow(-dSum1,dThird);
        }
        kSum1 = Rational(dSum1);

        Rational kRoot = kA2Div3 + kSum0 + kSum1;
        kRoot.ConvertTo(m_afRoot[0]);
        m_afRoot[0] = -m_afRoot[0];
        m_aiMultiplicity[0] = 1;
    }
    else if (kDiscr < ms_kZero)
    {
        // Three distinct real-valued roots.
        m_iQuantity = 3;
        m_aiMultiplicity[0] = 1;
        m_aiMultiplicity[1] = 1;
        m_aiMultiplicity[2] = 1;

        // Compute the eigenvalues by solving for the roots of the polynomial.
        double dMQ;
        kQ.ConvertTo(dMQ);
        dMQ = -dMQ;
        assert(dMQ > 0.0);

        double dMR;
        kR.ConvertTo(dMR);
        dMR = -dMR;

        double dMDiscr;
        kDiscr.ConvertTo(dMDiscr);
        dMDiscr = -dMDiscr;

        double dMA2d3;
        kA2Div3.ConvertTo(dMA2d3);
        dMA2d3 = -dMA2d3;

        double dSqrt3 = Mathd::Sqrt(3.0);
        double dMagnitude = Mathd::Sqrt(dMQ);
        double dAngle = Mathd::ATan2(Mathd::Sqrt(dMDiscr),dMR)/3.0;
        double dCos = Mathd::Cos(dAngle);
        double dSin = Mathd::Sin(dAngle);
        double dRoot0 = dMA2d3 + 2.0*dMagnitude*dCos;
        double dRoot1 = dMA2d3 - dMagnitude*(dCos + dSqrt3*dSin);
        double dRoot2 = dMA2d3 - dMagnitude*(dCos - dSqrt3*dSin);

        // Sort in increasing order.
        if (dRoot1 >= dRoot0)
        {
            m_afRoot[0] = (Real)dRoot0;
            m_afRoot[1] = (Real)dRoot1;
        }
        else
        {
            m_afRoot[0] = (Real)dRoot1;
            m_afRoot[1] = (Real)dRoot0;
        }

        if (dRoot2 >= (double)m_afRoot[1])
        {
            m_afRoot[2] = (Real)dRoot2;
        }
        else
        {
            m_afRoot[2] = m_afRoot[1];
            if (dRoot2 >= (double)m_afRoot[0])
            {
                m_afRoot[1] = (Real)dRoot2;
            }
            else
            {
                m_afRoot[1] = m_afRoot[0];
                m_afRoot[0] = (Real)dRoot2;
            }
        }
    }
    else
    {
        // Three real-valued roots, at least two of which are equal.
        if (kQ != ms_kZero)
        {
            // Two real-valued roots, one repeated.
            m_iQuantity = 2;

            const double dThird = 1.0/3.0;
            double dR;
            kR.ConvertTo(dR);
            if (dR >= 0.0)
            {
                dR = Mathd::Pow(dR,dThird);
            }
            else
            {
                dR = -Mathd::Pow(-dR,dThird);
            }
            kR = Rational(dR);

            Rational kRoot0 = kA2Div3 - kR;
            kRoot0.ConvertTo(m_afRoot[0]);
            m_afRoot[0] = -m_afRoot[0];
            m_aiMultiplicity[0] = 2;

            Rational kRoot1 = kA2Div3 + kTwo*kR;
            kRoot1.ConvertTo(m_afRoot[1]);
            m_afRoot[1] = -m_afRoot[1];
            m_aiMultiplicity[1] = 1;

            if (m_afRoot[1] < m_afRoot[0])
            {
                Real fSave = m_afRoot[0];
                m_afRoot[0] = m_afRoot[1];
                m_afRoot[1] = fSave;
                int iSave = m_aiMultiplicity[0];
                m_aiMultiplicity[0] = m_aiMultiplicity[1];
                m_aiMultiplicity[1] = iSave;
            }
        }
        else
        {
            // One real-valued root, all repeated.
            m_iQuantity = 1;
            kA2Div3.ConvertTo(m_afRoot[0]);
            m_afRoot[0] = -m_afRoot[0];
            m_aiMultiplicity[0] = 3;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Quartic (Real fC0, Real fC1, Real fC2, Real fC3,
    Real fC4)
{
    if (fC4 == (Real)0)
    {
        return Cubic(fC0,fC1,fC2,fC3);
    }

    // The equation is c4*x^4 + c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is
    // not zero.
    Rational kC0(fC0), kC1(fC1), kC2(fC2), kC3(fC3), kC4(fC4);

    // Create a monic polynomial, x^4 + a3*x^3 + a2*x^2 + a1*x + a0 = 0.
    Rational kInvC4 = ms_kOne/kC4;
    Rational kA3 = kC3*kInvC4;
    Rational kA2 = kC2*kInvC4;
    Rational kA1 = kC1*kInvC4;
    Rational kA0 = kC0*kInvC4;

    // Solve the equation.
    return Quartic(kA0,kA1,kA2,kA3);
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Quartic (const Rational& rkC0,
    const Rational& rkC1, const Rational& rkC2, const Rational& rkC3,
    const Rational& rkC4)
{
    if (rkC4 == ms_kZero)
    {
        return Cubic(rkC0,rkC1,rkC2,rkC3);
    }

    // The equation is c4*x^4 + c3*x^3 c2*x^2 + c1*x + c0 = 0, where c3 is
    // not zero.  Create a monic polynomial,
    // x^4 + a3*x^3 + a2*x^2 + a1*x + a0 = 0.
    Rational kInvC4 = ms_kOne/rkC4;
    Rational kA3 = rkC3*kInvC4;
    Rational kA2 = rkC2*kInvC4;
    Rational kA1 = rkC1*kInvC4;
    Rational kA0 = rkC0*kInvC4;

    // Solve the equation.
    return Quartic(kA0,kA1,kA2,kA3);
}
//----------------------------------------------------------------------------
template <class Real>
bool PolynomialRootsR<Real>::Quartic (const Rational& rkA0,
    const Rational& rkA1, const Rational& rkA2, const Rational& rkA3)
{
    m_iQuantity = 0;

    // Reduction to resolvent cubic polynomial y^3 + r2*y^2 + r1*y + r0 = 0.
    Rational kHalf(1,2), kFourth(1,4), kEighth(1,8), kFour(4), kTwo(2);
    Rational kR2 = -kHalf*rkA2;
    Rational kR1 = kFourth*rkA1*rkA3 - rkA0;
    Rational kR0 = -kEighth*(rkA1*rkA1 + rkA0*(rkA3*rkA3 - kFour*rkA2));

    // This always produces at least one root.
    PolynomialRootsR<Real> kPoly;
    kPoly.Cubic(kR0,kR1,kR2);
    Rational kY(kPoly.GetRoot(0));

    Rational kAlphaSqr = kFourth*rkA3*rkA3 - rkA2 + kTwo*kY;
    double dAlphaSqr;
    kAlphaSqr.ConvertTo(dAlphaSqr);
    if (dAlphaSqr < 0.0)
    {
        return false;
    }

    int i;

    if (dAlphaSqr > 0.0)
    {
        double dAlpha = Mathd::Sqrt(dAlphaSqr);
        Rational kAlpha = Rational(dAlpha);
        Rational kBeta = kHalf*(rkA3*kY - rkA1)/kAlpha;

        Rational kB0 = kY - kBeta;
        Rational kB1 = kHalf*rkA3 - kAlpha;
        kPoly.Quadratic(kB0,kB1);
        for (i = 0; i < kPoly.GetQuantity(); i++)
        {
            m_afRoot[m_iQuantity] = kPoly.GetRoot(i);
            m_aiMultiplicity[m_iQuantity] = kPoly.GetMultiplicity(i);
            m_iQuantity++;
        }

        kB0 = kY + kBeta;
        kB1 = kHalf*rkA3 + kAlpha;
        kPoly.Quadratic(kB0,kB1);
        for (i = 0; i < kPoly.GetQuantity(); i++)
        {
            m_afRoot[m_iQuantity] = kPoly.GetRoot(i);
            m_aiMultiplicity[m_iQuantity] = kPoly.GetMultiplicity(i);
            m_iQuantity++;
        }

        SortRoots();
        return m_iQuantity > 0;
    }

    Rational kBetaSqr = kY*kY - rkA0;
    double dBetaSqr;
    kBetaSqr.ConvertTo(dBetaSqr);
    if (dBetaSqr < 0.0)
    {
        return false;
    }

    if (dBetaSqr > 0.0)
    {
        double dBeta = Mathd::Sqrt(dBetaSqr);
        Rational kBeta(dBeta);

        Rational kB0 = kY - kBeta;
        Rational kB1 = kHalf*rkA3;
        kPoly.Quadratic(kB0,kB1);
        for (i = 0; i < kPoly.GetQuantity(); i++)
        {
            m_afRoot[m_iQuantity] = kPoly.GetRoot(i);
            m_aiMultiplicity[m_iQuantity] = kPoly.GetMultiplicity(i);
            m_iQuantity++;
        }

        kB0 = kY + kBeta;
        kPoly.Quadratic(kB0,kB1);
        for (i = 0; i < kPoly.GetQuantity(); i++)
        {
            m_afRoot[m_iQuantity] = kPoly.GetRoot(i);
            m_aiMultiplicity[m_iQuantity] = kPoly.GetMultiplicity(i);
            m_iQuantity++;
        }

        SortRoots();
        return m_iQuantity > 0;
    }

    kPoly.Quadratic(kY,kHalf*rkA3);
    for (i = 0; i < kPoly.GetQuantity(); i++)
    {
        m_afRoot[m_iQuantity] = kPoly.GetRoot(i);
        m_aiMultiplicity[m_iQuantity] = 2*kPoly.GetMultiplicity(i);
        m_iQuantity++;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
void PolynomialRootsR<Real>::SortRoots ()
{
    // Sort the roots as: root[0] <= ... <= root[quantity-1].
    int i0, i1;
    for (i0 = 0; i0 <= m_iQuantity-2; i0++)
    {
        // Locate the minimum root.
        i1 = i0;
        Real fMinRoot = m_afRoot[i1];
        int iMinMult = m_aiMultiplicity[i1];
        for (int i2 = i0 + 1; i2 < m_iQuantity; i2++)
        {
            if (m_afRoot[i2] < fMinRoot)
            {
                i1 = i2;
                fMinRoot = m_afRoot[i1];
                iMinMult = m_aiMultiplicity[i1];
            }
        }

        if (i1 != i0)
        {
            // Swap the roots and multiplicities.
            m_afRoot[i1] = m_afRoot[i0];
            m_afRoot[i0] = fMinRoot;
            m_aiMultiplicity[i1] = m_aiMultiplicity[i0];
            m_aiMultiplicity[i0] = iMinMult;
        }
    }

    // Combine the multiplicities, if necessary.
    for (i0 = 0; i0 < m_iQuantity-1; /**/)
    {
        if (m_afRoot[i0] == m_afRoot[i0+1])
        {
            // Combine the multiplicities.
            m_aiMultiplicity[i0] += m_aiMultiplicity[i0+1];

            // Eliminate the redundant root by shifting the array elements.
            m_iQuantity--;
            for (i1 = i0 + 1; i1 < m_iQuantity; i1++)
            {
                m_afRoot[i1] = m_afRoot[i1+1];
                m_aiMultiplicity[i1] = m_aiMultiplicity[i1+1];
            }
        }
        else
        {
            i0++;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class PolynomialRootsR<float>;

template<> const PolynomialRootsR<float>::Rational
PolynomialRootsR<float>::ms_kZero(0);
template<> const PolynomialRootsR<float>::Rational
PolynomialRootsR<float>::ms_kOne(1);

template WM4_FOUNDATION_ITEM
class PolynomialRootsR<double>;

template<> const PolynomialRootsR<double>::Rational
PolynomialRootsR<double>::ms_kZero(0);
template<> const PolynomialRootsR<double>::Rational
PolynomialRootsR<double>::ms_kOne(1);
//----------------------------------------------------------------------------
}
