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
// Version: 4.7.0 (2008/09/15)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrHalfspace3Box3.h"
#include "Wm4IntrUtility3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrHalfspace3Box3<Real>::IntrHalfspace3Box3 (
    const Plane3<Real>& rkHalfspace, const Box3<Real>& rkBox)
    :
    m_pkHalfspace(&rkHalfspace),
    m_pkBox(&rkBox)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrHalfspace3Box3<Real>::GetHalfspace () const
{
    return *m_pkHalfspace;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrHalfspace3Box3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrHalfspace3Box3<Real>::Test ()
{
    Real fMin, fMax;
    IntrAxis<Real>::GetProjection(m_pkHalfspace->Normal,*m_pkBox,fMin,fMax);
    return fMin <= m_pkHalfspace->Constant;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrHalfspace3Box3<Real>::Test (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    m_fContactTime = (Real)0;
    Real fTLast = Math<Real>::MAX_REAL;
    Vector3<Real> kVelocity = rkVelocity1 - rkVelocity0;

    Real fMin, fMax;
    IntrAxis<Real>::GetProjection(m_pkHalfspace->Normal,*m_pkBox,fMin,fMax);

    return IntrAxis<Real>::Test(m_pkHalfspace->Normal,kVelocity,
        -Math<Real>::MAX_REAL,m_pkHalfspace->Constant,fMin,fMax,fTMax,
        m_fContactTime,fTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrHalfspace3Box3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    m_fContactTime = (Real)0;
    Real fTLast = Math<Real>::MAX_REAL;
    Vector3<Real> kVelocity = rkVelocity1 - rkVelocity0;

    IntrConfiguration<Real> kCfg;
    IntrAxis<Real>::GetConfiguration(m_pkHalfspace->Normal,*m_pkBox,kCfg);

    if (!IntrAxis<Real>::Test(m_pkHalfspace->Normal,kVelocity,
        -Math<Real>::MAX_REAL,m_pkHalfspace->Constant,kCfg.Min,kCfg.Max,
        fTMax,m_fContactTime,fTLast) )
    {
        // never intersecting
        return false;
    }

    if (m_fContactTime == (Real)0)
    {
        // intersecting now
        return false;
    }

    // box on positive side (right)
    if (kCfg.Map == IntrConfiguration<Real>::m1_1)
    {
        // point intersection
        m_iQuantity = 1;
        m_akPoint[0] = GetPointFromIndex(kCfg.Index[0],*m_pkBox);
    }
    else if (kCfg.Map == IntrConfiguration<Real>::m2_2)
    {
        // segment intersection
        m_iQuantity = 2;
        m_akPoint[0] = GetPointFromIndex(kCfg.Index[0],*m_pkBox);
        m_akPoint[1] = GetPointFromIndex(kCfg.Index[1],*m_pkBox);
    }
    else // kContact.Map == IntrConfiguration<Real>::m44
    {
        // face intersection
        m_iQuantity = 4;
        m_akPoint[0] = GetPointFromIndex(kCfg.Index[0],*m_pkBox);
        m_akPoint[1] = GetPointFromIndex(kCfg.Index[1],*m_pkBox);
        m_akPoint[2] = GetPointFromIndex(kCfg.Index[2],*m_pkBox);
        m_akPoint[3] = GetPointFromIndex(kCfg.Index[3],*m_pkBox);
    }

    // adjust points to the correct place in time, as well
    Vector3<Real> kDiff = m_fContactTime*rkVelocity1;
    for (int i = 0; i < m_iQuantity; i++)
    {
        m_akPoint[i] += kDiff;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrHalfspace3Box3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrHalfspace3Box3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrHalfspace3Box3<float>;

template WM4_FOUNDATION_ITEM
class IntrHalfspace3Box3<double>;
//----------------------------------------------------------------------------
}
