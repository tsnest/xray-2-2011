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
// Version: 4.0.1 (2008/10/11)

#ifndef WM4DX9FRAMEBUFFER_H
#define WM4DX9FRAMEBUFFER_H

#include "Wm4Dx9RendererLIB.h"
#include "Wm4Dx9Renderer.h"
#include "Wm4Dx9Utility.h"
#include "Wm4Main.h"
#include "Wm4FrameBuffer.h"

namespace Wm4
{

class WM4_RENDERER_ITEM Dx9FrameBuffer : public FrameBuffer
{
    WM4_DECLARE_INITIALIZE;

public:
    Dx9FrameBuffer (FormatType eFormat, DepthType eDepth,
        StencilType eStencil, BufferingType eBuffering,
        MultisamplingType eMultisampling, Renderer* pkRenderer,
        Texture* pkTarget);

    virtual ~Dx9FrameBuffer ();

    bool InitializeBuffer ();
    void TerminateBuffer ();

    virtual void Enable ();
    virtual void Disable ();
    virtual void CopyToTexture (bool bFillImage);

protected:
    IDirect3DTexture9* m_pkTargetID;
    ID3DXRenderToSurface* m_pkRenderToTexture;
    IDirect3DSurface9* m_pkTextureSurface;
    IDirect3DSurface9* m_pkSaveTextureSurface;
    IDirect3DSurface9* m_pkDepthStencilSurface;
    IDirect3DSurface9* m_pkSaveDepthStencilSurface;

    // factory functions
    static FrameBuffer* Create (FormatType eFormat, DepthType eDepth,
        StencilType eStencil, BufferingType eBuffering,
        MultisamplingType eMultisampling, Renderer* pkRenderer,
        Texture* pkTarget);

    static void Destroy (FrameBuffer* pkBuffer);
};

WM4_REGISTER_INITIALIZE(Dx9FrameBuffer);

}

#endif
