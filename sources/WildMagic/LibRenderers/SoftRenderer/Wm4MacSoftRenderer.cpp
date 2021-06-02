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
// Version: 4.0.1 (2007/07/22)

#include "Wm4SoftRendererPCH.h"
#include "Wm4MacSoftRenderer.h"
using namespace Wm4;

//----------------------------------------------------------------------------
MacSoftRenderer::MacSoftRenderer (WindowRef pkWindow,
    FrameBuffer::FormatType eFormat, FrameBuffer::DepthType eDepth,
    FrameBuffer::StencilType eStencil, FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iWidth, int iHeight)
    :
    SoftRenderer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,iWidth,
        iHeight,false)
{
    m_pkWindow = pkWindow;
    m_auiData = WM4_NEW unsigned int [iWidth * iHeight];
    m_pkContext = CGBitmapContextCreate(m_auiData,iWidth,iHeight,8,
        iWidth * sizeof(unsigned int),CGColorSpaceCreateDeviceRGB(),
        kCGImageAlphaPremultipliedFirst);
    CGContextSelectFont(m_pkContext,"Courier",12,kCGEncodingMacRoman);

    // Set the main colorbuffer to be our one.
    m_kMain.SetColorBufferUI(m_auiData);

    // Set the active framebuffer to be the main framebuffer.
    SetRenderTarget(0);
}
//----------------------------------------------------------------------------
MacSoftRenderer::~MacSoftRenderer ()
{
    if (m_pkContext)
    {
        CGContextRelease(m_pkContext);
    }
    WM4_DELETE[] m_auiData;
}
//----------------------------------------------------------------------------
void MacSoftRenderer::ClearBackBuffer ()
{
    if (m_auiColorBuffer)
    {
        unsigned char ucR = (unsigned char)(255.0f*m_kClearColor.R());
        unsigned char ucG = (unsigned char)(255.0f*m_kClearColor.G());
        unsigned char ucB = (unsigned char)(255.0f*m_kClearColor.B());
        unsigned char ucA = (unsigned char)(255.0f*m_kClearColor.A());
        unsigned int uiClearColor =
            (ucB | (ucG << 8) | (ucR << 16) | (ucA << 24));

        unsigned int* puiColorBuffer = m_auiColorBuffer;
        ColorRGBA* pkColorBuffer = m_akColorBuffer;
        for (int i = 0; i < m_iQuantity; i++)
        {
            *puiColorBuffer++ = uiClearColor;
            *pkColorBuffer++ = m_kClearColor;
        }
    }
}
//----------------------------------------------------------------------------
void MacSoftRenderer::ClearSubBackBuffer (int iXMin, int iYMin, int iXMax,
    int iYMax)
{
    if (m_auiColorBuffer)
    {
        unsigned char ucR = (unsigned char)(255.0f*m_kClearColor.R());
        unsigned char ucG = (unsigned char)(255.0f*m_kClearColor.G());
        unsigned char ucB = (unsigned char)(255.0f*m_kClearColor.B());
        unsigned char ucA = (unsigned char)(255.0f*m_kClearColor.A());
        unsigned int uiClearColor =
            (ucB | (ucG << 8) | (ucR << 16) | (ucA << 24));

        int iBase = iXMin + m_iWidth*iYMin;
        for (int iY = iYMin; iY < iYMax; iY++)
        {
            unsigned int* auiColor = &m_auiColorBuffer[iBase];
            ColorRGBA* akColor = &m_akColorBuffer[iBase];
            for (int iX = iXMin, i = 0; iX < iXMax; iX++, i++)
            {
                auiColor[i] = uiClearColor;
                akColor[i] = m_kClearColor;
            }
            iBase += m_iWidth;
        }
    }
}
//----------------------------------------------------------------------------
void MacSoftRenderer::SetColorMask (bool bAllowRed, bool bAllowGreen,
    bool bAllowBlue, bool bAllowAlpha)
{
    Renderer::SetColorMask(bAllowRed,bAllowGreen,bAllowBlue,bAllowAlpha);

    m_uiColorMask = 0;
    if (m_bAllowRed)
    {
        m_uiColorMask |= 0x00FF0000;
    }
    if (m_bAllowGreen)
    {
        m_uiColorMask |= 0x0000FF00;
    }
    if (m_bAllowBlue)
    {
        m_uiColorMask |= 0x000000FF;
    }
    if (m_bAllowAlpha)
    {
        m_uiColorMask |= 0xFF000000;
    }
}
//----------------------------------------------------------------------------
void MacSoftRenderer::SetColorBuffer (int, int, int iIndex,
    const ColorRGBA& rkColor)
{
    unsigned char ucR = (unsigned char)(255.0f*rkColor.R());
    unsigned char ucG = (unsigned char)(255.0f*rkColor.G());
    unsigned char ucB = (unsigned char)(255.0f*rkColor.B());
    unsigned char ucA = (unsigned char)(255.0f*rkColor.A());
    unsigned int uiColor = (ucB | (ucG << 8) | (ucR << 16) | (ucA << 24));

    if (m_uiColorMask == (unsigned int)~0)
    {
        m_auiColorBuffer[iIndex] = uiColor;
        m_akColorBuffer[iIndex] = rkColor;
    }
    else
    {
        m_auiColorBuffer[iIndex] =
            (m_auiColorBuffer[iIndex] & ~m_uiColorMask) |
            (uiColor & m_uiColorMask);

        ColorRGBA& rkCurrentColor = m_akColorBuffer[iIndex];
        if (m_bAllowRed)
        {
            rkCurrentColor.R() = rkColor.R();
        }
        if (m_bAllowGreen)
        {
            rkCurrentColor.G() = rkColor.G();
        }
        if (m_bAllowBlue)
        {
            rkCurrentColor.B() = rkColor.B();
        }
        if (m_bAllowAlpha)
        {
            rkCurrentColor.A() = rkColor.A();
        }
    }
}
//----------------------------------------------------------------------------
void MacSoftRenderer::DisplayBackBuffer ()
{
    CGImageRef pkImage = CGBitmapContextCreateImage(m_pkContext);
    if (!pkImage)
    {
        return;
    }
    CGContextRef pkContext = 0;
    QDBeginCGContext(GetWindowPort(m_pkWindow),&pkContext);
    if (!pkContext)
    {
        CGImageRelease(pkImage);
        return;
    }

    CGContextDrawImage(pkContext,CGRectMake(0,0,GetWidth(),GetHeight()),
        pkImage);
    CGContextFlush(pkContext);
    QDEndCGContext(GetWindowPort(m_pkWindow),&pkContext);
    CGImageRelease(pkImage);
}
//----------------------------------------------------------------------------
int MacSoftRenderer::LoadFont (const char* acFace, int iSize, bool bBold,
    bool bItalic)
{
    // TO DO.
    return -1;
}
//----------------------------------------------------------------------------
void MacSoftRenderer::UnloadFont (int iFontID)
{
    // TO DO.
}
//----------------------------------------------------------------------------
bool MacSoftRenderer::SelectFont (int iFontID)
{
    // TO DO.
    return false;
}
//----------------------------------------------------------------------------
void MacSoftRenderer::Draw (int iX, int iY, const ColorRGBA& rkColor,
    const char* acText)
{
    CGContextSaveGState(m_pkContext);
    CGContextSetRGBFillColor(m_pkContext,rkColor.R(),rkColor.G(),rkColor.B(),
        rkColor.A());
    CGContextShowTextAtPoint(m_pkContext,iX,GetHeight() - iY,
        acText,strlen(acText));
    CGContextRestoreGState(m_pkContext);
}
//----------------------------------------------------------------------------
void MacSoftRenderer::Draw (const unsigned char* aucBuffer)
{
    const int iQuantity = m_iWidth*m_iHeight;
    const unsigned char* pucBuffer = aucBuffer;
    for (int i = 0; i < iQuantity; i++)
    {
        unsigned char ucB = *pucBuffer++;
        unsigned char ucG = *pucBuffer++;
        unsigned char ucR = *pucBuffer++;
        m_auiColorBuffer[i] = (ucB | (ucG << 8) | (ucR << 16) | 0xFF000000);
    }
}
//----------------------------------------------------------------------------

