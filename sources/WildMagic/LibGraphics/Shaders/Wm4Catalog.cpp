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
// Version: 4.5.4 (2008/10/11)

#include "Wm4GraphicsPCH.h"
#include "Wm4Catalog.h"

// Create catalogs for these graphics resources.
#include "Wm4Image.h"
#include "Wm4Texture.h"
#include "Wm4PixelProgram.h"
#include "Wm4VertexProgram.h"

namespace Wm4
{
//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template class Catalog<Image>;
template<> Catalog<Image>* Catalog<Image>::ms_pkActive = 0;
template<> std::vector<Catalog<Image>*>
    Catalog<Image>::ms_apkCatalogs =
    std::vector<Catalog<Image>*>();

template class Catalog<Texture>;
template<> Catalog<Texture>* Catalog<Texture>::ms_pkActive = 0;
template<> std::vector<Catalog<Texture>*>
    Catalog<Texture>::ms_apkCatalogs =
    std::vector<Catalog<Texture>*>();

template class Catalog<PixelProgram>;
template<> Catalog<PixelProgram>* Catalog<PixelProgram>::ms_pkActive = 0;
template<> std::vector<Catalog<PixelProgram>*>
    Catalog<PixelProgram>::ms_apkCatalogs =
    std::vector<Catalog<PixelProgram>*>();

template class Catalog<VertexProgram>;
template<> Catalog<VertexProgram>* Catalog<VertexProgram>::ms_pkActive = 0;
template<> std::vector<Catalog<VertexProgram>*>
    Catalog<VertexProgram>::ms_apkCatalogs =
    std::vector<Catalog<VertexProgram>*>();
//----------------------------------------------------------------------------
}
