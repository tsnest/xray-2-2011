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
#include "Wm4IntpAkimaUniform1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntpAkimaUniform1<Real>::IntpAkimaUniform1 (int iQuantity, Real fXMin,
    Real fXSpacing, Real* afF)
    :
    IntpAkima1<Real>(iQuantity,afF)
{
    assert(fXSpacing > (Real)0.0);

    m_fXMin = fXMin;
    m_fXMax = fXMin + fXSpacing*(iQuantity-1);
    m_fXSpacing = fXSpacing;

    // compute slopes
    Real fInvDX = ((Real)1.0)/fXSpacing;
    Real* afSlope = WM4_NEW Real[iQuantity+3];
    int i, iP1, iP2;
    for (i = 0, iP1 = 1, iP2 = 2; i < iQuantity-1; i++, iP1++, iP2++)
    {
        afSlope[iP2] = (afF[iP1] - afF[i])*fInvDX;
    }

    afSlope[1] = ((Real)2.0)*afSlope[2] - afSlope[3];
    afSlope[0] = ((Real)2.0)*afSlope[1] - afSlope[2];
    afSlope[iQuantity+1] = ((Real)2.0)*afSlope[iQuantity] -
        afSlope[iQuantity-1];
    afSlope[iQuantity+2] = ((Real)2.0)*afSlope[iQuantity+1] -
        afSlope[iQuantity];

    // construct derivatives
    Real* afFDer = WM4_NEW Real[iQuantity];
    for (i = 0; i < iQuantity; i++)
    {
        afFDer[i] = ComputeDerivative(afSlope+i);
    }

    // construct polynomials
    Real fInvDX2 = ((Real)1.0)/(fXSpacing*fXSpacing);
    Real fInvDX3 = fInvDX2/fXSpacing;
    for (i = 0, iP1 = 1; i < iQuantity-1; i++, iP1++)
    {
        typename IntpAkima1<Real>::Polynomial& rkPoly = m_akPoly[i];

        Real fF0 = afF[i], fF1 = afF[iP1], fDF = fF1 - fF0;
        Real fFDer0 = afFDer[i], fFDer1 = afFDer[iP1];

        rkPoly[0] = fF0;
        rkPoly[1] = fFDer0;
        rkPoly[2] = (((Real)3.0)*fDF-fXSpacing*(fFDer1 +
            ((Real)2.0)*fFDer0))*fInvDX2;
        rkPoly[3] = (fXSpacing*(fFDer0+fFDer1)-((Real)2.0)*fDF)*fInvDX3;
    }

    WM4_DELETE[] afSlope;
    WM4_DELETE[] afFDer;
}
//----------------------------------------------------------------------------
template <class Real>
IntpAkimaUniform1<Real>::~IntpAkimaUniform1 ()
{
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpAkimaUniform1<Real>::GetXMin () const
{
    return m_fXMin;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpAkimaUniform1<Real>::GetXMax () const
{
    return m_fXMax;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpAkimaUniform1<Real>::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntpAkimaUniform1<Real>::Lookup (Real fX, int& riIndex, Real& rfDX) const
{
    if (fX >= m_fXMin)
    {
        if (fX <= m_fXMax)
        {
            for (riIndex = 0; riIndex+1 < m_iQuantity; riIndex++)
            {
                if (fX < m_fXMin + m_fXSpacing*(riIndex+1))
                {
                    rfDX = fX - (m_fXMin + m_fXSpacing*riIndex);
                    return true;
                }
            }

            riIndex--;
            rfDX = fX - (m_fXMin + m_fXSpacing*riIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntpAkimaUniform1<float>;

template WM4_FOUNDATION_ITEM
class IntpAkimaUniform1<double>;
//----------------------------------------------------------------------------
}
