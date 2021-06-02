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
#include "Wm4IntrArc2Circle2.h"
#include "Wm4IntrCircle2Circle2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrArc2Circle2<Real>::IntrArc2Circle2 (const Arc2<Real>& rkArc,
    const Circle2<Real>& rkCircle)
    :
    m_pkArc(&rkArc),
    m_pkCircle(&rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Circle2<Real>::GetArc () const
{
    return *m_pkArc;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrArc2Circle2<Real>::GetCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrArc2Circle2<Real>::Find ()
{
    m_iQuantity = 0;

    Circle2<Real> kCircleOfArc(m_pkArc->Center,m_pkArc->Radius);
    IntrCircle2Circle2<Real> kIntr(*m_pkCircle,kCircleOfArc);
    if (!kIntr.Find())
    {
        // arc and circle do not intersect
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    if (kIntr.GetIntersectionType() == IT_OTHER)
    {
        // arc is on the circle
        m_iIntersectionType = IT_OTHER;
        return true;
    }

    // test if circle-circle intersection points are on the arc
    for (int i = 0; i < kIntr.GetQuantity(); i++)
    {
        if (m_pkArc->Contains(kIntr.GetPoint(i)))
        {
            m_akPoint[m_iQuantity++] = kIntr.GetPoint(i);
        }
    }

    m_iIntersectionType = (m_iQuantity > 0 ? IT_POINT : IT_EMPTY);
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrArc2Circle2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrArc2Circle2<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrArc2Circle2<Real>::GetIntersectionArc () const
{
    return *m_pkArc;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrArc2Circle2<float>;

template WM4_FOUNDATION_ITEM
class IntrArc2Circle2<double>;
//----------------------------------------------------------------------------
}
