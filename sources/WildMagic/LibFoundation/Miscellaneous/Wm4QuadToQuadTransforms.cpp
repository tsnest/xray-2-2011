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
#include "Wm4QuadToQuadTransforms.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
HmQuadToSqr<Real>::HmQuadToSqr (const Vector2<Real>& rkP00,
    const Vector2<Real>& rkP10, const Vector2<Real>& rkP11,
    const Vector2<Real>& rkP01)
{
    // translate to origin
    m_kT = rkP00;
    Vector2<Real> kQ10 = rkP10 - rkP00;
    Vector2<Real> kQ11 = rkP11 - rkP00;
    Vector2<Real> kQ01 = rkP01 - rkP00;

    Matrix2<Real> kInvM(kQ10.X(),kQ01.X(),kQ10.Y(),kQ01.Y());
    m_kM = kInvM.Inverse();

    // compute where p11 is mapped to
    Vector2<Real> kCorner = m_kM*kQ11;  // = (a,b)

    // Compute homogeneous transform of quadrilateral
    // {(0,0),(1,0),(a,b),(0,1)} to square {(0,0),(1,0),(1,1),(0,1)}
    m_kG.X() = (kCorner.Y() - (Real)1.0)/kCorner.X();
    m_kG.Y() = (kCorner.X() - (Real)1.0)/kCorner.Y();
    m_kD.X() = (Real)1.0 + m_kG.X();
    m_kD.Y() = (Real)1.0 + m_kG.Y();
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> HmQuadToSqr<Real>::Transform (const Vector2<Real>& rkP)
{
    Vector2<Real> kProd = m_kM*(rkP - m_kT);
    Real fInvDenom = ((Real)1.0)/((Real)1.0 + m_kG.Dot(kProd));
    Vector2<Real> kResult = fInvDenom*kProd;
    kResult.X() *= m_kD.X();
    kResult.Y() *= m_kD.Y();
    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
HmSqrToQuad<Real>::HmSqrToQuad (const Vector2<Real>& rkP00,
    const Vector2<Real>& rkP10, const Vector2<Real>& rkP11,
    const Vector2<Real>& rkP01)
{
    // translate to origin
    m_kT = rkP00;
    m_kM[0][0] = rkP10.X() - rkP00.X();
    m_kM[0][1] = rkP01.X() - rkP00.X();
    m_kM[1][0] = rkP10.Y() - rkP00.Y();
    m_kM[1][1] = rkP01.Y() - rkP00.Y();

    Matrix2<Real> kInvM = m_kM.Inverse();

    // find point which is mapped to p11
    Vector2<Real> kCorner = kInvM*(rkP11-rkP00);  // = (a,b)

    // compute homogeneous transform of square {(0,0),(1,0),(1,1),(0,1)} to
    // quadrilateral {(0,0),(1,0),(a,b),(0,1)}
    Real fInvDenom = ((Real)1.0)/(kCorner.X() + kCorner.Y() - (Real)1.0);
    m_kG.X() = fInvDenom*((Real)1.0 - kCorner.Y());
    m_kG.Y() = fInvDenom*((Real)1.0 - kCorner.X());
    m_kD.X() = fInvDenom*kCorner.X();
    m_kD.Y() = fInvDenom*kCorner.Y();
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> HmSqrToQuad<Real>::Transform (const Vector2<Real>& rkP)
{
    Real fInvDenom = ((Real)1.0)/((Real)1.0 + m_kG.Dot(rkP));
    Vector2<Real> kResult(m_kD.X()*rkP.X(),m_kD.Y()*rkP.Y());
    Vector2<Real> kProd = m_kM*kResult;
    kResult.X() = fInvDenom*kProd.X() + m_kT.X();
    kResult.Y() = fInvDenom*kProd.Y() + m_kT.Y();
    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
BiQuadToSqr<Real>::BiQuadToSqr (const Vector2<Real>& rkP00,
    const Vector2<Real>& rkP10, const Vector2<Real>& rkP11,
    const Vector2<Real>& rkP01)
    :
    m_kP00(rkP00)
{
    m_kB = rkP10 - rkP00;
    m_kC = rkP01 - rkP00;
    m_kD = rkP11 + rkP00 - rkP10 - rkP01;
    m_fBC = m_kB.DotPerp(m_kC);
    m_fBD = m_kB.DotPerp(m_kD);
    m_fCD = m_kC.DotPerp(m_kD);
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> BiQuadToSqr<Real>::Transform (const Vector2<Real>& rkP)
{
    Vector2<Real> kA = m_kP00 - rkP;
    Real fAB = kA.DotPerp(m_kB);
    Real fAC = kA.DotPerp(m_kC);

    // 0 = ac*bc+(bc^2+ac*bd-ab*cd)*s+bc*bd*s^2 = k0 + k1*s + k2*s^2
    Real fK0 = fAC*m_fBC;
    Real fK1 = m_fBC*m_fBC + fAC*m_fBD - fAB*m_fCD;
    Real fK2 = m_fBC*m_fBD;

    if (Math<Real>::FAbs(fK2) >= Math<Real>::ZERO_TOLERANCE)
    {
        // s-equation is quadratic
        Real fInv = ((Real)0.5)/fK2;
        Real fDiscr = fK1*fK1 - ((Real)4.0)*fK0*fK2;
        Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));

        Vector2<Real> kResult0;
        kResult0.X() = (-fK1 - fRoot)*fInv;
        kResult0.Y() = fAB/(m_fBC + m_fBD*kResult0.X());
        Real fDeviation0 = Deviation(kResult0);
        if (fDeviation0 == (Real)0.0)
        {
            return kResult0;
        }

        Vector2<Real> kResult1;
        kResult1.X() = (-fK1 + fRoot)*fInv;
        kResult1.Y() = fAB/(m_fBC + m_fBD*kResult1.X());
        Real fDeviation1 = Deviation(kResult1);
        if (fDeviation1 == (Real)0.0)
        {
            return kResult1;
        }

        if (fDeviation0 <= fDeviation1)
        {
            if (fDeviation0 <= Math<Real>::ZERO_TOLERANCE)
            {
                return kResult0;
            }
        }
        else
        {
            if (fDeviation1 <= Math<Real>::ZERO_TOLERANCE)
            {
                return kResult1;
            }
        }
    }
    else
    {
        // s-equation is linear
        Vector2<Real> kResult;

        kResult.X() = -fK0/fK1;
        kResult.Y() = fAB/(m_fBC + m_fBD*kResult.X());
        Real fDeviation = Deviation(kResult);
        if (fDeviation <= Math<Real>::ZERO_TOLERANCE)
        {
            return kResult;
        }
    }

    // point is outside the quadrilateral, return invalid
    return Vector2<Real>(Math<Real>::MAX_REAL,Math<Real>::MAX_REAL);
}
//----------------------------------------------------------------------------
template <class Real>
Real BiQuadToSqr<Real>::Deviation (const Vector2<Real>& rkSPoint)
{
    // deviation is the squared distance of the point from the unit square
    Real fDeviation = (Real)0.0;
    Real fDelta;

    if (rkSPoint.X() < (Real)0.0)
    {
        fDeviation += rkSPoint.X()*rkSPoint.X();
    }
    else if (rkSPoint.X() > (Real)1.0)
    {
        fDelta = rkSPoint.X() - (Real)1.0;
        fDeviation += fDelta*fDelta;
    }

    if (rkSPoint.Y() < (Real)0.0)
    {
        fDeviation += rkSPoint.Y()*rkSPoint.Y();
    }
    else if (rkSPoint.Y() > (Real)1.0)
    {
        fDelta = rkSPoint.Y() - (Real)1.0;
        fDeviation += fDelta*fDelta;
    }

    return fDeviation;
}
//----------------------------------------------------------------------------
template <class Real>
BiSqrToQuad<Real>::BiSqrToQuad (const Vector2<Real>& rkP00,
    const Vector2<Real>& rkP10, const Vector2<Real>& rkP11,
    const Vector2<Real>& rkP01)
{
    m_kS00 = rkP00;
    m_kS10 = rkP10;
    m_kS11 = rkP11;
    m_kS01 = rkP01;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> BiSqrToQuad<Real>::Transform (const Vector2<Real>& rkP)
{
    Vector2<Real> kOmP((Real)1.0-rkP.X(),(Real)1.0-rkP.Y());
    Vector2<Real> kResult;
    kResult.X() = kOmP.Y()*(kOmP.X()*m_kS00.X() + rkP.X()*m_kS10.X()) +
        rkP.Y()*(kOmP.X()*m_kS01.X() + rkP.X()*m_kS11.X());
    kResult.Y() = kOmP.Y()*(kOmP.X()*m_kS00.Y() + rkP.X()*m_kS10.Y()) +
        rkP.Y()*(kOmP.X()*m_kS01.Y() + rkP.X()*m_kS11.Y());
    return kResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class HmQuadToSqr<float>;

template WM4_FOUNDATION_ITEM
class HmSqrToQuad<float>;

template WM4_FOUNDATION_ITEM
class BiQuadToSqr<float>;

template WM4_FOUNDATION_ITEM
class BiSqrToQuad<float>;

template WM4_FOUNDATION_ITEM
class HmQuadToSqr<double>;

template WM4_FOUNDATION_ITEM
class HmSqrToQuad<double>;

template WM4_FOUNDATION_ITEM
class BiQuadToSqr<double>;

template WM4_FOUNDATION_ITEM
class BiSqrToQuad<double>;
//----------------------------------------------------------------------------
}
