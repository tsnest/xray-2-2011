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
#include "Wm4IntrLine3Triangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Triangle3<Real>::IntrLine3Triangle3 (const Line3<Real>& rkLine,
    const Triangle3<Real>& rkTriangle)
    :
    m_pkLine(&rkLine),
    m_pkTriangle(&rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Triangle3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrLine3Triangle3<Real>::GetTriangle () const
{
    return *m_pkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Triangle3<Real>::Test ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_pkLine->Origin - m_pkTriangle->V[0];
    Vector3<Real> kEdge1 = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    Vector3<Real> kEdge2 = m_pkTriangle->V[2] - m_pkTriangle->V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = line direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_pkLine->Direction.Dot(kNormal);
    Real fSign;
    if (fDdN > Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)1.0;
    }
    else if (fDdN < -Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Line and triangle are parallel, call it a "no intersection"
        // even if the line does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_pkLine->Direction.Dot(kDiff.Cross(kEdge2));
    if (fDdQxE2 >= (Real)0.0)
    {
        Real fDdE1xQ = fSign*m_pkLine->Direction.Dot(kEdge1.Cross(kDiff));
        if (fDdE1xQ >= (Real)0.0)
        {
            if (fDdQxE2 + fDdE1xQ <= fDdN)
            {
                // line intersects triangle
                return true;
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Triangle3<Real>::Find ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_pkLine->Origin - m_pkTriangle->V[0];
    Vector3<Real> kEdge1 = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    Vector3<Real> kEdge2 = m_pkTriangle->V[2] - m_pkTriangle->V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = line direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_pkLine->Direction.Dot(kNormal);
    Real fSign;
    if (fDdN > Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)1.0;
    }
    else if (fDdN < -Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Line and triangle are parallel, call it a "no intersection"
        // even if the line does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_pkLine->Direction.Dot(kDiff.Cross(kEdge2));
    if (fDdQxE2 >= (Real)0.0)
    {
        Real fDdE1xQ = fSign*m_pkLine->Direction.Dot(kEdge1.Cross(kDiff));
        if (fDdE1xQ >= (Real)0.0)
        {
            if (fDdQxE2 + fDdE1xQ <= fDdN)
            {
                // line intersects triangle
                Real fQdN = -fSign*kDiff.Dot(kNormal);
                Real fInv = ((Real)1.0)/fDdN;
                m_fLineT = fQdN*fInv;
                m_fTriB1 = fDdQxE2*fInv;
                m_fTriB2 = fDdE1xQ*fInv;
                m_fTriB0 = (Real)1.0 - m_fTriB1 - m_fTriB2;
                return true;
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLine3Triangle3<Real>::GetLineT () const
{
    return m_fLineT;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLine3Triangle3<Real>::GetTriB0 () const
{
    return m_fTriB0;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLine3Triangle3<Real>::GetTriB1 () const
{
    return m_fTriB1;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLine3Triangle3<Real>::GetTriB2 () const
{
    return m_fTriB2;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLine3Triangle3<float>;

template WM4_FOUNDATION_ITEM
class IntrLine3Triangle3<double>;
//----------------------------------------------------------------------------
}
