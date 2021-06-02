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

#ifndef WM4IMAGES_H
#define WM4IMAGES_H

#include "Wm4ImagicsLIB.h"
#include "Wm4Element.h"
#include "Wm4TImage.h"
#include "Wm4TImage2D.h"
#include "Wm4TImage3D.h"

namespace Wm4
{

typedef TImage<Echar>     ImageChar;
typedef TImage<Euchar>    ImageUChar;
typedef TImage<Eshort>    ImageShort;
typedef TImage<Eushort>   ImageUShort;
typedef TImage<Eint>      ImageInt;
typedef TImage<Euint>     ImageUInt;
typedef TImage<Elong>     ImageLong;
typedef TImage<Eulong>    ImageULong;
typedef TImage<Efloat>    ImageFloat;
typedef TImage<Edouble>   ImageDouble;
typedef TImage<Ergb5>     ImageRGB5;
typedef TImage<Ergb8>     ImageRGB8;

typedef TImage2D<Echar>   ImageChar2D;
typedef TImage2D<Euchar>  ImageUChar2D;
typedef TImage2D<Eshort>  ImageShort2D;
typedef TImage2D<Eushort> ImageUShort2D;
typedef TImage2D<Eint>    ImageInt2D;
typedef TImage2D<Euint>   ImageUInt2D;
typedef TImage2D<Elong>   ImageLong2D;
typedef TImage2D<Eulong>  ImageULong2D;
typedef TImage2D<Efloat>  ImageFloat2D;
typedef TImage2D<Edouble> ImageDouble2D;
typedef TImage2D<Ergb5>   ImageRGB52D;
typedef TImage2D<Ergb8>   ImageRGB82D;

typedef TImage3D<Echar>   ImageChar3D;
typedef TImage3D<Euchar>  ImageUChar3D;
typedef TImage3D<Eshort>  ImageShort3D;
typedef TImage3D<Eushort> ImageUShort3D;
typedef TImage3D<Eint>    ImageInt3D;
typedef TImage3D<Euint>   ImageUInt3D;
typedef TImage3D<Elong>   ImageLong3D;
typedef TImage3D<Eulong>  ImageULong3D;
typedef TImage3D<Efloat>  ImageFloat3D;
typedef TImage3D<Edouble> ImageDouble3D;
typedef TImage3D<Ergb5>   ImageRGB53D;
typedef TImage3D<Ergb8>   ImageRGB83D;

}

#endif
