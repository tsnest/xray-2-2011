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
#include "Wm4ContCylinder3.h"
#include "Wm4ApprLineFit3.h"
#include "Wm4DistVector3Line3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Cylinder3<Real> ContCylinder (int iQuantity, const Vector3<Real>* akPoint)
{
    Cylinder3<Real> kCylinder;

    Line3<Real> kLine = OrthogonalLineFit3(iQuantity,akPoint);

    Real fMaxRadiusSqr = (Real)0.0;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        Real fRadiusSqr = DistVector3Line3<Real>(akPoint[i],
            kLine).GetSquared();
        if (fRadiusSqr > fMaxRadiusSqr)
        {
            fMaxRadiusSqr = fRadiusSqr;
        }
    }

    Vector3<Real> kDiff = akPoint[0] - kLine.Origin;
    Real fWMin = kLine.Direction.Dot(kDiff), fWMax = fWMin;
    for (i = 1; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kLine.Origin;
        Real fW = kLine.Direction.Dot(kDiff);
        if (fW < fWMin)
        {
            fWMin = fW;
        }
        else if (fW > fWMax)
        {
            fWMax = fW;
        }
    }

    kCylinder.Segment.Origin = kLine.Origin +
        (((Real)0.5)*(fWMax+fWMin))*kLine.Direction;
    kCylinder.Segment.Direction = kLine.Direction;
    kCylinder.Radius = Math<Real>::Sqrt(fMaxRadiusSqr);
    kCylinder.Height = fWMax - fWMin;

    return kCylinder;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
Cylinder3<float> ContCylinder<float> (int, const Vector3<float>*);

template WM4_FOUNDATION_ITEM
Cylinder3<double> ContCylinder<double> (int, const Vector3<double>*);
//----------------------------------------------------------------------------
}
