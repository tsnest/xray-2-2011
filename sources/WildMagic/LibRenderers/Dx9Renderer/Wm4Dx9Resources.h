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
// Version: 4.0.1 (2007/06/16)

#ifndef WM4DX9RESOURCES_H
#define WM4DX9RESOURCES_H

#include "Wm4Dx9RendererLIB.h"
#include "Wm4Dx9Utility.h"
#include "Wm4Attributes.h"
#include "Wm4Bindable.h"

namespace Wm4
{

class WM4_RENDERER_ITEM VProgramID : public ResourceIdentifier
{
public:
    IDirect3DVertexShader9* ID;
};

class WM4_RENDERER_ITEM PProgramID : public ResourceIdentifier
{
public:
    IDirect3DPixelShader9* ID;
};

class WM4_RENDERER_ITEM TextureID : public ResourceIdentifier
{
public:
    IDirect3DBaseTexture9* ID;
    Texture* TextureObject;
};

class WM4_RENDERER_ITEM VBufferID : public ResourceIdentifier
{
public:
    Attributes IAttr;  // For matching inputs when doing multipass.
    Attributes OAttr;
    IDirect3DVertexBuffer9* ID;
    IDirect3DVertexDeclaration9* Declaration;
    int VertexSize;
};

class WM4_RENDERER_ITEM IBufferID : public ResourceIdentifier
{
public:
    LPDIRECT3DINDEXBUFFER9 ID;
};

}

#endif
