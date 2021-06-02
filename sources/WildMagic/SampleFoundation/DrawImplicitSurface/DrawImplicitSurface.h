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

#ifndef DRAWIMPLICITSURFACE_H
#define DRAWIMPLICITSURFACE_H

#include "Wm4WindowApplication2.h"
#include "RayTracer.h"
using namespace Wm4;

class DrawImplicitSurface : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    DrawImplicitSurface ();

    virtual bool OnInitialize ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnSpecialKeyDown (int iKey, int iX, int iY);

protected:
    RayTracer m_kRT;
    int m_iMaxSample;
    bool m_bBlur;
};

WM4_REGISTER_INITIALIZE(DrawImplicitSurface);

#endif
