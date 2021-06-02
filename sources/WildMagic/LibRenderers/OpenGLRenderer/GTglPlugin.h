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
// Version: 4.3.1 (2008/11/09)

#ifndef GTGLPLUGIN_H
#define GTGLPLUGIN_H

// The default behavior is to have no additional semantics per OpenGL call.
// Replace GTglPlugin.h and, if necessary, add a file GTglPlugin.c that
// implements additional semantics.  These can include report-log generation
// or profiling.

#define GT_ENTER_FUNCTION(glFunction)
#define GT_NONNULL_FUNCTION(glFunction)

//#define DEV_ENABLE_GL_ERROR_CHECKING
#ifdef DEV_ENABLE_GL_ERROR_CHECKING

// This function asserts when a null function is encountered.  The renderer
// is expecting the function to exist.
extern void ReportNullFunction(const char*);
#define GT_NULL_FUNCTION(glFunction) ReportNullFunction(#glFunction)

// This function checks glGetError after each OpenGL function call.
extern void ReportGLError(const char*);
#define GT_EXIT_FUNCTION(glFunction) ReportGLError(#glFunction)

#else

// Enable this block to disable error checking.
#define GT_NULL_FUNCTION(glFunction)
#define GT_EXIT_FUNCTION(glFunction)

#endif

#endif
