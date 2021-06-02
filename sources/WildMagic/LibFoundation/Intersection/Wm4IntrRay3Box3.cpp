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
#include "Wm4IntrRay3Box3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Box3<Real>::IntrRay3Box3 (const Ray3<Real>& rkRay,
    const Box3<Real>& rkBox)
    :
    m_pkRay(&rkRay),
    m_pkBox(&rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Box3<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrRay3Box3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Box3<Real>::Test ()
{
    Real afWdU[3], afAWdU[3], afDdU[3], afADdU[3], afAWxDdU[3], fRhs;

    Vector3<Real> kDiff = m_pkRay->Origin - m_pkBox->Center;

    afWdU[0] = m_pkRay->Direction.Dot(m_pkBox->Axis[0]);
    afAWdU[0] = Math<Real>::FAbs(afWdU[0]);
    afDdU[0] = kDiff.Dot(m_pkBox->Axis[0]);
    afADdU[0] = Math<Real>::FAbs(afDdU[0]);
    if (afADdU[0] > m_pkBox->Extent[0] && afDdU[0]*afWdU[0] >= (Real)0.0)
    {
        return false;
    }

    afWdU[1] = m_pkRay->Direction.Dot(m_pkBox->Axis[1]);
    afAWdU[1] = Math<Real>::FAbs(afWdU[1]);
    afDdU[1] = kDiff.Dot(m_pkBox->Axis[1]);
    afADdU[1] = Math<Real>::FAbs(afDdU[1]);
    if (afADdU[1] > m_pkBox->Extent[1] && afDdU[1]*afWdU[1] >= (Real)0.0)
    {
        return false;
    }

    afWdU[2] = m_pkRay->Direction.Dot(m_pkBox->Axis[2]);
    afAWdU[2] = Math<Real>::FAbs(afWdU[2]);
    afDdU[2] = kDiff.Dot(m_pkBox->Axis[2]);
    afADdU[2] = Math<Real>::FAbs(afDdU[2]);
    if (afADdU[2] > m_pkBox->Extent[2] && afDdU[2]*afWdU[2] >= (Real)0.0)
    {
        return false;
    }

    Vector3<Real> kWxD = m_pkRay->Direction.Cross(kDiff);

    afAWxDdU[0] = Math<Real>::FAbs(kWxD.Dot(m_pkBox->Axis[0]));
    fRhs = m_pkBox->Extent[1]*afAWdU[2] + m_pkBox->Extent[2]*afAWdU[1];
    if (afAWxDdU[0] > fRhs)
    {
        return false;
    }

    afAWxDdU[1] = Math<Real>::FAbs(kWxD.Dot(m_pkBox->Axis[1]));
    fRhs = m_pkBox->Extent[0]*afAWdU[2] + m_pkBox->Extent[2]*afAWdU[0];
    if (afAWxDdU[1] > fRhs)
    {
        return false;
    }

    afAWxDdU[2] = Math<Real>::FAbs(kWxD.Dot(m_pkBox->Axis[2]));
    fRhs = m_pkBox->Extent[0]*afAWdU[1] + m_pkBox->Extent[1]*afAWdU[0];
    if (afAWxDdU[2] > fRhs)
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Box3<Real>::Find ()
{
    Real fT0 = (Real)0.0, fT1 = Math<Real>::MAX_REAL;
    return IntrLine3Box3<Real>::DoClipping(fT0,fT1,m_pkRay->Origin,
        m_pkRay->Direction,*m_pkBox,true,m_iQuantity,m_akPoint,
        m_iIntersectionType);
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay3Box3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrRay3Box3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrRay3Box3<float>;

template WM4_FOUNDATION_ITEM
class IntrRay3Box3<double>;
//----------------------------------------------------------------------------
}
