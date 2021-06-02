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
// Version: 4.0.1 (2007/07/23)

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftResources.h"
#include "Wm4SoftFrameBuffer.h"
using namespace Wm4;

WM4_IMPLEMENT_INITIALIZE(SoftFrameBuffer);

//----------------------------------------------------------------------------
void SoftFrameBuffer::Initialize ()
{
    ms_aoCreate[Renderer::SOFTWARE] = &SoftFrameBuffer::Create;
    ms_aoDestroy[Renderer::SOFTWARE] = &SoftFrameBuffer::Destroy;
}
//----------------------------------------------------------------------------
FrameBuffer* SoftFrameBuffer::Create (FormatType eFormat, DepthType eDepth,
    StencilType eStencil, BufferingType eBuffering,
    MultisamplingType eMultisampling, Renderer* pkRenderer, Texture* pkTarget)
{
    // Offscreen framebuffers are created by the factory.  The SoftRenderer
    // uses the SoftFrameBuffer constructor directly.
    if (pkRenderer)
    {
        return WM4_NEW SoftFrameBuffer(eFormat,eDepth,eStencil,eBuffering,
            eMultisampling,pkRenderer,pkTarget,0,0,true);
    }
    return 0;
}
//----------------------------------------------------------------------------
void SoftFrameBuffer::Destroy (FrameBuffer* pkBuffer)
{
    WM4_DELETE pkBuffer;
}
//----------------------------------------------------------------------------
SoftFrameBuffer::SoftFrameBuffer (FormatType eFormat, DepthType eDepth,
    StencilType eStencil, BufferingType eBuffering,
    MultisamplingType eMultisampling, Renderer* pkRenderer, Texture* pkTarget,
    int iWidth, int iHeight, bool bCreateColorBuffer)
    :
    FrameBuffer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,pkRenderer,
        pkTarget)
{
    if (m_pkTarget)
    {
        Image* pkImage = m_pkTarget->GetImage();
        m_iWidth = pkImage->GetBound(0);
        m_iHeight = pkImage->GetBound(1);
    }
    else
    {
        m_iWidth = iWidth;
        m_iHeight = iHeight;
    }
    m_iQuantity = m_iWidth*m_iHeight;

    if (m_eFormat == FrameBuffer::FT_FORMAT_RGB
    ||  m_eFormat == FrameBuffer::FT_FORMAT_RGBA)
    {
        if (bCreateColorBuffer)
        {
            m_auiColorBuffer = WM4_NEW unsigned int[m_iQuantity];
            m_bColorBufferOwner = true;
        }
        else
        {
            m_auiColorBuffer = 0;
            m_bColorBufferOwner = false;
        }
        m_akColorBuffer = WM4_NEW ColorRGBA[m_iQuantity];
    }
    else
    {
        // The ofscreen framebuffer object has no color buffer, so it does
        // not matter about ownership of deallocation.
        m_auiColorBuffer = 0;
        m_akColorBuffer = 0;
        m_bColorBufferOwner = true;
    }

    switch (m_eDepth)
    {
    case FrameBuffer::DT_DEPTH_NONE:
        m_uiMaxDepthValue = 0;
        m_auiDepthBuffer = 0;
        m_afDepthBuffer = 0;
        break;
    case FrameBuffer::DT_DEPTH_16:
        m_uiMaxDepthValue = 0x0000FFFF;
        m_auiDepthBuffer = WM4_NEW unsigned int[m_iQuantity];
        m_afDepthBuffer = WM4_NEW float[m_iQuantity];
        break;
    case FrameBuffer::DT_DEPTH_24:
        m_uiMaxDepthValue = 0x00FFFFFF;
        m_auiDepthBuffer = WM4_NEW unsigned int[m_iQuantity];
        m_afDepthBuffer = WM4_NEW float[m_iQuantity];
        break;
    case FrameBuffer::DT_DEPTH_32:
        m_uiMaxDepthValue = 0xFFFFFFFF;
        m_auiDepthBuffer = WM4_NEW unsigned int[m_iQuantity];
        m_afDepthBuffer = WM4_NEW float[m_iQuantity];
        break;
    }

    switch (m_eStencil)
    {
    case FrameBuffer::ST_STENCIL_NONE:
        m_uiMaxStencilValue = 0;
        m_auiStencilBuffer = 0;
        break;
    case FrameBuffer::ST_STENCIL_8:
        m_uiMaxStencilValue = 0x000000FF;
        m_auiStencilBuffer = WM4_NEW unsigned int[m_iQuantity];
        break;
    }
}
//----------------------------------------------------------------------------
SoftFrameBuffer::~SoftFrameBuffer ()
{
    if (m_bColorBufferOwner)
    {
        WM4_DELETE[] m_auiColorBuffer;
    }

    WM4_DELETE[] m_akColorBuffer;
    WM4_DELETE[] m_auiDepthBuffer;
    WM4_DELETE[] m_afDepthBuffer;
    WM4_DELETE[] m_auiStencilBuffer;
}
//----------------------------------------------------------------------------
void SoftFrameBuffer::Enable ()
{
    assert(m_pkRenderer && m_pkTarget);

    // Use the offscreen framebuffer as the target of rendering.
    SoftRenderer* pkRenderer = (SoftRenderer*)m_pkRenderer;
    pkRenderer->SetRenderTarget(this);
}
//----------------------------------------------------------------------------
void SoftFrameBuffer::Disable ()
{
    assert(m_pkRenderer && m_pkTarget);

    // This is a render-to-texture copy.
    CopyToTexture(false);

    // Use the main framebuffer as the target of rendering.
    SoftRenderer* pkRenderer = (SoftRenderer*)m_pkRenderer;
    pkRenderer->SetRenderTarget(0);
}
//----------------------------------------------------------------------------
void SoftFrameBuffer::CopyToTexture (bool bFillImage)
{
    // Copy the colorbuffer data to the texture memory.  This is a copy from
    // the texture's Image data into the Sampler's image storage, with
    // subsequent mipmap generation if needed.
    ResourceIdentifier* pkID = m_pkTarget->GetIdentifier(m_pkRenderer);
    TextureID* pkResource = (TextureID*)pkID;
    SoftSampler* pkSampler = (SoftSampler*)pkResource->ID;

    if (m_akColorBuffer)
    {
        pkSampler->Recreate(m_akColorBuffer);
    }
    else if (m_afDepthBuffer)
    {
        pkSampler->Recreate(m_afDepthBuffer);
    }

    // Copy the colorbuffer data to the texture Image object.  This involves
    // a change of left-handed coordinates to right-handed coordinates.
    if (bFillImage)
    {
        Image* pkImage = m_pkTarget->GetImage();
        assert(pkImage && pkImage->GetDimension() == 2);
        pkImage->AllocateData();
        int iBound0 = pkImage->GetBound(0);
        int iBound1 = pkImage->GetBound(1);
        unsigned char* pucTrg = pkImage->GetData();
        const ColorRGBA* pkSrcBase;
        const ColorRGBA* pkSrc;
        int iX, iY;

        if (m_eFormat == FT_FORMAT_RGB)
        {
            pkSrcBase = &m_akColorBuffer[iBound0*(iBound1-1)];
            for (iY = 0; iY < iBound1; iY++)
            {
                pkSrc = pkSrcBase;
                for (iX = 0; iX < iBound0; iX++)
                {
                    *pucTrg++ = (unsigned char)(255.0f*pkSrc->R());
                    *pucTrg++ = (unsigned char)(255.0f*pkSrc->G());
                    *pucTrg++ = (unsigned char)(255.0f*pkSrc->B());
                    pkSrc++;
                }
                pkSrcBase -= iBound0;
            }
        }
        else if (m_eFormat == FT_FORMAT_RGBA)
        {
            pkSrcBase = &m_akColorBuffer[iBound0*(iBound1-1)];
            for (iY = 0; iY < iBound1; iY++)
            {
                pkSrc = pkSrcBase;
                for (iX = 0; iX < iBound0; iX++)
                {
                    *pucTrg++ = (unsigned char)(255.0f*pkSrc->R());
                    *pucTrg++ = (unsigned char)(255.0f*pkSrc->G());
                    *pucTrg++ = (unsigned char)(255.0f*pkSrc->B());
                    *pucTrg++ = (unsigned char)(255.0f*pkSrc->A());
                    pkSrc++;
                }
                pkSrcBase -= iBound0;
            }
        }
        else // m_eFormat == FT_FORMAT_DEPTH
        {
            const float* pfSrc = &m_afDepthBuffer[iBound0*(iBound1-1)];
            float* pfTrg = (float*)pucTrg;
            size_t uiSize = iBound0*sizeof(float);
            for (iY = 0; iY < iBound1; iY++)
            {
                System::Memcpy(pfTrg,uiSize,pfSrc,uiSize);
                pfSrc -= iBound0;
                pfTrg += iBound0;
            }
        }
    }

    // TO DO.  How to handle stencil buffer copying?
}
//----------------------------------------------------------------------------
