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
#include "Wm4IntrSegment3Ellipsoid3.h"
#include "Wm4Intersector1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment3Ellipsoid3<Real>::IntrSegment3Ellipsoid3 (
    const Segment3<Real>& rkSegment, const Ellipsoid3<Real>& rkEllipsoid)
    :
    m_pkSegment(&rkSegment),
    m_pkEllipsoid(&rkEllipsoid)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& IntrSegment3Ellipsoid3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipsoid3<Real>& IntrSegment3Ellipsoid3<Real>::GetEllipsoid () const
{
    return *m_pkEllipsoid;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Ellipsoid3<Real>::Test ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the segment is X = P+t*D.
    // Substitute the segment equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> kM;
    m_pkEllipsoid->GetM(kM);

    Vector3<Real> kDiff = m_pkSegment->Origin - m_pkEllipsoid->Center;
    Vector3<Real> kMatDir = kM*m_pkSegment->Direction;
    Vector3<Real> kMatDiff = kM*kDiff;
    Real fA2 = m_pkSegment->Direction.Dot(kMatDir);
    Real fA1 = m_pkSegment->Direction.Dot(kMatDiff);
    Real fA0 = kDiff.Dot(kMatDiff) - (Real)1.0;

    // no intersection if Q(t) has no real roots
    Real fDiscr = fA1*fA1 - fA0*fA2;
    if (fDiscr < (Real)0.0)
    {
        return false;
    }

    // test if segment origin is inside ellipsoid
    if (fA0 <= (Real)0.0)
    {
        return true;
    }

    // At this point, Q(0) = fA0 > 0 and Q(t) has real roots.  It is also
    // the case that fA2 > 0, since M is positive definite, implying that
    // D^T*M*D > 0 for any nonzero vector D.

    Real fQ, fQDer, fE = m_pkSegment->Extent;

    if (fA1 >= (Real)0.0)
    {
        // Roots are possible only on [-e,0], e is the segment extent.  At
        // least one root occurs if Q(-e) <= 0 or if Q(-e) > 0 and Q'(-e) < 0.
        fQ = fA0 + fE*(((Real)-2.0)*fA1 + fA2*fE);
        if (fQ <= (Real)0.0)
        {
            return true;
        }

        fQDer = fA1 - fA2*fE;
        if (fQDer < (Real)0.0)
        {
            return true;
        }
    }
    else
    {
        // Roots are only possible on [0,e], e is the segment extent.  At
        // least one root occurs if Q(e) <= 0 or if Q(e) > 0 and Q'(e) > 0.
        fQ = fA0 + fE*(((Real)2.0)*fA1 + fA2*fE);
        if (fQ <= (Real)0.0)
        {
            return true;
        }

        fQDer = fA1 + fA2*fE;
        if (fQDer < (Real)0.0)
        {
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Ellipsoid3<Real>::Find ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the line is X = P+t*D.
    // Substitute the line equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> kM;
    m_pkEllipsoid->GetM(kM);

    Vector3<Real> kDiff = m_pkSegment->Origin - m_pkEllipsoid->Center;
    Vector3<Real> kMatDir = kM*m_pkSegment->Direction;
    Vector3<Real> kMatDiff = kM*kDiff;
    Real fA2 = m_pkSegment->Direction.Dot(kMatDir);
    Real fA1 = m_pkSegment->Direction.Dot(kMatDiff);
    Real fA0 = kDiff.Dot(kMatDiff) - (Real)1.0;

    // intersection occurs if Q(t) has real roots with t >= 0
    Real fDiscr = fA1*fA1 - fA0*fA2;
    Real afT[2];
    if (fDiscr < (Real)0.0)
    {
        m_iIntersectionType = IT_EMPTY;
        m_iQuantity = 0;
    }
    else if (fDiscr > Math<Real>::ZERO_TOLERANCE)
    {
        Real fRoot = Math<Real>::Sqrt(fDiscr);
        Real fInv = ((Real)1.0)/fA2;
        afT[0] = (-fA1 - fRoot)*fInv;
        afT[1] = (-fA1 + fRoot)*fInv;

        // assert: t0 < t1 since fA2 > 0
        Intersector1<Real> kIntr(afT[0],afT[1],-m_pkSegment->Extent,
            m_pkSegment->Extent);
        kIntr.Find();

        m_iQuantity = kIntr.GetQuantity();
        if (m_iQuantity == 2)
        {
            m_iIntersectionType = IT_SEGMENT;
            m_akPoint[0] = m_pkSegment->Origin + kIntr.GetOverlap(0) *
                m_pkSegment->Direction;
            m_akPoint[1] = m_pkSegment->Origin + kIntr.GetOverlap(1) *
                m_pkSegment->Direction;
        }
        else if (m_iQuantity == 1)
        {
            m_iIntersectionType = IT_POINT;
            m_akPoint[0] = m_pkSegment->Origin + kIntr.GetOverlap(0) *
                m_pkSegment->Direction;
        }
        else
        {
            m_iIntersectionType = IT_EMPTY;
        }
    }
    else
    {
        afT[0] = -fA1/fA2;
        if (Math<Real>::FAbs(afT[0]) <= m_pkSegment->Extent)
        {
            m_iIntersectionType = IT_POINT;
            m_iQuantity = 1;
            m_akPoint[0] = m_pkSegment->Origin+afT[0]*m_pkSegment->Direction;
        }
        else
        {
            m_iIntersectionType = IT_EMPTY;
            m_iQuantity = 0;
        }
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment3Ellipsoid3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSegment3Ellipsoid3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrSegment3Ellipsoid3<float>;

template WM4_FOUNDATION_ITEM
class IntrSegment3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
