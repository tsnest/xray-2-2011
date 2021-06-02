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
#include "Wm4BSplineFitBasis.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
BSplineFitBasis<Real>::BSplineFitBasis (int iQuantity, int iDegree)
{
    assert(1 <= iDegree && iDegree < iQuantity);
    m_iQuantity = iQuantity;
    m_iDegree = iDegree;

    m_afValue = WM4_NEW Real[iDegree+1];
    m_afKnot = WM4_NEW Real[2*iDegree];
}
//----------------------------------------------------------------------------
template <class Real>
BSplineFitBasis<Real>::~BSplineFitBasis ()
{
    WM4_DELETE[] m_afValue;
    WM4_DELETE[] m_afKnot;
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineFitBasis<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineFitBasis<Real>::GetDegree () const
{
    return m_iDegree;
}
//----------------------------------------------------------------------------
template <class Real>
void BSplineFitBasis<Real>::Compute (Real fTime, int& iMin, int& iMax) const
{
    assert((Real)0.0 <= fTime && fTime <= (Real)1.0);

    // Use scaled time and scaled knots so that 1/(Q-D) does not need to
    // be explicitly stored by the class object.  Determine the extreme
    // indices affected by local control.
    Real fQmD = (Real)(m_iQuantity - m_iDegree);
    Real fT;
    if (fTime <= (Real)0.0)
    {
        fT = (Real)0.0;
        iMin = 0;
        iMax = m_iDegree;
    }
    else if (fTime >= (Real)1.0)
    {
        fT = fQmD;
        iMax = m_iQuantity - 1;
        iMin = iMax - m_iDegree;
    }
    else
    {
        fT = fQmD*fTime;
        iMin = (int)fT;
        iMax = iMin + m_iDegree;
    }

    // Precompute the knots.
    for (int i0 = 0, i1 = iMax+1-m_iDegree; i0 < 2*m_iDegree; i0++, i1++)
    {
        if (i1 <= m_iDegree)
        {
            m_afKnot[i0] = (Real)0.0f;
        }
        else if (i1 >= m_iQuantity)
        {
            m_afKnot[i0] = fQmD;
        }
        else
        {
            m_afKnot[i0] = (Real)(i1 - m_iDegree);
        }
    }

    // Initialize the basis function evaluation table.  The first degree-1
    // entries are zero, but they do not have to be set explicitly.
    m_afValue[m_iDegree] = (Real)1.0;

    // Update the basis function evaluation table, each iteration overwriting
    // the results from the previous iteration.
    for (int iRow = m_iDegree-1; iRow >= 0; iRow--)
    {
        int iK0 = m_iDegree, iK1 = iRow;
        Real fKnot0 = m_afKnot[iK0], fKnot1 = m_afKnot[iK1];
        Real fInvDenom = ((Real)1.0)/(fKnot0 - fKnot1);
        Real fC1 = (fKnot0 - fT)*fInvDenom, fC0;
        m_afValue[iRow] = fC1*m_afValue[iRow+1];

        for (int iCol = iRow+1; iCol < m_iDegree; iCol++)
        {
            fC0 = (fT - fKnot1)*fInvDenom;
            m_afValue[iCol] *= fC0;

            fKnot0 = m_afKnot[++iK0];
            fKnot1 = m_afKnot[++iK1];
            fInvDenom = ((Real)1.0)/(fKnot0 - fKnot1);
            fC1 = (fKnot0 - fT)*fInvDenom;
            m_afValue[iCol] += fC1*m_afValue[iCol+1];
        }

        fC0 = (fT - fKnot1)*fInvDenom;
        m_afValue[m_iDegree] *= fC0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real BSplineFitBasis<Real>::GetValue (int i) const
{
    assert(0 <= i && i <= m_iDegree);
    return m_afValue[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class BSplineFitBasis<float>;

template WM4_FOUNDATION_ITEM
class BSplineFitBasis<double>;
//----------------------------------------------------------------------------
}
