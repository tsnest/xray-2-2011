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
#include "Wm4ApprLineFit2.h"
#include "Wm4Eigen.h"
#include "Wm4LinearSystem.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
bool HeightLineFit2 (int iQuantity, const Vector2<Real>* akPoint, Real& rfA,
    Real& rfB)
{
    // You need at least two points to determine the line.  Even so, if
    // the points are on a vertical line, there is no least-squares fit in
    // the 'height' sense.  This will be trapped by the determinant of the
    // coefficient matrix being (nearly) zero.

    // compute sums for linear system
    Real fSumX = (Real)0.0, fSumY = (Real)0.0;
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        fSumX += akPoint[i].X();
        fSumY += akPoint[i].Y();
        fSumXX += akPoint[i].X()*akPoint[i].X();
        fSumXY += akPoint[i].X()*akPoint[i].Y();
    }

    Real aafA[2][2] =
    {
        {fSumXX, fSumX},
        {fSumX, (Real)iQuantity}
    };

    Real afB[2] =
    {
        fSumXY,
        fSumY
    };

    Real afX[2];

    bool bNonsingular = LinearSystem<Real>().Solve2(aafA,afB,afX);
    if (bNonsingular)
    {
        rfA = afX[0];
        rfB = afX[1];
    }
    else
    {
        rfA = Math<Real>::MAX_REAL;
        rfB = Math<Real>::MAX_REAL;
    }

    return bNonsingular;
}
//----------------------------------------------------------------------------
template <class Real>
Line2<Real> OrthogonalLineFit2 (int iQuantity, const Vector2<Real>* akPoint)
{
    Line2<Real> kLine(Vector2<Real>::ZERO,Vector2<Real>::ZERO);

    // compute the mean of the points
    kLine.Origin = akPoint[0];
    int i;
    for (i = 1; i < iQuantity; i++)
    {
        kLine.Origin += akPoint[i];
    }
    Real fInvQuantity = ((Real)1.0)/iQuantity;
    kLine.Origin *= fInvQuantity;

    // compute the covariance matrix of the points
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumYY = (Real)0.0;
    for (i = 0; i < iQuantity; i++) 
    {
        Vector2<Real> kDiff = akPoint[i] - kLine.Origin;
        fSumXX += kDiff.X()*kDiff.X();
        fSumXY += kDiff.X()*kDiff.Y();
        fSumYY += kDiff.Y()*kDiff.Y();
    }

    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumYY *= fInvQuantity;

    // set up the eigensolver
    Eigen<Real> kES(2);
    kES(0,0) = fSumYY;
    kES(0,1) = -fSumXY;
    kES(1,0) = -fSumXY;
    kES(1,1) = fSumXX;

    // compute eigenstuff, smallest eigenvalue is in last position
    kES.DecrSortEigenStuff2();

    // unit-length direction for best-fit line
    kES.GetEigenvector(1,kLine.Direction);

    return kLine;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
bool HeightLineFit2<float> (int, const Vector2<float>*, float&, float&);

template WM4_FOUNDATION_ITEM
Line2<float> OrthogonalLineFit2<float> (int, const Vector2<float>*);

template WM4_FOUNDATION_ITEM
bool HeightLineFit2<double> (int, const Vector2<double>*, double&, double&);

template WM4_FOUNDATION_ITEM
Line2<double> OrthogonalLineFit2<double> (int, const Vector2<double>*);
//----------------------------------------------------------------------------
}
