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
// Version: 4.0.1 (2006/08/06)

#ifndef WM4SOFTRESOURCES_H
#define WM4SOFTRESOURCES_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4SoftRenderer.h"

namespace Wm4
{

class WM4_RENDERER_ITEM VProgramID : public ResourceIdentifier
{
public:
    SoftRenderer::VProgram ID;
    Attributes OAttr;
};

class WM4_RENDERER_ITEM PProgramID : public ResourceIdentifier
{
public:
    SoftRenderer::PProgram ID;
};

class WM4_RENDERER_ITEM VBufferID : public ResourceIdentifier
{
public:
    Attributes IAttr;  // For matching inputs when doing multipass.
    Attributes OAttr;  // For tcoord information during mipmapping.
    int VQuantity;
    float* IVertex;
    int Channels;
};

class WM4_RENDERER_ITEM IBufferID : public ResourceIdentifier
{
public:
    const IndexBuffer* IBuffer;
};

class WM4_RENDERER_ITEM TextureID : public ResourceIdentifier
{
public:
    SoftSampler* ID;
    Texture* TextureObject;
};

}

#endif
