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

#ifndef WM4NATURALSPLINE1_H
#define WM4NATURALSPLINE1_H

#include "Wm4FoundationLIB.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM NaturalSpline1
{
public:
    // Construction and destruction.  NaturalSpline1 accepts responsibility
    // for deleting the input arrays.  Each input array must have iNumSamples
    // elements, where iNumSamples >= 2 is required.  If f(t) is the function
    // to be fit, then afValue[i] = f(afTime[i]) is the sample.  The time
    // samples must be increasing:  afTime[i+1] > afTime[i] for all i.  The
    // spline curve is referred to as S(t).  In the comments, tmin refers to
    // afTime[0] and tmax refers to afTime[iNumSamples-1].

    // Set bFree to 'true' for free splines or to 'false' for periodic
    // splines.  In the periodic case, the implementation uses f(tmin) for
    // computing coefficients.  It does not use f(tmax), which is assumed to
    // be equal to f(tmin).
    //
    // Free:  S"(tmin) = 0,  S"(tmax) = 0
    // Periodic:  S(tmin) = S(tmax), S'(tmin) = S'(tmax), S"(tmin) = S"(tmax)
    NaturalSpline1 (bool bFree, int iNumSamples, Real* afTime, Real* afValue);

    // Clamped:  S'(tmin) = slopeFirst, S'(tmax) = slopeLast
    NaturalSpline1 (int iNumSamples, Real* afTime, Real* afValue,
        Real fSlopeFirst, Real fSlopeLast);

    ~NaturalSpline1 ();

    // Evaluators for S(t), S'(t), S''(t), and S'''(t).
    Real GetFunction (Real fTime) const;
    Real GetFirstDerivative (Real fTime) const;
    Real GetSecondDerivative (Real fTime) const;
    Real GetThirdDerivative (Real fTime) const;

    // Access the coefficients of the polynomials.
    int GetNumSegments () const;
    const Real* GetA () const;
    const Real* GetB () const;
    const Real* GetC () const;
    const Real* GetD () const;

private:
    void CreateFreeSpline ();
    void CreateClampedSpline (Real fSlopeFirst, Real fSlopeLast);
    void CreatePeriodicSpline ();

    void GetKeyInfo (Real fTime, int& riKey, Real& rfDeltaTime) const;

    int m_iNumSamples, m_iNumSegments;
    Real* m_afTime;

    // The cubic polynomial coefficients.  All arrays have
    // m_iNumSegments elements.  The i-th polynomial is
    // S_i(t) =
    //   m_afA[i] +
    //   m_afB[i]*(t - m_afTime[i]) +
    //   m_afC[i]*(t - m_afTime[i])^2 +
    //   m_afD[i]*(t - m_afTime[i])^3
    Real* m_afA;
    Real* m_afB;
    Real* m_afC;
    Real* m_afD;
};

typedef NaturalSpline1<float> NaturalSpline1f;
typedef NaturalSpline1<double> NaturalSpline1d;

}

#endif
