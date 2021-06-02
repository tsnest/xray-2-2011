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
#include "Wm4RiemannianGeodesic.h"
#include "Wm4LinearSystem.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
RiemannianGeodesic<Real>::RiemannianGeodesic (int iDimension)
    :
    m_kMetric(iDimension,iDimension),
    m_kMetricInverse(iDimension,iDimension)
{
    assert(iDimension >= 2);
    m_iDimension = 2;

    m_akChristoffel1 = WM4_NEW GMatrix<Real>[m_iDimension];
    m_akChristoffel2 = WM4_NEW GMatrix<Real>[m_iDimension];
    m_akMetricDerivative = WM4_NEW GMatrix<Real>[m_iDimension];
    for (int i = 0; i < m_iDimension; i++)
    {
        m_akChristoffel1[i].SetSize(m_iDimension,m_iDimension);
        m_akChristoffel2[i].SetSize(m_iDimension,m_iDimension);
        m_akMetricDerivative[i].SetSize(m_iDimension,m_iDimension);
    }
    m_bMetricInverseExists = true;

    IntegralSamples = 16;
    SearchSamples = 32;
    DerivativeStep = (Real)1e-04;
    Subdivisions = 7;
    Refinements = 8;
    SearchRadius = (Real)1.0;

    m_fIntegralStep = ((Real)1.0)/(Real)(IntegralSamples-1);
    m_fSearchStep = ((Real)1.0)/(Real)SearchSamples;
    m_fDerivativeFactor = (Real)0.5/DerivativeStep;

    RefineCallback = 0;
    m_iSubdivide = 0;
    m_iRefine = 0;
    m_iCurrentQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
RiemannianGeodesic<Real>::~RiemannianGeodesic ()
{
}
//----------------------------------------------------------------------------
template <class Real>
int RiemannianGeodesic<Real>::GetDimension () const
{
    return m_iDimension;
}
//----------------------------------------------------------------------------
template <class Real>
Real RiemannianGeodesic<Real>::ComputeSegmentLength (
    const GVector<Real>& rkPoint0, const GVector<Real>& rkPoint1)
{
    // The Trapezoid Rule is used for integration of the length integral.

    GVector<Real> kDiff = rkPoint1 - rkPoint0;
    GVector<Real> kTemp(m_iDimension);

    // Evaluate the integrand at point0.
    ComputeMetric(rkPoint0);
    Real fQForm = m_kMetric.QForm(kDiff,kDiff);
    assert(fQForm > (Real)0.0);
    Real fLength = Math<Real>::Sqrt(fQForm);

    // Evaluate the integrand at point1.
    ComputeMetric(rkPoint1);
    fQForm = m_kMetric.QForm(kDiff,kDiff);
    assert(fQForm > (Real)0.0);
    fLength += Math<Real>::Sqrt(fQForm);
    fLength *= (Real)0.5;

    int iMax = IntegralSamples - 2;
    for (int i = 1; i <= iMax; i++)
    {
        // Evaluate the integrand at point0+t*(point1-point0).
        Real fT = m_fIntegralStep * (Real)i;
        kTemp = rkPoint0 + fT*kDiff;
        ComputeMetric(kTemp);
        fQForm = m_kMetric.QForm(kDiff,kDiff);
        assert(fQForm > (Real)0.0);
        fLength += Math<Real>::Sqrt(fQForm);
    }
    fLength *= m_fIntegralStep;

    return fLength;
}
//----------------------------------------------------------------------------
template <class Real>
Real RiemannianGeodesic<Real>::ComputeTotalLength (int iQuantity,
    const GVector<Real>* akPath)
{
    assert(iQuantity >= 2);

    Real fLength = ComputeSegmentLength(akPath[0],akPath[1]);
    for (int i = 1; i <= iQuantity-2; i++)
    {
        fLength += ComputeSegmentLength(akPath[i],akPath[i+1]);
    }
    return fLength;
}
//----------------------------------------------------------------------------
template <class Real>
void RiemannianGeodesic<Real>::ComputeGeodesic (const GVector<Real>& rkPoint0,
    const GVector<Real>& rkPoint1, int& riQuantity, GVector<Real>*& rakPath)
{
    assert(Subdivisions < 32);
    riQuantity = (1 << Subdivisions) + 1;

    rakPath = WM4_NEW GVector<Real>[riQuantity];
    int i;
    for (i = 0; i < riQuantity; i++)
    {
        rakPath[i].SetSize(m_iDimension);
    }

    m_iCurrentQuantity = 2;
    rakPath[0] = rkPoint0;
    rakPath[1] = rkPoint1;

    for (m_iSubdivide = 1; m_iSubdivide <= Subdivisions; m_iSubdivide++)
    {
        // A subdivision essentially doubles the number of points.
        int iNewQuantity = 2*m_iCurrentQuantity-1;
        assert(iNewQuantity <= riQuantity);

        // Copy the old points so that there are slots for the midpoints
        // during the subdivision, the slots interleaved between the old
        // points.
        for (i = m_iCurrentQuantity-1; i > 0; i--)
        {
            rakPath[2*i] = rakPath[i];
        }

        // Subdivide the polyline.
        for (i = 0; i <= m_iCurrentQuantity-2; i++)
        {
            Subdivide(rakPath[2*i],rakPath[2*i+1],rakPath[2*i+2]);
        }

        m_iCurrentQuantity = iNewQuantity;

        // Refine the current polyline vertices.
        for (m_iRefine = 1; m_iRefine <= Refinements; m_iRefine++)
        {
            for (i = 1; i <= m_iCurrentQuantity-2; i++)
            {
                Refine(rakPath[i-1],rakPath[i],rakPath[i+1]);
            }
        }
    }

    assert(m_iCurrentQuantity == riQuantity);
    m_iSubdivide = 0;
    m_iRefine = 0;
    m_iCurrentQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool RiemannianGeodesic<Real>::Subdivide (const GVector<Real>& rkEnd0,
    GVector<Real>& rkMid, const GVector<Real>& rkEnd1)
{
    rkMid = ((Real)0.5)*(rkEnd0 + rkEnd1);
    RefineCallbackFunction oSave = RefineCallback;
    RefineCallback = 0;
    bool bChanged = Refine(rkEnd0,rkMid,rkEnd1);
    RefineCallback = oSave;
    return bChanged;
}
//----------------------------------------------------------------------------
template <class Real>
bool RiemannianGeodesic<Real>::Refine (const GVector<Real>& rkEnd0,
     GVector<Real>& rkMid, const GVector<Real>& rkEnd1)
{
    // Estimate the gradient vector for F(m) = Length(e0,m) + Length(m,e1).
    GVector<Real> kTemp = rkMid, kGradient(m_iDimension);
    int i;
    for (i = 0; i < m_iDimension; i++)
    {
        kTemp[i] = rkMid[i] + DerivativeStep;
        kGradient[i] = ComputeSegmentLength(rkEnd0,kTemp);
        kGradient[i] += ComputeSegmentLength(kTemp,rkEnd1);

        kTemp[i] = rkMid[i] - DerivativeStep;
        kGradient[i] -= ComputeSegmentLength(rkEnd0,kTemp);
        kGradient[i] -= ComputeSegmentLength(kTemp,rkEnd1);

        kTemp[i] = rkMid[i];
        kGradient[i] *= m_fDerivativeFactor;
    }

    // Compute the length sum for the current midpoint.
    Real fLength0 = ComputeSegmentLength(rkEnd0,rkMid);
    Real fLength1 = ComputeSegmentLength(rkMid,rkEnd1);
    Real fOldLength = fLength0 + fLength1;

    Real fTRay, fNewLength;
    GVector<Real> kPRay(m_iDimension);

    Real fMultiplier = m_fSearchStep*SearchRadius;
    Real fMinLength = fOldLength;
    GVector<Real> kMinPoint = rkMid;
    int iMinIndex = 0;
    for (i = -SearchSamples; i <= SearchSamples; i++)
    {
        fTRay = fMultiplier*(Real)i;
        kPRay = rkMid - fTRay*kGradient;
        fLength0 = ComputeSegmentLength(rkEnd0,kPRay);
        fLength1 = ComputeSegmentLength(rkEnd1,kPRay);
        fNewLength = fLength0 + fLength1;
        if (fNewLength < fMinLength)
        {
            fMinLength = fNewLength;
            kMinPoint = kPRay;
            iMinIndex = i;
        }
    }

    rkMid = kMinPoint;

    if (RefineCallback)
    {
        RefineCallback();
    }

    return fMinLength < fOldLength;
}
//----------------------------------------------------------------------------
template <class Real>
int RiemannianGeodesic<Real>::GetSubdivisionStep () const
{
    return m_iSubdivide;
}
//----------------------------------------------------------------------------
template <class Real>
int RiemannianGeodesic<Real>::GetRefinementStep () const
{
    return m_iRefine;
}
//----------------------------------------------------------------------------
template <class Real>
int RiemannianGeodesic<Real>::GetCurrentQuantity () const
{
    return m_iCurrentQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
Real RiemannianGeodesic<Real>::ComputeSegmentCurvature (
    const GVector<Real>& rkPoint0, const GVector<Real>& rkPoint1)
{
    // The Trapezoid Rule is used for integration of the curvature integral.

    GVector<Real> kDiff = rkPoint1 - rkPoint0;
    GVector<Real> kTemp(m_iDimension);

    // Evaluate the integrand at point0.
    Real fCurvature = ComputeIntegrand(rkPoint0,kDiff);

    // Evaluate the integrand at point1.
    fCurvature += ComputeIntegrand(rkPoint1,kDiff);
    fCurvature *= (Real)0.5;

    int iMax = IntegralSamples - 2;
    for (int i = 1; i <= iMax; i++)
    {
        // Evaluate the integrand at point0+t*(point1-point0).
        Real fT = m_fIntegralStep * (Real)i;
        kTemp = rkPoint0 + fT*kDiff;
        fCurvature += ComputeIntegrand(kTemp,kDiff);
    }
    fCurvature *= m_fIntegralStep;

    return fCurvature;
}
//----------------------------------------------------------------------------
template <class Real>
Real RiemannianGeodesic<Real>::ComputeTotalCurvature (int iQuantity,
    const GVector<Real>* akPath)
{
    assert(iQuantity >= 2);

    Real fCurvature = ComputeSegmentCurvature(akPath[0],akPath[1]);
    for (int i = 1; i <= iQuantity-2; i++)
    {
        fCurvature += ComputeSegmentCurvature(akPath[i],akPath[i+1]);
    }
    return fCurvature;
}
//----------------------------------------------------------------------------
template <class Real>
Real RiemannianGeodesic<Real>::ComputeIntegrand (const GVector<Real>& rkPos,
    const GVector<Real>& rkDer)
{
    ComputeMetric(rkPos);
    ComputeChristoffel1(rkPos);
    ComputeMetricInverse();
    ComputeChristoffel2();

    // g_{ij}*der_{i}*der_{j}
    Real fQForm0 = m_kMetric.QForm(rkDer,rkDer);
    assert(fQForm0 > (Real)0.0);

    // gamma_{kij}*der_{k}*der_{i}*der_{j}
    GMatrix<Real> kMat(m_iDimension,m_iDimension);
    int k;
    for (k = 0; k < m_iDimension; k++)
    {
        kMat += rkDer[k]*m_akChristoffel1[k];
    }
    Real fQForm1 = kMat.QForm(rkDer,rkDer);

    Real fRatio = -fQForm1/fQForm0;

    // Compute the acceleration.
    GVector<Real> kAcc = fRatio*rkDer;
    for (k = 0; k < m_iDimension; k++)
    {
        kAcc[k] += m_akChristoffel2[k].QForm(rkDer,rkDer);
    }

    // Compute the curvature.
    Real fCurvature = Math<Real>::Sqrt(m_kMetric.QForm(kAcc,kAcc));
    return fCurvature;
}
//----------------------------------------------------------------------------
template <class Real>
bool RiemannianGeodesic<Real>::ComputeMetricInverse ()
{
    m_bMetricInverseExists = LinearSystem<Real>().Inverse(m_kMetric,
        m_kMetricInverse);

    return m_bMetricInverseExists;
}
//----------------------------------------------------------------------------
template <class Real>
void RiemannianGeodesic<Real>::ComputeMetricDerivative ()
{
    for (int iDerivative = 0; iDerivative < m_iDimension; iDerivative++)
    {
        for (int i0 = 0; i0 < m_iDimension; i0++)
        {
            for (int i1 = 0; i1 < m_iDimension; i1++)
            {
                m_akMetricDerivative[iDerivative][i0][i1] =
                    m_akChristoffel1[iDerivative][i0][i1] +
                    m_akChristoffel1[iDerivative][i1][i0];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool RiemannianGeodesic<Real>::ComputeChristoffel2 ()
{
    for (int i2 = 0; i2 < m_iDimension; i2++)
    {
        for (int i0 = 0; i0 < m_iDimension; i0++)
        {
            for (int i1 = 0; i1 < m_iDimension; i1++)
            {
                Real fValue = (Real)0.0;
                for (int j = 0; j < m_iDimension; j++)
                {
                    fValue += m_kMetricInverse[i2][j] *
                        m_akChristoffel1[j][i0][i1];
                }
                m_akChristoffel2[i2][i0][i1] = fValue;
            }
        }
    }

    return m_bMetricInverseExists;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class RiemannianGeodesic<float>;

template WM4_FOUNDATION_ITEM
class RiemannianGeodesic<double>;
//----------------------------------------------------------------------------
}
