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

#ifndef WM4ODEMIDPOINT_H
#define WM4ODEMIDPOINT_H

#include "Wm4FoundationLIB.h"
#include "Wm4OdeSolver.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM OdeMidpoint : public OdeSolver<Real>
{
public:
    OdeMidpoint (int iDim, Real fStep,
        typename OdeSolver<Real>::Function oFunction, void* pvData = 0);

    virtual ~OdeMidpoint ();

    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut);

    virtual void SetStepSize (Real fStep);

protected:
    using OdeSolver<Real>::m_iDim;
    using OdeSolver<Real>::m_fStep;
    using OdeSolver<Real>::m_pvData;
    using OdeSolver<Real>::m_afFValue;

    Real m_fHalfStep;
    Real* m_afXTemp;
};

typedef OdeMidpoint<float> OdeMidpointf;
typedef OdeMidpoint<double> OdeMidpointd;

}

#endif
