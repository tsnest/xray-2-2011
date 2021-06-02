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
// Version: 4.0.6 (2008/12/10)

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4OpenGLResources.h"
#include "Wm4OpenGLFrameBuffer.h"
using namespace Wm4;

WM4_IMPLEMENT_INITIALIZE(OpenGLFrameBuffer);

//----------------------------------------------------------------------------
void OpenGLFrameBuffer::Initialize ()
{
    ms_aoCreate[Renderer::OPENGL] = &OpenGLFrameBuffer::Create;
    ms_aoDestroy[Renderer::OPENGL] = &OpenGLFrameBuffer::Destroy;
}
//----------------------------------------------------------------------------
FrameBuffer* OpenGLFrameBuffer::Create (FormatType eFormat, DepthType eDepth,
    StencilType eStencil, BufferingType eBuffering,
    MultisamplingType eMultisampling, Renderer* pkRenderer, Texture* pkTarget)
{
    // Check for required extensions.
    if (!GTExistsGlExtFrameBufferObject)
    {
        return 0;
    }

    if (pkRenderer && pkTarget)
    {
        OpenGLFrameBuffer* pkBuffer = WM4_NEW OpenGLFrameBuffer(eFormat,
            eDepth,eStencil,eBuffering,eMultisampling,pkRenderer,pkTarget);

        if (pkBuffer->InitializeBuffer())
        {
            return pkBuffer;
        }

        WM4_DELETE pkBuffer;
    }
    return 0;
}
//----------------------------------------------------------------------------
void OpenGLFrameBuffer::Destroy (FrameBuffer* pkBuffer)
{
    ((OpenGLFrameBuffer*)pkBuffer)->TerminateBuffer();
    WM4_DELETE pkBuffer;
}
//----------------------------------------------------------------------------
OpenGLFrameBuffer::OpenGLFrameBuffer (FormatType eFormat, DepthType eDepth,
    StencilType eStencil, BufferingType eBuffering,
    MultisamplingType eMultisampling, Renderer* pkRenderer, Texture* pkTarget)
    :
    FrameBuffer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,pkRenderer,
        pkTarget)
{
    m_bEnabled = false;
}
//----------------------------------------------------------------------------
OpenGLFrameBuffer::~OpenGLFrameBuffer ()
{
}
//----------------------------------------------------------------------------
bool OpenGLFrameBuffer::InitializeBuffer ()
{
    Image* pkImage = m_pkTarget->GetImage();
    if (pkImage->IsCubeImage())
    {
        // TO DO.  Cube map images not supported yet, but they should be.
        return false;
    }

    ResourceIdentifier* pkID = m_pkTarget->GetIdentifier(m_pkRenderer);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;
    m_uiTargetID = pkResource->ID;
    glBindTexture(GL_TEXTURE_2D,m_uiTargetID);

    // Attach the texture to the framebuffer.
    glGenFramebuffersEXT(1,&m_uiFrameBufferID);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_uiFrameBufferID);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D,m_uiTargetID,0);

    if(m_eDepth != DT_DEPTH_NONE)
    {
        // Generate a depthbuffer attachment.
        glGenRenderbuffersEXT(1, &m_uiDepthBufferID);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_uiDepthBufferID);
        glRenderbufferStorageEXT
        (
            GL_RENDERBUFFER_EXT,
            GL_DEPTH_COMPONENT,
            pkImage->GetBound(0),
            pkImage->GetBound(1)
        );

        // Attach the depthbuffer to the framebuffer.
        glFramebufferRenderbufferEXT
        (
            GL_FRAMEBUFFER_EXT,
            GL_DEPTH_ATTACHMENT_EXT,
            GL_RENDERBUFFER_EXT,
            m_uiDepthBufferID
        );
    }
    else
    {
        m_uiDepthBufferID = 0;
    }

    GLenum uiStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    int iStopHere;
    switch (uiStatus)
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        iStopHere = 0;
        glBindTexture(GL_TEXTURE_2D,0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
        return true;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENTS_EXT:
        iStopHere = 1;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        iStopHere = 2;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        iStopHere = 3;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        iStopHere = 4;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        iStopHere = 5;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        iStopHere = 6;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        iStopHere = 7;
        break;
    }
    assert(false);
    return false;
}
//----------------------------------------------------------------------------
void OpenGLFrameBuffer::TerminateBuffer ()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1,&m_uiFrameBufferID);
    glDeleteRenderbuffersEXT(1, &m_uiDepthBufferID);
}
//----------------------------------------------------------------------------
void OpenGLFrameBuffer::Enable ()
{
    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_uiFrameBufferID);

    OpenGLRenderer* pkRenderer = (OpenGLRenderer*)m_pkRenderer;
    Image* pkImage = m_pkTarget->GetImage();
    glViewport(0,0,pkImage->GetBound(0),pkImage->GetBound(1));
    pkRenderer->OnFrustumChange();
    pkRenderer->OnFrameChange();

    m_bEnabled = true;
}
//----------------------------------------------------------------------------
void OpenGLFrameBuffer::Disable ()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    glBindTexture(GL_TEXTURE_2D,m_uiTargetID);

    OpenGLRenderer* pkRenderer = (OpenGLRenderer*)m_pkRenderer;
    pkRenderer->OnViewportChange();
    pkRenderer->OnFrustumChange();
    pkRenderer->OnFrameChange();

    m_bEnabled = false;
}
//----------------------------------------------------------------------------
void OpenGLFrameBuffer::CopyToTexture (bool bFillImage)
{
    // TO DO.  Only the software renderer actually does something when
    // bFillImage is 'false'.  If that is not necessary, then CopyToTexture
    // should have no parameters and should be called only when you want to
    // copy the VRAM copy into system memory.

    if (bFillImage)
    {
        Image* pkImage = m_pkTarget->GetImage();
        assert(pkImage);
        pkImage->AllocateData();
        Image::FormatMode eFormat = pkImage->GetFormat();
        unsigned char* aucData = pkImage->GetData();
        int iBound0 = pkImage->GetBound(0);
        int iBound1 = pkImage->GetBound(1);

        GLenum eGLFormat, eGLType;
        switch(eFormat)
        {
        case Image::IT_RGB888:
            eGLFormat = GL_RGB;
            eGLType = GL_UNSIGNED_BYTE;
            break;
        case Image::IT_RGBA8888:
            eGLFormat = GL_RGBA;
            eGLType = GL_UNSIGNED_BYTE;
            break;
        case Image::IT_RGB32F:
            eGLFormat = GL_RGB;
            eGLType = GL_FLOAT;
            break;
        case Image::IT_RGBA32F:
            eGLFormat = GL_RGBA;
            eGLType = GL_FLOAT;
            break;
        case Image::IT_RGB16F:
            eGLFormat = GL_RGB;
            eGLType = GL_HALF_FLOAT_ARB;
            break;
        case Image::IT_RGBA16F:
            eGLFormat = GL_RGBA;
            eGLType = GL_HALF_FLOAT_ARB;
            break;
        case Image::IT_RGB16I:
            eGLFormat = GL_RGB;
            eGLType = GL_UNSIGNED_SHORT;
            break;
        case Image::IT_RGBA16I:
            eGLFormat = GL_RGBA;
            eGLType = GL_UNSIGNED_SHORT;
            break;
        case Image::IT_INTENSITY8I:
            eGLFormat = GL_INTENSITY;
            eGLType = GL_UNSIGNED_BYTE;
            break;
        case Image::IT_INTENSITY16I:
            eGLFormat = GL_INTENSITY;
            eGLType = GL_UNSIGNED_SHORT;
            break;
        case Image::IT_INTENSITY16F:
            eGLFormat = GL_INTENSITY;
            eGLType = GL_HALF_FLOAT_ARB;
            break;
        case Image::IT_INTENSITY32F:
            eGLFormat = GL_INTENSITY;
            eGLType = GL_FLOAT;
            break;
        case Image::IT_RGB565:
            eGLFormat = GL_RGB;
            eGLType = GL_UNSIGNED_BYTE;
            break;
        case Image::IT_RGBA5551:
            eGLFormat = GL_RGBA;
            eGLType = GL_UNSIGNED_BYTE;
            break;
        case Image::IT_RGBA4444:
            eGLFormat = GL_RGBA;
            eGLType = GL_UNSIGNED_BYTE;
            break;
        default:
            assert(false);
            return;
        }

        bool bEnabled = m_bEnabled;
        if(!bEnabled)
        {
            Enable();
        }

        glReadPixels(0,0,iBound0,iBound1,eGLFormat,eGLType,aucData);

        if(!bEnabled)
        {
            Disable();
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLFrameBuffer::CopyTo (VertexBuffer* pkVBuffer,
    bool bCopyToSystemMemory)
{
    assert(pkVBuffer);

    Image* pkImage = m_pkTarget->GetImage();
    assert(pkImage);
    Image::FormatMode eFormat = pkImage->GetFormat();
    int iBound0 = pkImage->GetBound(0);
    int iBound1 = pkImage->GetBound(1);

    GLenum eGLFormat, eGLType;
    switch(eFormat)
    {
    case Image::IT_RGB888:
        eGLFormat = GL_RGB;
        eGLType = GL_UNSIGNED_BYTE;
        break;
    case Image::IT_RGBA8888:
        eGLFormat = GL_RGBA;
        eGLType = GL_UNSIGNED_BYTE;
        break;
    case Image::IT_RGB32F:
        eGLFormat = GL_RGB;
        eGLType = GL_FLOAT;
        break;
    case Image::IT_RGBA32F:
        eGLFormat = GL_RGBA;
        eGLType = GL_FLOAT;
        break;
    case Image::IT_RGB16F:
        eGLFormat = GL_RGB;
        eGLType = GL_HALF_FLOAT_ARB;
        break;
    case Image::IT_RGBA16F:
        eGLFormat = GL_RGBA;
        eGLType = GL_HALF_FLOAT_ARB;
        break;
    case Image::IT_RGB16I:
        eGLFormat = GL_RGB;
        eGLType = GL_UNSIGNED_SHORT;
        break;
    case Image::IT_RGBA16I:
        eGLFormat = GL_RGBA;
        eGLType = GL_UNSIGNED_SHORT;
        break;
    case Image::IT_INTENSITY8I:
        eGLFormat = GL_INTENSITY;
        eGLType = GL_UNSIGNED_BYTE;
        break;
    case Image::IT_INTENSITY16I:
        eGLFormat = GL_INTENSITY;
        eGLType = GL_UNSIGNED_SHORT;
        break;
    case Image::IT_INTENSITY16F:
        eGLFormat = GL_INTENSITY;
        eGLType = GL_HALF_FLOAT_ARB;
        break;
    case Image::IT_INTENSITY32F:
        eGLFormat = GL_INTENSITY;
        eGLType = GL_FLOAT;
        break;
    case Image::IT_RGB565:
        eGLFormat = GL_RGB;
        eGLType = GL_UNSIGNED_SHORT_5_6_5_REV;
        break;
    case Image::IT_RGBA5551:
        eGLFormat = GL_RGBA;
        eGLType = GL_UNSIGNED_SHORT_1_5_5_5_REV;
        break;
    case Image::IT_RGBA4444:
        eGLFormat = GL_RGBA;
        eGLType = GL_UNSIGNED_SHORT_4_4_4_4_REV;
        break;
    default:
        assert(false);
        return;
    }

    bool bEnabled = m_bEnabled;
    if(!bEnabled)
    {
        Enable();
    }

    // Get the OpenGL handle for the vertex buffer.
    ResourceIdentifier* pkID = pkVBuffer->GetIdentifier(m_pkRenderer);
    assert(pkID);
    VBufferID* pkResource = (VBufferID*)pkID;

    // This is the standard way to render-to-vertexbuffer in OpenGL.  It
    // avoids a copy from VRAM to system memory.
    glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB,pkResource->ID);
    glReadPixels(0,0,iBound0,iBound1,eGLFormat,eGLType,0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB,0);

    if(!bEnabled)
    {
        Disable();
    }

    if(bCopyToSystemMemory)
    {
        unsigned int uiSize = iBound0*iBound1*pkImage->GetBytesPerPixel();
        glBindBuffer(GL_ARRAY_BUFFER,pkResource->ID);
        void* pvData = glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
        System::Memcpy(pkVBuffer->GetData(),uiSize,pvData,uiSize);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}
//----------------------------------------------------------------------------
