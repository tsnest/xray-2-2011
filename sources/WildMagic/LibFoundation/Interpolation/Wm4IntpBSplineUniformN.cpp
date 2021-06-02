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
#include "Wm4IntpBSplineUniformN.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntpBSplineUniformN<Real>::IntpBSplineUniformN (int iDims, int iDegree,
    const int* aiDim, Real* afData)
    :
    IntpBSplineUniform<Real>(iDims,iDegree,aiDim,afData)
{
    m_aiEvI = WM4_NEW int[m_iDims];
    m_aiCiLoop = WM4_NEW int[m_iDims];
    m_aiCiDelta = WM4_NEW int[m_iDims-1];
    m_aiOpI = WM4_NEW int[m_iDims];
    m_aiOpJ = WM4_NEW int[m_iDims];
    m_aiOpDelta = WM4_NEW int[m_iDims-1];

    int iTemp = 1;
    for (int iDim = 0; iDim < m_iDims-1; iDim++)
    {
        m_aiCiDelta[iDim] = iTemp*(m_aiDim[iDim]-m_iDp1);
        iTemp *= m_aiDim[iDim];
    }
}
//----------------------------------------------------------------------------
template <class Real>
IntpBSplineUniformN<Real>::~IntpBSplineUniformN ()
{
    WM4_DELETE[] m_aiEvI;
    WM4_DELETE[] m_aiCiLoop;
    WM4_DELETE[] m_aiOpI;
    WM4_DELETE[] m_aiOpJ;
    WM4_DELETE[] m_aiOpDelta;
    WM4_DELETE[] m_aiCiDelta;
}
//----------------------------------------------------------------------------
template <class Real>
int IntpBSplineUniformN<Real>::Index (int* aiI) const
{
    int iIndex = aiI[m_iDims-1];
    for (int iDim = m_iDims-2; iDim >= 0; iDim--)
    {
        iIndex *= m_aiDim[iDim];
        iIndex += aiI[iDim];
    }
    return iIndex;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpBSplineUniformN<Real>::operator() (Real* afX)
{
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        m_aiBase[iDim] = (int)Math<Real>::Floor(afX[iDim]);
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        if (m_aiOldBase[iDim] != m_aiBase[iDim])
        {
            // switch to new local grid
            for (int k = 0; k < m_iDims; k++)
            {
                m_aiOldBase[k] = m_aiBase[k];
                m_aiGridMin[k] = m_aiBase[k] - 1;
                m_aiGridMax[k] = m_aiGridMin[k] + m_iDegree;
            }

            // fill in missing grid m_afData if necessary
            if (m_oEvaluateCallback)
            {
                EvaluateUnknownData();
            }

            ComputeIntermediate();
            break;
        }
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        SetPolynomial(0,afX[iDim]-m_aiBase[iDim],m_aafPoly[iDim]);
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        m_aiOpI[iDim] = 0;
    }

    Real fResult = (Real)0.0;
    int k;
    for (k = 0; k < m_iDp1ToN; k++)
    {
        Real fTemp = m_afInter[k];
        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            fTemp *= m_aafPoly[iDim][m_aiOpI[iDim]];
        }
        fResult += fTemp;

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if (++m_aiOpI[iDim] <= m_iDegree)
            {
                break;
            }
            m_aiOpI[iDim] = 0;
        }
    }
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpBSplineUniformN<Real>::operator() (int* aiDx, Real* afX)
{
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        m_aiBase[iDim] = (int)Math<Real>::Floor(afX[iDim]);
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        if (m_aiOldBase[iDim] != m_aiBase[iDim])
        {
            // switch to new local grid
            for (int k = 0; k < m_iDims; k++)
            {
                m_aiOldBase[k] = m_aiBase[k];
                m_aiGridMin[k] = m_aiBase[k] - 1;
                m_aiGridMax[k] = m_aiGridMin[k] + m_iDegree;
            }

            // fill in missing grid m_afData if necessary
            if (m_oEvaluateCallback)
            {
                EvaluateUnknownData();
            }

            ComputeIntermediate();
            break;
        }
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        SetPolynomial(aiDx[iDim],afX[iDim]-m_aiBase[iDim],m_aafPoly[iDim]);
    }

    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        m_aiOpJ[iDim] = aiDx[iDim];
    }

    int iTemp = 1;
    for (iDim = 0; iDim < m_iDims-1; iDim++)
    {
        m_aiOpDelta[iDim] = iTemp*aiDx[iDim];
        iTemp *= m_iDp1;
    }

    Real fResult = (Real)0.0;
    int k = m_aiOpJ[m_iDims-1];
    for (iDim = m_iDims-2; iDim >= 0; iDim--)
    {
        k *= m_iDp1;
        k += m_aiOpJ[iDim];
    }
    for (/**/; k < m_iDp1ToN; k++)
    {
        Real fTemp = m_afInter[k];
        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            fTemp *= m_aafPoly[iDim][m_aiOpJ[iDim]];
        }
        fResult += fTemp;

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if (++m_aiOpJ[iDim] <= m_iDegree)
            {
                continue;
            }
            m_aiOpJ[iDim] = aiDx[iDim];
            k += m_aiOpDelta[iDim];
        }
    }
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
void IntpBSplineUniformN<Real>::EvaluateUnknownData ()
{
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        m_aiEvI[iDim] = m_aiGridMin[iDim];
    }

    for (int j = 0; j < m_iDp1ToN; j++)
    {
        int iIndex = Index(m_aiEvI);
        if (m_afData[iIndex] == Math<Real>::MAX_REAL)
        {
            m_afData[iIndex] = m_oEvaluateCallback(iIndex);
        }

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if (++m_aiEvI[iDim] <= m_aiGridMax[iDim])
            {
                break;
            }
            m_aiEvI[iDim] = m_aiGridMin[iDim];
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntpBSplineUniformN<Real>::ComputeIntermediate ()
{
    // fetch subblock of m_afData to cache
    int iDim;
    for (iDim = 0; iDim < m_iDims; iDim++)
    {
        m_aiCiLoop[iDim] = m_aiGridMin[iDim];
    }
    int iIndex = Index(m_aiCiLoop);
    int k;
    for (k = 0; k < m_iDp1ToN; k++, iIndex++)
    {
        m_afCache[k] = m_afData[iIndex];

        for (iDim = 0; iDim < m_iDims; iDim++)
        {
            if (++m_aiCiLoop[iDim] <= m_aiGridMax[iDim])
            {
                break;
            }
            m_aiCiLoop[iDim] = m_aiGridMin[iDim];
            iIndex += m_aiCiDelta[iDim];
        }
    }

    // compute and save the intermediate product
    for (int i = 0, j = 0; i < m_iDp1ToN; i++)
    {
        Real fSum = (Real)0.0;
        for (k = 0; k < m_iDp1ToN; k += m_aiSkip[j], j += m_aiSkip[j])
        {
            fSum += m_afProduct[j]*m_afCache[k];
        }
        m_afInter[i] = fSum;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntpBSplineUniformN<float>;

template WM4_FOUNDATION_ITEM
class IntpBSplineUniformN<double>;
//----------------------------------------------------------------------------
}
