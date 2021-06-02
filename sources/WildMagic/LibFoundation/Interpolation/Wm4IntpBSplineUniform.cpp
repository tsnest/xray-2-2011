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
#include "Wm4IntpBSplineUniform.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntpBSplineUniform<Real>::IntpBSplineUniform (int iDims, int iDegree,
    const int* aiDim, Real* afData)
{
    // get input data
    assert(iDims > 0 && iDegree > 0 && aiDim && afData);
    int i;
    for (i = 0; i < iDims; i++)
    {
        assert(aiDim[i] > iDegree+1);
    }

    m_iDims = iDims;
    m_iDegree = iDegree;
    m_aiDim = WM4_NEW int[m_iDims];
    size_t uiSize = m_iDims*sizeof(int);
    System::Memcpy(m_aiDim,uiSize,aiDim,uiSize);
    m_afData = afData;

    // setup degree constants
    m_iDp1 = m_iDegree+1;
    m_iDp1ToN = 1;
    for (i = 0; i < m_iDims; i++)
    {
        m_iDp1ToN *= m_iDp1;
    }
    m_iDp1To2N = m_iDp1ToN*m_iDp1ToN;

    // compute domain [min,max] for B-spline
    m_afDomMin = WM4_NEW Real[m_iDims];
    m_afDomMax = WM4_NEW Real[m_iDims];
    for (i = 0; i < m_iDims; i++)
    {
        Real fDomSup = Real(m_aiDim[i]-m_iDegree+1);
        Real fNext = ((Real)0.5)*(1.0f+fDomSup);
        do
        {
            m_afDomMax[i] = fNext;
            fNext = ((Real)0.5)*(fNext+fDomSup);
        }
        while (fNext < fDomSup);
        m_afDomMin[i] = (Real)1.0;
    }

    // initialize grid extremes
    m_aiGridMin = WM4_NEW int[m_iDims];
    m_aiGridMax = WM4_NEW int[m_iDims];
    for (i = 0; i < m_iDims; i++)
    {
        m_aiGridMin[i] = -1;
        m_aiGridMax[i] = -1;
    }

    // initialize base indices
    m_aiBase = WM4_NEW int[m_iDims];
    m_aiOldBase = WM4_NEW int[m_iDims];
    for (i = 0; i < m_iDims; i++)
    {
        m_aiOldBase[i] = -1;
    }

    // generate spline blending matrix
    m_aafMatrix = BlendMatrix(m_iDegree);

    // cache for optimizing compute_intermediate()
    m_afCache = WM4_NEW Real[m_iDp1ToN];

    // storage for intermediate tensor product
    m_afInter = WM4_NEW Real[m_iDp1ToN];

    // polynomial allocations
    m_aafPoly = WM4_NEW Real*[m_iDims];
    for (i = 0; i < m_iDims; i++)
    {
        m_aafPoly[i] = WM4_NEW Real[m_iDp1];
    }

    // coefficients for polynomial calculations
    m_aafCoeff = WM4_NEW Real*[m_iDp1];
    for (int iRow = 0; iRow <= m_iDegree; iRow++)
    {
        m_aafCoeff[iRow] = WM4_NEW Real[m_iDp1];
        for (int iCol = iRow; iCol <= m_iDegree; iCol++)
        {
            m_aafCoeff[iRow][iCol] = 1.0f;
            for (i = 0; i <= iRow-1; i++)
            {
                m_aafCoeff[iRow][iCol] *= Real(iCol-i);
            }
        }
    }

    // tensor product of m with itself N times
    m_afProduct = WM4_NEW Real[m_iDp1To2N];
    m_aiSkip = WM4_NEW int[m_iDp1To2N];
    int* aiCoord = WM4_NEW int[2*m_iDims];  // for address decoding
    int j;
    for (j = 0; j < m_iDp1To2N; j++)
    {
        int iTemp = j;
        for (i = 0; i < 2*m_iDims; i++)
        {
            aiCoord[i] = iTemp % m_iDp1;
            iTemp /= m_iDp1;
        }

        m_afProduct[j] = 1.0f;
        for (i = 0; i < m_iDims; i++)
        {
            m_afProduct[j] *= m_aafMatrix[aiCoord[i]][aiCoord[i+m_iDims]];
        }

        m_aiSkip[j] = 1;
    }
    WM4_DELETE[] aiCoord;

    // compute increments to skip zero values of mtensor
    for (i = 0; i < m_iDp1To2N; /**/)
    {
        for (j = i+1; j < m_iDp1To2N && m_afProduct[j] == 0.0f; j++)
        {
            m_aiSkip[i]++;
        }
        i = j;
    }

    m_oEvaluateCallback = 0;
}
//----------------------------------------------------------------------------
template <class Real>
IntpBSplineUniform<Real>::~IntpBSplineUniform ()
{
    WM4_DELETE[] m_aiDim;
    WM4_DELETE[] m_afDomMin;
    WM4_DELETE[] m_afDomMax;
    WM4_DELETE[] m_aiGridMin;
    WM4_DELETE[] m_aiGridMax;
    WM4_DELETE[] m_aiBase;
    WM4_DELETE[] m_aiOldBase;
    WM4_DELETE[] m_afCache;
    WM4_DELETE[] m_afInter;
    WM4_DELETE[] m_afProduct;
    WM4_DELETE[] m_aiSkip;

    int i;
    for (i = 0; i < m_iDims; i++)
    {
        WM4_DELETE[] m_aafPoly[i];
    }
    WM4_DELETE[] m_aafPoly;

    for (i = 0; i <= m_iDegree; i++)
    {
        WM4_DELETE[] m_aafMatrix[i];
        WM4_DELETE[] m_aafCoeff[i];
    }
    WM4_DELETE[] m_aafMatrix;
    WM4_DELETE[] m_aafCoeff;
}
//----------------------------------------------------------------------------
template <class Real>
int IntpBSplineUniform<Real>::GetDimension () const
{
    return m_iDims;
}
//----------------------------------------------------------------------------
template <class Real>
int IntpBSplineUniform<Real>::GetDegree () const
{
    return m_iDegree;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpBSplineUniform<Real>::GetDomainMin (int i) const
{
    assert(0 <= i && i < m_iDims);
    return m_afDomMin[i];
}
//----------------------------------------------------------------------------
template <class Real>
Real IntpBSplineUniform<Real>::GetDomainMax (int i) const
{
    assert(0 <= i && i < m_iDims);
    return m_afDomMax[i];
}
//----------------------------------------------------------------------------
template <class Real>
int IntpBSplineUniform<Real>::GetGridMin (int i) const
{
    assert(0 <= i && i < m_iDims);
    return m_aiGridMin[i];
}
//----------------------------------------------------------------------------
template <class Real>
int IntpBSplineUniform<Real>::GetGridMax (int i) const
{
    assert(0 <= i && i < m_iDims);
    return m_aiGridMax[i];
}
//----------------------------------------------------------------------------
template <class Real>
void IntpBSplineUniform<Real>::SetPolynomial (int iOrder, Real fDiff,
    Real* afPoly)
{
    Real fDiffPower = (Real)1.0;
    for (int i = iOrder; i <= m_iDegree; i++)
    {
        afPoly[i] = m_aafCoeff[iOrder][i]*fDiffPower;
        fDiffPower *= fDiff;
    }
}
//----------------------------------------------------------------------------
template <class Real>
int IntpBSplineUniform<Real>::Choose (int iN, int iK)
{
    // computes combination "n choose k"
    if (iN <= 1 || iK >= iN)
    {
        return 1;
    }

    int iResult = 1;
    int i;
    for (i = 0; i < iK; i++)
    {
        iResult *= iN-i;
    }
    for (i = 1; i <= iK; i++)
    {
        iResult /= i;
    }

    return iResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real** IntpBSplineUniform<Real>::BlendMatrix (int iDegree)
{
    int iDegP1 = iDegree+1;
    int iRow, iCol, i, j, k;

    // allocate triple arrays
    int*** aaaiAMat = WM4_NEW int**[iDegP1];
    int*** aaaiBMat = WM4_NEW int**[iDegP1];
    for (k = 0; k <= iDegree; k++)
    {
        aaaiAMat[k] = WM4_NEW int*[iDegP1];
        aaaiBMat[k] = WM4_NEW int*[iDegP1];
        for (iRow = 0; iRow <= iDegree; iRow++)
        {
            aaaiAMat[k][iRow] = WM4_NEW int[iDegP1];
            aaaiBMat[k][iRow] = WM4_NEW int[iDegP1];
            for (iCol = 0; iCol <= iDegree; iCol++)
            {
                aaaiAMat[k][iRow][iCol] = 0;
                aaaiBMat[k][iRow][iCol] = 0;
            }
        }
    }

    aaaiAMat[0][0][0] = 1;
    aaaiBMat[0][0][0] = 1;

    for (k = 1; k <= iDegree; k++)
    {
        // compute A[]
        for (iRow = 0; iRow <= k; iRow++)
        {
            for (iCol = 0; iCol <= k; iCol++)
            {
                aaaiAMat[k][iRow][iCol] = 0;
                if (iCol >= 1)
                {
                    aaaiAMat[k][iRow][iCol] += aaaiAMat[k-1][iRow][iCol-1];
                    if (iRow >= 1)
                    {
                        aaaiAMat[k][iRow][iCol] -=
                            aaaiBMat[k-1][iRow-1][iCol-1];
                    }
                }
                if (iRow >= 1)
                {
                    aaaiAMat[k][iRow][iCol] +=
                        (k+1)*aaaiBMat[k-1][iRow-1][iCol];
                }
            }
        }

        // compute B[]
        for (iRow = 0; iRow <= k; iRow++)
        {
            for (iCol = 0; iCol <= k; iCol++)
            {
                aaaiBMat[k][iRow][iCol]= 0;
                for (i = iCol; i <= k; i++)
                {
                    if ((i-iCol) % 2)
                    {
                        aaaiBMat[k][iRow][iCol] -=
                            Choose(i,iCol)*aaaiAMat[k][iRow][i];
                    }
                    else
                    {
                        aaaiBMat[k][iRow][iCol] +=
                            Choose(i,iCol)*aaaiAMat[k][iRow][i];
                    }
                }
            }
        }
    }

    Real** aafCMat = WM4_NEW Real*[iDegP1];
    for (iRow = 0; iRow <= iDegree; iRow++)
    {
        aafCMat[iRow] = WM4_NEW Real[iDegP1];
        for (iCol = 0; iCol <= iDegree; iCol++)
        {
            aafCMat[iRow][iCol]= 0;
            for (i = iCol; i <= iDegree; i++)
            {
                int iProd = 1;
                for (j = 1; j <= i-iCol; j++)
                {
                    iProd *= iDegree-iRow;
                }
                aafCMat[iRow][iCol] += iProd*Choose(i,iCol) *
                    aaaiAMat[iDegree][iDegree-iRow][i];
            }
        }
    }

    Real fFactorial = 1;
    for (k = 1; k <= iDegree; k++)
    {
        fFactorial *= k;
    }
    Real fInvFactorial = 1.0f/fFactorial;
    Real** aafMatrix = WM4_NEW Real*[iDegP1];
    for (iRow = 0; iRow <= iDegree; iRow++)
    {
        aafMatrix[iRow] = WM4_NEW Real[iDegP1];
        for (iCol = 0; iCol <= iDegree; iCol++)
        {
            aafMatrix[iRow][iCol] = aafCMat[iRow][iCol]*fInvFactorial;
        }
    }

    // deallocate triple arrays
    for (k = 0; k <= iDegree; k++)
    {
        for (iRow = 0; iRow <= iDegree; iRow++)
        {
            WM4_DELETE[] aaaiBMat[k][iRow];
            WM4_DELETE[] aaaiAMat[k][iRow];
        }
        WM4_DELETE[] aaaiBMat[k];
        WM4_DELETE[] aaaiAMat[k];
    }
    WM4_DELETE[] aaaiBMat;
    WM4_DELETE[] aaaiAMat;

    // deallocate integer matrix
    for (k = 0; k <= iDegree; k++)
    {
        WM4_DELETE[] aafCMat[k];
    }
    WM4_DELETE[] aafCMat;

    return aafMatrix;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntpBSplineUniform<float>;

template WM4_FOUNDATION_ITEM
class IntpBSplineUniform<double>;
//----------------------------------------------------------------------------
}
