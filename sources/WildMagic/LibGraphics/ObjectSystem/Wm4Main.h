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

#ifndef WM4MAIN_H
#define WM4MAIN_H

#include "Wm4GraphicsLIB.h"
#include "Wm4System.h"
#include "Wm4MainMCR.h"
#include "Wm4Catalog.h"

namespace Wm4
{

class Image;
class Texture;
class VertexProgram;
class PixelProgram;

class WM4_GRAPHICS_ITEM Main
{
public:
    typedef void (*Initializer)(void);
    typedef std::vector<Initializer> InitializerArray;
    static void AddInitializer (Initializer oInitialize);
    static void Initialize ();

    typedef void (*Terminator)(void);
    typedef std::vector<Terminator> TerminatorArray;
    static void AddTerminator (Terminator oTerminate);
    static void Terminate ();

private:
    static InitializerArray* ms_pkInitializers;
    static TerminatorArray* ms_pkTerminators;
    static int ms_iStartObjects;
    static int ms_iFinalObjects;

    static Catalog<Image>* ms_pkImageCatalog;
    static Catalog<Texture>* ms_pkTextureCatalog;
    static Catalog<VertexProgram>* ms_pkVertexProgramCatalog;
    static Catalog<PixelProgram>* ms_pkPixelProgramCatalog;
};

}

#endif
