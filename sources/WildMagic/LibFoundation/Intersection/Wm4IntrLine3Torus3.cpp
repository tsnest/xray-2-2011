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
#include "Wm4IntrLine3Torus3.h"
#include "Wm4PolynomialRoots.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Torus3<Real>::IntrLine3Torus3 (const Line3<Real>& rkLine,
    const Torus3<Real>& rkTorus)
    :
    m_pkLine(&rkLine),
    m_pkTorus(&rkTorus)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Torus3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Torus3<Real>& IntrLine3Torus3<Real>::GetTorus () const
{
    return *m_pkTorus;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Torus3<Real>::Find ()
{
    // compute coefficients of quartic polynomial
    Real fRo2 = m_pkTorus->Ro*m_pkTorus->Ro;
    Real fRi2 = m_pkTorus->Ri*m_pkTorus->Ri;
    Real fDD = m_pkLine->Direction.Dot(m_pkLine->Direction);
    Real fDE = m_pkLine->Origin.Dot(m_pkLine->Direction);
    Real fVal = m_pkLine->Origin.Dot(m_pkLine->Origin) - (fRo2 + fRi2);

    Polynomial1<Real> kPoly(4);
    kPoly[0] = fVal*fVal - ((Real)4.0)*fRo2*(fRi2 -
        m_pkLine->Origin.Z()*m_pkLine->Origin.Z());
    kPoly[1] = ((Real)4.0)*fDE*fVal +
        ((Real)8.0)*fRo2*m_pkLine->Direction.Z()*m_pkLine->Origin.Z();
    kPoly[2] = ((Real)2.0)*fDD*fVal + ((Real)4.0)*fDE*fDE +
        ((Real)4.0)*fRo2*m_pkLine->Direction.Z()*m_pkLine->Direction.Z();
    kPoly[3] = ((Real)4.0)*fDD*fDE;
    kPoly[4] = fDD*fDD;

    // solve the quartic
    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    kPR.FindB(kPoly,6);
    m_iQuantity = kPR.GetCount();
    const Real* afRoot = kPR.GetRoots();

    // get the intersection points
    for (int i = 0; i < m_iQuantity; i++)
    {
        m_akPoint[i] = m_pkLine->Origin + afRoot[i]*m_pkLine->Direction;
    }

    m_iIntersectionType = ( m_iQuantity > 0 ? IT_POINT : IT_EMPTY );
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Torus3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Torus3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLine3Torus3<float>;

template WM4_FOUNDATION_ITEM
class IntrLine3Torus3<double>;
//----------------------------------------------------------------------------
}
