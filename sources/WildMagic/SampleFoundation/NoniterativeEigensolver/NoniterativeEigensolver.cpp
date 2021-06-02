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
// Version: 4.0.0 (2006/10/09)

#include "NoniterativeEigensolver.h"
#include "Wm4NoniterativeEigen3x3.h"
using namespace Wm4;

WM4_CONSOLE_APPLICATION(NoniterativeEigensolver);

// Enable only one of these.  You should run all of these in release mode.
#define MEASURE_NONITERATIVE
//#define TIMING_NONITERATIVE
//#define TIMING_ITERATIVE

//----------------------------------------------------------------------------
int NoniterativeEigensolver::Main (int, char**)
{
#ifdef MEASURE_NONITERATIVE
    float fMaxIError = 0.0f, fMaxNError = 0.0f;
    int iIErrorIndex = -1, iNErrorIndex = -1;
    float fMinIDet = 1.0f, fMinNDet = 1.0f;
    int iIDetIndex = -1, iNDetIndex = -1;
    const int iMax = (1 << 28);
    std::ofstream kDbOStr("measures.txt");
#endif

#if defined(TIMING_ITERATIVE) || defined(TIMING_NONITERATIVE)
    const int iMax = (1 << 24);
    clock_t lStart = clock();
#endif

    for (int i = 0, iType = 0; i < iMax; i++)
    {
        Vector3f kAxis = Vector3f(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(),Mathf::SymmetricRandom());
        kAxis.Normalize();
        float fAngle = Mathf::IntervalRandom(0.0f,Mathf::PI);
        Matrix3f kRot(kAxis,fAngle);
        float fD0, fD1, fD2;

        if (iType == 0)
        {
            // d0 = d1 = d2
            fD1 = Mathf::IntervalRandom(-1.0f,1.0f);
            fD0 = fD1;
            fD2 = fD1;
        }
        else if (iType == 1)
        {
            // d0 = d1 < d2
            fD1 = Mathf::IntervalRandom(-1.0f,1.0f);
            fD0 = fD1;
            fD2 = Mathf::IntervalRandom(0.5f*(fD1+1.0f),1.0f);
        }
        else if (iType == 2)
        {
            // d0 < d1 = d2
            fD1 = Mathf::IntervalRandom(-1.0f,1.0f);
            fD0 = Mathf::IntervalRandom(-1.0f,0.5f*(fD1-1.0f));
            fD2 = fD1;
        }
        else
        {
            // d0 < d1 < d2
            fD1 = Mathf::IntervalRandom(-1.0f,1.0f);
            fD0 = Mathf::IntervalRandom(-1.0f,0.5f*(fD1-1.0f));
            fD2 = Mathf::IntervalRandom(0.5f*(fD1+1.0f),1.0f);
        }

        Matrix3f kA = kRot.TransposeTimes(Matrix3f(fD0,fD1,fD2)*kRot);

#ifdef MEASURE_NONITERATIVE
        float fIError, fNError, fIDet, fNDet;
        SolveAndMeasure(kA,fIError,fNError,fIDet,fNDet);

        if (fIError > fMaxIError)
        {
            fMaxIError = fIError;
            iIErrorIndex = i;
        }

        if (fNError > fMaxNError)
        {
            fMaxNError = fNError;
            iNErrorIndex = i;
        }

        if (fIDet < fMinIDet)
        {
            fMinIDet = fIDet;
            iIDetIndex = i;
        }

        if (fNDet < fMinNDet)
        {
            fMinNDet = fNDet;
            iNDetIndex = i;
        }
#endif

#ifdef TIMING_ITERATIVE
        Eigenf kIES(kA);
        kIES.IncrSortEigenStuff3();
#endif

#ifdef TIMING_NONITERATIVE
        NoniterativeEigen3x3f kNES(kA);
#endif

        if (++iType == 4)
        {
            iType = 0;
        }
    }

#ifdef MEASURE_NONITERATIVE
    kDbOStr << "iterations = " << iMax << std::endl;
    kDbOStr << "max iterative error = " << fMaxIError << std::endl;
    kDbOStr << "max noniterative error = " << fMaxNError << std::endl;
    kDbOStr << "min iterative determinant = " << fMinIDet << std::endl;
    kDbOStr << "min noniterative determinant = " << fMinNDet << std::endl;
    kDbOStr.close();
#endif

#if defined(TIMING_ITERATIVE) || defined(TIMING_NONITERATIVE)
    clock_t lFinal = clock();
    long lDiff = lFinal - lStart;
#endif

#ifdef TIMING_ITERATIVE
    std::ofstream kItOStr("iterative.txt");
    kItOStr << "ticks = " << lDiff << std::endl;
    kItOStr.close();
#endif

#ifdef TIMING_NONITERATIVE
    std::ofstream kNonOStr("noniterative.txt");
    kNonOStr << "ticks = " << lDiff << std::endl;
    kNonOStr.close();
#endif

    return 0;
}
//----------------------------------------------------------------------------
void NoniterativeEigensolver::SolveAndMeasure (const Matrix3f& rkA,
    float& rfIError, float& rfNError, float& rfIDet, float& rfNDet)
{
    int i;
    Vector3f kResult;
    float fLength;
    rfIError = 0.0f;
    rfNError = 0.0f;

    // Iterative eigensolver.
    Eigenf kIES(rkA);
    kIES.IncrSortEigenStuff3();
    for (i = 0; i < 3; i++)
    {
        m_afIEValue[i] = kIES.GetEigenvalue(i);
        kIES.GetEigenvector(i,m_akIEVector[i]);
        kResult = rkA*m_akIEVector[i] - m_afIEValue[i]*m_akIEVector[i];
        fLength = kResult.Length();
        if (fLength > rfIError)
        {
            rfIError = fLength;
        }
    }
    rfIDet = Mathf::FAbs(
        m_akIEVector[0].Dot(m_akIEVector[1].Cross(m_akIEVector[2])));

    // Bounded-time eigensolver.
    NoniterativeEigen3x3f kNES(rkA);
    for (i = 0; i < 3; i++)
    {
        m_afNEValue[i] = kNES.GetEigenvalue(i);
        m_akNEVector[i] = kNES.GetEigenvector(i);
        kResult = rkA*m_akNEVector[i] - m_afNEValue[i]*m_akNEVector[i];
        fLength = kResult.Length();
        if (fLength > rfNError)
        {
            rfNError = fLength;
        }
    }
    rfNDet = Mathf::FAbs(
        m_akNEVector[0].Dot(m_akNEVector[1].Cross(m_akNEVector[2])));
}
//----------------------------------------------------------------------------
