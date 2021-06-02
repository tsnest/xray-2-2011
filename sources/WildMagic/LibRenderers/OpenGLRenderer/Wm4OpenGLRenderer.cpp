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
// Version: 4.0.6 (2007/09/23)

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4OpenGLRenderer.h"
#include "Wm4Camera.h"
using namespace Wm4;

//----------------------------------------------------------------------------
OpenGLRenderer::OpenGLRenderer (FrameBuffer::FormatType eFormat,
    FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
    FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iWidth, int iHeight)
    :
    Renderer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,iWidth,iHeight)
{
}
//----------------------------------------------------------------------------
OpenGLRenderer::~OpenGLRenderer ()
{
}
//----------------------------------------------------------------------------
void OpenGLRenderer::InitializeState ()
{
    // Vertices always exist.
    glEnableClientState(GL_VERTEX_ARRAY);

    // Colors disabled, current color is WHITE, clear color is WHITE.
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4fv((const float*)ColorRGBA::WHITE);
    glClearColor(m_kClearColor[0],m_kClearColor[1],m_kClearColor[2],
        m_kClearColor[3]);

    // Normals are disabled.
    glDisableClientState(GL_NORMAL_ARRAY);

    // Query for capabilities.

    // Get the number of supported texture images for vertex program.
    ms_iMaxVShaderImages = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,&ms_iMaxVShaderImages);

    // Get the number of supported texture units for pixel programs.
    ms_iMaxPShaderImages = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&ms_iMaxPShaderImages);
    if (ms_iMaxPShaderImages > 0)
    {
        m_apkActiveSamplers =
            WM4_NEW SamplerInformation*[ms_iMaxPShaderImages];

        memset(m_apkActiveSamplers,0,ms_iMaxPShaderImages*
            sizeof(SamplerInformation*));
    }

    // TO DO.  The OpenGL shading language has separate concepts of "texture
    // image units" and "texture coordinate sets".  The values iMaxTextures
    // and iMaxTCoords can be different.  Replace m_iMaxTextures with
    // m_iMaxTextureImageUnits and add m_iMaxTCoords.  Anytime shader programs
    // are loaded, the number of texture coordinate inputs/outputs and the
    // number of samplers need to be compared with what the hardware can
    // support.
    ms_iMaxTCoords = 0;
    glGetIntegerv(GL_MAX_TEXTURE_COORDS,&ms_iMaxTCoords);

    // OpenGL supports a primary and a secondary color
    ms_iMaxColors = 2;

    // Set up light model.  TO DO:  The GL light function calls here are
    // not necessary for a shader-based engine.
    GLint iMaxLights;
    glGetIntegerv(GL_MAX_LIGHTS,&iMaxLights);
    assert(iMaxLights > 0);
    ms_iMaxLights = (int)iMaxLights;
    m_aspkLight = WM4_NEW ObjectPtr[ms_iMaxLights];
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,(const float*)ColorRGBA::BLACK);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
    glDisable(GL_LIGHTING);

    // Get the stencil buffer size.
    GLint iBits = 0;
    glGetIntegerv(GL_STENCIL_BITS,&iBits);
    ms_iMaxStencilIndices = (iBits > 0 ? (1 << iBits) : 0);

    // Get the maximum number of user-defined clip planes.
    GLint iMaxUseClipPlanes = 0;
    glGetIntegerv(GL_MAX_CLIP_PLANES,&iMaxUseClipPlanes);
    ms_iMaxUserClipPlanes = (int)iMaxUseClipPlanes;

    // Get the maximum anisotropic distortion.  The default is 1.0f.
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&ms_fMaxAnisotropy);

    // Disable drawing of lines as sequences of dashes.
    glDisable(GL_LINE_STIPPLE);

    // Initialize global render state to default settings.
    SetGlobalState(GlobalState::Default);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBackBuffer ()
{
    glClearColor(m_kClearColor[0],m_kClearColor[1],m_kClearColor[2],
        m_kClearColor[3]);

    glClear(GL_COLOR_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearZBuffer ()
{
    glClearDepth((double)m_fClearDepth);

    glClear(GL_DEPTH_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearStencilBuffer ()
{
    glClearStencil((GLint)m_uiClearStencil);

    glClear(GL_STENCIL_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBuffers ()
{
    glClearColor(m_kClearColor[0],m_kClearColor[1],m_kClearColor[2],
        m_kClearColor[3]);
    glClearDepth((double)m_fClearDepth);
    glClearStencil((GLint)m_uiClearStencil);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBackBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glClearColor(m_kClearColor[0],m_kClearColor[1],m_kClearColor[2],
        m_kClearColor[3]);

    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearZBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glClearDepth((double)m_fClearDepth);

    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glClearStencil((GLint)m_uiClearStencil);

    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glClear(GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::ClearBuffers (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    glClearColor(m_kClearColor[0],m_kClearColor[1],m_kClearColor[2],
        m_kClearColor[3]);
    glClearDepth((double)m_fClearDepth);
    glClearStencil((GLint)m_uiClearStencil);

    glEnable(GL_SCISSOR_TEST);
    glScissor(iXPos,iYPos,iWidth,iHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glStencilMask((GLuint)~0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetColorMask (bool bAllowRed, bool bAllowGreen,
    bool bAllowBlue, bool bAllowAlpha)
{
    Renderer::SetColorMask(bAllowRed,bAllowGreen,bAllowBlue,bAllowAlpha);

    glColorMask((GLboolean)bAllowRed,(GLboolean)bAllowGreen,
        (GLboolean)bAllowBlue,(GLboolean)bAllowAlpha);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::EnableUserClipPlane (int i, const Plane3f& rkPlane)
{
    GLdouble adPlane[4] =
    {
        (double)rkPlane.Normal.X(),
        (double)rkPlane.Normal.Y(),
        (double)rkPlane.Normal.Z(),
        (double)-rkPlane.Constant
    };
    glClipPlane(GL_CLIP_PLANE0 + i,adPlane);
    glEnable(GL_CLIP_PLANE0 + i);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DisableUserClipPlane (int i)
{
    glDisable(GL_CLIP_PLANE0 + i);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetPointSize (float fSize)
{
    Renderer::SetPointSize(fSize);
    glPointSize(m_fPointSize);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetLineWidth (float fWidth)
{
    Renderer::SetLineWidth(fWidth);
    glLineWidth(m_fLineWidth);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetLineStipple (int iRepeat, unsigned short usPattern)
{
    Renderer::SetLineStipple(iRepeat,usPattern);
    if (m_iLineStippleRepeat && m_usLineStipplePattern)
    {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(m_iLineStippleRepeat,m_usLineStipplePattern);
    }
    else
    {
        glDisable(GL_LINE_STIPPLE);
    }
}
//----------------------------------------------------------------------------
