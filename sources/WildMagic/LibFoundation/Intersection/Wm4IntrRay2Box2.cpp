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
#include "Wm4IntrRay2Box2.h"
#include "Wm4IntrLine2Box2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay2Box2<Real>::IntrRay2Box2 (const Ray2<Real>& rkRay,
    const Box2<Real>& rkBox)
    :
    m_pkRay(&rkRay),
    m_pkBox(&rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray2<Real>& IntrRay2Box2<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Box2<Real>& IntrRay2Box2<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay2Box2<Real>::Test ()
{
    Real fWdU[2], fAWdU[2], fDdU[2], fADdU[2], fRhs;

    Vector2<Real> kDiff = m_pkRay->Origin - m_pkBox->Center;

    fWdU[0] = m_pkRay->Direction.Dot(m_pkBox->Axis[0]);
    fAWdU[0] = Math<Real>::FAbs(fWdU[0]);
    fDdU[0] = kDiff.Dot(m_pkBox->Axis[0]);
    fADdU[0] = Math<Real>::FAbs(fDdU[0]);
    if (fADdU[0] > m_pkBox->Extent[0] && fDdU[0]*fWdU[0] >= 0.0f)
    {
        return false;
    }

    fWdU[1] = m_pkRay->Direction.Dot(m_pkBox->Axis[1]);
    fAWdU[1] = Math<Real>::FAbs(fWdU[1]);
    fDdU[1] = kDiff.Dot(m_pkBox->Axis[1]);
    fADdU[1] = Math<Real>::FAbs(fDdU[1]);
    if (fADdU[1] > m_pkBox->Extent[1] && fDdU[1]*fWdU[1] >= 0.0f)
    {
        return false;
    }

    Vector2<Real> kPerp = m_pkRay->Direction.Perp();
    Real fLhs = Math<Real>::FAbs(kPerp.Dot(kDiff));
    Real fPart0 = Math<Real>::FAbs(kPerp.Dot(m_pkBox->Axis[0]));
    Real fPart1 = Math<Real>::FAbs(kPerp.Dot(m_pkBox->Axis[1]));
    fRhs = m_pkBox->Extent[0]*fPart0 + m_pkBox->Extent[1]*fPart1;
    return fLhs <= fRhs;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay2Box2<Real>::Find ()
{
    Real fT0 = (Real)0.0, fT1 = Math<Real>::MAX_REAL;
    return IntrLine2Box2<Real>::DoClipping(fT0,fT1,m_pkRay->Origin,
        m_pkRay->Direction,*m_pkBox,true,m_iQuantity,m_akPoint,
        m_iIntersectionType);
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay2Box2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrRay2Box2<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrRay2Box2<float>;

template WM4_FOUNDATION_ITEM
class IntrRay2Box2<double>;
//----------------------------------------------------------------------------
}
