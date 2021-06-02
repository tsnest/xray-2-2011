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
#include "Wm4IntrRay3Sphere3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Sphere3<Real>::IntrRay3Sphere3 (const Ray3<Real>& rkRay,
    const Sphere3<Real>& rkSphere)
    :
    m_pkRay(&rkRay),
    m_pkSphere(&rkSphere)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Sphere3<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrRay3Sphere3<Real>::GetSphere () const
{
    return *m_pkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Sphere3<Real>::Test ()
{
    Vector3<Real> kDiff = m_pkRay->Origin - m_pkSphere->Center;
    Real fA0 = kDiff.Dot(kDiff) - m_pkSphere->Radius*m_pkSphere->Radius;
    if (fA0 <= (Real)0.0)
    {
        // P is inside the sphere
        return true;
    }
    // else: P is outside the sphere

    Real fA1 = m_pkRay->Direction.Dot(kDiff);
    if (fA1 >= (Real)0.0)
    {
        return false;
    }

    // quadratic has a real root if discriminant is nonnegative
    return fA1*fA1 >= fA0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Sphere3<Real>::Find ()
{
    Vector3<Real> kDiff = m_pkRay->Origin - m_pkSphere->Center;
    Real fA0 = kDiff.Dot(kDiff) - m_pkSphere->Radius*m_pkSphere->Radius;
    Real fA1, fDiscr, fRoot;
    if (fA0 <= (Real)0.0)
    {
        // P is inside the sphere
        m_iQuantity = 1;
        fA1 = m_pkRay->Direction.Dot(kDiff);
        fDiscr = fA1*fA1 - fA0;
        fRoot = Math<Real>::Sqrt(fDiscr);
        m_afRayT[0] = -fA1 + fRoot;
        m_akPoint[0] = m_pkRay->Origin + m_afRayT[0]*m_pkRay->Direction;
        return true;
    }
    // else: P is outside the sphere

    fA1 = m_pkRay->Direction.Dot(kDiff);
    if (fA1 >= (Real)0.0)
    {
        m_iQuantity = 0;
        return false;
    }

    fDiscr = fA1*fA1 - fA0;
    if (fDiscr < (Real)0.0)
    {
        m_iQuantity = 0;
    }
    else if (fDiscr >= Math<Real>::ZERO_TOLERANCE)
    {
        fRoot = Math<Real>::Sqrt(fDiscr);
        m_afRayT[0] = -fA1 - fRoot;
        m_afRayT[1] = -fA1 + fRoot;
        m_akPoint[0] = m_pkRay->Origin + m_afRayT[0]*m_pkRay->Direction;
        m_akPoint[1] = m_pkRay->Origin + m_afRayT[1]*m_pkRay->Direction;
        m_iQuantity = 2;
    }
    else
    {
        m_afRayT[0] = -fA1;
        m_akPoint[0] = m_pkRay->Origin + m_afRayT[0]*m_pkRay->Direction;
        m_iQuantity = 1;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrRay3Sphere3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrRay3Sphere3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrRay3Sphere3<Real>::GetRayT (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_afRayT[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrRay3Sphere3<float>;

template WM4_FOUNDATION_ITEM
class IntrRay3Sphere3<double>;
//----------------------------------------------------------------------------
}
