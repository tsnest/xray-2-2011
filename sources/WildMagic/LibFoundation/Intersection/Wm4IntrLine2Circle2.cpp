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
#include "Wm4IntrLine2Circle2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine2Circle2<Real>::IntrLine2Circle2 (const Line2<Real>& rkLine,
    const Circle2<Real>& rkCircle)
    :
    m_pkLine(&rkLine),
    m_pkCircle(&rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line2<Real>& IntrLine2Circle2<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrLine2Circle2<Real>::GetCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine2Circle2<Real>::Find ()
{
    Real afT[2];
    bool bIntersects = Find(m_pkLine->Origin,m_pkLine->Direction,
        m_pkCircle->Center,m_pkCircle->Radius,m_iQuantity,afT);

    if (bIntersects)
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            m_akPoint[i] = m_pkLine->Origin + afT[i]*m_pkLine->Direction;
        }
    }

    m_iIntersectionType = (m_iQuantity > 0 ? IT_POINT : IT_EMPTY);
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine2Circle2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrLine2Circle2<Real>::GetPoint (int i)
    const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine2Circle2<Real>::Find (const Vector2<Real>& rkOrigin,
    const Vector2<Real>& rkDirection, const Vector2<Real>& rkCenter,
    Real fRadius, int& riRootCount, Real afT[2])
{
    // Intersection of a the line P+t*D and the circle |X-C| = R.  The line
    // direction is unit length. The t value is a root to the quadratic
    // equation:
    //   0 = |t*D+P-C|^2 - R^2
    //     = t^2 + 2*Dot(D,P-C)*t + |P-C|^2-R^2
    //     = t^2 + 2*a1*t + a0
    // If two roots are returned, the order is T[0] < T[1].

    Vector2<Real> kDiff = rkOrigin - rkCenter;
    Real fA0 = kDiff.SquaredLength() - fRadius*fRadius;
    Real fA1 = rkDirection.Dot(kDiff);
    Real fDiscr = fA1*fA1 - fA0;
    if (fDiscr > Math<Real>::ZERO_TOLERANCE)
    {
        riRootCount = 2;
        fDiscr = Math<Real>::Sqrt(fDiscr);
        afT[0] = -fA1 - fDiscr;
        afT[1] = -fA1 + fDiscr;
    }
    else if (fDiscr < -Math<Real>::ZERO_TOLERANCE)
    {
        riRootCount = 0;
    }
    else  // fDiscr == 0
    {
        riRootCount = 1;
        afT[0] = -fA1;
    }

    return riRootCount != 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLine2Circle2<float>;

template WM4_FOUNDATION_ITEM
class IntrLine2Circle2<double>;
//----------------------------------------------------------------------------
}
