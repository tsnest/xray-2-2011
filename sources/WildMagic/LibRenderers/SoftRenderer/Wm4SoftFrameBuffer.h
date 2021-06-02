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

#ifndef WM4SOFTFRAMEBUFFER_H
#define WM4SOFTFRAMEBUFFER_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4Main.h"
#include "Wm4ColorRGBA.h"
#include "Wm4FrameBuffer.h"

namespace Wm4
{

class WM4_RENDERER_ITEM SoftFrameBuffer : public FrameBuffer
{
    WM4_DECLARE_INITIALIZE;

public:
    SoftFrameBuffer (FormatType eFormat, DepthType eDepth,
        StencilType eStencil, BufferingType eBuffering,
        MultisamplingType eMultisampling, Renderer* pkRenderer,
        Texture* pkTarget, int iWidth, int iHeight, bool bCreateColorBuffer);

    virtual ~SoftFrameBuffer ();

    virtual void Enable ();
    virtual void Disable ();
    virtual void CopyToTexture (bool bFillImage);

    // Accessors for the software renderer to switch between framebuffer
    // objects.  The SetColorBufferUI call allows for a hardware-based buffer
    // to be used as the main colorbuffer.
    int GetWidth () const;
    int GetHeight () const;
    int GetQuantity () const;
    void SetColorBufferUI (unsigned int* auiColorBuffer);
    unsigned int* GetColorBufferUI () const;
    ColorRGBA* GetColorBufferRGBA () const;
    unsigned int GetMaxDepthValue () const;
    unsigned int* GetDepthBufferUI () const;
    float* GetDepthBufferF () const;
    unsigned int GetMaxStencilValue () const;
    unsigned int* GetStencilBuffer () const;

protected:
    // TO DO.  Only single-buffering is supported.  Multisampling is not yet
    // supported.  Only 2D buffers are supported.

    // The 32-bit color buffer.  The format depends on the platform (little
    // endian or big endian) and how the graphics system on the platform
    // stores the color channels.
    int m_iWidth, m_iHeight;
    int m_iQuantity;
    unsigned int* m_auiColorBuffer;

    // Backing storage to support alpha blending.  The conversion of colors
    // with channels in [0,1] to integer channels in [0,255] is used to
    // write the frame buffer.  To read it back for blending, a conversion
    // from [0,255] to [0,1] will not always produce the original color due
    // to floating-point round-off errors.  One way to avoid this is to read
    // back the frame buffer by converting the integer channels to float
    // channels in [0,255], blend the results appropriately, and then clamp
    // to integer-valued [0,255].  Alternatively, a copy of the frame buffer
    // can be maintained to be used in any floating-point operations, which
    // is what I have chosen to do.
    ColorRGBA* m_akColorBuffer;

    // The depth buffer (16-bit, 24-bit, or 32-bit).  All are stored as
    // 32-bit integers to avoid performance loss due to masking and shifting.
    unsigned int m_uiMaxDepthValue;
    unsigned int* m_auiDepthBuffer;

    // Backing storage to support render-to-depth-buffer.
    float* m_afDepthBuffer;

    // The stencil buffer (8-bit).  Stored as 32-bit integers to avoid
    // performance loss due to masking and shifting.
    unsigned int m_uiMaxStencilValue;
    unsigned int* m_auiStencilBuffer;

    // The colorbuffer for the main framebuffer might have been passed to
    // this object.  If so, the caller is responsible for deleting it.  If
    // not, this object allocated it and must delete it.
    bool m_bColorBufferOwner;

    // factory functions
    static FrameBuffer* Create (FormatType eFormat, DepthType eDepth,
        StencilType eStencil, BufferingType eBuffering,
        MultisamplingType eMultisampling, Renderer* pkRenderer,
        Texture* pkTarget);

    static void Destroy (FrameBuffer* pkBuffer);
};

WM4_REGISTER_INITIALIZE(SoftFrameBuffer);
#include "Wm4SoftFrameBuffer.inl"

}

#endif
