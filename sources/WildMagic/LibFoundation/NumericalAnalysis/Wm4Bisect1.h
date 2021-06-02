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

#ifndef WM4BISECT1_H
#define WM4BISECT1_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM Bisect1
{
public:
    typedef Real (*Function)(Real);

    Bisect1 (Function oF, int iMaxLevel, Real fTolerance);

    bool Bisect (Real fX0, Real fX1, Real& rfRoot);

private:
    // input data and functions
    Function m_oF;
    int m_iMaxLevel;
    Real m_fTolerance;
};

typedef Bisect1<float> Bisect1f;
typedef Bisect1<double> Bisect1d;

}

#endif
