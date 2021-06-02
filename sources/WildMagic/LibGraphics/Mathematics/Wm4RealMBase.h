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
// Version: 4.0.0 (2006/09/06)

#ifndef WM4REALMBASE_H
#define WM4REALMBASE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Math.h"

namespace Wm4
{

template <typename Real>
class RealMBase
{
public:
    // Abstract base class.  The destructor is not virtual.  The derived
    // classes implement nonvirtual destructors, which hide the base class
    // destructor.  This is intentional and is not a problem since the base
    // class has no need for destroying anything.  Avoiding the virtual
    // destructor means that there is no implicit virtual function table
    // pointer per RealMBase object.
    ~RealMBase ();

    // coordinate access
    operator const Real* () const;
    operator Real* ();
    const Real* operator[] (int iRow) const;
    Real* operator[] (int iRow);
    Real operator() (int iRow, int iCol) const;
    Real& operator() (int iRow, int iCol);

protected:
    // constructors
    RealMBase ();  // uninitialized
    RealMBase (bool bZero); // zero if 'true', Math<Real>::MAX_REAL if 'false'
    RealMBase (const RealMBase& rkMatrix);

    // assignment
    RealMBase& operator= (const RealMBase& rkMatrix);

    // The matrix is stored in row-major order.
    Real m_afEntry[16];
};

#include "Wm4RealMBase.inl"

}

#endif
