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

#ifndef WM4MATRIX2ARRAY_H
#define WM4MATRIX2ARRAY_H

#include "Wm4GraphicsLIB.h"
#include "Wm4TSharedArray.h"
#include "Wm4Matrix2.h"

namespace Wm4
{
typedef TSharedArray<Matrix2f> Matrix2fArray;
typedef Pointer<Matrix2fArray> Matrix2fArrayPtr;
WM4_REGISTER_STREAM(Matrix2fArray);

typedef TSharedArray<Matrix2d> Matrix2dArray;
typedef Pointer<Matrix2dArray> Matrix2dArrayPtr;
WM4_REGISTER_STREAM(Matrix2dArray);
}

#endif
