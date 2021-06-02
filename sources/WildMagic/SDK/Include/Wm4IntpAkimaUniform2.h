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

#ifndef WM4INTPAKIMAUNIFORM2_H
#define WM4INTPAKIMAUNIFORM2_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntpAkimaUniform2
{
public:
    // Construction and destruction.  IntpAkimaUniform2 does not accept
    // responsibility for deleting the input array.  The application must do
    // so.  The interpolator is for uniformly spaced (x,y)-values.  The
    // function values are assumed to be organized as f(x,y) = F[y][x].
    IntpAkimaUniform2 (int iXBound, int iYBound, Real fXMin,
        Real fXSpacing, Real fYMin, Real fYSpacing, Real** aafF);

    ~IntpAkimaUniform2 ();

    class WM4_FOUNDATION_ITEM Polynomial
    {
    public:
        Polynomial ()
        {
            memset(&m_aafA[0][0],0,16*sizeof(Real));
        }

        // P(x,y) = (1,x,x^2,x^3)*A*(1,y,y^2,y^3).  The matrix term A[i][j]
        // corresponds to the polynomial term x^i y^j.
        Real& A (int iX, int iY)
        {
            return m_aafA[iX][iY];
        }

        Real operator() (Real fX, Real fY) const
        {
            Real afB[4];
            for (int i = 0; i < 4; i++)
            {
                afB[i] = m_aafA[i][0] + fY*(m_aafA[i][1] + fY*(m_aafA[i][2] +
                    fY*m_aafA[i][3]));
            }

            return afB[0] + fX*(afB[1] + fX*(afB[2] + fX*afB[3]));
        }

        Real operator() (int iXOrder, int iYOrder, Real fX, Real fY) const
        {
            Real afXPow[4];
            switch (iXOrder)
            {
            case 0:
                afXPow[0] = (Real)1.0;
                afXPow[1] = fX;
                afXPow[2] = fX*fX;
                afXPow[3] = fX*fX*fX;
                break;
            case 1:
                afXPow[0] = (Real)0.0;
                afXPow[1] = (Real)1.0;
                afXPow[2] = ((Real)2.0)*fX;
                afXPow[3] = ((Real)3.0)*fX*fX;
                break;
            case 2:
                afXPow[0] = (Real)0.0;
                afXPow[1] = (Real)0.0;
                afXPow[2] = (Real)2.0;
                afXPow[3] = ((Real)6.0)*fX;
                break;
            case 3:
                afXPow[0] = (Real)0.0;
                afXPow[1] = (Real)0.0;
                afXPow[2] = (Real)0.0;
                afXPow[3] = (Real)6.0;
                break;
            default:
                return (Real)0.0;
            }

            Real afYPow[4];
            switch (iYOrder)
            {
            case 0:
                afYPow[0] = (Real)1.0;
                afYPow[1] = fY;
                afYPow[2] = fY*fY;
                afYPow[3] = fY*fY*fY;
                break;
            case 1:
                afYPow[0] = (Real)0.0;
                afYPow[1] = (Real)1.0;
                afYPow[2] = ((Real)2.0)*fY;
                afYPow[3] = ((Real)3.0)*fY*fY;
                break;
            case 2:
                afYPow[0] = (Real)0.0;
                afYPow[1] = (Real)0.0;
                afYPow[2] = (Real)2.0;
                afYPow[3] = ((Real)6.0)*fY;
                break;
            case 3:
                afYPow[0] = (Real)0.0;
                afYPow[1] = (Real)0.0;
                afYPow[2] = (Real)0.0;
                afYPow[3] = (Real)6.0;
                break;
            default:
                return (Real)0.0;
            }

            Real fP = (Real)0.0;
            for (int iY = 0; iY <= 3; iY++)
            {
                for (int iX = 0; iX <= 3; iX++)
                {
                    fP += m_aafA[iX][iY]*afXPow[iX]*afYPow[iY];
                }
            }

            return fP;
        }

    protected:
        Real m_aafA[4][4];
    };

    int GetXBound () const;
    int GetYBound () const;
    int GetQuantity () const;
    Real** GetF () const;
    Polynomial** GetPolynomials () const;
    const Polynomial& GetPolynomial (int iX, int iY) const;

    Real GetXMin () const;
    Real GetXMax () const;
    Real GetXSpacing () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetYSpacing () const;

    // Evaluate the function and its derivatives.  The application is
    // responsible for ensuring that xmin <= x <= xmax and ymin <= y <= ymax.
    // If (x,y) is outside the extremes, the function returns MAXREAL.  The
    // first operator is for function evaluation.  The second operator is for
    // function or derivative evaluations.  The uiXOrder argument is the order
    // of the x-derivative and the uiYOrder argument is the order of the
    // y-derivative.  Both orders are zero to get the function value itself.
    Real operator() (Real fX, Real fY) const;
    Real operator() (int iXOrder, int iYOrder, Real fX, Real fY) const;

private:
    Real ComputeDerivative (Real* afSlope) const;
    void Construct (Polynomial& rkPoly, Real aafF[2][2],
        Real aafFX[2][2], Real aafFY[2][2], Real aafFXY[2][2]);

    bool XLookup (Real fX, int& riXIndex, Real& rfDX) const;
    bool YLookup (Real fY, int& riYIndex, Real& rfDY) const;

    int m_iXBound, m_iYBound, m_iQuantity;
    Real** m_aafF;
    Polynomial** m_aakPoly;
    Real m_fXMin, m_fXMax, m_fXSpacing;
    Real m_fYMin, m_fYMax, m_fYSpacing;
};

typedef IntpAkimaUniform2<float> IntpAkimaUniform2f;
typedef IntpAkimaUniform2<double> IntpAkimaUniform2d;

}

#endif
