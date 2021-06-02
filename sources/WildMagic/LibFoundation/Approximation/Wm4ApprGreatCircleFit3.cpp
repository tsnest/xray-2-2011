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
// Version: 4.6.1 (2008/02/26)

#include "Wm4FoundationPCH.h"
#include "Wm4ApprGreatCircleFit3.h"
#include "Wm4Eigen.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> GreatCircleFit3 (int iQuantity, const Vector3<Real>* akVector)
{
    // Compute the covariance matrix of the vectors.
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumXZ = (Real)0.0;
    Real fSumYY = (Real)0.0, fSumYZ = (Real)0.0, fSumZZ = (Real)0.0;
    for (int i = 0; i < iQuantity; i++) 
    {
        Vector3<Real> kDiff = akVector[i];
        fSumXX += kDiff.X()*kDiff.X();
        fSumXY += kDiff.X()*kDiff.Y();
        fSumXZ += kDiff.X()*kDiff.Z();
        fSumYY += kDiff.Y()*kDiff.Y();
        fSumYZ += kDiff.Y()*kDiff.Z();
        fSumZZ += kDiff.Z()*kDiff.Z();
    }

    Real fInvQuantity = ((Real)1.0)/iQuantity;
    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumXZ *= fInvQuantity;
    fSumYY *= fInvQuantity;
    fSumYZ *= fInvQuantity;
    fSumZZ *= fInvQuantity;

    // Set up the eigensolver.
    Eigen<Real> kES(3);
    kES(0,0) = fSumXX;
    kES(0,1) = fSumXY;
    kES(0,2) = fSumXZ;
    kES(1,0) = kES(0,1);
    kES(1,1) = fSumYY;
    kES(1,2) = fSumYZ;
    kES(2,0) = kES(0,2);
    kES(2,1) = kES(1,2);
    kES(2,2) = fSumZZ;

    // Compute eigenstuff; the smallest eigenvalue is in last position.
    kES.DecrSortEigenStuff3();

    // Unit-length direction for best-fit great circle.
    Vector3<Real> kNormal;
    kES.GetEigenvector(2,kNormal);
    return kNormal;
}
//----------------------------------------------------------------------------
template <class Real>
GreatArcFit3<Real>::GreatArcFit3 (int iQuantity,
    const Vector3<Real>* akVector, Vector3<Real>& rkNormal,
    Vector3<Real>& rkArcEnd0, Vector3<Real>& rkArcEnd1)
{
    // Get the least-squares great circle for the vectors.  The circle is on
    // the plane Dot(N,X) = 0.
    rkNormal = GreatCircleFit3<Real>(iQuantity,akVector);

    // Compute a coordinate system to allow projection of the vectors onto
    // the great circle.  The coordinates axes have directions U, V, and N.
    Vector3<Real> kU, kV;
    Vector3<Real>::GenerateComplementBasis(kU,kV,rkNormal);

    // The vectors are X[i] = u[i]*U + v[i]*V + w[i]*N.  The projections
    // are P[i] = (u[i]*U + v[i]*V)/sqrt(u[i]*u[i] + v[i]*v[i]).  The great
    // circle is parameterized by C(t) = cos(t)*U + sin(t)*V.  Compute the
    // angles t in [-pi,pi] for the projections onto the great circle.  It
    // is not necesarily to normalize (u[i],v[i]), instead computing
    // t = atan2(v[i],u[i]).
    std::vector<Item> kItems(iQuantity);
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        Item& rkItem = kItems[i];
        rkItem.U = kU.Dot(akVector[i]);
        rkItem.V = kV.Dot(akVector[i]);
        rkItem.Angle = Math<Real>::ATan2(rkItem.V,rkItem.U);
    }
    std::sort(kItems.begin(),kItems.end());

    // Locate the pair of consecutive angles whose difference is a maximum.
    // Effectively, we are constructing a cone of minimum angle that contains
    // the unit-length vectors.
    int iQm1 = iQuantity - 1;
    Real fMaxDiff = Math<Real>::TWO_PI + kItems[0].Angle - kItems[iQm1].Angle;
    int iEnd0 = 0, iEnd1 = iQm1;
    for (int i0 = 0, i1 = 1; i0 < iQm1; i0 = i1++)
    {
        Real fDiff = kItems[i1].Angle - kItems[i0].Angle;
        if (fDiff > fMaxDiff)
        {
            fMaxDiff = fDiff;
            iEnd0 = i1;
            iEnd1 = i0;
        }
    }

    rkArcEnd0 = kItems[iEnd0].U*kU + kItems[iEnd0].V*kV;
    rkArcEnd1 = kItems[iEnd1].U*kU + kItems[iEnd1].V*kV;
    rkArcEnd0.Normalize();
    rkArcEnd1.Normalize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
Vector3<float> GreatCircleFit3<float> (int, const Vector3<float>*);

template WM4_FOUNDATION_ITEM
class GreatArcFit3<float>;

template WM4_FOUNDATION_ITEM
Vector3<double> GreatCircleFit3<double> (int, const Vector3<double>*);

template WM4_FOUNDATION_ITEM
class GreatArcFit3<double>;
//----------------------------------------------------------------------------
}
