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
#include "Wm4OdeRungeKutta4.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
OdeRungeKutta4<Real>::OdeRungeKutta4 (int iDim, Real fStep,
    typename OdeSolver<Real>::Function oFunction, void* pvData)
    :
    OdeSolver<Real>(iDim,fStep,oFunction,pvData)
{
    m_fHalfStep = ((Real)0.5)*fStep;
    m_fSixthStep = fStep/((Real)6.0);
    m_afTemp1 = WM4_NEW Real[iDim];
    m_afTemp2 = WM4_NEW Real[iDim];
    m_afTemp3 = WM4_NEW Real[iDim];
    m_afTemp4 = WM4_NEW Real[iDim];
    m_afXTemp = WM4_NEW Real[iDim];
}
//----------------------------------------------------------------------------
template <class Real>
OdeRungeKutta4<Real>::~OdeRungeKutta4 ()
{
    WM4_DELETE[] m_afTemp1;
    WM4_DELETE[] m_afTemp2;
    WM4_DELETE[] m_afTemp3;
    WM4_DELETE[] m_afTemp4;
    WM4_DELETE[] m_afXTemp;
}
//----------------------------------------------------------------------------
template <class Real>
void OdeRungeKutta4<Real>::Update (Real fTIn, Real* afXIn, Real& rfTOut,
    Real* afXOut)
{
    int i;

    // first step
    m_oFunction(fTIn,afXIn,m_pvData,m_afTemp1);
    for (i = 0; i < m_iDim; i++)
    {
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp1[i];
    }

    // second step
    Real fHalfT = fTIn + m_fHalfStep;
    m_oFunction(fHalfT,m_afXTemp,m_pvData,m_afTemp2);
    for (i = 0; i < m_iDim; i++)
    {
        m_afXTemp[i] = afXIn[i] + m_fHalfStep*m_afTemp2[i];
    }

    // third step
    m_oFunction(fHalfT,m_afXTemp,m_pvData,m_afTemp3);
    for (i = 0; i < m_iDim; i++)
    {
        m_afXTemp[i] = afXIn[i] + m_fStep*m_afTemp3[i];
    }

    // fourth step
    rfTOut = fTIn + m_fStep;
    m_oFunction(rfTOut,m_afXTemp,m_pvData,m_afTemp4);
    for (i = 0; i < m_iDim; i++)
    {
        afXOut[i] = afXIn[i] + m_fSixthStep*(m_afTemp1[i] +
            ((Real)2.0)*(m_afTemp2[i] + m_afTemp3[i]) + m_afTemp4[i]);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void OdeRungeKutta4<Real>::SetStepSize (Real fStep)
{
    m_fStep = fStep;
    m_fHalfStep = ((Real)0.5)*fStep;
    m_fSixthStep = fStep/((Real)6.0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class OdeRungeKutta4<float>;

template WM4_FOUNDATION_ITEM
class OdeRungeKutta4<double>;
//----------------------------------------------------------------------------
}
