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
// Version: 4.0.1 (2007/07/25)

#ifndef WM4SOFTSAMPLERCUBE_H
#define WM4SOFTSAMPLERCUBE_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4SoftSampler2.h"

namespace Wm4
{

class WM4_RENDERER_ITEM SoftSamplerCube : public SoftSampler
{
    WM4_DECLARE_INITIALIZE;

public:
    SoftSamplerCube (const Texture* pkTexture);
    virtual ~SoftSamplerCube ();

    virtual void Recreate (const ColorRGBA* akCImage);

    virtual void ComputeMipmapParameters (const int aiX[3], const int aiY[3],
        const float* aafVertex[3], const Attributes& rkAttr, int iUnit);
    virtual void DisallowMipmapping ();
    virtual void CurrentPixel (int iX, int iY);

    virtual ColorRGBA operator() (const float* afCoord);

private:
    // +x, -x, +y, -y, +z, -z
    Texture* m_apkTexture[6];
    SoftSampler2* m_apkSampler[6];

    // Factory creation of samplers.
    static SoftSampler* CreateCube (const Texture* pkTexture);
};

WM4_REGISTER_INITIALIZE(SoftSamplerCube);

}

#endif
