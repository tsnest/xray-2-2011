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

#ifndef WM4SOFTSHADERLIBRARY_H
#define WM4SOFTSHADERLIBRARY_H

#include "Wm4SoftShaderLibraryMCR.h"

// vertex programs
WM4_REGISTER_VPROGRAM(Default);
WM4_REGISTER_VPROGRAM(L1a);
WM4_REGISTER_VPROGRAM(L1d);
WM4_REGISTER_VPROGRAM(L1p);
WM4_REGISTER_VPROGRAM(L1s);
WM4_REGISTER_VPROGRAM(Material);
WM4_REGISTER_VPROGRAM(MaterialTexture);
WM4_REGISTER_VPROGRAM(Texture);
WM4_REGISTER_VPROGRAM(T0d2T1d2PassThrough);
WM4_REGISTER_VPROGRAM(VertexColor3);
WM4_REGISTER_VPROGRAM(VertexColor4);

// pixel programs
WM4_REGISTER_PPROGRAM(Default);
WM4_REGISTER_PPROGRAM(MaterialTexture);
WM4_REGISTER_PPROGRAM(PassThrough3);
WM4_REGISTER_PPROGRAM(PassThrough4);
WM4_REGISTER_PPROGRAM(Texture);
WM4_REGISTER_PPROGRAM(T0s1d0T1s1d1);
WM4_REGISTER_PPROGRAM(T0s1d0T1s2d0);
WM4_REGISTER_PPROGRAM(T0s1d0T1s3d1);

#endif
