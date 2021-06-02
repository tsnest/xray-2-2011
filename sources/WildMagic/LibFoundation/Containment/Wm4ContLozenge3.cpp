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
#include "Wm4ContLozenge3.h"
#include "Wm4ApprGaussPointsFit3.h"
#include "Wm4DistVector3Line3.h"
#include "Wm4DistVector3Rectangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Lozenge3<Real> ContLozenge (int iQuantity, const Vector3<Real>* akPoint)
{
    // Fit with Gaussian.  Axis(0) corresponds to the smallest eigenvalue.
    Box3<Real> kBox = GaussPointsFit3<Real>(iQuantity,akPoint);

    Vector3<Real> kDiff = akPoint[0] - kBox.Center;
    Real fWMin = kBox.Axis[0].Dot(kDiff), fWMax = fWMin, fW;
    int i;
    for (i = 1; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kBox.Center;
        fW = kBox.Axis[0].Dot(kDiff);
        if (fW < fWMin)
        {
            fWMin = fW;
        }
        else if (fW > fWMax)
        {
            fWMax = fW;
        }
    }

    Real fRadius = ((Real)0.5)*(fWMax - fWMin);
    Real fRSqr = fRadius*fRadius;
    kBox.Center += (((Real)0.5)*(fWMax + fWMin))*kBox.Axis[0];

    Real fAMin = Math<Real>::MAX_REAL, fAMax = -fAMin;
    Real fBMin = Math<Real>::MAX_REAL, fBMax = -fBMin;
    Real fDiscr, fRadical, fU, fV, fTest;
    for (i = 0; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kBox.Center;
        fU = kBox.Axis[2].Dot(kDiff);
        fV = kBox.Axis[1].Dot(kDiff);
        fW = kBox.Axis[0].Dot(kDiff);
        fDiscr = fRSqr - fW*fW;
        fRadical = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));

        fTest = fU + fRadical;
        if (fTest < fAMin)
        {
            fAMin = fTest;
        }

        fTest = fU - fRadical;
        if (fTest > fAMax)
        {
            fAMax = fTest;
        }

        fTest = fV + fRadical;
        if (fTest < fBMin)
        {
            fBMin = fTest;
        }

        fTest = fV - fRadical;
        if (fTest > fBMax)
        {
            fBMax = fTest;
        }
    }

    // enclosing region might be a capsule or a sphere
    if (fAMin >= fAMax)
    {
        fTest = ((Real)0.5)*(fAMin + fAMax);
        fAMin = fTest;
        fAMax = fTest;
    }
    if (fBMin >= fBMax)
    {
        fTest = ((Real)0.5)*(fBMin + fBMax);
        fBMin = fTest;
        fBMax = fTest;
    }

    // Make correction for points inside mitered corner but outside quarter
    // sphere.
    for (i = 0; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kBox.Center;
        fU = kBox.Axis[2].Dot(kDiff);
        fV = kBox.Axis[1].Dot(kDiff);

        Real* pfAExtreme = 0;
        Real* pfBExtreme = 0;

        if (fU > fAMax)
        {
            if (fV > fBMax)
            {
                pfAExtreme = &fAMax;
                pfBExtreme = &fBMax;
            }
            else if (fV < fBMin)
            {
                pfAExtreme = &fAMax;
                pfBExtreme = &fBMin;
            }
        }
        else if (fU < fAMin)
        {
            if (fV > fBMax)
            {
                pfAExtreme = &fAMin;
                pfBExtreme = &fBMax;
            }
            else if (fV < fBMin)
            {
                pfAExtreme = &fAMin;
                pfBExtreme = &fBMin;
            }
        }

        if (pfAExtreme)
        {
            Real fDeltaU = fU - *pfAExtreme;
            Real fDeltaV = fV - *pfBExtreme;
            Real fDeltaSumSqr = fDeltaU*fDeltaU + fDeltaV*fDeltaV;
            fW = kBox.Axis[0].Dot(kDiff);
            Real fWSqr = fW*fW;
            fTest = fDeltaSumSqr + fWSqr;
            if (fTest > fRSqr)
            {
                fDiscr = (fRSqr - fWSqr)/fDeltaSumSqr;
                Real fT = -Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
                *pfAExtreme = fU + fT*fDeltaU;
                *pfBExtreme = fV + fT*fDeltaV;
            }
        }
    }

    Lozenge3<Real> kLozenge;
    kLozenge.Radius = fRadius;
    kLozenge.Rectangle.Axis[0] = kBox.Axis[2];
    kLozenge.Rectangle.Axis[1] = kBox.Axis[1];

    if (fAMin < fAMax)
    {
        if (fBMin < fBMax)
        {
            // container is a lozenge
            kLozenge.Rectangle.Center = kBox.Center + fAMin*kBox.Axis[2] +
                fBMin*kBox.Axis[1];
            kLozenge.Rectangle.Extent[0] = ((Real)0.5)*(fAMax - fAMin);
            kLozenge.Rectangle.Extent[1] = ((Real)0.5)*(fBMax - fBMin);
        }
        else
        {
            // container is a capsule
            kLozenge.Rectangle.Center = kBox.Center + fAMin*kBox.Axis[2] +
                (((Real)0.5)*(fBMin+fBMax))*kBox.Axis[1];
            kLozenge.Rectangle.Extent[0] = ((Real)0.5)*(fAMax - fAMin);
            kLozenge.Rectangle.Extent[1] = (Real)0.0;
        }
    }
    else
    {
        if (fBMin < fBMax)
        {
            // container is a capsule
            kLozenge.Rectangle.Center = kBox.Center + fBMin*kBox.Axis[1] +
                (((Real)0.5)*(fAMin+fAMax))*kBox.Axis[2];
            kLozenge.Rectangle.Extent[0] = (Real)0.0;
            kLozenge.Rectangle.Extent[1] = ((Real)0.5)*(fBMax - fBMin);
        }
        else
        {
            // container is a sphere
            kLozenge.Rectangle.Center = kBox.Center +
                (((Real)0.5)*(fAMin+fAMax))*kBox.Axis[2] +
                (((Real)0.5)*(fBMin+fBMax))*kBox.Axis[1];
            kLozenge.Rectangle.Extent[0] = (Real)0.0;
            kLozenge.Rectangle.Extent[1] = (Real)0.0;
        }
    }

    return kLozenge;
}
//----------------------------------------------------------------------------
template <class Real>
bool InLozenge (const Vector3<Real>& rkPoint, const Lozenge3<Real>& rkLozenge)
{
    Real fDistance = DistVector3Rectangle3<Real>(rkPoint,
        rkLozenge.Rectangle).Get();
    return fDistance <= rkLozenge.Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
Lozenge3<float> ContLozenge<float> (int, const Vector3<float>*);

template WM4_FOUNDATION_ITEM
bool InLozenge<float> (const Vector3<float>&, const Lozenge3<float>&);

template WM4_FOUNDATION_ITEM
Lozenge3<double> ContLozenge<double> (int, const Vector3<double>*);

template WM4_FOUNDATION_ITEM
bool InLozenge<double> (const Vector3<double>&, const Lozenge3<double>&);
//----------------------------------------------------------------------------
}
