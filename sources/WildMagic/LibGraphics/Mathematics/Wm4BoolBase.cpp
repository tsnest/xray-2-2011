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
// Version: 4.0.1 (2007/05/06)

#include "Wm4GraphicsPCH.h"
#include "Wm4BoolBase.h"
#include <cassert>
using namespace Wm4;

//----------------------------------------------------------------------------
BoolBase::BoolBase ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
BoolBase::~BoolBase ()
{
}
//----------------------------------------------------------------------------
BoolBase::BoolBase (const BoolBase&)
{
    // The copy constructor should never be called.
    assert(false);
}
//----------------------------------------------------------------------------
BoolBase& BoolBase::operator= (const BoolBase&)
{
    // The assignment operator should never be called.
    assert(false);
    return *this;
}
//----------------------------------------------------------------------------
BoolBase::operator const bool* () const
{
    return m_abTuple;
}
//----------------------------------------------------------------------------
BoolBase::operator bool* ()
{
    return m_abTuple;
}
//----------------------------------------------------------------------------
bool BoolBase::operator[] (int i) const
{
    // Assert:  0 <= i < N, where N in {1,2,3,4} depending on which derived
    // class object is making the call.
    return m_abTuple[i];
}
//----------------------------------------------------------------------------
bool& BoolBase::operator[] (int i)
{
    // Assert:  0 <= i < N, where N in {1,2,3,4} depending on which derived
    // class object is making the call.
    return m_abTuple[i];
}
//----------------------------------------------------------------------------
