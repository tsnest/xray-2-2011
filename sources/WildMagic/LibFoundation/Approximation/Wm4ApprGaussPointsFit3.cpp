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
#include "Wm4ApprGaussPointsFit3.h"
#include "Wm4Eigen.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Box3<Real> GaussPointsFit3 (int iQuantity, const Vector3<Real>* akPoint)
{
    Box3<Real> kBox(Vector3<Real>::ZERO,Vector3<Real>::UNIT_X,
        Vector3<Real>::UNIT_Y,Vector3<Real>::UNIT_Z,(Real)1.0,(Real)1.0,
        (Real)1.0);

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
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumXZ = (Real)0.0;
    Real fSumYY = (Real)0.0, fSumYZ = (Real)0.0, fSumZZ = (Real)0.0;
    for (i = 0; i < iQuantity; i++)
    {
        Vector3<Real> kDiff = akPoint[i] - kBox.Center;
        fSumXX += kDiff.X()*kDiff.X();
        fSumXY += kDiff.X()*kDiff.Y();
        fSumXZ += kDiff.X()*kDiff.Z();
        fSumYY += kDiff.Y()*kDiff.Y();
        fSumYZ += kDiff.Y()*kDiff.Z();
        fSumZZ += kDiff.Z()*kDiff.Z();
    }

    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumXZ *= fInvQuantity;
    fSumYY *= fInvQuantity;
    fSumYZ *= fInvQuantity;
    fSumZZ *= fInvQuantity;

    // setup the eigensolver
    Eigen<Real> kES(3);
    kES(0,0) = fSumXX;
    kES(0,1) = fSumXY;
    kES(0,2) = fSumXZ;
    kES(1,0) = fSumXY;
    kES(1,1) = fSumYY;
    kES(1,2) = fSumYZ;
    kES(2,0) = fSumXZ;
    kES(2,1) = fSumYZ;
    kES(2,2) = fSumZZ;
    kES.IncrSortEigenStuff3();

    for (i = 0; i < 3; i++)
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
Box3<float> GaussPointsFit3<float> (int, const Vector3<float>*);

template WM4_FOUNDATION_ITEM
Box3<double> GaussPointsFit3<double> (int, const Vector3<double>*);
//----------------------------------------------------------------------------
}
