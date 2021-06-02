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
// Version: 4.3.2 (2007/07/15)

#include "GTglExtensions.h"
#include <cassert>
#include <cstdio>

// Disable the MSVC8 warnings about deprecated functions.
#pragma warning( push )
#pragma warning( disable : 4996 )

//----------------------------------------------------------------------------
void ReportNullFunction (const char*)
{
    assert(false);
}
//----------------------------------------------------------------------------
void ReportGLError (const char* acFunction)
{
    GLenum code = glGetError();
    if (code != GL_NO_ERROR)
    {
        FILE* pkOFile = fopen("OpenGLErrors.txt","at");
        switch (code)
        {
        case GL_INVALID_ENUM:
            fprintf(pkOFile,"invalid enum, function %s\n",acFunction);
            break;
        case GL_INVALID_VALUE:
            fprintf(pkOFile,"invalid value, function %s\n",acFunction);
            break;
        case GL_INVALID_OPERATION:
            fprintf(pkOFile,"invalid operation, function %s\n",acFunction);
            break;
        case GL_STACK_OVERFLOW:
            fprintf(pkOFile,"stack overflow, function %s\n",acFunction);
            break;
        case GL_STACK_UNDERFLOW:
            fprintf(pkOFile,"stack underflow, function %s\n",acFunction);
            break;
        case GL_OUT_OF_MEMORY:
            fprintf(pkOFile,"out of memory, function %s\n",acFunction);
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
            fprintf(pkOFile,"invalid framebuffer operation, function %s\n",
                acFunction);
            break;
        default:
            fprintf(pkOFile,"unknown error, function %s\n",acFunction);
            break;
        }
        fclose(pkOFile);
    }
}
//----------------------------------------------------------------------------

#pragma warning( pop )
