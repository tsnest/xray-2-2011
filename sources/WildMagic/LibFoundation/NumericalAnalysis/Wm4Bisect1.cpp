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
#include "Wm4Bisect1.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Bisect1<Real>::Bisect1 (Function oF, int iMaxLevel, Real fTolerance)
{
    m_oF = oF;
    m_iMaxLevel = iMaxLevel;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect1<Real>::Bisect (Real fX0, Real fX1, Real& rfRoot)
{
    // test two endpoints
    Real fF0 = m_oF(fX0);
    if (Math<Real>::FAbs(fF0) <= m_fTolerance)
    {
        rfRoot = fX0;
        return true;
    }

    Real fF1 = m_oF(fX1);
    if (Math<Real>::FAbs(fF1) <= m_fTolerance)
    {
        rfRoot = fX1;
        return true;
    }

    if (fF0*fF1 > (Real)0.0)
        return false;

    for (int iLevel = 0; iLevel < m_iMaxLevel; iLevel++)
    {
        Real fXm = ((Real)0.5)*(fX0+fX1);
        Real fFm = m_oF(fXm);
        if (Math<Real>::FAbs(fFm) <= m_fTolerance)
        {
            rfRoot = fXm;
            return true;
        }

        if (fF0*fFm < (Real)0.0)
        {
            fX1 = fXm;
            fF1 = fFm;
        }
        else if (fF1*fFm < (Real)0.0)
        {
            fX0 = fXm;
            fF0 = fFm;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class Bisect1<float>;

template WM4_FOUNDATION_ITEM
class Bisect1<double>;
//----------------------------------------------------------------------------
}
