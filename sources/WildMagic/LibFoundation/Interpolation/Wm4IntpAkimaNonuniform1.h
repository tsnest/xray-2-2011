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

#ifndef WM4INTPAKIMANONUNIFORM1_H
#define WM4INTPAKIMANONUNIFORM1_H

#include "Wm4FoundationLIB.h"
#include "Wm4IntpAkima1.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntpAkimaNonuniform1 : public IntpAkima1<Real>
{
public:
    // Construction and destruction.  IntpAkimaNonuniform1 does not
    // accept responsibility for deleting the input arrays.  The application
    // must do so.  The interpolator is for arbitrarily spaced x-values.
    IntpAkimaNonuniform1 (int iQuantity, Real* afX, Real* afF);
    virtual ~IntpAkimaNonuniform1 ();

    const Real* GetX () const;
    virtual Real GetXMin () const;
    virtual Real GetXMax () const;

protected:
    using IntpAkima1<Real>::m_iQuantity;
    using IntpAkima1<Real>::m_akPoly;

    virtual bool Lookup (Real fX, int& riIndex, Real& rfDX) const;

    Real* m_afX;
};

typedef IntpAkimaNonuniform1<float> IntpAkimaNonuniform1f;
typedef IntpAkimaNonuniform1<double> IntpAkimaNonuniform1d;

}

#endif
