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

#include "Wm4GraphicsPCH.h"
#include "Wm4FrameBuffer.h"
#include "Wm4Renderer.h"
#include "Wm4Texture.h"
#include "Wm4Image.h"
using namespace Wm4;

FrameBuffer::Creator FrameBuffer::ms_aoCreate[Renderer::MAX_RENDERER_TYPES];
FrameBuffer::Destroyer FrameBuffer::ms_aoDestroy[
    Renderer::MAX_RENDERER_TYPES];

//----------------------------------------------------------------------------
FrameBuffer* FrameBuffer::Create (FormatType eFormat, DepthType eDepth,
    StencilType eStencil, BufferingType eBuffering,
    MultisamplingType eMultisampling, Renderer* pkRenderer, Texture* pkTarget)
{
    Creator oCreator = ms_aoCreate[pkRenderer->GetType()];
    assert(oCreator);
    if (oCreator)
    {
        return oCreator(eFormat,eDepth,eStencil,eBuffering,eMultisampling,
            pkRenderer,pkTarget);
    }
    return 0;
}
//----------------------------------------------------------------------------
void FrameBuffer::Destroy (FrameBuffer* pkBuffer)
{
    Renderer* pkRenderer = pkBuffer->GetRenderer();
    Destroyer oDestroyer = ms_aoDestroy[pkRenderer->GetType()];
    assert(oDestroyer);
    if (oDestroyer)
    {
        oDestroyer(pkBuffer);
    }
}
//----------------------------------------------------------------------------
FrameBuffer::FrameBuffer (FormatType eFormat, DepthType eDepth,
    StencilType eStencil, BufferingType eBuffering,
    MultisamplingType eMultisampling, Renderer* pkRenderer, Texture* pkTarget)
{
    m_eFormat = eFormat;
    m_eDepth = eDepth;
    m_eStencil = eStencil;
    m_eBuffering = eBuffering;
    m_eMultisampling = eMultisampling;
    m_pkRenderer = pkRenderer;
    m_pkTarget = pkTarget;
}
//----------------------------------------------------------------------------
FrameBuffer::~FrameBuffer ()
{
}
//----------------------------------------------------------------------------
