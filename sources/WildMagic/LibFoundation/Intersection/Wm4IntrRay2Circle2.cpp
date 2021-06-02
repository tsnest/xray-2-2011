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
#include "Wm4IntrRay2Circle2.h"
#include "Wm4IntrLine2Circle2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay2Circle2<Real>::IntrRay2Circle2 (const Ray2<Real>& rkRay,
    const Circle2<Real>& rkCircle)
    :
    m_pkRay(&rkRay),
    m_pkCircle(&rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray2<Real>& IntrRay2Circle2<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrRay2Circle2<Real>::GetCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay2Circle2<Real>::Find ()
{
    Real afT[2];
    bool bIntersects = IntrLine2Circle2<Real>::Find(m_pkRay->Origin,
        m_pkRay->Direction,m_pkCircle->Center,m_pkCircle->Radius,m_iQuantity,
        afT);

    if (bIntersects)
    {
        // reduce root count if line-circle intersections are not on ray
        if (m_iQuantity == 1)
        {
            if (afT[0] < (Real)0.0)
            {
                m_iQuantity = 0;
            }
        }
        else
        {
            if (afT[1] < (Real)0.0)
            {
                m_iQuantity = 0;
            }
            else if (afT[0] < (Real)0.0)
            {
                m_iQuantity = 1;
                afT[0] = afT[1];
            }
        }

        for (int i = 0; i < m_iQuantity; i++)
        {
            m_akPoint[i] = m_pkRay->Origin + afT[i]*m_pkRay->Direction;
        }
    }

    m_iIntersectionType = (m_iQuantity > 0 ? IT_POINT : IT_EMPTY);
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay2Circle2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrRay2Circle2<Real>::GetPoint (int i)
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
class IntrRay2Circle2<float>;

template WM4_FOUNDATION_ITEM
class IntrRay2Circle2<double>;
//----------------------------------------------------------------------------
}
