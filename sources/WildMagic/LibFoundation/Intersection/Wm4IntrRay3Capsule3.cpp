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
#include "Wm4IntrRay3Capsule3.h"
#include "Wm4DistRay3Segment3.h"
#include "Wm4IntrLine3Capsule3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Capsule3<Real>::IntrRay3Capsule3 (const Ray3<Real>& rkRay,
    const Capsule3<Real>& rkCapsule)
    :
    m_pkRay(&rkRay),
    m_pkCapsule(&rkCapsule)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Capsule3<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrRay3Capsule3<Real>::GetCapsule () const
{
    return *m_pkCapsule;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Capsule3<Real>::Test ()
{
    Real fDist = DistRay3Segment3<Real>(*m_pkRay,m_pkCapsule->Segment).Get();
    return fDist <= m_pkCapsule->Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Capsule3<Real>::Find ()
{
    Real afT[2];
    int iQuantity = IntrLine3Capsule3<Real>::Find(m_pkRay->Origin,
        m_pkRay->Direction,*m_pkCapsule,afT);

    m_iQuantity = 0;
    for (int i = 0; i < iQuantity; i++)
    {
        if (afT[i] >= (Real)0.0)
        {
            m_akPoint[m_iQuantity++] = m_pkRay->Origin +
                afT[i]*m_pkRay->Direction;
        }
    }

    if (m_iQuantity == 2)
    {
        m_iIntersectionType = IT_SEGMENT;
    }
    else if (m_iQuantity == 1)
    {
        m_iIntersectionType = IT_POINT;
    }
    else
    {
        m_iIntersectionType = IT_EMPTY;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay3Capsule3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrRay3Capsule3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrRay3Capsule3<float>;

template WM4_FOUNDATION_ITEM
class IntrRay3Capsule3<double>;
//----------------------------------------------------------------------------
}
