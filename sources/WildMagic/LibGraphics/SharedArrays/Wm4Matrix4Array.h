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

#ifndef WM4MATRIX4ARRAY_H
#define WM4MATRIX4ARRAY_H

#include "Wm4GraphicsLIB.h"
#include "Wm4TSharedArray.h"
#include "Wm4Matrix4.h"

namespace Wm4
{
typedef TSharedArray<Matrix4f> Matrix4fArray;
typedef Pointer<Matrix4fArray> Matrix4fArrayPtr;
WM4_REGISTER_STREAM(Matrix4fArray);

typedef TSharedArray<Matrix4d> Matrix4dArray;
typedef Pointer<Matrix4dArray> Matrix4dArrayPtr;
WM4_REGISTER_STREAM(Matrix4dArray);
}

#endif
