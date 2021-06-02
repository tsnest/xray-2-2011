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
#include "Wm4IntrArc2Arc2.h"
#include "Wm4IntrCircle2Circle2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrArc2Arc2<Real>::IntrArc2Arc2 (const Arc2<Real>& rkArc0,
    const Arc2<Real>& rkArc1)
    :
    m_pkArc0(&rkArc0),
    m_pkArc1(&rkArc1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetArc0 () const
{
    return *m_pkArc0;
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetArc1 () const
{
    return *m_pkArc1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrArc2Arc2<Real>::Find ()
{
    m_iQuantity = 0;

    Circle2<Real> kCircle0(m_pkArc0->Center,m_pkArc0->Radius);
    Circle2<Real> kCircle1(m_pkArc1->Center,m_pkArc1->Radius);
    IntrCircle2Circle2<Real> kIntr(kCircle0,kCircle1);
    if (!kIntr.Find())
    {
        // arcs do not intersect
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    if (kIntr.GetIntersectionType() == IT_OTHER)
    {
        // Arcs are cocircular.  Determine if they overlap.  Let arc0 be
        // <A0,A1> and arc1 be <B0,B1>, the points ordered counterclockwise
        // around the circle of the arc.
        if (m_pkArc1->Contains(m_pkArc0->End0))
        {
            if (m_pkArc1->Contains(m_pkArc0->End1))
            {
                // arc0 inside arc 1, <B0,A0,A1,B1>
                m_iIntersectionType = IT_OTHER;
                m_kIntersectionArc = *m_pkArc0;
            }
            else
            {
                if (m_pkArc0->End0 != m_pkArc1->End1)
                {
                    // arc0 and arc1 overlap, <B0,A0,B1,A1>
                    m_iIntersectionType = IT_OTHER;
                    m_kIntersectionArc.Center = m_pkArc0->Center;
                    m_kIntersectionArc.Radius = m_pkArc0->Radius;
                    m_kIntersectionArc.End0 = m_pkArc0->End0;
                    m_kIntersectionArc.End1 = m_pkArc1->End1;
                }
                else
                {
                    // arc0 and arc1 share end point, <B0,A0,B1,A1>, A0 = B1
                    m_iIntersectionType = IT_POINT;
                    m_iQuantity = 1;
                    m_akPoint[0] = m_pkArc0->End0;
                }
            }
            return true;
        }

        if (m_pkArc1->Contains(m_pkArc0->End1))
        {
            if (m_pkArc0->End1 != m_pkArc1->End0)
            {
                // arc0 and arc1 overlap, <A0,B0,A1,B1>
                m_iIntersectionType = IT_OTHER;
                m_kIntersectionArc.Center = m_pkArc0->Center;
                m_kIntersectionArc.Radius = m_pkArc0->Radius;
                m_kIntersectionArc.End0 = m_pkArc1->End0;
                m_kIntersectionArc.End1 = m_pkArc0->End1;
            }
            else
            {
                // arc0 and arc1 share end point, <A0,B0,A1,B1>, B0 = A1
                m_iIntersectionType = IT_POINT;
                m_iQuantity = 1;
                m_akPoint[0] = m_pkArc1->End0;
            }
            return true;
        }

        if (m_pkArc0->Contains(m_pkArc1->End0))
        {
            // arc1 inside arc0, <A0,B0,B1,A1>
            m_iIntersectionType = IT_OTHER;
            m_kIntersectionArc = *m_pkArc1;
            return true;
        }
        else
        {
            // arcs do not overlap, <A0,A1,B0,B1>
            m_iIntersectionType = IT_EMPTY;
            return false;
        }
    }

    // test if circle-circle intersection points are on the arcs
    for (int i = 0; i < kIntr.GetQuantity(); i++)
    {
        if (m_pkArc0->Contains(kIntr.GetPoint(i))
        &&  m_pkArc1->Contains(kIntr.GetPoint(i)))
        {
            m_akPoint[m_iQuantity++] = kIntr.GetPoint(i);
        }
    }

    m_iIntersectionType = (m_iQuantity > 0 ? IT_POINT : IT_EMPTY);
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrArc2Arc2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrArc2Arc2<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetIntersectionArc () const
{
    return m_kIntersectionArc;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrArc2Arc2<float>;

template WM4_FOUNDATION_ITEM
class IntrArc2Arc2<double>;
//----------------------------------------------------------------------------
}
