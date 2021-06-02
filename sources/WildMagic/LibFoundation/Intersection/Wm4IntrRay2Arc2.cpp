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
#include "Wm4IntrRay2Arc2.h"
#include "Wm4IntrLine2Circle2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay2Arc2<Real>::IntrRay2Arc2 (const Ray2<Real>& rkRay,
    const Arc2<Real>& rkArc)
    :
    m_pkRay(&rkRay),
    m_pkArc(&rkArc)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray2<Real>& IntrRay2Arc2<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Arc2<Real>& IntrRay2Arc2<Real>::GetArc () const
{
    return *m_pkArc;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay2Arc2<Real>::Find ()
{
    Real afT[2];
    int iQuantity;
    bool bIntersects = IntrLine2Circle2<Real>::Find(m_pkRay->Origin,
        m_pkRay->Direction,m_pkArc->Center,m_pkArc->Radius,iQuantity,afT);

    m_iQuantity = 0;
    if (bIntersects)
    {
        // reduce root count if line-circle intersections are not on ray
        if (iQuantity == 1)
        {
            if (afT[0] < (Real)0.0)
            {
                iQuantity = 0;
            }
        }
        else
        {
            if (afT[1] < (Real)0.0)
            {
                iQuantity = 0;
            }
            else if (afT[0] < (Real)0.0)
            {
                iQuantity = 1;
                afT[0] = afT[1];
            }
        }

        for (int i = 0; i < iQuantity; i++)
        {
            Vector2<Real> kPoint = m_pkRay->Origin + m_pkRay->Direction*afT[i];
            if (m_pkArc->Contains(kPoint))
            {
                m_akPoint[m_iQuantity++] = kPoint;
            }
        }
    }

    m_iIntersectionType = (m_iQuantity > 0 ? IT_POINT : IT_EMPTY);
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay2Arc2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrRay2Arc2<Real>::GetPoint (int i)
    const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrRay2Arc2<float>;

template WM4_FOUNDATION_ITEM
class IntrRay2Arc2<double>;
//----------------------------------------------------------------------------
}
