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
#include "Wm4ApprQuadraticFit2.h"
#include "Wm4Eigen.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Real QuadraticFit2 (int iQuantity, const Vector2<Real>* akPoint,
    Real afCoeff[6])
{
    Eigen<Real> kES(6);
    int iRow, iCol;
    for (iRow = 0; iRow < 6; iRow++)
    {
        for (iCol = 0; iCol < 6; iCol++)
        {
            kES(iRow,iCol) = (Real)0.0;
        }
    }

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX = akPoint[i].X();
        Real fY = akPoint[i].Y();
        Real fX2 = fX*fX;
        Real fY2 = fY*fY;
        Real fXY = fX*fY;
        Real fX3 = fX*fX2;
        Real fXY2 = fX*fY2;
        Real fX2Y = fX*fXY;
        Real fY3 = fY*fY2;
        Real fX4 = fX*fX3;
        Real fX2Y2 = fX*fXY2;
        Real fX3Y = fX*fX2Y;
        Real fY4 = fY*fY3;
        Real fXY3 = fX*fY3;

        kES(0,1) += fX;
        kES(0,2) += fY;
        kES(0,3) += fX2;
        kES(0,4) += fY2;
        kES(0,5) += fXY;
        kES(1,3) += fX3;
        kES(1,4) += fXY2;
        kES(1,5) += fX2Y;
        kES(2,4) += fY3;
        kES(3,3) += fX4;
        kES(3,4) += fX2Y2;
        kES(3,5) += fX3Y;
        kES(4,4) += fY4;
        kES(4,5) += fXY3;
    }

    kES(0,0) = (Real)iQuantity;
    kES(1,1) = kES(0,3);
    kES(1,2) = kES(0,5);
    kES(2,2) = kES(0,4);
    kES(2,3) = kES(1,5);
    kES(2,5) = kES(1,4);
    kES(5,5) = kES(3,4);

    for (iRow = 0; iRow < 6; iRow++)
    {
        for (iCol = 0; iCol < iRow; iCol++)
        {
            kES(iRow,iCol) = kES(iCol,iRow);
        }
    }

    Real fInvQuantity = ((Real)1.0)/(Real)iQuantity;
    for (iRow = 0; iRow < 6; iRow++)
    {
        for (iCol = 0; iCol < 6; iCol++)
        {
            kES(iRow,iCol) *= fInvQuantity;
        }
    }

    kES.IncrSortEigenStuffN();

    GVector<Real> kEVector = kES.GetEigenvector(0);
    size_t uiSize = 6*sizeof(Real);
    System::Memcpy(afCoeff,uiSize,(Real*)kEVector,uiSize);

    // For exact fit, numeric round-off errors may make the minimum
    // eigenvalue just slightly negative.  Return absolute value since
    // application may rely on the return value being nonnegative.
    return Math<Real>::FAbs(kES.GetEigenvalue(0));
}
//----------------------------------------------------------------------------
template <class Real>
Real QuadraticCircleFit2 (int iQuantity, const Vector2<Real>* akPoint,
    Vector2<Real>& rkCenter, Real& rfRadius)
{
    Eigen<Real> kES(4);
    int iRow, iCol;
    for (iRow = 0; iRow < 4; iRow++)
    {
        for (iCol = 0; iCol < 4; iCol++)
        {
            kES(iRow,iCol) = (Real)0.0;
        }
    }

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX = akPoint[i].X();
        Real fY = akPoint[i].Y();
        Real fX2 = fX*fX;
        Real fY2 = fY*fY;
        Real fXY = fX*fY;
        Real fR2 = fX2+fY2;
        Real fXR2 = fX*fR2;
        Real fYR2 = fY*fR2;
        Real fR4 = fR2*fR2;

        kES(0,1) += fX;
        kES(0,2) += fY;
        kES(0,3) += fR2;
        kES(1,1) += fX2;
        kES(1,2) += fXY;
        kES(1,3) += fXR2;
        kES(2,2) += fY2;
        kES(2,3) += fYR2;
        kES(3,3) += fR4;
    }

    kES(0,0) = (Real)iQuantity;

    for (iRow = 0; iRow < 4; iRow++)
    {
        for (iCol = 0; iCol < iRow; iCol++)
        {
            kES(iRow,iCol) = kES(iCol,iRow);
        }
    }

    Real fInvQuantity = ((Real)1.0)/(Real)iQuantity;
    for (iRow = 0; iRow < 4; iRow++)
    {
        for (iCol = 0; iCol < 4; iCol++)
        {
            kES(iRow,iCol) *= fInvQuantity;
        }
    }

    kES.IncrSortEigenStuffN();

    GVector<Real> kEVector = kES.GetEigenvector(0);
    Real fInv = ((Real)1.0)/kEVector[3];  // beware zero divide
    Real afCoeff[3];
    for (iRow = 0; iRow < 3; iRow++)
    {
        afCoeff[iRow] = fInv*kEVector[iRow];
    }

    rkCenter.X() = -((Real)0.5)*afCoeff[1];
    rkCenter.Y() = -((Real)0.5)*afCoeff[2];
    rfRadius = Math<Real>::Sqrt(Math<Real>::FAbs(rkCenter.X()*rkCenter.X() +
        rkCenter.Y()*rkCenter.Y() - afCoeff[0]));

    // For exact fit, numeric round-off errors may make the minimum
    // eigenvalue just slightly negative.  Return absolute value since
    // application may rely on the return value being nonnegative.
    return Math<Real>::FAbs(kES.GetEigenvalue(0));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
float QuadraticFit2<float> (int, const Vector2<float>*, float[6]);

template WM4_FOUNDATION_ITEM
float QuadraticCircleFit2<float> (int, const Vector2<float>*,
    Vector2<float>&, float&);

template WM4_FOUNDATION_ITEM
double QuadraticFit2<double> (int, const Vector2<double>*, double[6]);

template WM4_FOUNDATION_ITEM
double QuadraticCircleFit2<double> (int, const Vector2<double>*,
    Vector2<double>&, double&);
//----------------------------------------------------------------------------
}
