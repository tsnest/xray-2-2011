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

#ifndef WM4SOFTSAMPLER1_H
#define WM4SOFTSAMPLER1_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4SoftSampler.h"

namespace Wm4
{

class WM4_RENDERER_ITEM SoftSampler1 : public SoftSampler
{
    WM4_DECLARE_INITIALIZE;

public:
    SoftSampler1 (const Texture* pkTexture);
    virtual ~SoftSampler1 ();

    virtual void Recreate (const ColorRGBA* akCImage);

    virtual void ComputeMipmapParameters (const int aiX[3], const int aiY[3],
        const float* aafVertex[3], const Attributes& rkAttr, int iUnit);
    virtual void DisallowMipmapping ();
    virtual void CurrentPixel (int iX, int iY);

    virtual ColorRGBA operator() (const float* afCoord);

private:
    void CreateMipmaps ();
    void RecreateMipmaps ();

    ColorRGBA GetNearestColor (float fXIm) const;
    ColorRGBA GetLinearColor (float fXIm) const;

    int* m_aiXBound;
    int m_iXBound0, m_iXBound1;
    int m_iCurrXBound;

    // Mipmapping parameters.  The texture coordinate and derivatives at
    // the pixel (x,y) are
    //   u = (a*x + b*y + c)/(d*x + e*y + f)
    //   du/dx = (+(a*e-b*d)*y + (a*f-c*d))/(d*x + e*y + f)^2
    //   du/dy = (-(a*e-b*d)*x + (b*f-c*e))/(d*x + e*y + f)^2
    float m_fA, m_fB, m_fC;
    float m_fAEmBD, m_fAFmCD, m_fBFmCE;

    // Factory creation of samplers.
    static SoftSampler* Create1 (const Texture* pkTexture);
};

WM4_REGISTER_INITIALIZE(SoftSampler1);

}

#endif
