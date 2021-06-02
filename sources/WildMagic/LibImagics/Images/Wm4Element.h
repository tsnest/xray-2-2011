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
// Version: 4.0.1 (2006/09/21)

#ifndef WM4ELEMENT_H
#define WM4ELEMENT_H

#include "Wm4ImagicsLIB.h"
#include "Wm4System.h"

// wrappers for native types
const int WM4_ELEMENT_QUANTITY = 12;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short rgb5;
typedef unsigned int rgb8;

#define Wm4DeclareElement(T) \
class WM4_IMAGICS_ITEM E##T \
{ \
public: \
    E##T (T tValue = 0) { m_tValue = tValue; } \
    \
    E##T& operator= (E##T kElement) \
    { \
        m_tValue = kElement.m_tValue; \
        return *this; \
    } \
    \
    operator T () { return m_tValue; } \
    \
    static int GetRTTI () { return ms_iRTTI; } \
    \
protected: \
    T m_tValue; \
    static const int ms_iRTTI; \
}

#define Wm4ImplementElement(T,iRTTI) \
const int E##T::ms_iRTTI = iRTTI

namespace Wm4
{

Wm4DeclareElement(char);
Wm4DeclareElement(uchar);
Wm4DeclareElement(short);
Wm4DeclareElement(ushort);
Wm4DeclareElement(int);
Wm4DeclareElement(uint);
Wm4DeclareElement(long);
Wm4DeclareElement(ulong);
Wm4DeclareElement(float);
Wm4DeclareElement(double);
Wm4DeclareElement(rgb5);
Wm4DeclareElement(rgb8);

#include "Wm4Element.inl"

}

#endif
