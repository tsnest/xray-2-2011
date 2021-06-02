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

#ifndef SIMPLEPATCH_H
#define SIMPLEPATCH_H

#include "Wm4SurfacePatch.h"

namespace Wm4
{

class SimplePatch : public SurfacePatch
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    SimplePatch ();
    virtual ~SimplePatch ();

    float Amplitude;

    // position and derivatives up to second order
    virtual Vector3f P (float fU, float fV) const;
    virtual Vector3f PU (float fU, float fV) const;
    virtual Vector3f PV (float fU, float fV) const;
    virtual Vector3f PUU (float fU, float fV) const;
    virtual Vector3f PUV (float fU, float fV) const;
    virtual Vector3f PVV (float fU, float fV) const;
};

WM4_REGISTER_STREAM(SimplePatch);
typedef Pointer<SimplePatch> SimplePatchPtr;

}

#endif
