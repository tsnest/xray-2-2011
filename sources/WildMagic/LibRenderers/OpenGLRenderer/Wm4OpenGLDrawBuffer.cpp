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

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4OpenGLRenderer.h"
using namespace Wm4;

//----------------------------------------------------------------------------
void OpenGLRenderer::Draw (const unsigned char* aucBuffer)
{
    if (!aucBuffer)
    {
        return;
    }

    // disable other states that are not necessary for the buffer copy
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

    // Set raster position to window coord (0,H-1).  The hack here avoids
    // problems with invalid raster positions which would cause glDrawPixels
    // not to execute.  OpenGL uses right-handed screen coordinates, so using
    // (0,H-1) as the raster position followed by glPixelZoom(1,-1) tells
    // OpenGL to draw the screen in left-handed coordinates starting at the
    // top row of the screen and finishing at the bottom row.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0,(double)m_iWidth,0.0,(double)m_iHeight,0.0,1.0);
    glRasterPos3f(0.0,0.0,0.0);
    GLubyte aucBitmap[1] = {0};
    glBitmap(0,0,0.0f,0.0f,0.0f,(float)m_iHeight,aucBitmap);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPixelZoom(1.0f,-1.0f);
    glDrawPixels(m_iWidth,m_iHeight,GL_BGR,GL_UNSIGNED_BYTE,aucBuffer);
    glPixelZoom(1.0f,1.0f);

    // reenable states
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
}
//----------------------------------------------------------------------------
