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

#include "Wm4FoundationPCH.h"
#include "Wm4Integrate1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Real Integrate1<Real>::RombergIntegral (int iOrder, Real fA, Real fB,
    Function oF, void* pvUserData)
{
    assert(iOrder > 0);
    Real** aafRom;
    Allocate<Real>(iOrder,2,aafRom);

    Real fH = fB - fA;

    aafRom[0][0] = ((Real)0.5)*fH*(oF(fA,pvUserData)+oF(fB,pvUserData));
    for (int i0=2, iP0=1; i0 <= iOrder; i0++, iP0 *= 2, fH *= (Real)0.5)
    {
        // approximations via the trapezoid rule
        Real fSum = (Real)0.0;
        int i1;
        for (i1 = 1; i1 <= iP0; i1++)
        {
            fSum += oF(fA + fH*(i1-((Real)0.5)),pvUserData);
        }

        // Richardson extrapolation
        aafRom[1][0] = ((Real)0.5)*(aafRom[0][0] + fH*fSum);
        for (int i2 = 1, iP2 = 4; i2 < i0; i2++, iP2 *= 4)
        {
            aafRom[1][i2] = (iP2*aafRom[1][i2-1] - aafRom[0][i2-1])/(iP2-1);
        }

        for (i1 = 0; i1 < i0; i1++)
        {
            aafRom[0][i1] = aafRom[1][i1];
        }
    }

    Real fResult = aafRom[0][iOrder-1];
    Deallocate<Real>(aafRom);
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Integrate1<Real>::GaussianQuadrature (Real fA, Real fB, Function oF,
    void* pvUserData)
{
    // Legendre polynomials
    // P_0(x) = 1
    // P_1(x) = x
    // P_2(x) = (3x^2-1)/2
    // P_3(x) = x(5x^2-3)/2
    // P_4(x) = (35x^4-30x^2+3)/8
    // P_5(x) = x(63x^4-70x^2+15)/8

    // generation of polynomials
    //   d/dx[ (1-x^2) dP_n(x)/dx ] + n(n+1) P_n(x) = 0
    //   P_n(x) = sum_{k=0}^{floor(n/2)} c_k x^{n-2k}
    //     c_k = (-1)^k (2n-2k)! / [ 2^n k! (n-k)! (n-2k)! ]
    //   P_n(x) = ((-1)^n/(2^n n!)) d^n/dx^n[ (1-x^2)^n ]
    //   (n+1)P_{n+1}(x) = (2n+1) x P_n(x) - n P_{n-1}(x)
    //   (1-x^2) dP_n(x)/dx = -n x P_n(x) + n P_{n-1}(x)

    // roots of the Legendre polynomial of specified degree
    const int iDegree = 5;
    const Real afRoot[iDegree] =
    {
        (Real)-0.9061798459,
        (Real)-0.5384693101,
        (Real) 0.0,
        (Real)+0.5384693101,
        (Real)+0.9061798459
    };
    const Real afCoeff[iDegree] =
    {
        (Real)0.2369268850,
        (Real)0.4786286705,
        (Real)0.5688888889,
        (Real)0.4786286705,
        (Real)0.2369268850
    };

    // Need to transform domain [a,b] to [-1,1].  If a <= x <= b and
    // -1 <= t <= 1, then x = ((b-a)*t+(b+a))/2.
    Real fRadius = ((Real)0.5)*(fB - fA);
    Real fCenter = ((Real)0.5)*(fB + fA);

    Real fResult = (Real)0.0;
    for (int i = 0; i < iDegree; i++)
    {
        fResult += afCoeff[i]*oF(fRadius*afRoot[i]+fCenter,pvUserData);
    }
    fResult *= fRadius;

    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Integrate1<Real>::TrapezoidRule (int iNumSamples, Real fA, Real fB,
    Function oF, void* pvUserData)
{
    assert(iNumSamples >= 2);
    Real fH = (fB - fA)/(Real)(iNumSamples - 1);
    Real fResult = ((Real)0.5)*(oF(fA,pvUserData) + oF(fB,pvUserData));
    for (int i = 1; i <= iNumSamples - 2; i++)
    {
        fResult += oF(fA+i*fH,pvUserData);
    }
    fResult *= fH;
    return fResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class Integrate1<float>;

template WM4_FOUNDATION_ITEM
class Integrate1<double>;
//----------------------------------------------------------------------------
}
