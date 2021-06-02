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

#ifndef WM4REALVBASE_H
#define WM4REALVBASE_H

#include "Wm4GraphicsLIB.h"

namespace Wm4
{

template <typename Real>
class RealVBase
{
public:
    // Abstract base class.  The destructor is not virtual.  The derived
    // classes implement nonvirtual destructors, which hide the base class
    // destructor.  This is intentional and is not a problem since the base
    // class has no need for destroying anything.  Avoiding the virtual
    // destructor means that there is no implicit virtual function table
    // pointer per RealVBase object.
    ~RealVBase ();

    // coordinate access
    operator const Real* () const;
    operator Real* ();
    Real operator[] (int i) const;
    Real& operator[] (int i);

    // swizzling
    void Set (int i0, const RealVBase& rkVector);
    void Set (int i0, int i1, const RealVBase& rkVector);
    void Set (int i0, int i1, int i2, const RealVBase& rkVector);
    void Set (int i0, int i1, int i2, int i3, const RealVBase& rkVector);
    RealVBase Get (int i0) const;
    RealVBase Get (int i0, int i1) const;
    RealVBase Get (int i0, int i1, int i2) const;
    RealVBase Get (int i0, int i1, int i2, int i3) const;

protected:
    // constructors
    RealVBase ();  // uninitialized
    RealVBase (Real fV0, Real fV1, Real fV2, Real fV3);
    RealVBase (const RealVBase& rkVector);

    // assignment
    RealVBase& operator= (const RealVBase& rkVector);

    Real m_afTuple[4];
};

#include "Wm4RealVBase.inl"

}

#endif
