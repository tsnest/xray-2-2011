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
#include "Wm4XSoftRenderer.h"
using namespace Wm4;

//----------------------------------------------------------------------------
XSoftRenderer::XSoftRenderer (Display* pkDisplay, unsigned long ulWindow,
    FrameBuffer::FormatType eFormat, FrameBuffer::DepthType eDepth,
    FrameBuffer::StencilType eStencil, FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iWidth, int iHeight)
    :
    SoftRenderer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,iWidth,
        iHeight,true)
{
    m_pkDisplay = pkDisplay;
    m_ulWindow = ulWindow;

    unsigned long ulValueMask = 0;
    XGCValues kValues;
    m_pkContext = XCreateGC(m_pkDisplay,m_ulWindow,ulValueMask,&kValues);
    assert(m_pkContext);

    int iScreen = DefaultScreen(m_pkDisplay);
    unsigned int uiDepth = (unsigned int)DefaultDepth(m_pkDisplay,iScreen);
    m_kPixmap = XCreatePixmap(m_pkDisplay,m_ulWindow,(unsigned int)iWidth,
        (unsigned int)iHeight,uiDepth);

    // Set the active framebuffer to be the main framebuffer.
    SetRenderTarget(0);
}
//----------------------------------------------------------------------------
XSoftRenderer::~XSoftRenderer ()
{
    XFreePixmap(m_pkDisplay,m_kPixmap);
    XFreeGC(m_pkDisplay,m_pkContext);
}
//----------------------------------------------------------------------------
void XSoftRenderer::ClearBackBuffer ()
{
    if (m_auiColorBuffer)
    {
        unsigned char ucR = (unsigned char)(255.0f*m_kClearColor.R());
        unsigned char ucG = (unsigned char)(255.0f*m_kClearColor.G());
        unsigned char ucB = (unsigned char)(255.0f*m_kClearColor.B());
        unsigned char ucA = (unsigned char)(255.0f*m_kClearColor.A());
        unsigned int uiClearColor =
            (ucB | (ucG << 8) | (ucR << 16) | (ucA << 24));

        XSetBackground(m_pkDisplay,m_pkContext,(unsigned long)uiClearColor);
        XSetForeground(m_pkDisplay,m_pkContext,(unsigned long)uiClearColor);

        unsigned int* puiColorBuffer = m_auiColorBuffer;
        ColorRGBA* pkColorBuffer = m_akColorBuffer;
        for (int iY = 0; iY < m_iHeight; iY++)
        {
            for (int iX = 0; iX < m_iWidth; iX++)
            {
                *puiColorBuffer++ = uiClearColor;
                *pkColorBuffer++ = m_kClearColor;
                XDrawPoint(m_pkDisplay,m_kPixmap,m_pkContext,iX,iY);
            }
        }
    }
}
//----------------------------------------------------------------------------
void XSoftRenderer::ClearSubBackBuffer (int iXMin, int iYMin, int iXMax,
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

        XSetBackground(m_pkDisplay,m_pkContext,(unsigned long)uiClearColor);
        XSetForeground(m_pkDisplay,m_pkContext,(unsigned long)uiClearColor);

        int iBase = iXMin + m_iWidth*iYMin;
        for (int iY = iYMin; iY < iYMax; iY++)
        {
            unsigned int* auiColor = &m_auiColorBuffer[iBase];
            ColorRGBA* akColor = &m_akColorBuffer[iBase];
            for (int iX = iXMin, i = 0; iX < iXMax; iX++, i++)
            {
                auiColor[i] = uiClearColor;
                akColor[i] = m_kClearColor;
                XDrawPoint(m_pkDisplay,m_kPixmap,m_pkContext,iX,iY);
            }
            iBase += m_iWidth;
        }
    }
}
//----------------------------------------------------------------------------
void XSoftRenderer::DisplayBackBuffer ()
{
    XCopyArea(m_pkDisplay,m_kPixmap,m_ulWindow,m_pkContext,0,0,m_iWidth,
        m_iHeight,0,0);
    XFlush(m_pkDisplay);
}
//----------------------------------------------------------------------------
void XSoftRenderer::SetColorMask (bool bAllowRed, bool bAllowGreen,
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
void XSoftRenderer::SetColorBuffer (int iX, int iY, int iIndex,
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

    XSetForeground(m_pkDisplay,m_pkContext,
        (unsigned long)m_auiColorBuffer[iIndex]);
    XDrawPoint(m_pkDisplay,m_kPixmap,m_pkContext,iX,iY);
}
//----------------------------------------------------------------------------
int XSoftRenderer::LoadFont (const char* acFace, int iSize, bool bBold,
    bool bItalic)
{
    // TO DO.
    return -1;
}
//----------------------------------------------------------------------------
void XSoftRenderer::UnloadFont (int iFontID)
{
    // TO DO.
}
//----------------------------------------------------------------------------
bool XSoftRenderer::SelectFont (int iFontID)
{
    // TO DO.
    return false;
}
//----------------------------------------------------------------------------
void XSoftRenderer::Draw (int iX, int iY, const ColorRGBA& rkColor,
    const char* acText)
{
    unsigned char ucR = (unsigned char)(255.0f*rkColor.R());
    unsigned char ucG = (unsigned char)(255.0f*rkColor.G());
    unsigned char ucB = (unsigned char)(255.0f*rkColor.B());
    unsigned long ulColor = ucR | (ucG << 8) | (ucB << 16) | (255 << 24);
    XSetForeground(m_pkDisplay,m_pkContext,ulColor);

    ucR = (unsigned char)(255.0f*m_kClearColor.R());
    ucG = (unsigned char)(255.0f*m_kClearColor.G());
    ucB = (unsigned char)(255.0f*m_kClearColor.B());
    ulColor = ucR | (ucG << 8) | (ucB << 16);  // alpha = 0
    XSetBackground(m_pkDisplay,m_pkContext,ulColor);

    XDrawString(m_pkDisplay,m_kPixmap,m_pkContext,iX,iY,acText,
        (int)strlen(acText));
}
//----------------------------------------------------------------------------
void XSoftRenderer::Draw (const unsigned char* aucBuffer)
{
    const int iQuantity = m_iWidth*m_iHeight;
    const unsigned char* pucBuffer = aucBuffer;
    for (int iY = 0, iIndex = 0; iY < m_iHeight; iY++)
    {
        for (int iX = 0; iX < m_iWidth; iX++)
        {
            unsigned char ucB = *pucBuffer++;
            unsigned char ucG = *pucBuffer++;
            unsigned char ucR = *pucBuffer++;
            m_auiColorBuffer[iIndex] =
                (ucB | (ucG << 8) | (ucR << 16) | 0xFF000000);
            XSetForeground(m_pkDisplay,m_pkContext,
                (unsigned long)m_auiColorBuffer[iIndex]);
            XDrawPoint(m_pkDisplay,m_kPixmap,m_pkContext,iX,iY);
        }
    }
}
//----------------------------------------------------------------------------
