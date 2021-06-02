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
#include "Wm4IntrLinComp2LinComp2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLinComp2LinComp2<Real>::IntrLinComp2LinComp2 (
    const LinComp2<Real>& rkComponent0, const LinComp2<Real>& rkComponent1)
    :
    m_pkComponent0(&rkComponent0),
    m_pkComponent1(&rkComponent1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const LinComp2<Real>& IntrLinComp2LinComp2<Real>::GetComponent0 () const
{
    return *m_pkComponent0;
}
//----------------------------------------------------------------------------
template <class Real>
const LinComp2<Real>& IntrLinComp2LinComp2<Real>::GetComponent1 () const
{
    return *m_pkComponent1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLinComp2LinComp2<Real>::Test ()
{
    Vector2<Real> kDiff;
    m_iIntersectionType = Classify(*m_pkComponent0,*m_pkComponent1,
        m_afParameter,&kDiff);

    if (m_iIntersectionType == IT_POINT)
    {
        // The lines of the components intersect in a single point.  Test if
        // that point is on both components.
        if (!m_pkComponent0->Contains(m_afParameter[0])
        ||  !m_pkComponent1->Contains(m_afParameter[1]))
        {
            m_iIntersectionType = IT_EMPTY;
        }
    }
    else if (m_iIntersectionType == IT_LINE)
    {
        // The components are on the same line.  Test for overlap of the
        // components.
        Real fOrigin1 = kDiff.Dot(m_pkComponent0->Direction);
        Real fSign = m_pkComponent0->Direction.Dot(m_pkComponent1->Direction);
        Real fMin1, fMax1;
        if (fSign > (Real)0.0)
        {
            // components are in the same direction
            fMin1 = fOrigin1 + m_pkComponent1->GetMin();
            fMax1 = fOrigin1 + m_pkComponent1->GetMax();
        }
        else
        {
            // components are in opposite directions
            fMin1 = fOrigin1 - m_pkComponent1->GetMax();
            fMax1 = fOrigin1 - m_pkComponent1->GetMin();
        }

        Intersector1<Real> kIntr(m_pkComponent0->GetMin(),
            m_pkComponent0->GetMax(),fMin1,fMax1);

        kIntr.Find();

        int iQuantity = kIntr.GetQuantity();
        if (iQuantity == 2)
        {
            m_iIntersectionType = LinComp2<Real>::GetTypeFromInterval(
                kIntr.GetOverlap(0),kIntr.GetOverlap(1));
        }
        else if (iQuantity == 1)
        {
            m_iIntersectionType = IT_POINT;
        }
        else
        {
            m_iIntersectionType = IT_EMPTY;
        }
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLinComp2LinComp2<Real>::Find ()
{
    Vector2<Real> kDiff;
    m_iIntersectionType = Classify(*m_pkComponent0,*m_pkComponent1,
        m_afParameter,&kDiff);

    if (m_iIntersectionType == IT_POINT)
    {
        // The lines of the components intersect in a single point.  Test if
        // that point is on both components.
        if (m_pkComponent0->Contains(m_afParameter[0])
        &&  m_pkComponent1->Contains(m_afParameter[1]))
        {
            // point
            m_kIntrComponent.Origin = m_pkComponent0->Origin;
            m_kIntrComponent.Direction = m_pkComponent0->Direction;
            m_kIntrComponent.SetInterval(m_afParameter[0],m_afParameter[0]);
        }
        else
        {
            // empty set
            m_kIntrComponent.SetInterval((Real)1.0,(Real)-1.0);
        }
    }
    else if (m_iIntersectionType == IT_LINE)
    {
        // The components are on the same line.  Test for overlap of the
        // components.
        Real fOrigin1 = kDiff.Dot(m_pkComponent0->Direction);
        Real fSign = m_pkComponent0->Direction.Dot(m_pkComponent1->Direction);
        Real fMin1, fMax1;
        if (fSign > (Real)0.0)
        {
            // components are in the same direction
            fMin1 = fOrigin1 + m_pkComponent1->GetMin();
            fMax1 = fOrigin1 + m_pkComponent1->GetMax();
        }
        else
        {
            // components are in opposite directions
            fMin1 = fOrigin1 - m_pkComponent1->GetMax();
            fMax1 = fOrigin1 - m_pkComponent1->GetMin();
        }

        Intersector1<Real> kIntr(m_pkComponent0->GetMin(),
            m_pkComponent0->GetMax(),fMin1,fMax1);

        kIntr.Find();

        int iQuantity = kIntr.GetQuantity();
        if (iQuantity == 2)
        {
            // line, ray, or segment
            m_kIntrComponent.Origin = m_pkComponent0->Origin;
            m_kIntrComponent.Direction = m_pkComponent0->Direction;
            m_kIntrComponent.SetInterval(kIntr.GetOverlap(0),
                kIntr.GetOverlap(1));
        }
        else if (iQuantity == 1)
        {
            // point
            m_kIntrComponent.Origin = m_pkComponent0->Origin;
            m_kIntrComponent.Direction = m_pkComponent0->Direction;
            m_kIntrComponent.SetInterval(m_afParameter[0],m_afParameter[0]);
        }
        else
        {
            // empty set
            m_kIntrComponent.SetInterval((Real)1.0,(Real)-1.0);
        }
    }

    m_kIntrComponent.MakeCanonical();
    m_iIntersectionType = m_kIntrComponent.GetType();
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
const LinComp2<Real>&
IntrLinComp2LinComp2<Real>::GetIntersectionComponent () const
{
    return m_kIntrComponent;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLinComp2LinComp2<Real>::GetParameter0 () const
{
    return m_afParameter[0];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLinComp2LinComp2<Real>::GetParameter1 () const
{
    return m_afParameter[1];
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLinComp2LinComp2<Real>::Classify (const LinComp2<Real>& rkComponent0,
    const LinComp2<Real>& rkComponent1, Real* afS, Vector2<Real>* pkDiff,
    Vector2<Real>* pkDiffN)
{
    // The intersection of two lines is a solution to P0+s0*D0 = P1+s1*D1.
    // Rewrite this as s0*D0 - s1*D1 = P1 - P0 = Q.  If D0.Dot(Perp(D1)) = 0,
    // the lines are parallel.  Additionally, if Q.Dot(Perp(D1)) = 0, the
    // lines are the same.  If D0.Dot(Perp(D1)) is not zero, then
    //   s0 = Q.Dot(Perp(D1))/D0.Dot(Perp(D1))
    // produces the point of intersection.  Also,
    //   s1 = Q.Dot(Perp(D0))/D0.Dot(Perp(D1))

    Vector2<Real> kDiff = rkComponent1.Origin - rkComponent0.Origin;
    if (pkDiff)
    {
        *pkDiff = kDiff;
    }

    Real fD0DotPerpD1 =
        rkComponent0.Direction.DotPerp(rkComponent1.Direction);
    if (Math<Real>::FAbs(fD0DotPerpD1) > Math<Real>::ZERO_TOLERANCE)
    {
        // lines intersect in a single point
        if (afS)
        {
            Real fInvD0DotPerpD1 = ((Real)1.0)/fD0DotPerpD1;
            Real fDiffDotPerpD0 = kDiff.DotPerp(rkComponent0.Direction);
            Real fDiffDotPerpD1 = kDiff.DotPerp(rkComponent1.Direction);
            afS[0] = fDiffDotPerpD1*fInvD0DotPerpD1;
            afS[1] = fDiffDotPerpD0*fInvD0DotPerpD1;
        }
        return IT_POINT;
    }

    // lines are parallel
    kDiff.Normalize();
    if (pkDiffN)
    {
        *pkDiffN = kDiff;
    }

    Real fDiffNDotPerpD1 = kDiff.DotPerp(rkComponent1.Direction);
    if (Math<Real>::FAbs(fDiffNDotPerpD1) <= Math<Real>::ZERO_TOLERANCE)
    {
        // lines are colinear
        return IT_LINE;
    }

    // lines are parallel, but distinct
    return IT_EMPTY;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLinComp2LinComp2<float>;

template WM4_FOUNDATION_ITEM
class IntrLinComp2LinComp2<double>;
//----------------------------------------------------------------------------
}
