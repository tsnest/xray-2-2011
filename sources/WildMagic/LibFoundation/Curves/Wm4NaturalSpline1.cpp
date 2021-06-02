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
// Version: 4.6.0 (2008/01/05)

#include "Wm4FoundationPCH.h"
#include "Wm4NaturalSpline1.h"
#include "Wm4LinearSystem.h"
#include "Wm4Matrix4.h"
#include "Wm4Polynomial1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
NaturalSpline1<Real>::NaturalSpline1 (bool bFree, int iNumSamples,
    Real* afTime, Real* afValue)
    :
    m_iNumSamples(iNumSamples),
    m_iNumSegments(iNumSamples - 1),
    m_afTime(afTime),
    m_afA(afValue)
{
    assert(iNumSamples >= 2 && afTime && afValue);
    if (bFree)
    {
        CreateFreeSpline();
    }
    else
    {
        CreatePeriodicSpline();
    }
}
//----------------------------------------------------------------------------
template <class Real>
NaturalSpline1<Real>::NaturalSpline1 (int iNumSamples, Real* afTime,
    Real* afValue, Real fSlopeFirst, Real fSlopeLast)
    :
    m_iNumSamples(iNumSamples),
    m_iNumSegments(iNumSamples - 1),
    m_afTime(afTime),
    m_afA(afValue)
{
    assert(iNumSamples >= 2 && afTime && afValue);
    CreateClampedSpline(fSlopeFirst,fSlopeLast);
}
//----------------------------------------------------------------------------
template <class Real>
NaturalSpline1<Real>::~NaturalSpline1 ()
{
    WM4_DELETE[] m_afTime;
    WM4_DELETE[] m_afA;
    WM4_DELETE[] m_afB;
    WM4_DELETE[] m_afC;
    WM4_DELETE[] m_afD;
}
//----------------------------------------------------------------------------
template <class Real>
Real NaturalSpline1<Real>::GetFunction (Real fTime) const
{
    int iKey;
    Real fDeltaTime;
    GetKeyInfo(fTime,iKey,fDeltaTime);

    Real fResult = m_afA[iKey] + fDeltaTime*(m_afB[iKey] +
        fDeltaTime*(m_afC[iKey] + fDeltaTime*m_afD[iKey]));

    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real NaturalSpline1<Real>::GetFirstDerivative (Real fTime) const
{
    int iKey;
    Real fDeltaTime;
    GetKeyInfo(fTime,iKey,fDeltaTime);

    Real fResult = m_afB[iKey] + fDeltaTime*(m_afC[iKey]*((Real)2.0) +
        m_afD[iKey]*(((Real)3.0)*fDeltaTime));

    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real NaturalSpline1<Real>::GetSecondDerivative (Real fTime) const
{
    int iKey;
    Real fDeltaTime;
    GetKeyInfo(fTime,iKey,fDeltaTime);

    Real fResult = m_afC[iKey]*((Real)2.0) +
        m_afD[iKey]*(((Real)6.0)*fDeltaTime);

    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real NaturalSpline1<Real>::GetThirdDerivative (Real fTime) const
{
    int iKey;
    Real fDeltaTime;
    GetKeyInfo(fTime,iKey,fDeltaTime);

    Real fResult = ((Real)6.0)*m_afD[iKey];

    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
int NaturalSpline1<Real>::GetNumSegments () const
{
    return m_iNumSegments;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* NaturalSpline1<Real>::GetA () const
{
    return m_afA;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* NaturalSpline1<Real>::GetB () const
{
    return m_afB;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* NaturalSpline1<Real>::GetC () const
{
    return m_afC;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* NaturalSpline1<Real>::GetD () const
{
    return m_afD;
}
//----------------------------------------------------------------------------
template <class Real>
void NaturalSpline1<Real>::CreateFreeSpline ()
{
    m_afB = WM4_NEW Real[m_iNumSegments];
    m_afC = WM4_NEW Real[m_iNumSegments + 1];
    m_afD = WM4_NEW Real[m_iNumSegments];

    Real* afDelta = WM4_NEW Real[m_iNumSamples];
    Real* afInvDelta = WM4_NEW Real[m_iNumSamples];
    Real* afFDeriv = WM4_NEW Real[m_iNumSamples];
    int i;
    for (i = 0; i < m_iNumSamples; i++)
    {
        afDelta[i] = m_afTime[i+1] - m_afTime[i];
        afInvDelta[i] = ((Real)1)/afDelta[i];
        afFDeriv[i] = (m_afA[i+1] - m_afA[i])*afInvDelta[i];
    }

    const int iNumSegmentsM1 = m_iNumSegments - 1;
    Real* afDiagonal = WM4_NEW Real[iNumSegmentsM1];
    Real* afRHS = WM4_NEW Real[iNumSegmentsM1];
    for (i = 0; i < iNumSegmentsM1; i++)
    {
        afDiagonal[i] = ((Real)2)*(afDelta[i+1] + afDelta[i]);
        afRHS[i] = ((Real)3)*(afFDeriv[i+1] - afFDeriv[i]);
    }

    // The boundary conditions.
    m_afC[0] = (Real)0;
    m_afC[m_iNumSegments] = (Real)0;

    // The linear system that determines C[1] through C[numSegs-1].
    bool bSolved = LinearSystem<Real>().SolveTri(iNumSegmentsM1,&afDelta[1],
        afDiagonal,&afDelta[1],afRHS,&m_afC[1]);
    assert(bSolved);
    (void)bSolved;  // avoid release build warning

    const Real fOneThird = ((Real)1)/(Real)3;
    for (i = 0; i < m_iNumSegments; i++)
    {
        m_afB[i] = afFDeriv[i] - afDelta[i]*fOneThird*(m_afC[i+1] +
            ((Real)2)*m_afC[i]);

        m_afD[i] = fOneThird*(m_afC[i+1] - m_afC[i])*afInvDelta[i];
    }

    WM4_DELETE[] afDelta;
    WM4_DELETE[] afInvDelta;
    WM4_DELETE[] afFDeriv;
    WM4_DELETE[] afDiagonal;
    WM4_DELETE[] afRHS;
}
//----------------------------------------------------------------------------
template <class Real>
void NaturalSpline1<Real>::CreateClampedSpline (Real fSlopeFirst,
    Real fSlopeLast)
{
    m_afB = WM4_NEW Real[m_iNumSegments + 1];
    m_afC = WM4_NEW Real[m_iNumSegments];
    m_afD = WM4_NEW Real[m_iNumSegments];

    Real* afDelta = WM4_NEW Real[m_iNumSamples];
    Real* afInvDelta = WM4_NEW Real[m_iNumSamples];
    Real* afFDeriv = WM4_NEW Real[m_iNumSamples];
    int i;
    for (i = 0; i < m_iNumSamples; i++)
    {
        afDelta[i] = m_afTime[i+1] - m_afTime[i];
        afInvDelta[i] = ((Real)1)/afDelta[i];
        afFDeriv[i] = (m_afA[i+1] - m_afA[i])*afInvDelta[i];
    }

    const int iNumSegmentsM1 = m_iNumSegments - 1;
    Real* afDiagonal = WM4_NEW Real[iNumSegmentsM1];
    Real* afRHS = WM4_NEW Real[iNumSegmentsM1];
    for (i = 0; i < iNumSegmentsM1; i++)
    {
        afDiagonal[i] = ((Real)2)*(afDelta[i+1] + afDelta[i]);
        afRHS[i] = ((Real)3)*(afDelta[i]*afFDeriv[i+1] +
            afDelta[i+1]*afFDeriv[i]);
    }
    afRHS[0] -= fSlopeFirst*afDelta[1];
    afRHS[m_iNumSegments-2] -= fSlopeLast*afDelta[m_iNumSegments-2];

    // The boundary conditions.
    m_afB[0] = fSlopeFirst;
    m_afB[m_iNumSegments] = fSlopeLast;

    // The linear system that determines B[1] through B[numSegs-1].
    bool bSolved = LinearSystem<Real>().SolveTri(iNumSegmentsM1,&afDelta[2],
        afDiagonal,afDelta,afRHS,&m_afB[1]);
    assert(bSolved);
    (void)bSolved;  // avoid release build warning

    const Real fOneThird = ((Real)1)/(Real)3;
    for (i = 0; i < m_iNumSegments; i++)
    {
        m_afC[i] = (((Real)3)*afFDeriv[i] - m_afB[i+1] -
            ((Real)2)*m_afB[i])*afInvDelta[i];

        m_afD[i] = fOneThird*(m_afB[i+1] - m_afB[i] -
            ((Real)2)*afDelta[i]*m_afC[i])*afInvDelta[i]*afInvDelta[i];
    }

    WM4_DELETE[] afDelta;
    WM4_DELETE[] afInvDelta;
    WM4_DELETE[] afFDeriv;
    WM4_DELETE[] afDiagonal;
    WM4_DELETE[] afRHS;
}
//----------------------------------------------------------------------------
template <class Real>
void NaturalSpline1<Real>::CreatePeriodicSpline ()
{
    m_afB = WM4_NEW Real[m_iNumSegments];
    m_afC = WM4_NEW Real[m_iNumSegments];
    m_afD = WM4_NEW Real[m_iNumSegments];

#if 1
    // Solving the system using a standard linear solver appears to be
    // numerically stable.
    const int iSize = 4*m_iNumSegments;
    GMatrix<Real> kMat(iSize,iSize);
    GVector<Real> kRhs(iSize);
    int i, j, k;
    Real fDelta, fDelta2, fDelta3;
    for (i = 0, j = 0; i < m_iNumSegments-1; i++, j += 4)
    {
        fDelta = m_afTime[i+1] - m_afTime[i];
        fDelta2 = fDelta*fDelta;
        fDelta3 = fDelta*fDelta2;

        kMat[j+0][j+0] = (Real)1;
        kMat[j+0][j+1] = (Real)0;
        kMat[j+0][j+2] = (Real)0;
        kMat[j+0][j+3] = (Real)0;
        kMat[j+1][j+0] = (Real)1;
        kMat[j+1][j+1] = fDelta;
        kMat[j+1][j+2] = fDelta2;
        kMat[j+1][j+3] = fDelta3;
        kMat[j+2][j+0] = (Real)0;
        kMat[j+2][j+1] = (Real)1;
        kMat[j+2][j+2] = ((Real)2)*fDelta;
        kMat[j+2][j+3] = ((Real)3)*fDelta2;
        kMat[j+3][j+0] = (Real)0;
        kMat[j+3][j+1] = (Real)0;
        kMat[j+3][j+2] = (Real)1;
        kMat[j+3][j+3] = ((Real)3)*fDelta;

        k = j + 4;
        kMat[j+0][k+0] = (Real)0;
        kMat[j+0][k+1] = (Real)0;
        kMat[j+0][k+2] = (Real)0;
        kMat[j+0][k+3] = (Real)0;
        kMat[j+1][k+0] = (Real)-1;
        kMat[j+1][k+1] = (Real)0;
        kMat[j+1][k+2] = (Real)0;
        kMat[j+1][k+3] = (Real)0;
        kMat[j+2][k+0] = (Real)0;
        kMat[j+2][k+1] = (Real)-1;
        kMat[j+2][k+2] = (Real)0;
        kMat[j+2][k+3] = (Real)0;
        kMat[j+3][k+0] = (Real)0;
        kMat[j+3][k+1] = (Real)0;
        kMat[j+3][k+2] = (Real)-1;
        kMat[j+3][k+3] = (Real)0;
    }

    fDelta = m_afTime[i+1] - m_afTime[i];
    fDelta2 = fDelta*fDelta;
    fDelta3 = fDelta*fDelta2;

    kMat[j+0][j+0] = (Real)1;
    kMat[j+0][j+1] = (Real)0;
    kMat[j+0][j+2] = (Real)0;
    kMat[j+0][j+3] = (Real)0;
    kMat[j+1][j+0] = (Real)1;
    kMat[j+1][j+1] = fDelta;
    kMat[j+1][j+2] = fDelta2;
    kMat[j+1][j+3] = fDelta3;
    kMat[j+2][j+0] = (Real)0;
    kMat[j+2][j+1] = (Real)1;
    kMat[j+2][j+2] = ((Real)2)*fDelta;
    kMat[j+2][j+3] = ((Real)3)*fDelta2;
    kMat[j+3][j+0] = (Real)0;
    kMat[j+3][j+1] = (Real)0;
    kMat[j+3][j+2] = (Real)1;
    kMat[j+3][j+3] = ((Real)3)*fDelta;

    k = 0;
    kMat[j+0][k+0] = (Real)0;
    kMat[j+0][k+1] = (Real)0;
    kMat[j+0][k+2] = (Real)0;
    kMat[j+0][k+3] = (Real)0;
    kMat[j+1][k+0] = (Real)-1;
    kMat[j+1][k+1] = (Real)0;
    kMat[j+1][k+2] = (Real)0;
    kMat[j+1][k+3] = (Real)0;
    kMat[j+2][k+0] = (Real)0;
    kMat[j+2][k+1] = (Real)-1;
    kMat[j+2][k+2] = (Real)0;
    kMat[j+2][k+3] = (Real)0;
    kMat[j+3][k+0] = (Real)0;
    kMat[j+3][k+1] = (Real)0;
    kMat[j+3][k+2] = (Real)-1;
    kMat[j+3][k+3] = (Real)0;

    for (i = 0, j = 0; i < m_iNumSegments; i++, j += 4)
    {
        kRhs[j+0] = m_afA[i];
        kRhs[j+1] = (Real)0;
        kRhs[j+2] = (Real)0;
        kRhs[j+3] = (Real)0;
    }

    GVector<Real> kCoeff(iSize);
    bool bSolved = LinearSystem<Real>().Solve(kMat, kRhs, kCoeff);
    assert(bSolved);
    (void)bSolved;  // avoid warning in release builds

    for (i = 0, j = 0; i < m_iNumSegments; i++)
    {
        j++;
        m_afB[i] = kCoeff[j++];
        m_afC[i] = kCoeff[j++];
        m_afD[i] = kCoeff[j++];
    }
#endif

#if 0
    // Solving the system using the equations derived in the PDF
    // "Fitting a Natural Spline to Samples of the Form (t,f(t))"
    // is ill-conditioned.  TODO: Find a way to row-reduce the matrix of the
    // PDF in a numerically stable manner yet retaining the O(n) asymptotic
    // behavior.

    // Compute the inverses M[i]^{-1}.
    const int iNumSegmentsM1 = m_iNumSegments - 1;
    Matrix4<Real>* akInvM = WM4_NEW Matrix4<Real>[iNumSegmentsM1];
    Real fDelta;
    int i;
    for (i = 0; i < iNumSegmentsM1; i++)
    {
        fDelta = m_afTime[i+1] - m_afTime[i];
        Real fInvDelta1 = ((Real)1)/fDelta;
        Real fInvDelta2 = fInvDelta1/fDelta;
        Real fInvDelta3 = fInvDelta2/fDelta;

        Matrix4<Real>& rkInvM = akInvM[i];
        rkInvM[0][0] = (Real)1;
        rkInvM[0][1] = (Real)0;
        rkInvM[0][2] = (Real)0;
        rkInvM[0][3] = (Real)0;
        rkInvM[1][0] = ((Real)(-3))*fInvDelta1;
        rkInvM[1][1] = ((Real)3)*fInvDelta1;
        rkInvM[1][2] = (Real)(-2);
        rkInvM[1][3] = fDelta;
        rkInvM[2][0] = ((Real)3)*fInvDelta2;
        rkInvM[2][1] = ((Real)(-3))*fInvDelta2;
        rkInvM[2][2] = ((Real)3)*fInvDelta1;
        rkInvM[2][3] = (Real)(-2);
        rkInvM[3][0] = -fInvDelta3;
        rkInvM[3][1] = fInvDelta3;
        rkInvM[3][2] = -fInvDelta2;
        rkInvM[3][3] = fInvDelta1;
    }

    // Matrix M[n-1].
    fDelta = m_afTime[i+1] - m_afTime[i];
    Real fDelta2 = fDelta*fDelta;
    Real fDelta3 = fDelta2*fDelta;
    Matrix4<Real> kLastM
    (
        (Real)1, (Real)0, (Real)0, (Real)0,
        (Real)1, fDelta, fDelta2, fDelta3,
        (Real)0, (Real)1, ((Real)2)*fDelta, ((Real)3)*fDelta2,
        (Real)0, (Real)0, (Real)1, ((Real)3)*fDelta
    );

    // Matrix L.
    Matrix4<Real> kL
    (
        (Real)0, (Real)0, (Real)0, (Real)0,
        (Real)1, (Real)0, (Real)0, (Real)0,
        (Real)0, (Real)1, (Real)0, (Real)0,
        (Real)0, (Real)0, (Real)1, (Real)0
    );

    // Vector U.
    Vector4<Real> kU((Real)1,(Real)0,(Real)0,(Real)0);

    // Initialize P = L and Q = f[n-2]*U.
    Matrix4<Real> kP = kL;

    const int iNumSegmentsM2 = m_iNumSegments - 2;
    Vector4<Real> kQ = m_afA[iNumSegmentsM2]*kU;

    // Compute P and Q.
    for (i = iNumSegmentsM2; i >= 0; i--)
    {
        // Matrix L*M[i]^{-1}.
        Matrix4<Real> kLMInv = kL*akInvM[i];

        // Update P.
        kP = kLMInv*kP;

        // Update Q.
        if (i > 0)
        {
            kQ = m_afA[i-1]*kU + kLMInv*kQ;
        }
        else
        {
            kQ = m_afA[iNumSegmentsM1]*kU + kLMInv*kQ;
        }
    }

    // Final update of P.
    kP = kLastM - kP;

    // Compute P^{-1}.
    Matrix4<Real> kInvP = kP.Inverse();

    // Compute K[n-1].
    Vector4<Real> kCoeff = kInvP*kQ;
    m_afB[iNumSegmentsM1] = kCoeff[1];
    m_afC[iNumSegmentsM1] = kCoeff[2];
    m_afD[iNumSegmentsM1] = kCoeff[3];

    // Back substitution for the other K[i].
    for (i = iNumSegmentsM2; i >= 0; i--)
    {
        kCoeff = akInvM[i]*(m_afA[i]*kU + kL*kCoeff);
        m_afB[i] = kCoeff[1];
        m_afC[i] = kCoeff[2];
        m_afD[i] = kCoeff[3];
    }

    WM4_DELETE[] akInvM;
#endif
}
//----------------------------------------------------------------------------
template <class Real>
void NaturalSpline1<Real>::GetKeyInfo (Real fTime, int& riKey,
    Real& rfDeltaTime) const
{
    if (fTime <= m_afTime[0])
    {
        riKey = 0;
        rfDeltaTime = (Real)0.0;
    }
    else if (fTime >= m_afTime[m_iNumSegments])
    {
        riKey = m_iNumSegments - 1;
        rfDeltaTime = m_afTime[m_iNumSegments] - m_afTime[riKey];
    }
    else
    {
        for (int i = 0; i < m_iNumSegments; i++)
        {
            if (fTime < m_afTime[i+1])
            {
                riKey = i;
                rfDeltaTime = fTime - m_afTime[i];
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class NaturalSpline1<float>;

template WM4_FOUNDATION_ITEM
class NaturalSpline1<double>;
//----------------------------------------------------------------------------
}
