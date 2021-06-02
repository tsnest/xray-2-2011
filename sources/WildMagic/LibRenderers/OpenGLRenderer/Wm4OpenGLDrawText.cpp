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
// Version: 4.0.1 (2008/01/20)

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4OpenGLRenderer.h"
#include "Wm4OpenGLBitmapFont.h"
using namespace Wm4;

//#define USE_TEXT_DISPLAY_LIST

//----------------------------------------------------------------------------
bool OpenGLRenderer::SelectFont (int iFontID)
{
    if (0 <= iFontID && iFontID < (int)m_kDLInfo.size())
    {
        if (m_kDLInfo[iFontID].Quantity > 0)
        {
            m_iFontID = iFontID;
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void OpenGLRenderer::UnloadFont (int iFontID)
{
    // You may not unload the default font (id = 0).
    if (1 <= iFontID && iFontID < (int)m_kDLInfo.size())
    {
        // Unload the specified font.
        DisplayListInfo kInfo = m_kDLInfo[iFontID];
        glDeleteLists(kInfo.Start,kInfo.Quantity);
        m_kDLInfo.erase(m_kDLInfo.begin()+iFontID);
        
        // Update the currently selected font, if needed.
        if (m_iFontID == iFontID)
        {
            m_iFontID = 0;
        }
        else if (m_iFontID > iFontID)
        {
            m_iFontID--;
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::DrawCharacter (const BitmapFont& rkFont, char cChar)
{
    unsigned int uiIndex = (unsigned int)cChar;
    const BitmapFontChar* pkBFC = rkFont.Chars[uiIndex];

    // save unpack state
    GLint iSwapBytes, iLSBFirst, iRowLength, iSkipRows, iSkipPixels;
    GLint iAlignment;
    glGetIntegerv(GL_UNPACK_SWAP_BYTES,&iSwapBytes);
    glGetIntegerv(GL_UNPACK_LSB_FIRST,&iLSBFirst);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH,&iRowLength);
    glGetIntegerv(GL_UNPACK_SKIP_ROWS,&iSkipRows);
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS,&iSkipPixels);
    glGetIntegerv(GL_UNPACK_ALIGNMENT,&iAlignment);

    glPixelStorei(GL_UNPACK_SWAP_BYTES,GL_FALSE);
    glPixelStorei(GL_UNPACK_LSB_FIRST,GL_FALSE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBitmap(pkBFC->XSize,pkBFC->YSize,(float)pkBFC->XOrigin,
        (float)pkBFC->YOrigin,(float)pkBFC->XSize,0.0f,
        (const GLubyte*)pkBFC->Bitmap);

    // restore unpack state
    glPixelStorei(GL_UNPACK_SWAP_BYTES,iSwapBytes);
    glPixelStorei(GL_UNPACK_LSB_FIRST,iLSBFirst);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,iRowLength);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,iSkipRows);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS,iSkipPixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT,iAlignment);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::Draw (int iX, int iY, const ColorRGBA& rkColor,
    const char* acText)
{
    assert(acText);

#ifdef USE_TEXT_DISPLAY_LIST
    GLint iListBase;
    glGetIntegerv(GL_LIST_BASE,&iListBase);
#endif

    // switch to orthogonal view
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-0.5f,m_iWidth-0.5f,-0.5f,m_iHeight-0.5f,-1.0f,1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // disable depth, lighting, and texturing
    GLboolean bDepthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean bLighting = glIsEnabled(GL_LIGHTING);
    GLboolean bTexture2D = glIsEnabled(GL_TEXTURE_2D);
    if (bDepthTest)
    {
        glDisable(GL_DEPTH_TEST);
    }
    if (bLighting)
    {
        glDisable(GL_LIGHTING);
    }
    if (bTexture2D)
    {
        glDisable(GL_TEXTURE_2D);
    }

    // set the text color
    glColor4fv((const float*)rkColor);

    // draw text string (use right-handed coordinates)
    glRasterPos3i(iX,m_iHeight-1-iY,0);
#ifdef USE_TEXT_DISPLAY_LIST
    glListBase(m_kDLInfo[m_iFontID].Base);
    glCallLists((GLsizei)strlen(acText),GL_UNSIGNED_BYTE,acText);
#else
    int iLength = (int)strlen(acText);
    for (int i = 0; i < iLength; i++)
    {
        DrawCharacter(g_kVerdana_S16B0I0,acText[i]);
    }
#endif

    // restore depth, lighting, and texturing
    if (bDepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    if (bLighting)
    {
        glEnable(GL_LIGHTING);
    }
    if (bTexture2D)
    {
        glEnable(GL_TEXTURE_2D);
    }

    // restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // restore projective view
    OnFrustumChange();

#ifdef USE_TEXT_DISPLAY_LIST
    glListBase(iListBase);
#endif
}
//----------------------------------------------------------------------------
