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
// Version: 4.0.0 (2006/06/28)

#include "Wm4FoundationPCH.h"
#include "Wm4IntpAkima1.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntpAkima1<Real>::IntpAkima1 (int iQuantity, Real* afF)
{
    // At least three data points are needed to construct the estimates of
    // the boundary derivatives.
    assert(iQuantity >= 3 && afF);

    m_iQuantity = iQuantity;
    m_afF = afF;
    m_akPoly = WM4_NEW Polynomial[iQuantity-1];
}
//----------------------------------------------------------------------------
template <class Real>
IntpAkima1<Real>::~IntpAkima1 ()
{
    WM4_DELETE[] m_akPoly;
}
//----------------------------------------------------------------------------
template <class Real>
int IntpAkima1<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* IntpAkima1<Real>::GetF () const
{
    return m_afF;
}
//----------------------------------------------------------------------------
template <class Real>
const typename IntpAkima1<Real>::Polynomial*
IntpAkima1<Real>::GetPolynomials () const
{
    return m_akPoly;
}
//----------------------------------------------------------------------------
template <class Real>
const typename IntpAkima1<Real>::Polynomial&
IntpAkima1<Real>::GetPolynomial (int i) const
{
    assert(0 <= i && i < m_iQuantity-1);
    return m_akPoly[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpAkima1<Real>::ComputeDerivative (Real* afSlope) const
{
    if (afSlope[1] != afSlope[2])
    {
        if (afSlope[0] != afSlope[1])
        {
            if (afSlope[2] != afSlope[3])
            {
                Real fAD0 = Math<Real>::FAbs(afSlope[3] - afSlope[2]);
                Real fAD1 = Math<Real>::FAbs(afSlope[0] - afSlope[1]);
                return (fAD0*afSlope[1]+fAD1*afSlope[2])/(fAD0+fAD1);
            }
            else
            {
                return afSlope[2];
            }
        }
        else
        {
            if (afSlope[2] != afSlope[3])
            {
                return afSlope[1];
            }
            else
            {
                return ((Real)0.5)*(afSlope[1]+afSlope[2]);
            }
        }
    }
    else
    {
        return afSlope[1];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpAkima1<Real>::operator() (Real fX) const
{
    int iIndex;
    Real fDX;

    if (Lookup(fX,iIndex,fDX))
    {
        return m_akPoly[iIndex](fDX);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpAkima1<Real>::operator() (int iOrder, Real fX) const
{
    int iIndex;
    Real fDX;

    if (Lookup(fX,iIndex,fDX))
    {
        return m_akPoly[iIndex](iOrder,fDX);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntpAkima1<float>;

template WM4_FOUNDATION_ITEM
class IntpAkima1<double>;
//----------------------------------------------------------------------------
}
