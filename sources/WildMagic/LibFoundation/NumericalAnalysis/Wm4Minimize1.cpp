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
#include "Wm4Minimize1.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Minimize1<Real>::Minimize1 (Function oFunction, int iMaxLevel,
    int iMaxBracket, void* pvData)
{
    assert(oFunction);
    m_oFunction = oFunction;
    m_iMaxLevel = iMaxLevel;
    m_iMaxBracket = iMaxBracket;
    m_pvData = pvData;
}
//----------------------------------------------------------------------------
template <class Real>
int& Minimize1<Real>::MaxLevel ()
{
    return m_iMaxLevel;
}
//----------------------------------------------------------------------------
template <class Real>
int& Minimize1<Real>::MaxBracket ()
{
    return m_iMaxBracket;
}
//----------------------------------------------------------------------------
template <class Real>
void*& Minimize1<Real>::UserData ()
{
    return m_pvData;
}
//----------------------------------------------------------------------------
template <class Real>
void Minimize1<Real>::GetMinimum (Real fT0, Real fT1, Real fTInitial,
    Real& rfTMin, Real& rfFMin)
{
    assert(fT0 <= fTInitial && fTInitial <= fT1);

    m_fTMin = Math<Real>::MAX_REAL;
    m_fFMin = Math<Real>::MAX_REAL;

    Real fF0 = m_oFunction(fT0,m_pvData);
    Real fFInitial = m_oFunction(fTInitial,m_pvData);
    Real fF1 = m_oFunction(fT1,m_pvData);

    GetMinimum(fT0,fF0,fTInitial,fFInitial,fT1,fF1,m_iMaxLevel);

    rfTMin = m_fTMin;
    rfFMin = m_fFMin;
}
//----------------------------------------------------------------------------
template <class Real>
void Minimize1<Real>::GetMinimum (Real fT0, Real fF0, Real fTm, Real fFm,
    Real fT1, Real fF1, int iLevel)
{
    if (fF0 < m_fFMin)
    {
        m_fTMin = fT0;
        m_fFMin = fF0;
    }

    if (fFm < m_fFMin)
    {
        m_fTMin = fTm;
        m_fFMin = fFm;
    }

    if (fF1 < m_fFMin)
    {
        m_fTMin = fT1;
        m_fFMin = fF1;
    }

    if (iLevel-- == 0)
    {
        return;
    }

    if ((fT1 - fTm)*(fF0 - fFm) > (fTm - fT0)*(fFm - fF1))
    {
        // quadratic fit has positive second derivative at midpoint

        if (fF1 > fF0)
        {
            if (fFm >= fF0)
            {
                // increasing, repeat on [t0,tm]
                GetMinimum(fT0,fF0,fTm,fFm,iLevel);
            }
            else
            {
                // not monotonic, have a bracket
                GetBracketedMinimum(fT0,fF0,fTm,fFm,fT1,fF1,iLevel);
            }
        }
        else if (fF1 < fF0)
        {
            if (fFm >= fF1)
            {
                // decreasing, repeat on [tm,t1]
                GetMinimum(fTm,fFm,fT1,fF1,iLevel);
            }
            else
            {
                // not monotonic, have a bracket
                GetBracketedMinimum(fT0,fF0,fTm,fFm,fT1,fF1,iLevel);
            }
        }
        else
        {
            // constant, repeat on [t0,tm] and [tm,t1]
            GetMinimum(fT0,fF0,fTm,fFm,iLevel);
            GetMinimum(fTm,fFm,fT1,fF1,iLevel);
        }
    }
    else
    {
        // quadratic fit has nonpositive second derivative at midpoint

        if (fF1 > fF0)
        {
            // repeat on [t0,tm]
            GetMinimum(fT0,fF0,fTm,fFm,iLevel);
        }
        else if ( fF1 < fF0 )
        {
            // repeat on [tm,t1]
            GetMinimum(fTm,fFm,fT1,fF1,iLevel);
        }
        else
        {
            // repeat on [t0,tm] and [tm,t1]
            GetMinimum(fT0,fF0,fTm,fFm,iLevel);
            GetMinimum(fTm,fFm,fT1,fF1,iLevel);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Minimize1<Real>::GetMinimum (Real fT0, Real fF0, Real fT1, Real fF1,
    int iLevel)
{
    if (fF0 < m_fFMin)
    {
        m_fTMin = fT0;
        m_fFMin = fF0;
    }

    if (fF1 < m_fFMin)
    {
        m_fTMin = fT1;
        m_fFMin = fF1;
    }

    if (iLevel-- == 0)
    {
        return;
    }

    Real fTm = ((Real)0.5)*(fT0+fT1);
    Real fFm = m_oFunction(fTm,m_pvData);

    if (fF0 - ((Real)2.0)*fFm + fF1 > (Real)0.0)
    {
        // quadratic fit has positive second derivative at midpoint

        if (fF1 > fF0)
        {
            if (fFm >= fF0)
            {
                // increasing, repeat on [t0,tm]
                GetMinimum(fT0,fF0,fTm,fFm,iLevel);
            }
            else
            {
                // not monotonic, have a bracket
                GetBracketedMinimum(fT0,fF0,fTm,fFm,fT1,fF1,iLevel);
            }
        }
        else if (fF1 < fF0)
        {
            if (fFm >= fF1)
            {
                // decreasing, repeat on [tm,t1]
                GetMinimum(fTm,fFm,fT1,fF1,iLevel);
            }
            else
            {
                // not monotonic, have a bracket
                GetBracketedMinimum(fT0,fF0,fTm,fFm,fT1,fF1,iLevel);
            }
        }
        else
        {
            // constant, repeat on [t0,tm] and [tm,t1]
            GetMinimum(fT0,fF0,fTm,fFm,iLevel);
            GetMinimum(fTm,fFm,fT1,fF1,iLevel);
        }
    }
    else
    {
        // quadratic fit has nonpositive second derivative at midpoint

        if (fF1 > fF0)
        {
            // repeat on [t0,tm]
            GetMinimum(fT0,fF0,fTm,fFm,iLevel);
        }
        else if (fF1 < fF0)
        {
            // repeat on [tm,t1]
            GetMinimum(fTm,fFm,fT1,fF1,iLevel);
        }
        else
        {
            // repeat on [t0,tm] and [tm,t1]
            GetMinimum(fT0,fF0,fTm,fFm,iLevel);
            GetMinimum(fTm,fFm,fT1,fF1,iLevel);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Minimize1<Real>::GetBracketedMinimum (Real fT0, Real fF0, Real fTm,
    Real fFm, Real fT1, Real fF1, int iLevel)
{
    for (int i = 0; i < m_iMaxBracket; i++)
    {
        // update minimum value
        if (fFm < m_fFMin)
        {
            m_fTMin = fTm;
            m_fFMin = fFm;
        }

        // test for convergence
        const Real fEps = (Real)1e-08, fTol = (Real)1e-04;
        if (Math<Real>::FAbs(fT1-fT0) <= ((Real)2.0)*fTol*
            Math<Real>::FAbs(fTm) + fEps )
        {
            break;
        }

        // compute vertex of interpolating parabola
        Real fDT0 = fT0 - fTm, fDT1 = fT1 - fTm;
        Real fDF0 = fF0 - fFm, fDF1 = fF1 - fFm;
        Real fTmp0 = fDT0*fDF1, fTmp1 = fDT1*fDF0;
        Real fDenom = fTmp1 - fTmp0;
        if (Math<Real>::FAbs(fDenom) < fEps)
        {
           return;
        }

        Real fTv = fTm + ((Real)0.5)*(fDT1*fTmp1-fDT0*fTmp0)/fDenom;
        assert(fT0 <= fTv && fTv <= fT1);
        Real fFv = m_oFunction(fTv,m_pvData);

        if (fTv < fTm)
        {
            if (fFv < fFm)
            {
                fT1 = fTm;
                fF1 = fFm;
                fTm = fTv;
                fFm = fFv;
            }
            else
            {
                fT0 = fTv;
                fF0 = fFv;
            }
        }
        else if (fTv > fTm)
        {
            if (fFv < fFm)
            {
                fT0 = fTm;
                fF0 = fFm;
                fTm = fTv;
                fFm = fFv;
            }
            else
            {
                fT1 = fTv;
                fF1 = fFv;
            }
        }
        else
        {
            // vertex of parabola is already at middle sample point
            GetMinimum(fT0,fF0,fTm,fFm,iLevel);
            GetMinimum(fTm,fFm,fT1,fF1,iLevel);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class Minimize1<float>;

template WM4_FOUNDATION_ITEM
class Minimize1<double>;
//----------------------------------------------------------------------------
}
