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

#ifndef WM4ODESOLVER_H
#define WM4ODESOLVER_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM OdeSolver
{
public:
    // abstract base class
    virtual ~OdeSolver ();

    // The system is dx/dt = F(t,x).  The dimension of x is passed to the
    // constructor of OdeSolver.
    typedef void (*Function)(
        Real,         // t
        const Real*,  // x
        void*,        // user-specified data
        Real*);       // F(t,x)

    virtual void Update (Real fTIn, Real* afXIn, Real& rfTOut,
        Real* afXOut) = 0;

    virtual void SetStepSize (Real fStep) = 0;
    Real GetStepSize () const;

    void SetData (void* pvData);
    void* GetData () const;

protected:
    OdeSolver (int iDim, Real fStep, Function oFunction, void* pvData = 0);

    int m_iDim;
    Real m_fStep;
    Function m_oFunction;
    void* m_pvData;
    Real* m_afFValue;
};

typedef OdeSolver<float> OdeSolverf;
typedef OdeSolver<double> OdeSolverd;

}

#endif
