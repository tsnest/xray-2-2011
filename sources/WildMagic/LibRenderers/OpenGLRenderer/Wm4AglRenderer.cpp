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

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4AglRenderer.h"
using namespace Wm4;

Boolean AglRenderer::ms_bDSpStarted = false;

//----------------------------------------------------------------------------
AglRenderer::AglRenderer (AGLDevice hDevice, WindowRef pkWindow,
    FrameBuffer::FormatType eFormat, FrameBuffer::DepthType eDepth,
    FrameBuffer::StencilType eStencil, FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iX, int iY, int iWidth,
    int iHeight)
    :
    OpenGLRenderer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,iWidth,
        iHeight)
{
    assert(m_iWidth > 0 && m_iHeight > 0);
    m_iSaveWidth = m_iWidth;
    m_iSaveHeight = m_iHeight;

    m_iX = iX;
    m_iY = iY;
    m_hAglDevice = hDevice;
    m_pkAglWindow = pkWindow;
    m_pkAglDrawable = (AGLDrawable)GetWindowPort(pkWindow);
    m_pkFullScreenDrawable = 0;
    m_pkDSpContext = 0;
    
    // See if we have a renderer available.
    AGLRendererInfo kAGLInfo = aglQueryRendererInfo(&m_hAglDevice,1);
    if (!kAGLInfo)
    {
        kAGLInfo = aglQueryRendererInfo(0,0);
    }
    if (!kAGLInfo)
    {
        FatalErrorMessage("Cannot obtain information about the renderer.");
    }

    // Set the desired attributes for the renderer's pixel format.
    int aiAttributes[256];
    int iPos = 0;

    aiAttributes[iPos++] = AGL_ACCELERATED;
    aiAttributes[iPos++] = AGL_NO_RECOVERY;

    // The color buffer is RGBA8888.
    aiAttributes[iPos++] = AGL_RGBA;
    aiAttributes[iPos++] = AGL_RED_SIZE;
    aiAttributes[iPos++] = 8;
    aiAttributes[iPos++] = AGL_GREEN_SIZE;
    aiAttributes[iPos++] = 8;
    aiAttributes[iPos++] = AGL_BLUE_SIZE;
    aiAttributes[iPos++] = 8;
    aiAttributes[iPos++] = AGL_ALPHA_SIZE;
    aiAttributes[iPos++] = 8;

    // Request a depth buffer.
    if (m_eDepth != FrameBuffer::DT_DEPTH_NONE)
    {
        aiAttributes[iPos++] = AGL_DEPTH_SIZE;
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
    }

    // Request a stencil buffer.
    if (m_eStencil == FrameBuffer::ST_STENCIL_8)
    {
        aiAttributes[iPos++] = AGL_STENCIL_SIZE;
        aiAttributes[iPos++] = 8;
    }

    // Request buffering.
    if (m_eBuffering == FrameBuffer::BT_BUFFERED_DOUBLE)
    {
        aiAttributes[iPos++] = AGL_DOUBLEBUFFER;
    }

    // Request sampling.
    if (m_eMultisampling != FrameBuffer::MT_SAMPLING_NONE)
    {
        aiAttributes[iPos++] = AGL_SAMPLE_BUFFERS_ARB;
        aiAttributes[iPos++] = 1;
        aiAttributes[iPos++] = AGL_SAMPLES_ARB;
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
    aiAttributes[iPos] = AGL_NONE;

    AGLPixelFormat pkPixelFormat = aglChoosePixelFormat(&m_hAglDevice,1,
        aiAttributes);
    if (!pkPixelFormat)
    {
        // Retry without specifying a device.
        pkPixelFormat = aglChoosePixelFormat(0,0,aiAttributes);
        if (!pkPixelFormat)
        {
            FatalErrorMessage("Cannot choose pixel format.");
        }
    }
    
    // Create an OpenGL context.
    m_pkAglContext = aglCreateContext(pkPixelFormat,0);
    if (!m_pkAglContext || aglGetError() != noErr)
    {
        FatalErrorMessage("Cannot create rendering context.");
    }
    if (!aglSetDrawable(m_pkAglContext,m_pkAglDrawable)
    ||   aglGetError() != noErr)
    {
        FatalErrorMessage("Cannot set the current drawable.");
    }
    if (!aglSetCurrentContext(m_pkAglContext) || aglGetError() != noErr)
    {
        FatalErrorMessage("Cannot set the current rendering context.");
    }
    aglDestroyPixelFormat(pkPixelFormat);
    SetUpBufferRect();

    // Create default screen text font.
    const char* acDefaultName = "Monaco";
    int iDefaultSize = 9;
    int iFontID = LoadFont(acDefaultName,iDefaultSize,false,false);
    assert(iFontID == 0);
    (void)iFontID;  // avoid warning in release build
    
    // Complete initialization.
    // Load the function pointers for the OpenGL extensions.  This only
    // needs to occur once.  If you want a file to be created that stores
    // the OpenGL driver information, change the input filename to a
    // valid string; for example, "OpenGLDriverInfo.txt".
    const char* acInfoFile = 0;
    GTInitGL(acInfoFile);
    InitializeState();

    // Disable vertical sync.  If you want to enable this, change the value
    // of iVSValue to 1.
    int iVSValue = 0;
    aglSetInteger(m_pkAglContext,AGL_SWAP_INTERVAL,&iVSValue);

    // Allocate temporary back buffer to be used for font management.
    PixMapHandle hPixmap = (**(GDHandle)hDevice).gdPMap;
    Rect kArea = { 0, 0, 32, 32 };
    int iDepth = GetPixDepth(hPixmap);
    
    OSErr iError = NewGWorld(&m_pkBack,iDepth,&kArea,0,0,
        useTempMem | pixPurge);
    if (iError != noErr || !m_pkBack)
    {
        FatalErrorMessage("Cannot initialize font management.");
    }

    // Assign desired font settings to back buffer.
    GWorldPtr pkCurrentWorld;
    GDHandle kCurrentDevice;
    GetGWorld(&pkCurrentWorld,&kCurrentDevice);
    SetGWorld(m_pkBack,0);

    Str255 kFontName;
    kFontName[0] = (int)strlen(acDefaultName);
    strcpy((char*)kFontName+1,acDefaultName);
    short usFNum;
    GetFNum(kFontName,&usFNum);
    TextFont(usFNum);
    TextSize(iDefaultSize);

    SetGWorld(pkCurrentWorld,kCurrentDevice);
}
//----------------------------------------------------------------------------
AglRenderer::~AglRenderer ()
{
    if (m_bFullscreen)
    {
        ToggleFullscreen();
    }

    for (int i = 0; i < (int)m_kDLInfo.size(); i++)
    {
        DisplayListInfo kInfo = m_kDLInfo[i];
        glDeleteLists(kInfo.Start,kInfo.Quantity);
    }

    DisposeGWorld(m_pkBack);
    aglSetDrawable(m_pkAglContext,0);
    aglDestroyContext(m_pkAglContext);
}
//----------------------------------------------------------------------------
void AglRenderer::Activate ()
{
    if (aglGetCurrentContext() != m_pkAglContext)
    {
        aglSetCurrentContext(m_pkAglContext);
        OpenGLRenderer::Activate();
    }
}
//----------------------------------------------------------------------------
bool AglRenderer::IsActive () const
{
    return aglGetCurrentContext() == m_pkAglContext;
}
//----------------------------------------------------------------------------
void AglRenderer::Resize (int iWidth, int iHeight)
{
    AGLContext kAglContext = aglGetCurrentContext();
    if (kAglContext != m_pkAglContext)
    {
        aglSetCurrentContext(m_pkAglContext);
    }

    OpenGLRenderer::Resize(iWidth,iHeight);
    SetUpBufferRect();

    if (kAglContext != m_pkAglContext)
    {
        aglSetCurrentContext(kAglContext);
    }
}
//----------------------------------------------------------------------------
void AglRenderer::ToggleFullscreen ()
{
    OpenGLRenderer::ToggleFullscreen();
    glFinish();

    if (m_bFullscreen)
    {
        PixMapHandle hPixmap = (**(GDHandle)m_hAglDevice).gdPMap;
        Rect kArea;
        GetPixBounds(hPixmap,&kArea);
        int iWidth = kArea.right - kArea.left;
        int iHeight = kArea.bottom - kArea.top;
        int iDepth = GetPixDepth(hPixmap);

        if (!ms_bDSpStarted)
        {
            if (DSpStartup() != noErr)
            {
                FatalErrorMessage("FullScreen: Cannot start DrawSprocket.");
            }
            ms_bDSpStarted = true;
        }

        DSpContextAttributes kContextAttributes;
        BlockZero(&kContextAttributes,sizeof(DSpContextAttributes));
        kContextAttributes.displayWidth = iWidth;
        kContextAttributes.displayHeight = iHeight;
        kContextAttributes.displayBestDepth = iDepth;
        kContextAttributes.backBufferBestDepth = iDepth;
        kContextAttributes.colorNeeds = kDSpColorNeeds_Require;
        kContextAttributes.displayDepthMask =
            (OptionBits)(kDSpDepthMask_All);
        kContextAttributes.backBufferDepthMask =
            (OptionBits)(kDSpDepthMask_All);
        kContextAttributes.pageCount = 1;
        OSStatus eErr = DSpFindBestContext(&kContextAttributes,
            &m_pkDSpContext);
        if (eErr != noErr || !m_pkDSpContext)
        {
            FatalErrorMessage("FullScreen: Cannot find best context.");
        }
        if (kContextAttributes.gameMustConfirmSwitch)
        {
            FatalErrorMessage("FullScreen: Must confirm switch.");
        }
        eErr = DSpContext_Reserve(m_pkDSpContext,&kContextAttributes);
        if (eErr != noErr)
        {
            FatalErrorMessage("FullScreen: Cannot reserve context.");
        }
        eErr = DSpContext_SetState(m_pkDSpContext,kDSpContextState_Active);
        if (eErr != noErr)
        {
            FatalErrorMessage("FullScreen: Cannot activate context.");
        }
        eErr = DSpContext_GetFrontBuffer(m_pkDSpContext,
            &m_pkFullScreenDrawable);
        if (eErr != noErr || !m_pkFullScreenDrawable)
        {
            FatalErrorMessage("FullScreen: Cannot get front buffer.");
        }

        if (!aglSetDrawable(m_pkAglContext,m_pkFullScreenDrawable)
        ||   aglGetError() != noErr)
        {
            FatalErrorMessage("FullScreen: Cannot set new drawable.");
        }
        aglUpdateContext(m_pkAglContext);

        // TO DO.  Why do we need to save these?
        m_iSaveWidth = iWidth;
        m_iSaveHeight = iHeight;
    }
    else
    {
        aglSetDrawable(m_pkAglContext,0);
        if (m_pkDSpContext)
        {
            OSStatus eErr = DSpContext_SetState(m_pkDSpContext,
                kDSpContextState_Inactive);
            if (eErr != noErr)
            {
                FatalErrorMessage("FullScreen: Cannot deactivate context.");
            }
            eErr = DSpContext_Release(m_pkDSpContext);
            if (eErr != noErr)
            {
                FatalErrorMessage("FullScreen: Cannot release context.");
            }
            m_pkDSpContext = 0;
            m_pkFullScreenDrawable = 0;
        }
        
        aglSetDrawable(m_pkAglContext,m_pkAglDrawable);
        aglUpdateContext(m_pkAglContext);
        
        // TO DO.  Why do we need the kArea dimensions?
        CGrafPtr pkPort = (CGrafPtr)m_pkAglDrawable;
        Rect kArea;
        GetPortBounds(pkPort,&kArea);
        m_iSaveWidth = kArea.right - kArea.left;
        m_iSaveHeight = kArea.bottom - kArea.top;
    }
}
//----------------------------------------------------------------------------
bool AglRenderer::BeginScene ()
{
    return aglGetCurrentContext() == m_pkAglContext;
}
//----------------------------------------------------------------------------
void AglRenderer::DisplayBackBuffer ()
{
    if (aglGetDrawable(m_pkAglContext))
    {
        aglSwapBuffers(m_pkAglContext);
    }
}
//----------------------------------------------------------------------------
int AglRenderer::LoadFont (const char* acFace, int iSize, bool bBold,
    bool bItalic)
{
    DisplayListInfo kInfo;
    kInfo.Quantity = 256;
    kInfo.Start = glGenLists(256);
    kInfo.Base = 1;
    m_kDLInfo.push_back(kInfo);

    Str255 kFontName;
    kFontName[0] = (int)strlen(acFace);
    strcpy((char*)kFontName+1,acFace);

    short usFNum;
    GetFNum(kFontName,&usFNum);

    Style cStyle = normal;
    if (bBold)
    {
        cStyle |= bold;
    }
    if (bItalic)
    {
        cStyle |= italic;
    }

    if (!aglUseFont(m_pkAglContext,usFNum,cStyle,iSize,0,kInfo.Quantity,
        kInfo.Start))
    {
        glDeleteLists(kInfo.Start,kInfo.Quantity);
        m_kDLInfo.pop_back();
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH,0);

    return (int)m_kDLInfo.size() - 1;
}
//----------------------------------------------------------------------------
AGLDevice AglRenderer::GetDevice ()
{
    return m_hAglDevice;
}
//----------------------------------------------------------------------------
WindowRef AglRenderer::GetWindow ()
{
    return m_pkAglWindow;
}
//----------------------------------------------------------------------------
AGLDrawable AglRenderer::GetDrawable ()
{
    return m_pkAglDrawable;
}
//----------------------------------------------------------------------------
AGLContext AglRenderer::GetContext ()
{
    return m_pkAglContext;
}
//----------------------------------------------------------------------------
void AglRenderer::SetUpBufferRect ()
{
    // If the context is smaller than the underlying surface, we clip it.
    CGrafPtr pkPort = aglGetDrawable(m_pkAglContext);
    Rect kPortRect = { 0 };
    GetPortBounds(pkPort,&kPortRect);
    int iPortWidth = kPortRect.right - kPortRect.left;
    int iPortHeight = kPortRect.bottom - kPortRect.top;
    if (iPortWidth != m_iWidth || iPortHeight != m_iHeight)
    {
        int iHeight = iPortHeight + 1;
        int iY = iHeight - m_iY;
        if (--iY < 0)
        {
            iY = 0;
        }
        GLint aiRect[4] = { m_iX, iY - m_iHeight, m_iWidth, m_iHeight };
        if (!aglSetInteger(m_pkAglContext,AGL_BUFFER_RECT,aiRect))
        {
            FatalErrorMessage("Cannot assign AGL_BUFFER_RECT.");
        }
        if (!aglEnable(m_pkAglContext,AGL_BUFFER_RECT))
        {
            FatalErrorMessage("Cannot enable AGL_BUFFER_RECT.");
        }
        aglUpdateContext(m_pkAglContext);
    }
    else
    {
        if (!aglDisable(m_pkAglContext,AGL_BUFFER_RECT))
        {
            FatalErrorMessage("Cannot disable AGL_BUFFER_RECT.");
        }
        aglUpdateContext(m_pkAglContext);
    }
}
//----------------------------------------------------------------------------
void AglRenderer::FatalErrorMessage (const char* acMessage)
{
    if (m_pkDSpContext)
    {
        DSpContextState kContextState;
        if (DSpContext_GetState(m_pkDSpContext,&kContextState) == noErr
        &&  kContextState == kDSpContextState_Active)
        {
            DSpContext_SetState(m_pkDSpContext,kDSpContextState_Inactive);
            DSpContext_Release(m_pkDSpContext);
        }
    }
    
    char acHeader[256];
    GLenum eAglError = aglGetError();
    sprintf(acHeader,"AglRenderer encountered error %ld.",(long)eAglError);
    Str255 kHeader;
    kHeader[0] = strlen(acHeader);
    strncpy((char*)kHeader+1,acHeader,kHeader[0]);

    int iLength = strlen(acMessage);
    Str255 kMessage;
    kMessage[0] = iLength;
    strncpy((char*)kMessage+1,acMessage,iLength);
    SInt16 siItem;
    StandardAlert(kAlertStopAlert,kHeader,kMessage,0,&siItem);
    ExitToShell();
}
//----------------------------------------------------------------------------
void AglRenderer::SaveContext ()
{
    m_pkSaveAglContext = aglGetCurrentContext();
}
//----------------------------------------------------------------------------
void AglRenderer::RestoreContext ()
{
    aglSetCurrentContext(m_pkSaveAglContext);
}
//----------------------------------------------------------------------------


