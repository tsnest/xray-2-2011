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

#ifndef WM4BOOLBASE_H
#define WM4BOOLBASE_H

#include "Wm4GraphicsLIB.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM BoolBase
{
public:
    // Abstract base class.  The destructor is not virtual.  The derived
    // classes implement nonvirtual destructors, which hide the base class
    // destructor.  This is intentional and is not a problem since the base
    // class has no need for destroying anything.  Avoiding the virtual
    // destructor means that there is no implicit virtual function table
    // pointer per BoolBase object.
    ~BoolBase ();

    // coordinate access
    operator const bool* () const;
    operator bool* ();
    bool operator[] (int i) const;
    bool& operator[] (int i);

protected:
    // constructor
    BoolBase ();

    // These two functions are not public to prevent them from being called.
    BoolBase (const BoolBase& rkVector);
    BoolBase& operator= (const BoolBase& rkVector);

    bool m_abTuple[4];
};

}

#endif
