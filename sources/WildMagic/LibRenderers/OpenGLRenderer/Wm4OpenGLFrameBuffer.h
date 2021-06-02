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
// Version: 4.0.2 (2008/04/14)

#ifndef WM4OPENGLFRAMEBUFFER_H
#define WM4OPENGLFRAMEBUFFER_H

#include "Wm4OpenGLRendererLIB.h"
#include "Wm4OpenGLRenderer.h"
#include "Wm4Main.h"
#include "Wm4FrameBuffer.h"

namespace Wm4
{

class WM4_RENDERER_ITEM OpenGLFrameBuffer : public FrameBuffer
{
    WM4_DECLARE_INITIALIZE;

public:
    OpenGLFrameBuffer (FormatType eFormat, DepthType eDepth,
        StencilType eStencil, BufferingType eBuffering,
        MultisamplingType eMultisampling, Renderer* pkRenderer,
        Texture* pkTarget);

    virtual ~OpenGLFrameBuffer ();

    bool InitializeBuffer ();
    void TerminateBuffer ();

    virtual void Enable ();
    virtual void Disable ();
    virtual void CopyToTexture (bool bFillImage);
    virtual void CopyTo (VertexBuffer* pkVBuffer, bool bCopyToSystemMemory);

protected:
    GLuint m_uiFrameBufferID;
    GLuint m_uiDepthBufferID;
    GLuint m_uiStencilBufferID;
    GLuint m_uiTargetID;
    bool m_bEnabled;

    // TO DO.  What about buffering and multisampling?

    // factory functions
    static FrameBuffer* Create (FormatType eFormat, DepthType eDepth,
        StencilType eStencil, BufferingType eBuffering,
        MultisamplingType eMultisampling, Renderer* pkRenderer,
        Texture* pkTarget);

    static void Destroy (FrameBuffer* pkBuffer);
};

WM4_REGISTER_INITIALIZE(OpenGLFrameBuffer);

}

#endif
