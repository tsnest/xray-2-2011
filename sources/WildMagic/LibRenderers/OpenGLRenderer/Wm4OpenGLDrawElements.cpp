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
// Version: 4.0.5 (2008/07/01)

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4OpenGLRenderer.h"
using namespace Wm4;

GLenum OpenGLRenderer::ms_aeObjectType[Geometry::GT_MAX_QUANTITY] =
{
    GL_POINTS,          // GT_POLYPOINT
    GL_LINES,           // GT_POLYLINE_SEGMENTS
    GL_LINE_STRIP,      // GT_POLYLINE_OPEN
    GL_LINE_STRIP,      // GT_POLYLINE_CLOSED (Wm4::Polyline has extra index)
    GL_TRIANGLES,       // GT_TRIMESH
    GL_TRIANGLE_STRIP,  // GT_TRISTRIP
    GL_TRIANGLE_FAN     // GT_TRIFAN
};

//----------------------------------------------------------------------------
//#define COUNT_PIXELS
#ifdef COUNT_PIXELS
//----------------------------------------------------------------------------
static GLuint BeginQuery ()
{
    GLuint uiQueryHandle = 0;
    glGenQueriesARB(1,&uiQueryHandle);
    glBeginQueryARB(GL_SAMPLES_PASSED_ARB,uiQueryHandle);
    return uiQueryHandle;
}
//----------------------------------------------------------------------------
static GLuint EndQuery (GLuint uiQueryHandle)
{
    glEndQueryARB(GL_SAMPLES_PASSED_ARB);
    GLint iAvailable = 0;
    while(!iAvailable)
    {
        glGetQueryObjectivARB(uiQueryHandle,GL_QUERY_RESULT_AVAILABLE_ARB,
            &iAvailable);
    }

    GLuint uiPixelsDrawn = 0;
    glGetQueryObjectuivARB(uiQueryHandle,GL_QUERY_RESULT_ARB,&uiPixelsDrawn);
    glDeleteQueriesARB(1,&uiPixelsDrawn);
    return uiPixelsDrawn;
}
//----------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------
void OpenGLRenderer::DrawElements ()
{
    VertexBuffer* pkVBuffer = m_pkGeometry->VBuffer;
    IndexBuffer* pkIBuffer = m_pkGeometry->IBuffer;
    assert(pkIBuffer);
    GLenum eType = ms_aeObjectType[m_pkGeometry->Type];
    int iOffset = pkIBuffer->GetOffset();

#ifdef COUNT_PIXELS
    GLuint uiQueryHandle = BeginQuery();
#endif

    glDrawRangeElements(eType,0,pkVBuffer->GetVertexQuantity()-1,
        pkIBuffer->GetIndexQuantity(),GL_UNSIGNED_INT,
        (const GLvoid*)((int*)0 + iOffset));

#ifdef COUNT_PIXELS
    GLuint uiPixelsDrawn = EndQuery(uiQueryHandle);

    // Avoid the compiler warning about unreferenced variable.  The goal is
    // to allow you to set a breakpoint and see how many pixels were drawn.
    (void)uiPixelsDrawn;
#endif
}
//----------------------------------------------------------------------------
