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
#include "Wm4IntpThinPlateSpline2.h"
#include "Wm4LinearSystem.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntpThinPlateSpline2<Real>::IntpThinPlateSpline2 (int iQuantity, Real* afX,
    Real* afY, Real* afF, Real fSmooth, bool bOwner)
{
    assert(iQuantity >= 3 && afX && afY && afF && fSmooth >= (Real)0.0);

    m_bInitialized = false;
    m_iQuantity = iQuantity;
    m_afX = WM4_NEW Real[m_iQuantity];
    m_afY = WM4_NEW Real[m_iQuantity];
    m_afA = WM4_NEW Real[m_iQuantity];

    int i, iRow, iCol;

    // map input (x,y) to unit square
    m_fXMin = afX[0];
    m_fXMax = m_fXMin;
    for (i = 1; i < m_iQuantity; i++)
    {
        if (afX[i] < m_fXMin)
        {
            m_fXMin = afX[i];
        }
        if (afX[i] > m_fXMax)
        {
            m_fXMax = afX[i];
        }
    }
    assert( m_fXMax > m_fXMin );
    m_fXInvRange = ((Real)1.0)/(m_fXMax - m_fXMin);
    for (i = 0; i < m_iQuantity; i++)
    {
        m_afX[i] = (afX[i] - m_fXMin)*m_fXInvRange;
    }

    m_fYMin = afY[0];
    m_fYMax = m_fYMin;
    for (i = 1; i < m_iQuantity; i++)
    {
        if (afY[i] < m_fYMin)
        {
            m_fYMin = afY[i];
        }
        if (afY[i] > m_fYMax)
        {
            m_fYMax = afY[i];
        }
    }
    assert(m_fYMax > m_fYMin);
    m_fYInvRange = ((Real)1.0)/(m_fYMax - m_fYMin);
    for (i = 0; i < m_iQuantity; i++)
    {
        m_afY[i] = (afY[i] - m_fYMin)*m_fYInvRange;
    }

    // compute matrix A = E+smooth*I [NxN matrix]
    GMatrix<Real> kA(m_iQuantity,m_iQuantity);
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        for (iCol = 0; iCol < m_iQuantity; iCol++)
        {
            if (iRow == iCol)
            {
                kA[iRow][iCol] = fSmooth;
            }
            else
            {
                Real fDx = m_afX[iRow] - m_afX[iCol];
                Real fDy = m_afY[iRow] - m_afY[iCol];
                Real fT = Math<Real>::Sqrt(fDx*fDx+fDy*fDy);
                kA[iRow][iCol] = Kernel(fT);
            }
        }
    }

    // compute matrix B [Nx3 matrix]
    GMatrix<Real> kB(m_iQuantity,3);
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        kB[iRow][0] = (Real)1.0;
        kB[iRow][1] = m_afX[iRow];
        kB[iRow][2] = m_afY[iRow];
    }

    // compute A^{-1}
    GMatrix<Real> kInvA(m_iQuantity,m_iQuantity);
    if (!LinearSystem<Real>().Inverse(kA,kInvA))
    {
        return;
    }

    // compute P = B^t A^{-1}  [3xN matrix]
    GMatrix<Real> kP = kB.TransposeTimes(kInvA);

    // compute Q = P B = B^t A^{-1} B  [3x3 matrix]
    GMatrix<Real> kQ = kP*kB;

    // compute Q^{-1}
    GMatrix<Real> kInvQ(3,3);
    if (!LinearSystem<Real>().Inverse(kQ,kInvQ))
    {
        return;
    }

    // compute P*z
    Real afProd[3];
    for (iRow = 0; iRow < 3; iRow++)
    {
        afProd[iRow] = (Real)0.0;
        for (i = 0; i < m_iQuantity; i++)
        {
            afProd[iRow] += kP[iRow][i]*afF[i];
        }
    }

    // compute 'b' vector for smooth thin plate spline
    for (iRow = 0; iRow < 3; iRow++)
    {
        m_afB[iRow] = (Real)0.0;
        for (i = 0; i < 3; i++)
        {
            m_afB[iRow] += kInvQ[iRow][i]*afProd[i];
        }
    }

    // compute z-B*b
    Real* afTmp = WM4_NEW Real[m_iQuantity];
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        afTmp[iRow] = afF[iRow];
        for (i = 0; i < 3; i++)
        {
            afTmp[iRow] -= kB[iRow][i]*m_afB[i];
        }
    }

    // compute 'a' vector for smooth thin plate spline
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        m_afA[iRow] = (Real)0.0;
        for (i = 0; i < m_iQuantity; i++)
        {
            m_afA[iRow] += kInvA[iRow][i]*afTmp[i];
        }
    }
    WM4_DELETE[] afTmp;

    m_bInitialized = true;

    if (bOwner)
    {
        WM4_DELETE[] afX;
        WM4_DELETE[] afY;
        WM4_DELETE[] afF;
    }
}
//----------------------------------------------------------------------------
template <class Real>
IntpThinPlateSpline2<Real>::~IntpThinPlateSpline2 ()
{
    WM4_DELETE[] m_afX;
    WM4_DELETE[] m_afY;
    WM4_DELETE[] m_afA;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntpThinPlateSpline2<Real>::IsInitialized () const
{
    return m_bInitialized;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpThinPlateSpline2<Real>::operator() (Real fX, Real fY)
{
    if (m_bInitialized)
    {
        // map (x,y) to the unit square
        fX = (fX - m_fXMin)*m_fXInvRange;
        fY = (fY - m_fYMin)*m_fYInvRange;

        Real fResult = m_afB[0] + m_afB[1]*fX + m_afB[2]*fY;
        for (int i = 0; i < m_iQuantity; i++)
        {
            Real fDx = fX - m_afX[i];
            Real fDy = fY - m_afY[i];
            Real fT = Math<Real>::Sqrt(fDx*fDx+fDy*fDy);
            fResult += m_afA[i]*Kernel(fT);
        }
        return fResult;
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpThinPlateSpline2<Real>::Kernel (Real fT)
{
    if (fT > (Real)0.0)
    {
        Real fT2 = fT*fT;
        return fT2*Math<Real>::Log(fT2);
    }
    return (Real)0.0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntpThinPlateSpline2<float>;

template WM4_FOUNDATION_ITEM
class IntpThinPlateSpline2<double>;
//----------------------------------------------------------------------------
}
