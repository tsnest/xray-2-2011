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

#ifndef WM4BOOL_H
#define WM4BOOL_H

#include "Wm4GraphicsLIB.h"
#include "Wm4BoolBase.h"

namespace Wm4
{

template <int N>
class Bool : public BoolBase
{
public:
    // construction
    Bool ();
    Bool (const Bool& rkVector);

    // assignment
    Bool& operator= (const Bool& rkVector);
};

#include "Wm4Bool.inl"

typedef Bool<1> Bool1;
typedef Bool<2> Bool2;
typedef Bool<3> Bool3;
typedef Bool<4> Bool4;

}

#endif
