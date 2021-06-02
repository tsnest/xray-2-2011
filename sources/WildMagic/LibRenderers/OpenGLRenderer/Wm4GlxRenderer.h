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

#ifndef WM4GLXRENDERER_H
#define WM4GLXRENDERER_H

#include "Wm4OpenGLRendererLIB.h"
#include "Wm4OpenGLRenderer.h"
#include "GTglxExtensions.h"

namespace Wm4
{

class WM4_RENDERER_ITEM GlxRenderer : public OpenGLRenderer
{
public:
    // construction and destruction
    GlxRenderer (Display* pkDisplay, FrameBuffer::FormatType eFormat,
        FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
        FrameBuffer::BufferingType eBuffering,
        FrameBuffer::MultisamplingType eMultisampling, int iWidth,
        int iHeight);

    virtual void Activate ();
    virtual bool IsActive () const;
    virtual void ToggleFullscreen ();
    virtual bool BeginScene ();
    virtual void DisplayBackBuffer ();
    virtual int LoadFont (const char* acFace, int iSize, bool bBold,
        bool bItalic) { return -1; }
    virtual void UnloadFont (int iFontID) { /**/ }

    Display* GetDisplay () { return m_pkDisplay; }
    XVisualInfo* GetVisual () { return m_pkVisual; }
    Window GetWindow () { return m_ulWindow; }
    GLXContext GetContext () { return m_kContext; }

private:
    // context handling
    virtual void SaveContext ();
    virtual void RestoreContext ();

    // window parameters
    Display* m_pkDisplay;
    XVisualInfo* m_pkVisual;
    Window m_ulWindow;
    GLXContext m_kContext;

    // for switching contexts
    Display* m_pkSaveDisplay;
    Window m_ulSaveWindow;
    GLXContext m_kSaveContext;

    // fullscreen/window support
    int m_iSaveWidth, m_iSaveHeight;

    bool m_bIsConstructed;

// internal use
public:
    // For the application layer and pbuffers.
    void FinishConstruction (Window ulWindow);
    bool IsConstructed () const { return m_bIsConstructed; }
};

}

#endif


