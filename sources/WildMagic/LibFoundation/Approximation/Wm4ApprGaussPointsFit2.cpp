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
#include "Wm4ApprGaussPointsFit2.h"
#include "Wm4Eigen.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Box2<Real> GaussPointsFit2 (int iQuantity, const Vector2<Real>* akPoint)
{
    Box2<Real> kBox(Vector2<Real>::ZERO,Vector2<Real>::UNIT_X,
        Vector2<Real>::UNIT_Y,(Real)1.0,(Real)1.0);

    // compute the mean of the points
    kBox.Center = akPoint[0];
    int i;
    for (i = 1; i < iQuantity; i++)
    {
        kBox.Center += akPoint[i];
    }
    Real fInvQuantity = ((Real)1.0)/iQuantity;
    kBox.Center *= fInvQuantity;

    // compute the covariance matrix of the points
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumYY = (Real)0.0;
    for (i = 0; i < iQuantity; i++)
    {
        Vector2<Real> kDiff = akPoint[i] - kBox.Center;
        fSumXX += kDiff.X()*kDiff.X();
        fSumXY += kDiff.X()*kDiff.Y();
        fSumYY += kDiff.Y()*kDiff.Y();
    }

    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumYY *= fInvQuantity;

    // setup the eigensolver
    Eigen<Real> kES(2);
    kES(0,0) = fSumXX;
    kES(0,1) = fSumXY;
    kES(1,0) = fSumXY;
    kES(1,1) = fSumYY;
    kES.IncrSortEigenStuff2();

    for (i = 0; i < 2; i++)
    {
        kBox.Extent[i] = kES.GetEigenvalue(i);
        kES.GetEigenvector(i,kBox.Axis[i]);
    }

    return kBox;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
Box2<float> GaussPointsFit2<float> (int, const Vector2<float>*);

template WM4_FOUNDATION_ITEM
Box2<double> GaussPointsFit2<double> (int, const Vector2<double>*);
//----------------------------------------------------------------------------
}
