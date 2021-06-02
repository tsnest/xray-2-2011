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

#ifndef WM4INTPAKIMA1_H
#define WM4INTPAKIMA1_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntpAkima1
{
public:
    // abstract base class
    virtual ~IntpAkima1 ();

    class WM4_FOUNDATION_ITEM Polynomial
    {
    public:
        // P(x) = A[0]+A[1]*x+A[2]*x^2+A[3]*x^3
        Real& operator[] (int i) { return m_afA[i]; }

        Real operator() (Real fX) const
        {
            return m_afA[0]+fX*(m_afA[1]+fX*(m_afA[2]+fX*m_afA[3]));
        }

        Real operator() (int iOrder, Real fX) const
        {
            switch ( iOrder )
            {
            case 0:
                return m_afA[0]+fX*(m_afA[1]+fX*(m_afA[2]+fX*m_afA[3]));
            case 1:
                return m_afA[1]+fX*(((Real)2.0)*m_afA[2] +
                    fX*((Real)3.0)*m_afA[3]);
            case 2:
                return ((Real)2.0)*m_afA[2]+fX*((Real)6.0)*m_afA[3];
            case 3:
                return ((Real)6.0)*m_afA[3];
            default:
                return (Real)0.0f;
            }
        }

    private:
        Real m_afA[4];
    };

    int GetQuantity () const;
    const Real* GetF () const;
    const Polynomial* GetPolynomials () const;
    const Polynomial& GetPolynomial (int i) const;

    virtual Real GetXMin () const = 0;
    virtual Real GetXMax () const = 0;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax.  If x is outside the
    // extremes, the function returns MAXREAL.  The first operator is for
    // function evaluation.  The second operator is for function or derivative
    // evaluations.  The uiOrder argument is the order of the derivative, zero
    // for the function itself.
    Real operator() (Real fX) const;
    Real operator() (int iOrder, Real fX) const;

protected:
    // Construction.  IntpAkima1 does not accept responsibility for
    // deleting the input array.  The application must do so.
    IntpAkima1 (int iQuantity, Real* afF);

    Real ComputeDerivative (Real* afSlope) const;
    virtual bool Lookup (Real fX, int& riIndex, Real& rfDX) const = 0;

    int m_iQuantity;
    Real* m_afF;
    Polynomial* m_akPoly;
};

typedef IntpAkima1<float> IntpAkima1f;
typedef IntpAkima1<double> IntpAkima1d;

}

#endif
