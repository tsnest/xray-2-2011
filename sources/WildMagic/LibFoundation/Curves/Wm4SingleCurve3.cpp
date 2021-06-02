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
// Version: 4.0.1 (2007/01/25)

#include "Wm4FoundationPCH.h"
#include "Wm4SingleCurve3.h"
#include "Wm4Integrate1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
SingleCurve3<Real>::SingleCurve3 (Real fTMin, Real fTMax)
    :
    Curve3<Real>(fTMin,fTMax)
{
}
//----------------------------------------------------------------------------
template <class Real>
Real SingleCurve3<Real>::GetSpeedWithData (Real fTime, void* pvData)
{
    return ((Curve3<Real>*)pvData)->GetSpeed(fTime);
}
//----------------------------------------------------------------------------
template <class Real>
Real SingleCurve3<Real>::GetLength (Real fT0, Real fT1) const
{
    assert(m_fTMin <= fT0 && fT0 <= m_fTMax);
    assert(m_fTMin <= fT1 && fT1 <= m_fTMax);
    assert(fT0 <= fT1);

    return Integrate1<Real>::RombergIntegral(8,fT0,fT1,GetSpeedWithData,
        (void*)this);
}
//----------------------------------------------------------------------------
template <class Real>
Real SingleCurve3<Real>::GetTime (Real fLength, int iIterations,
    Real fTolerance) const
{
    if (fLength <= (Real)0.0)
    {
        return m_fTMin;
    }

    if (fLength >= GetTotalLength())
    {
        return m_fTMax;
    }

    // initial guess for Newton's method
    Real fRatio = fLength/GetTotalLength();
    Real fOmRatio = (Real)1.0 - fRatio;
    Real fTime = fOmRatio*m_fTMin + fRatio*m_fTMax;

    for (int i = 0; i < iIterations; i++)
    {
        Real fDifference = GetLength(m_fTMin,fTime) - fLength;
        if (Math<Real>::FAbs(fDifference) < fTolerance)
        {
            return fTime;
        }

        fTime -= fDifference/GetSpeed(fTime);
    }

    // Newton's method failed.  You might want to increase iterations or
    // tolerance or integration accuracy.  However, in this application it
    // is likely that the time values are oscillating, due to the limited
    // numerical precision of 32-bit floats.  It is safe to use the last
    // computed time.
    return fTime;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class SingleCurve3<float>;

template WM4_FOUNDATION_ITEM
class SingleCurve3<double>;
//----------------------------------------------------------------------------
}
