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
// Version: 4.0.2 (2006/08/21)

#ifndef WM4AGLRENDERER_H
#define WM4AGLRENDERER_H

#include "Wm4OpenGLRendererLIB.h"
#include "Wm4OpenGLRenderer.h"
#include <AGL/agl.h>
#include <Carbon/Carbon.h>
#include <DrawSprocket/DrawSprocket.h>

namespace Wm4
{

class WM4_RENDERER_ITEM AglRenderer : public OpenGLRenderer
{
public:
    // construction and destruction
    AglRenderer (AGLDevice hDevice, WindowRef pkWindow,
        FrameBuffer::FormatType eFormat, FrameBuffer::DepthType eDepth,
        FrameBuffer::StencilType eStencil,
        FrameBuffer::BufferingType eBuffering,
        FrameBuffer::MultisamplingType eMultisampling, int iX, int iY,
        int iWidth, int iHeight);

    virtual ~AglRenderer ();

    virtual void Activate ();
    virtual bool IsActive () const;
    virtual void Resize (int iWidth, int iHeight);
    virtual void ToggleFullscreen ();

    virtual bool BeginScene ();
    virtual void DisplayBackBuffer ();

    virtual int LoadFont (const char* acFace, int iSize, bool bBold,
        bool bItalic);

    // window parameters
    AGLDevice GetDevice ();
    WindowRef GetWindow ();
    AGLDrawable GetDrawable ();
    AGLContext GetContext ();

private:
    // set up drawing area
    void SetUpBufferRect ();

    // display error message and terminate
    void FatalErrorMessage (const char* acMessage);

    // context handling
    virtual void SaveContext ();
    virtual void RestoreContext ();

    // display parameters
    AGLDevice m_hAglDevice;
    WindowRef m_pkAglWindow;
    AGLDrawable m_pkAglDrawable;
    AGLContext m_pkAglContext;
    AGLDrawable m_pkFullScreenDrawable;
    DSpContextReference m_pkDSpContext;

    // for switching contexts
    AGLContext m_pkSaveAglContext;

    // font management
    GWorldPtr m_pkBack;

    // fullscreen/window support
    int m_iX, m_iY;
    int m_iSaveWidth, m_iSaveHeight;

    static Boolean ms_bDSpStarted;
};

}

#endif



