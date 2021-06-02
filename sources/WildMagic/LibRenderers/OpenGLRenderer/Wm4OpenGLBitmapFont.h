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

#ifndef WM4OPENGLBITMAPFONT_H
#define WM4OPENGLBITMAPFONT_H

#include "Wm4OpenGLRendererLIB.h"

namespace Wm4
{

class WM4_RENDERER_ITEM BitmapFontChar
{
public:
    BitmapFontChar (int iXOrigin, int iYOrigin, int iXSize, int iYSize,
        const unsigned char* aucBitmap)
    {
        XOrigin = iXOrigin;
        YOrigin = iYOrigin;
        XSize = iXSize;
        YSize = iYSize;
        Bitmap = aucBitmap;
    }

    int XOrigin, YOrigin, XSize, YSize;
    const unsigned char* Bitmap;
};

class WM4_RENDERER_ITEM BitmapFont
{
public:
    BitmapFont (const char* acName, int iQuantity,
        const BitmapFontChar* const* pkChars)
    {
        Name = acName;
        Quantity = iQuantity;
        Chars = pkChars;
    }

    const char* Name;
    int Quantity;
    const BitmapFontChar* const* Chars;
};

extern const BitmapFont g_kVerdana_S16B0I0;

}

#endif
