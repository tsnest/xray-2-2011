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
#include "Wm4IntrLine3Sphere3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Sphere3<Real>::IntrLine3Sphere3 (const Line3<Real>& rkLine,
    const Sphere3<Real>& rkSphere)
    :
    m_pkLine(&rkLine),
    m_pkSphere(&rkSphere)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Sphere3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrLine3Sphere3<Real>::GetSphere () const
{
    return *m_pkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Sphere3<Real>::Test ()
{
    Vector3<Real> kDiff = m_pkLine->Origin - m_pkSphere->Center;
    Real fA0 = kDiff.Dot(kDiff) - m_pkSphere->Radius*m_pkSphere->Radius;
    Real fA1 = m_pkLine->Direction.Dot(kDiff);
    Real fDiscr = fA1*fA1 - fA0;
    return fDiscr >= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Sphere3<Real>::Find ()
{
    Vector3<Real> kDiff = m_pkLine->Origin - m_pkSphere->Center;
    Real fA0 = kDiff.Dot(kDiff) - m_pkSphere->Radius*m_pkSphere->Radius;
    Real fA1 = m_pkLine->Direction.Dot(kDiff);
    Real fDiscr = fA1*fA1 - fA0;

    if (fDiscr < (Real)0.0)
    {
        m_iQuantity = 0;
    }
    else if (fDiscr >= Math<Real>::ZERO_TOLERANCE)
    {
        Real fRoot = Math<Real>::Sqrt(fDiscr);
        m_afLineT[0] = -fA1 - fRoot;
        m_afLineT[1] = -fA1 + fRoot;
        m_akPoint[0] = m_pkLine->Origin + m_afLineT[0]*m_pkLine->Direction;
        m_akPoint[1] = m_pkLine->Origin + m_afLineT[1]*m_pkLine->Direction;
        m_iQuantity = 2;
    }
    else
    {
        m_afLineT[0] = -fA1;
        m_akPoint[0] = m_pkLine->Origin + m_afLineT[0]*m_pkLine->Direction;
        m_iQuantity = 1;
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Sphere3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Sphere3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrLine3Sphere3<Real>::GetLineT (int i) const
{
    assert( 0 <= i && i < m_iQuantity );
    return m_afLineT[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLine3Sphere3<float>;

template WM4_FOUNDATION_ITEM
class IntrLine3Sphere3<double>;
//----------------------------------------------------------------------------
}
