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
// Version: 4.0.1 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4DistVector3Ellipsoid3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Ellipsoid3<Real>::DistVector3Ellipsoid3 (
    const Vector3<Real>& rkVector, const Ellipsoid3<Real>& rkEllipsoid)
    :
    m_pkVector(&rkVector),
    m_pkEllipsoid(&rkEllipsoid)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Ellipsoid3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipsoid3<Real>& DistVector3Ellipsoid3<Real>::GetEllipsoid ()
    const
{
    return *m_pkEllipsoid;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ellipsoid3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ellipsoid3<Real>::GetSquared ()
{
    // compute coordinates of point in ellipsoid coordinate system
    Vector3<Real> kDiff = *m_pkVector - m_pkEllipsoid->Center;
    Vector3<Real> kEPoint(kDiff.Dot(m_pkEllipsoid->Axis[0]),
        kDiff.Dot(m_pkEllipsoid->Axis[1]),kDiff.Dot(m_pkEllipsoid->Axis[2]));

    const Real* afExtent = m_pkEllipsoid->Extent;
    Real fA2 = afExtent[0]*afExtent[0];
    Real fB2 = afExtent[1]*afExtent[1];
    Real fC2 = afExtent[2]*afExtent[2];
    Real fU2 = kEPoint.X()*kEPoint.X();
    Real fV2 = kEPoint.Y()*kEPoint.Y();
    Real fW2 = kEPoint.Z()*kEPoint.Z();
    Real fA2U2 = fA2*fU2, fB2V2 = fB2*fV2, fC2W2 = fC2*fW2;

    // initial guess
    Real fURatio = kEPoint.X()/afExtent[0];
    Real fVRatio = kEPoint.Y()/afExtent[1];
    Real fWRatio = kEPoint.Z()/afExtent[2];
    Real fT;
    if (fURatio*fURatio+fVRatio*fVRatio+fWRatio*fWRatio < (Real)1.0)
    {
        fT = (Real)0.0;
    }
    else
    {
        Real fMax = afExtent[0];
        if (afExtent[1] > fMax)
        {
            fMax = afExtent[1];
        }
        if (afExtent[2] > fMax)
        {
            fMax = afExtent[2];
        }

        fT = fMax*kEPoint.Length();
    }

    // Newton's method
    const int iMaxIteration = 64;
    Real fP = (Real)1.0, fQ = (Real)1.0, fR = (Real)1.0;
    for (int i = 0; i < iMaxIteration; i++)
    {
        fP = fT+fA2;
        fQ = fT+fB2;
        fR = fT+fC2;
        Real fP2 = fP*fP;
        Real fQ2 = fQ*fQ;
        Real fR2 = fR*fR;
        Real fS = fP2*fQ2*fR2-fA2U2*fQ2*fR2-fB2V2*fP2*fR2-fC2W2*fP2*fQ2;
        if (Math<Real>::FAbs(fS) < Math<Real>::ZERO_TOLERANCE)
        {
            break;
        }

        Real fPQ = fP*fQ, fPR = fP*fR, fQR = fQ*fR, fPQR = fP*fQ*fR;
        Real fDS = ((Real)2.0)*(fPQR*(fQR+fPR+fPQ)-fA2U2*fQR*(fQ+fR)-
            fB2V2*fPR*(fP+fR)-fC2W2*fPQ*(fP+fQ));
        fT -= fS/fDS;
    }

    Vector3<Real> kClosest(fA2*kEPoint.X()/fP,fB2*kEPoint.Y()/fQ,
        fC2*kEPoint.Z()/fR);
    kDiff = kClosest - kEPoint;
    Real fSqrDistance = kDiff.SquaredLength();

    m_kClosestPoint0 = *m_pkVector;
    m_kClosestPoint1 = m_pkEllipsoid->Center +
        kClosest.X()*m_pkEllipsoid->Axis[0] +
        kClosest.Y()*m_pkEllipsoid->Axis[1] +
        kClosest.Z()*m_pkEllipsoid->Axis[2];

    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ellipsoid3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkEllipsoid->Center + fT*rkVelocity1;
    Ellipsoid3<Real> kMEllipsoid(kMCenter,m_pkEllipsoid->Axis,
        m_pkEllipsoid->Extent);
    return DistVector3Ellipsoid3<Real>(kMVector,kMEllipsoid).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Ellipsoid3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkEllipsoid->Center + fT*rkVelocity1;
    Ellipsoid3<Real> kMEllipsoid(kMCenter,m_pkEllipsoid->Axis,
        m_pkEllipsoid->Extent);
    return DistVector3Ellipsoid3<Real>(kMVector,kMEllipsoid).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Ellipsoid3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
