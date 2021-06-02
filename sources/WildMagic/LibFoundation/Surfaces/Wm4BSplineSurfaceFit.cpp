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
// Version: 4.7.0 (2008/09/13)

#include "Wm4FoundationPCH.h"
#include "Wm4BSplineSurfaceFit.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
BSplineSurfaceFit<Real>::BSplineSurfaceFit (int iDegree0,
    int iControlQuantity0, int iSampleQuantity0, int iDegree1,
    int iControlQuantity1, int iSampleQuantity1,
    Vector3<Real>** aakSamplePoint)
{
    assert(1 <= iDegree0 && iDegree0 < iControlQuantity0);
    assert(iControlQuantity0 <= iSampleQuantity0);
    assert(1 <= iDegree1 && iDegree1 < iControlQuantity1);
    assert(iControlQuantity1 <= iSampleQuantity1);

    m_aiDegree[0] = iDegree0;
    m_aiSampleQuantity[0] = iSampleQuantity0;
    m_aiControlQuantity[0] = iControlQuantity0;
    m_aiDegree[1] = iDegree1;
    m_aiSampleQuantity[1] = iSampleQuantity1;
    m_aiControlQuantity[1] = iControlQuantity1;
    m_aakSamplePoint = aakSamplePoint;
    Allocate(iControlQuantity0,iControlQuantity1,m_aakControlPoint);

    // The double-precision basis functions are used to help with the
    // numerical round-off errors.
    BSplineFitBasisd* apkDBasis[2];
    double adTMultiplier[2];
    int iDim;
    for (iDim = 0; iDim < 2; iDim++)
    {
        m_apkBasis[iDim] = WM4_NEW BSplineFitBasis<Real>(
            m_aiControlQuantity[iDim],m_aiDegree[iDim]);

        apkDBasis[iDim] = WM4_NEW BSplineFitBasisd(
            m_aiControlQuantity[iDim],m_aiDegree[iDim]);

        adTMultiplier[iDim] = 1.0/(double)(m_aiSampleQuantity[iDim] - 1);
    }

    // Fit the data points with a B-spline surface using a least-squares error
    // metric.  The problem is of the form A0^T*A0*Q*A1^T*A1 = A0^T*P*A1, where
    // A0^T*A0 and A1^T*A1 are banded matrices, P contains the sample data, and
    // Q is the unknown matrix of control points.

    double dT;
    int i0, i1, i2, iMin, iMax;

    // Construct the matrices A0^T*A0 and A1^T*A1.
    BandedMatrixd* apkATAMat[2];
    for (iDim = 0; iDim < 2; iDim++)
    {
        apkATAMat[iDim] = WM4_NEW BandedMatrixd(m_aiControlQuantity[iDim],
            m_aiDegree[iDim]+1,m_aiDegree[iDim]+1);

        for (i0 = 0; i0 < m_aiControlQuantity[iDim]; i0++)
        {
            for (i1 = 0; i1 < i0; i1++)
            {
                (*apkATAMat[iDim])(i0,i1) = (*apkATAMat[iDim])(i1,i0);
            }

            int i1Max = i0 + m_aiDegree[iDim];
            if (i1Max >= m_aiControlQuantity[iDim])
            {
                i1Max = m_aiControlQuantity[iDim] - 1;
            }

            for (i1 = i0; i1 <= i1Max; i1++)
            {
                double dValue = 0.0;
                for (i2 = 0; i2 < m_aiSampleQuantity[iDim]; i2++)
                {
                    dT = adTMultiplier[iDim]*(double)i2;
                    apkDBasis[iDim]->Compute(dT,iMin,iMax);
                    if (iMin <= i0 && i0 <= iMax && iMin <= i1 && i1 <= iMax)
                    {
                        double dB0 = apkDBasis[iDim]->GetValue(i0 - iMin);
                        double dB1 = apkDBasis[iDim]->GetValue(i1 - iMin);
                        dValue += dB0*dB1;
                    }
                }
                (*apkATAMat[iDim])(i0,i1) = dValue;
            }
        }
    }

    // Construct the matrices A0^T and A1^T.
    double** aaadATMat[2];
    for (iDim = 0; iDim < 2; iDim++)
    {
        Allocate(m_aiSampleQuantity[iDim],m_aiControlQuantity[iDim],
            aaadATMat[iDim]);
        memset(aaadATMat[iDim][0],0,m_aiControlQuantity[iDim]
            *m_aiSampleQuantity[iDim]*sizeof(double));
        for (i0 = 0; i0 < m_aiControlQuantity[iDim]; i0++)
        {
            for (i1 = 0; i1 < m_aiSampleQuantity[iDim]; i1++)
            {
                dT = adTMultiplier[iDim]*(double)i1;
                apkDBasis[iDim]->Compute(dT,iMin,iMax);
                if (iMin <= i0 && i0 <= iMax)
                {
                    aaadATMat[iDim][i0][i1] =
                        apkDBasis[iDim]->GetValue(i0 - iMin);
                }
            }
        }
    }

    // Compute X0 = (A0^T*A0)^{-1}*A0^T and X1 = (A1^T*A1)^{-1}*A1^T by
    // solving the linear systems A0^T*A0*X0 = A0^T and A1^T*A1*X1 = A1^T.
    for (iDim = 0; iDim < 2; iDim++)
    {
        bool bSolved = apkATAMat[iDim]->SolveSystem(aaadATMat[iDim],
            m_aiSampleQuantity[iDim]);
        assert(bSolved);
        (void)bSolved;  // Avoid warning in release build.
    }

    // The control points for the fitted surface are stored in the matrix
    // Q = X0*P*X1^T, where P is the matrix of sample data.
    for (i0 = 0; i0 < m_aiControlQuantity[0]; i0++)
    {
        for (i1 = 0; i1 < m_aiControlQuantity[1]; i1++)
        {
            Vector3<Real> kSum = Vector3<Real>::ZERO;
            for (int j0 = 0; j0 < m_aiSampleQuantity[0]; j0++)
            {
                Real fX0Value = (Real)aaadATMat[0][i0][j0];
                for (int j1 = 0; j1 < m_aiSampleQuantity[1]; j1++)
                {
                    Real fX1Value = (Real)aaadATMat[1][i1][j1];
                    kSum += (fX0Value*fX1Value)*m_aakSamplePoint[j0][j1];
                }
            }
            m_aakControlPoint[i0][i1] = kSum;
        }
    }

    for (iDim = 0; iDim < 2; iDim++)
    {
        delete apkDBasis[iDim];
        delete apkATAMat[iDim];
        Deallocate(aaadATMat[iDim]);
    }
}
//----------------------------------------------------------------------------
template <class Real>
BSplineSurfaceFit<Real>::~BSplineSurfaceFit ()
{
    delete m_apkBasis[0];
    delete m_apkBasis[1];
    Deallocate(m_aakControlPoint);
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineSurfaceFit<Real>::GetSampleQuantity (int i) const
{
    assert(0 <= i && i < 2);
    return m_aiSampleQuantity[i];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real>** BSplineSurfaceFit<Real>::GetSamplePoints () const
{
    return m_aakSamplePoint;
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineSurfaceFit<Real>::GetDegree (int i) const
{
    assert(0 <= i && i < 2);
    return m_aiDegree[i];
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineSurfaceFit<Real>::GetControlQuantity (int i) const
{
    assert(0 <= i && i < 2);
    return m_aiControlQuantity[i];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real>** BSplineSurfaceFit<Real>::GetControlPoints () const
{
    return m_aakControlPoint;
}
//----------------------------------------------------------------------------
template <class Real>
const BSplineFitBasis<Real>& BSplineSurfaceFit<Real>::GetBasis (int i) const
{
    assert(0 <= i && i < 2);
    return *m_apkBasis[i];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineSurfaceFit<Real>::GetPosition (Real fU, Real fV) const
{
    int iUMin, iUMax, iVMin, iVMax;
    m_apkBasis[0]->Compute(fU,iUMin,iUMax);
    m_apkBasis[1]->Compute(fV,iVMin,iVMax);

    Vector3<Real> kPosition = Vector3<Real>::ZERO;
    for (int iU = iUMin, i = 0; iU <= iUMax; iU++, i++)
    {
        Real fValue0 = m_apkBasis[0]->GetValue(i);
        for (int iV = iVMin, j = 0; iV <= iVMax; iV++, j++)
        {
            Real fValue1 = m_apkBasis[1]->GetValue(j);
            kPosition += (fValue0*fValue1)*m_aakControlPoint[iU][iV];
        }
    }
    return kPosition;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class BSplineSurfaceFit<float>;

template WM4_FOUNDATION_ITEM
class BSplineSurfaceFit<double>;
//----------------------------------------------------------------------------
}
