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

#ifndef WM4MINIMIZEN_H
#define WM4MINIMIZEN_H

#include "Wm4FoundationLIB.h"
#include "Wm4Minimize1.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM MinimizeN
{
public:
    typedef Real (*Function)(const Real*,void*);

    MinimizeN (int iDimensions, Function oFunction, int iMaxLevel,
        int iMaxBracket, int iMaxIterations, void* pvData = 0);

    ~MinimizeN ();

    int& MaxLevel ();
    int& MaxBracket ();
    void*& UserData ();

    // find minimum on Cartesian-product domain
    void GetMinimum (const Real* afT0, const Real* afT1,
        const Real* afTInitial, Real* afTMin, Real& rfFMin);

private:
    int m_iDimensions;
    Function m_oFunction;
    int m_iMaxIterations;
    void* m_pvData;
    Minimize1<Real> m_kMinimizer;
    Real* m_afDirectionStorage;
    Real** m_aafDirection;
    Real* m_afDConj;
    Real* m_afDCurr;
    Real* m_afTSave;
    Real* m_afTCurr;
    Real m_fFCurr;
    Real* m_afLineArg;

    void ComputeDomain (const Real* afT0, const Real* afT1, Real& rfL0,
        Real& rfL1);

    static Real LineFunction (Real fT, void* pvData);
};

typedef MinimizeN<float> MinimizeNf;
typedef MinimizeN<double> MinimizeNd;

}

#endif
