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
#include "Wm4OdeSolver.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
OdeSolver<Real>::OdeSolver (int iDim, Real fStep, Function oFunction,
    void* pvData)
{
    m_iDim = iDim;
    m_fStep = fStep;
    m_oFunction = oFunction;
    m_pvData = pvData;
    m_afFValue = WM4_NEW Real[iDim];
}
//----------------------------------------------------------------------------
template <class Real>
OdeSolver<Real>::~OdeSolver ()
{
    WM4_DELETE[] m_afFValue;
}
//----------------------------------------------------------------------------
template <class Real>
Real OdeSolver<Real>::GetStepSize () const
{
    return m_fStep;
}
//----------------------------------------------------------------------------
template <class Real>
void OdeSolver<Real>::SetData (void* pvData)
{
    m_pvData = pvData;
}
//----------------------------------------------------------------------------
template <class Real>
void* OdeSolver<Real>::GetData () const
{
    return m_pvData;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class OdeSolver<float>;

template WM4_FOUNDATION_ITEM
class OdeSolver<double>;
//----------------------------------------------------------------------------
}
