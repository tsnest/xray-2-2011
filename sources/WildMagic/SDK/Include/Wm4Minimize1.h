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

#ifndef WM4MINIMIZE1_H
#define WM4MINIMIZE1_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM Minimize1
{
public:
    typedef Real (*Function)(Real,void*);

    Minimize1 (Function oFunction, int iMaxLevel, int iMaxBracket,
        void* pvData = 0);

    int& MaxLevel ();
    int& MaxBracket ();
    void*& UserData ();

    void GetMinimum (Real fT0, Real fT1, Real fTInitial, Real& rfTMin,
        Real& rfFMin);

private:
    Function m_oFunction;
    int m_iMaxLevel, m_iMaxBracket;
    Real m_fTMin, m_fFMin;
    void* m_pvData;

    void GetMinimum (Real fT0, Real fF0, Real fT1, Real fF1, int iLevel);

    void GetMinimum (Real fT0, Real fF0, Real fTm, Real fFm, Real fT1,
        Real fF1, int iLevel);

    void GetBracketedMinimum (Real fT0, Real fF0, Real fTm,
        Real fFm, Real fT1, Real fF1, int iLevel);
};

typedef Minimize1<float> Minimize1f;
typedef Minimize1<double> Minimize1d;

}

#endif
