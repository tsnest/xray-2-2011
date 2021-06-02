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
// Version: 4.0.1 (2006/07/23)

#include "Wm4FoundationPCH.h"
#include "Wm4ContCapsule3.h"
#include "Wm4ApprLineFit3.h"
#include "Wm4DistVector3Line3.h"
#include "Wm4DistVector3Segment3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Capsule3<Real> ContCapsule (int iQuantity, const Vector3<Real>* akPoint)
{
    Line3<Real> kLine = OrthogonalLineFit3<Real>(iQuantity,akPoint);

    Real fMaxRadiusSqr = (Real)0.0;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        Real fRSqr = DistVector3Line3<Real>(akPoint[i],kLine).GetSquared();
        if (fRSqr > fMaxRadiusSqr)
        {
            fMaxRadiusSqr = fRSqr;
        }
    }

    Vector3<Real> kU, kV, kW = kLine.Direction;
    Vector3<Real>::GenerateComplementBasis(kU,kV,kW);

    Real fMin = Math<Real>::MAX_REAL, fMax = -fMin;
    for (i = 0; i < iQuantity; i++)
    {
        Vector3<Real> kDiff = akPoint[i] - kLine.Origin;
        Real fU = kU.Dot(kDiff);
        Real fV = kV.Dot(kDiff);
        Real fW = kW.Dot(kDiff);
        Real fDiscr = fMaxRadiusSqr - (fU*fU + fV*fV);
        Real fRadical = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));

        Real fTest = fW + fRadical;
        if (fTest < fMin)
        {
            fMin = fTest;
        }

        fTest = fW - fRadical;
        if (fTest > fMax)
        {
            fMax = fTest;
        }
    }

    Capsule3<Real> kCapsule;
    kCapsule.Radius = Math<Real>::Sqrt(fMaxRadiusSqr);
    kCapsule.Segment.Origin = kLine.Origin +
        (((Real)0.5)*(fMin+fMax))*kLine.Direction;
    kCapsule.Segment.Direction = kLine.Direction;

    if (fMax > fMin)
    {
        // container is a capsule
        kCapsule.Segment.Extent = ((Real)0.5)*(fMax - fMin);
    }
    else
    {
        // container is a sphere
        kCapsule.Segment.Extent = (Real)0.0;
    }

    return kCapsule;
}
//----------------------------------------------------------------------------
template <class Real>
bool InCapsule (const Vector3<Real>& rkPoint, const Capsule3<Real>& rkCapsule)
{
    Real fDistance = DistVector3Segment3<Real>(rkPoint,
        rkCapsule.Segment).Get();
    return fDistance <= rkCapsule.Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool InCapsule (const Sphere3<Real>& rkSphere,
    const Capsule3<Real>& rkCapsule)
{
    Real fRDiff = rkCapsule.Radius - rkSphere.Radius;
    if (fRDiff >= (Real)0.0)
    {
        Real fDistance = DistVector3Segment3<Real>(rkSphere.Center,
            rkCapsule.Segment).Get();
        return fDistance <= fRDiff;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool InCapsule (const Capsule3<Real>& rkTestCapsule,
    const Capsule3<Real>& rkCapsule)
{
    Sphere3<Real> kSpherePosEnd(
        rkTestCapsule.Segment.GetPosEnd(),
        rkTestCapsule.Radius);
        
    Sphere3<Real> kSphereNegEnd(
        rkTestCapsule.Segment.GetNegEnd(),
        rkTestCapsule.Radius);

    return InCapsule<Real>(kSpherePosEnd,rkCapsule)
        && InCapsule<Real>(kSphereNegEnd,rkCapsule);
}
//----------------------------------------------------------------------------
template <class Real>
Capsule3<Real> MergeCapsules (const Capsule3<Real>& rkCapsule0,
    const Capsule3<Real>& rkCapsule1)
{
    if (InCapsule<Real>(rkCapsule0,rkCapsule1))
    {
        return rkCapsule1;
    }

    if (InCapsule<Real>(rkCapsule1,rkCapsule0))
    {
        return rkCapsule0;
    }

    const Vector3<Real>& rkP0 = rkCapsule0.Segment.Origin;
    const Vector3<Real>& rkP1 = rkCapsule1.Segment.Origin;
    const Vector3<Real>& rkD0 = rkCapsule0.Segment.Direction;
    const Vector3<Real>& rkD1 = rkCapsule1.Segment.Direction;

    // axis of final capsule
    Line3<Real> kLine;

    // axis center is average of input axis centers
    kLine.Origin = ((Real)0.5)*(rkP0 + rkP1);

    // axis unit direction is average of input axis unit directions
    if (rkD0.Dot(rkD1) >= (Real)0.0)
    {
        kLine.Direction = rkD0 + rkD1;
    }
    else
    {
        kLine.Direction = rkD0 - rkD1;
    }
    kLine.Direction.Normalize();

    // Cylinder with axis 'kLine' must contain the spheres centered at the
    // end points of the input capsules.
    Vector3<Real> kPosEnd0 = rkCapsule0.Segment.GetPosEnd();
    Real fRadius = DistVector3Line3<Real>(kPosEnd0,kLine).Get() +
        rkCapsule0.Radius;

    Vector3<Real> kNegEnd0 = rkCapsule0.Segment.GetNegEnd();
    Real fTmp = DistVector3Line3<Real>(kNegEnd0,kLine).Get() +
        rkCapsule0.Radius;
    if (fTmp > fRadius)
    {
        fRadius = fTmp;
    }

    Vector3<Real> kPosEnd1 = rkCapsule1.Segment.GetPosEnd();
    fTmp = DistVector3Line3<Real>(kPosEnd1,kLine).Get() + rkCapsule1.Radius;
    if (fTmp > fRadius)
    {
        fRadius = fTmp;
    }

    Vector3<Real> kNegEnd1 = rkCapsule1.Segment.GetNegEnd();
    fTmp = DistVector3Line3<Real>(kNegEnd1,kLine).Get() + rkCapsule1.Radius;
    if (fTmp > fRadius)
    {
        fRadius = fTmp;
    }

    // process sphere <PosEnd0,r0>
    Real fRDiff = fRadius - rkCapsule0.Radius;
    Real fRDiffSqr = fRDiff*fRDiff;
    Vector3<Real> kDiff = kLine.Origin - kPosEnd0;
    Real fK0 = kDiff.SquaredLength() - fRDiffSqr;
    Real fK1 = kDiff.Dot(kLine.Direction);
    Real fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    Real fTPos = -fK1 - fRoot;
    Real fTNeg = -fK1 + fRoot;

    // process sphere <NegEnd0,r0>
    kDiff = kLine.Origin - kNegEnd0;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction);
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    fTmp = -fK1 - fRoot;
    if (fTmp > fTPos)
    {
        fTPos = fTmp;
    }
    fTmp = -fK1 + fRoot;
    if (fTmp < fTNeg)
    {
        fTNeg = fTmp;
    }

    // process sphere <PosEnd1,r1>
    fRDiff = fRadius - rkCapsule1.Radius;
    fRDiffSqr = fRDiff*fRDiff;
    kDiff = kLine.Origin - kPosEnd1;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction);
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    fTmp = -fK1 - fRoot;
    if (fTmp > fTPos)
    {
        fTPos = fTmp;
    }
    fTmp = -fK1 + fRoot;
    if (fTmp < fTNeg)
    {
        fTNeg = fTmp;
    }

    // process sphere <NegEnd1,r1>
    kDiff = kLine.Origin - kNegEnd1;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction);
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    fTmp = -fK1 - fRoot;
    if (fTmp > fTPos)
    {
        fTPos = fTmp;
    }
    fTmp = -fK1 + fRoot;
    if (fTmp < fTNeg)
    {
        fTNeg = fTmp;
    }

    Capsule3<Real> kCapsule;
    kCapsule.Radius = fRadius;
    kCapsule.Segment.Origin = kLine.Origin +
        ((Real)0.5)*(fTPos+fTNeg)*kLine.Direction;
    kCapsule.Segment.Direction = kLine.Direction;

    if (fTPos > fTNeg)
    {
        // container is a capsule
        kCapsule.Segment.Extent = ((Real)0.5)*(fTPos - fTNeg);
    }
    else
    {
        // container is a sphere
        kCapsule.Segment.Extent = (Real)0.0;
    }

    return kCapsule;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
Capsule3<float> ContCapsule<float> (int, const Vector3<float>*);

template WM4_FOUNDATION_ITEM
bool InCapsule<float> (const Vector3<float>&, const Capsule3<float>&);

template WM4_FOUNDATION_ITEM
bool InCapsule<float> (const Sphere3<float>&, const Capsule3<float>&);

template WM4_FOUNDATION_ITEM
bool InCapsule<float> (const Capsule3<float>&, const Capsule3<float>&);

template WM4_FOUNDATION_ITEM
Capsule3<float> MergeCapsules<float> (const Capsule3<float>&,
    const Capsule3<float>&);

template WM4_FOUNDATION_ITEM
Capsule3<double> ContCapsule<double> (int, const Vector3<double>*);

template WM4_FOUNDATION_ITEM
bool InCapsule<double> (const Vector3<double>&, const Capsule3<double>&);

template WM4_FOUNDATION_ITEM
bool InCapsule<double> (const Sphere3<double>&, const Capsule3<double>&);

template WM4_FOUNDATION_ITEM
bool InCapsule<double> (const Capsule3<double>&, const Capsule3<double>&);

template WM4_FOUNDATION_ITEM
Capsule3<double> MergeCapsules<double> (const Capsule3<double>&,
    const Capsule3<double>&);
//----------------------------------------------------------------------------
}
