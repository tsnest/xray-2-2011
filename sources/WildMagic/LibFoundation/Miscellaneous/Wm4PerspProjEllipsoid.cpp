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
#include "Wm4PerspProjEllipsoid.h"
#include "Wm4Eigen.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
void PerspectiveProjectEllipsoid (const Ellipsoid3<Real>& rkEllipsoid,
    const Vector3<Real>& rkEye, const Plane3<Real>& rkPlane,
    const Vector3<Real>& rkU, const Vector3<Real>& rkV, Vector3<Real>& rkP,
    Ellipse2<Real>& rkEllipse)
{
    // get coefficients for ellipsoid as X^T*A*X + B^T*X + C = 0
    Matrix3<Real> kA;
    Vector3<Real> kB;
    Real fC;
    rkEllipsoid.ToCoefficients(kA,kB,fC);

    // compute matrix M (see PerspectiveProjectionEllipsoid.pdf)
    Vector3<Real> kAE = kA*rkEye;
    Real fEAE = rkEye.Dot(kAE);
    Real fBE = kB.Dot(rkEye);
    Real fQuadE = ((Real)4.0)*(fEAE + fBE + fC);
    Vector3<Real> kBp2AE = kB + ((Real)2.0)*kAE;
    Matrix3<Real> kMat = Matrix3<Real>(kBp2AE,kBp2AE) - fQuadE*kA;

    // compute coefficients for projected ellipse
    Vector3<Real> kMU = kMat*rkU;
    Vector3<Real> kMV = kMat*rkV;
    Vector3<Real> kMN = kMat*rkPlane.Normal;
    Real fDmNdE = -rkPlane.DistanceTo(rkEye);
    rkP = rkEye + fDmNdE*rkPlane.Normal;

    Matrix2<Real> kAOut;
    Vector2<Real> kBOut;
    Real fCOut;
    kAOut[0][0] = rkU.Dot(kMU);
    kAOut[0][1] = rkU.Dot(kMV);
    kAOut[1][1] = rkV.Dot(kMV);
    kAOut[1][0] = kAOut[0][1];
    kBOut[0] = ((Real)2.0)*fDmNdE*(rkU.Dot(kMN));
    kBOut[1] = ((Real)2.0)*fDmNdE*(rkV.Dot(kMN));
    fCOut = fDmNdE*fDmNdE*(rkPlane.Normal.Dot(kMN));
    rkEllipse.FromCoefficients(kAOut,kBOut,fCOut);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
void PerspectiveProjectEllipsoid<float> (const Ellipsoid3<float>&,
    const Vector3<float>&, const Plane3<float>&, const Vector3<float>&,
    const Vector3<float>&, Vector3<float>&, Ellipse2<float>&);

template WM4_FOUNDATION_ITEM
void PerspectiveProjectEllipsoid<double> (const Ellipsoid3<double>&,
    const Vector3<double>&, const Plane3<double>&, const Vector3<double>&,
    const Vector3<double>&, Vector3<double>&, Ellipse2<double>&);
//----------------------------------------------------------------------------
}
