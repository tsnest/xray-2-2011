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
// Version: 4.0.1 (2008/09/13)

#include "Wm4FoundationPCH.h"
#include "Wm4BSplineCurveFit.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
BSplineCurveFit<Real>::BSplineCurveFit (int iDimension, int iSampleQuantity,
    const Real* afSampleData, int iDegree, int iControlQuantity)
    :
    m_kBasis(iControlQuantity,iDegree)
{
    assert(iDimension >= 1);
    assert(1 <= iDegree && iDegree < iControlQuantity);
    assert(iControlQuantity <= iSampleQuantity);

    m_iDimension = iDimension;
    m_iSampleQuantity = iSampleQuantity;
    m_afSampleData = afSampleData;
    m_iDegree = iDegree;
    m_iControlQuantity = iControlQuantity;
    m_afControlData = WM4_NEW Real[m_iDimension*iControlQuantity];

    // The double-precision basis functions are used to help with the
    // numerical round-off errors.
    BSplineFitBasisd kDBasis(m_iControlQuantity,m_iDegree);
    double dTMultiplier = 1.0/(double)(m_iSampleQuantity - 1);

    // Fit the data points with a B-spline curve using a least-squares error
    // metric.  The problem is of the form A^T*A*Q = A^T*P, where A^T*A is a
    // banded matrix, P contains the sample data, and Q is the unknown vector
    // of control points.

    double dT;
    int i0, i1, i2, iMin, iMax, j;

    // Construct the matrix A^T*A.
    BandedMatrixd* pkATAMat = WM4_NEW BandedMatrixd(m_iControlQuantity,
        m_iDegree+1,m_iDegree+1);

    for (i0 = 0; i0 < m_iControlQuantity; i0++)
    {
        for (i1 = 0; i1 < i0; i1++)
        {
            (*pkATAMat)(i0,i1) = (*pkATAMat)(i1,i0);
        }

        int i1Max = i0 + m_iDegree;
        if (i1Max >= m_iControlQuantity)
        {
            i1Max = m_iControlQuantity - 1;
        }

        for (i1 = i0; i1 <= i1Max; i1++)
        {
            double dValue = 0.0;
            for (i2 = 0; i2 < m_iSampleQuantity; i2++)
            {
                dT = dTMultiplier*(double)i2;
                kDBasis.Compute(dT,iMin,iMax);
                if (iMin <= i0 && i0 <= iMax && iMin <= i1 && i1 <= iMax)
                {
                    double dB0 = kDBasis.GetValue(i0 - iMin);
                    double dB1 = kDBasis.GetValue(i1 - iMin);
                    dValue += dB0*dB1;
                }
            }
            (*pkATAMat)(i0,i1) = dValue;
        }
    }

    // Construct the matrix A^T.
    double** aadATMat;
    Allocate(m_iSampleQuantity,m_iControlQuantity,aadATMat);
    memset(aadATMat[0],0,m_iControlQuantity*m_iSampleQuantity*sizeof(double));
    for (i0 = 0; i0 < m_iControlQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iSampleQuantity; i1++)
        {
            dT = dTMultiplier*(double)i1;
            kDBasis.Compute(dT,iMin,iMax);
            if (iMin <= i0 && i0 <= iMax)
            {
                aadATMat[i0][i1] = kDBasis.GetValue(i0 - iMin);
            }
        }
    }

    // Compute X0 = (A^T*A)^{-1}*A^T by solving the linear system
    // A^T*A*X = A^T.
    bool bSolved = pkATAMat->SolveSystem(aadATMat,m_iSampleQuantity);
    assert(bSolved);
    (void)bSolved;  // Avoid warning in release build.

    // The control points for the fitted curve are stored in the vector
    // Q = X0*P, where P is the vector of sample data.
    memset(m_afControlData,0,m_iControlQuantity*m_iDimension*sizeof(Real));
    for (i0 = 0; i0 < m_iControlQuantity; i0++)
    {
        Real* afQ = m_afControlData + i0*m_iDimension;
        for (i1 = 0; i1 < m_iSampleQuantity; i1++)
        {
            const Real* afP = m_afSampleData + i1*m_iDimension;
            Real fXValue = (Real)aadATMat[i0][i1];
            for (j = 0; j < m_iDimension; j++)
            {
                afQ[j] += fXValue*afP[j];
            }
        }
    }

    // Set the first and last output control points to match the first and
    // last input samples.  This supports the application of fitting keyframe
    // data with B-spline curves.  The user expects that the curve passes
    // through the first and last positions in order to support matching two
    // consecutive keyframe sequences.
    Real* pfCEnd0 = m_afControlData;
    const Real* pfSEnd0 = m_afSampleData;
    Real* pfCEnd1 = &m_afControlData[m_iDimension*(m_iControlQuantity-1)];
    const Real* pfSEnd1 = &m_afSampleData[m_iDimension*(m_iSampleQuantity-1)];
    for (j = 0; j < m_iDimension; j++)
    {
        *pfCEnd0++ = *pfSEnd0++;
        *pfCEnd1++ = *pfSEnd1++;
    }

    Deallocate(aadATMat);
    WM4_DELETE pkATAMat;
}
//----------------------------------------------------------------------------
template <class Real>
BSplineCurveFit<Real>::~BSplineCurveFit ()
{
    WM4_DELETE[] m_afControlData;
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineCurveFit<Real>::GetDimension () const
{
    return m_iDimension;
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineCurveFit<Real>::GetSampleQuantity () const
{
    return m_iSampleQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* BSplineCurveFit<Real>::GetSampleData () const
{
    return m_afSampleData;
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineCurveFit<Real>::GetDegree () const
{
    return m_iDegree;
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineCurveFit<Real>::GetControlQuantity () const
{
    return m_iControlQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* BSplineCurveFit<Real>::GetControlData () const
{
    return m_afControlData;
}
//----------------------------------------------------------------------------
template <class Real>
const BSplineFitBasis<Real>& BSplineCurveFit<Real>::GetBasis () const
{
    return m_kBasis;
}
//----------------------------------------------------------------------------
template <class Real>
void BSplineCurveFit<Real>::GetPosition (Real fT, Real* afPosition) const
{
    assert(afPosition);

    int iMin, iMax;
    m_kBasis.Compute(fT,iMin,iMax);

    Real* pfSource = &m_afControlData[m_iDimension*iMin];
    Real fBasisValue = m_kBasis.GetValue(0);
    int j;
    for (j = 0; j < m_iDimension; j++)
    {
        afPosition[j] = fBasisValue*(*pfSource++);
    }

    for (int i = iMin+1, iIndex = 1; i <= iMax; i++, iIndex++)
    {
        fBasisValue = m_kBasis.GetValue(iIndex);
        for (j = 0; j < m_iDimension; j++)
        {
            afPosition[j] += fBasisValue*(*pfSource++);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class BSplineCurveFit<float>;

template WM4_FOUNDATION_ITEM
class BSplineCurveFit<double>;
//----------------------------------------------------------------------------
}
