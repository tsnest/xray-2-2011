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
// Version: 4.0.2 (2007/08/05)

#include "Wm4ApplicationPCH.h"
#include "Wm4WindowApplication.h"
using namespace Wm4;

//----------------------------------------------------------------------------
WindowApplication::WindowApplication (const char* acWindowTitle,
    int iXPosition, int iYPosition, int iWidth, int iHeight,
    const ColorRGBA& rkBackgroundColor)
    :
    m_acWindowTitle(acWindowTitle),
    m_iXPosition(iXPosition),
    m_iYPosition(iYPosition),
    m_iWidth(iWidth),
    m_iHeight(iHeight),
    m_kBackgroundColor(rkBackgroundColor)
{
    m_iWindowID = 0;
    m_pkRenderer = 0;

    // Default buffer parameters.  These may be overridden by the derived
    // class constructor.  Current hardware seems to pack a 24-bit depth
    // buffer and 8-bit stencil buffer into a 32-bit buffer, so I suggest
    // not overriding these.  Perhaps the only one you should is the
    // multisampling value.
    m_eFormat = FrameBuffer::FT_FORMAT_RGBA;
    m_eDepth = FrameBuffer::DT_DEPTH_24;
    m_eStencil = FrameBuffer::ST_STENCIL_8;
    m_eBuffering = FrameBuffer::BT_BUFFERED_DOUBLE;
    m_eMultisampling = FrameBuffer::MT_SAMPLING_NONE;
}
//----------------------------------------------------------------------------
WindowApplication::~WindowApplication ()
{
}
//----------------------------------------------------------------------------
int WindowApplication::Run (int iQuantity, char** apcArgument)
{
    WindowApplication* pkTheApp = (WindowApplication*)TheApplication;
    return pkTheApp->Main(iQuantity,apcArgument);
}
//----------------------------------------------------------------------------
bool WindowApplication::OnInitialize ()
{
    m_pkRenderer->SetClearColor(m_kBackgroundColor);

    // Now that the renderer has been created, provide the program loaders
    // with the information needed to load the correct programs.
    Program::RendererType = m_pkRenderer->GetExtension();
    Program::CommentChar = m_pkRenderer->GetCommentCharacter();

    return true;
}
//----------------------------------------------------------------------------
void WindowApplication::OnTerminate ()
{
    // Notify the program loaders that the renderer is going away.
    Program::RendererType = "";
    Program::CommentChar = 0;

    WM4_DELETE m_pkRenderer;
    m_pkRenderer = 0;
}
//----------------------------------------------------------------------------
void WindowApplication::OnMove (int iX, int iY)
{
    m_iXPosition = iX;
    m_iYPosition = iY;
}
//----------------------------------------------------------------------------
void WindowApplication::OnResize (int iWidth, int iHeight)
{
    if (iWidth > 0 && iHeight > 0)
    {
        if (m_pkRenderer)
        {
            m_pkRenderer->Resize(iWidth,iHeight);
        }

        m_iWidth = iWidth;
        m_iHeight = iHeight;
    }
}
//----------------------------------------------------------------------------
