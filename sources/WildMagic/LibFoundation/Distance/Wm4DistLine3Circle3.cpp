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
#include "Wm4DistLine3Circle3.h"
#include "Wm4PolynomialRoots.h"
#include "Wm4DistVector3Circle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistLine3Circle3<Real>::DistLine3Circle3 (const Line3<Real>& rkLine,
    const Circle3<Real>& rkCircle)
    :
    m_pkLine(&rkLine),
    m_pkCircle(&rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistLine3Circle3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle3<Real>& DistLine3Circle3<Real>::GetCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_pkLine->Origin - m_pkCircle->Center;
    Real fDSqrLen = kDiff.SquaredLength();
    Real fMdM = m_pkLine->Direction.SquaredLength();
    Real fDdM = kDiff.Dot(m_pkLine->Direction);
    Real fNdM = m_pkCircle->N.Dot(m_pkLine->Direction);
    Real fDdN = kDiff.Dot(m_pkCircle->N);

    Real fA0 = fDdM;
    Real fA1 = fMdM;
    Real fB0 = fDdM - fNdM*fDdN;
    Real fB1 = fMdM - fNdM*fNdM;
    Real fC0 = fDSqrLen - fDdN*fDdN;
    Real fC1 = fB0;
    Real fC2 = fB1;
    Real fRSqr = m_pkCircle->Radius*m_pkCircle->Radius;

    Real fA0Sqr = fA0*fA0;
    Real fA1Sqr = fA1*fA1;
    Real fTwoA0A1 = ((Real)2.0)*fA0*fA1;
    Real fB0Sqr = fB0*fB0;
    Real fB1Sqr = fB1*fB1;
    Real fTwoB0B1 = ((Real)2.0)*fB0*fB1;
    Real fTwoC1 = ((Real)2.0)*fC1;

    // The minimum point B+t*M occurs when t is a root of the quartic
    // equation whose coefficients are defined below.
    Polynomial1<Real> kPoly(4);
    kPoly[0] = fA0Sqr*fC0 - fB0Sqr*fRSqr;
    kPoly[1] = fTwoA0A1*fC0 + fA0Sqr*fTwoC1 - fTwoB0B1*fRSqr;
    kPoly[2] = fA1Sqr*fC0 + fTwoA0A1*fTwoC1 + fA0Sqr*fC2 - fB1Sqr*fRSqr;
    kPoly[3] = fA1Sqr*fTwoC1 + fTwoA0A1*fC2;
    kPoly[4] = fA1Sqr*fC2;

    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    kPR.FindB(kPoly,6);
    int iCount = kPR.GetCount();
    const Real* afRoot = kPR.GetRoots();

    Real fMinSqrDist = Math<Real>::MAX_REAL;
    for (int i = 0; i < iCount; i++)
    {
        // compute distance from P(t) to circle
        Vector3<Real> kP = m_pkLine->Origin + afRoot[i]*m_pkLine->Direction;
        DistVector3Circle3<Real> kVCDist(kP,*m_pkCircle);
        Real fSqrDist = kVCDist.GetSquared();
        if (fSqrDist < fMinSqrDist)
        {
            fMinSqrDist = fSqrDist;
            m_kClosestPoint0 = kVCDist.GetClosestPoint0();
            m_kClosestPoint1 = kVCDist.GetClosestPoint1();
        }
    }

    return fMinSqrDist;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkLine->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkCircle->Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_pkLine->Direction);
    Circle3<Real> kMCircle(kMCenter,m_pkCircle->U,m_pkCircle->V,
        m_pkCircle->N,m_pkCircle->Radius);
    return DistLine3Circle3<Real>(kMLine,kMCircle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Circle3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkLine->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkCircle->Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_pkLine->Direction);
    Circle3<Real> kMCircle(kMCenter,m_pkCircle->U,m_pkCircle->V,
        m_pkCircle->N,m_pkCircle->Radius);
    return DistLine3Circle3<Real>(kMLine,kMCircle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistLine3Circle3<float>;

template WM4_FOUNDATION_ITEM
class DistLine3Circle3<double>;
//----------------------------------------------------------------------------
}
