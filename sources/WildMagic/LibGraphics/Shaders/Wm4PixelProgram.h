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
// Version: 4.0.2 (2007/08/11)

#ifndef WM4PIXELPROGRAM_H
#define WM4PIXELPROGRAM_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Program.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM PixelProgram : public Program
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.
    PixelProgram (const std::string& rkProgramName);
    virtual ~PixelProgram ();

    // To support Catalog<PixelProgram>.
    static PixelProgram* Load (const std::string& rkProgramName);

protected:
    // Streaming support.
    PixelProgram ();
};

WM4_REGISTER_STREAM(PixelProgram);
typedef Pointer<PixelProgram> PixelProgramPtr;

}

#endif
