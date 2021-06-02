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
// Version: 4.0.0 (2006/09/21)

#ifndef WM4IMAGICSLIB_H
#define WM4IMAGICSLIB_H

// For the DLL library.
#ifdef WM4_IMAGICS_DLL_EXPORT
#define WM4_IMAGICS_ITEM __declspec(dllexport)

// For a client of the DLL library.
#else
#ifdef WM4_IMAGICS_DLL_IMPORT
#define WM4_IMAGICS_ITEM __declspec(dllimport)

// For the static library.
#else
#define WM4_IMAGICS_ITEM

#endif
#endif
#endif
