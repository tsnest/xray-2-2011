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
// Version: 4.0.1 (2006/07/25)

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4GlxRenderer.h"
using namespace Wm4;

//----------------------------------------------------------------------------
GlxRenderer::GlxRenderer (Display* pkDisplay, FrameBuffer::FormatType eFormat,
    FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
    FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iWidth, int iHeight)
    :
    OpenGLRenderer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,iWidth,
        iHeight)
{
    assert(m_iWidth > 0 && m_iHeight > 0);
    if (m_iWidth <= 0 || m_iHeight <= 0)
    {
        m_bIsConstructed = false;
        return;
    }

    m_pkDisplay = pkDisplay;
    m_ulWindow = 0;  // set by the application after window creation
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_iSaveWidth = m_iWidth;
    m_iSaveHeight = m_iHeight;

    // Select the attributes for the frame buffer.
    int aiAttributes[256];

    // The color buffer is RGBA888 on Linux, but RGB888 on SGI.
    int iPos = 0;
    aiAttributes[iPos++] = GLX_RGBA;
#ifndef __sgi
    aiAttributes[iPos++] = GLX_RED_SIZE;
    aiAttributes[iPos++] = 8;
    aiAttributes[iPos++] = GLX_GREEN_SIZE;
    aiAttributes[iPos++] = 8;
    aiAttributes[iPos++] = GLX_BLUE_SIZE;
    aiAttributes[iPos++] = 8;
    aiAttributes[iPos++] = GLX_ALPHA_SIZE;
    aiAttributes[iPos++] = 8;
#else
    aiAttributes[iPos++] = GLX_RED_SIZE;
    aiAttributes[iPos++] = 1;
    aiAttributes[iPos++] = GLX_GREEN_SIZE;
    aiAttributes[iPos++] = 1;
    aiAttributes[iPos++] = GLX_BLUE_SIZE;
    aiAttributes[iPos++] = 1;
#endif

    // Request a depth buffer.
    if (m_eDepth != FrameBuffer::DT_DEPTH_NONE)
    {
        aiAttributes[iPos++] = GLX_DEPTH_SIZE;
#ifndef __sgi
        if (m_eDepth == FrameBuffer::DT_DEPTH_16)
        {
            aiAttributes[iPos++] = 16;
        }
        else if (m_eDepth == FrameBuffer::DT_DEPTH_24)
        {
            aiAttributes[iPos++] = 24;
        }
        else // m_eDepth == FrameBuffer::DT_DEPTH_32
        {
            aiAttributes[iPos++] = 32;
        }
#else
        aiAttributes[iPos++] = 12;
#endif
    }

#ifndef __sgi
    // Request a stencil buffer.
    if (m_eStencil == FrameBuffer::ST_STENCIL_8)
    {
        aiAttributes[iPos++] = GLX_STENCIL_SIZE;
        aiAttributes[iPos++] = 8;
    }
#endif

    // Request buffering.
    if (m_eBuffering == FrameBuffer::BT_BUFFERED_DOUBLE)
    {
        aiAttributes[iPos++] = GLX_DOUBLEBUFFER;
#ifndef __sgi
        aiAttributes[iPos++] = 1;
#endif
    }

    // Request sampling.
    if (m_eMultisampling != FrameBuffer::MT_SAMPLING_NONE)
    {
        aiAttributes[iPos++] = GLX_SAMPLE_BUFFERS_ARB;
        aiAttributes[iPos++] = 1;
        aiAttributes[iPos++] = GLX_SAMPLES_ARB;
        if (m_eMultisampling == FrameBuffer::MT_SAMPLING_2)
        {
            aiAttributes[iPos++] = 2;
        }
        else  // m_eMultisampling == FrameBuffer::MT_SAMPLING_4
        {
            aiAttributes[iPos++] = 4;
        }
    }

    // The list is zero terminated.
    aiAttributes[iPos] = 0;

    // Get an OpenGL-capable visual.
    int iScreen = DefaultScreen(m_pkDisplay);
    m_pkVisual = glXChooseVisual(m_pkDisplay,iScreen,aiAttributes);
    assert(m_pkVisual);
    if (!m_pkVisual)
    {
        m_pkDisplay = 0;
        m_pkVisual = 0;
        m_ulWindow = 0;
        m_kContext = 0;
        m_bIsConstructed = false;
        return;
    }

    // Create an OpenGL rendering context.
    GLXContext kSharedList = 0;
    Bool bDirectRender = True;
    m_kContext = glXCreateContext(m_pkDisplay,m_pkVisual,kSharedList,
        bDirectRender);
    if (!m_kContext)
    {
        m_pkDisplay = 0;
        m_pkVisual = 0;
        m_ulWindow = 0;
        m_kContext = 0;
        m_bIsConstructed = false;
        return;
    }

    m_bIsConstructed = true;
}
//----------------------------------------------------------------------------
void GlxRenderer::FinishConstruction (Window ulWindow)
{
    // Bind the rendering context to the window.
    m_ulWindow = ulWindow;
    if (!glXMakeCurrent(m_pkDisplay,m_ulWindow,m_kContext))
    {
        m_bIsConstructed = false;
    }

    // Load the function pointers for the OpenGL extensions.  This only
    // needs to occur once.  If you want a file to be created that stores
    // the OpenGL driver information, change the input filename to a
    // valid string; for example, "OpenGLDriverInfo.txt".
    const char* acInfoFile = 0;
    GTInitGLX(acInfoFile);

    InitializeState();
}
//----------------------------------------------------------------------------
void GlxRenderer::Activate ()
{
    if (glXGetCurrentContext() != m_kContext)
    {
        glXMakeCurrent(m_pkDisplay,m_ulWindow,m_kContext);
        OpenGLRenderer::Activate();
    }
}
//----------------------------------------------------------------------------
bool GlxRenderer::IsActive () const
{
    return glXGetCurrentContext() == m_kContext;
}
//----------------------------------------------------------------------------
void GlxRenderer::ToggleFullscreen ()
{
    // TO DO.  How to make full screen GLX window?

    //OpenGLRenderer::ToggleFullscreen();
    //
    //if ( m_bFullscreen )
    //{
    //    m_iSaveWidth = m_iWidth;
    //    m_iSaveHeight = m_iHeight;
    //    glutFullScreen();
    //}
    //else
    //{
    //    glutPositionWindow(24,24);
    //    glutReshapeWindow(m_iSaveWidth,m_iSaveHeight);
    //    Resize(m_iSaveWidth,m_iSaveHeight);
    //}
}
//----------------------------------------------------------------------------
bool GlxRenderer::BeginScene ()
{
    return glXGetCurrentContext() == m_kContext;
}
//----------------------------------------------------------------------------
void GlxRenderer::DisplayBackBuffer ()
{
    glXSwapBuffers(m_pkDisplay,m_ulWindow);
}
//----------------------------------------------------------------------------
void GlxRenderer::SaveContext ()
{
    m_pkSaveDisplay = glXGetCurrentDisplay();
    m_ulSaveWindow = glXGetCurrentDrawable();
    m_kSaveContext = glXGetCurrentContext();
}
//----------------------------------------------------------------------------
void GlxRenderer::RestoreContext ()
{
    glXMakeCurrent(m_pkSaveDisplay,m_ulSaveWindow,m_kSaveContext);
}
//----------------------------------------------------------------------------


