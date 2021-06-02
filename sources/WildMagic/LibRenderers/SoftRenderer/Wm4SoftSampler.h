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

#ifndef WM4SOFTSAMPLER_H
#define WM4SOFTSAMPLER_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4BitHacks.h"
#include "Wm4Image.h"
#include "Wm4Texture.h"

namespace Wm4
{

class WM4_RENDERER_ITEM SoftSampler
{
public:
    // Factory creation based on dimension of the texture image.
    static SoftSampler* Create (const Texture* pkTexture);

    // Abstract base class.
    virtual ~SoftSampler ();

    // Support for offscreen rendering.  The color samplers will use the
    // function with ColorRGBA input.  The depth sampler will use the
    // input as an array of float.
    virtual void Recreate (const ColorRGBA* akCImage);
    virtual void Recreate (const float* afDImage);

    // Support for mipmapping.  The texture-coordinate derivative equations
    // need access to the current pixel location.
    bool MipmappingEnabled () const;
    virtual void ComputeMipmapParameters (const int aiX[3], const int aiY[3],
        const float* aafVertex[3], const Attributes& rkAttr, int iUnit);
    virtual void DisallowMipmapping () = 0;
    virtual void CurrentPixel (int iX, int iY) = 0;

    // The input array is assumed to be of the dimension of the actual
    // sampler.
    virtual ColorRGBA operator() (const float* afCoord) = 0;

protected:
    SoftSampler (const Texture* pkTexture);

    // Transform the input texture coordinate according to the current wrap
    // mode for the texture.
    float GetImageCoordinate (int i, float fCoord, int iBound) const;

    // Clamp the mipmap levels to [0,miplevels-1].
    void ClampLevels ();

    // Shared code for mipmaps.
    void CreateMipmap1 (int iLevel, int* aiXBound, int iXBD2);
    void RecreateMipmap1 (int iLevel, int iXBD2);

    void CreateMipmap2 (int iLevel, int* aiXBound, int* aiYBound,
        int* aiLog2XBound, int iXB, int iXBD2, int iYBD2, int iLXB,
        int iLXBM1);
    void RecreateMipmap2 (int iLevel, int iXB, int iXBD2, int iYBD2,
        int iLXB, int iLXBM1);

    void CreateMipmap3 (int iLevel, int* aiXBound, int* aiYBound,
        int* aiZBound, int* aiLog2XBound, int* aiLog2YBound, int iXB,
        int iXBYB, int iXBD2, int iYBD2, int iZBD2, int iLXB, int iLYB,
        int iLXBM1, int iLYBM1);
    void RecreateMipmap3 (int iLevel, int iXB, int iXBYB, int iXBD2,
        int iYBD2, int iZBD2, int iLXB, int iLYB, int iLXBM1, int iLYBM1);

    const Texture* m_pkTexture;

    // When the sampler is created, the texture image is mapped to an image
    // of ColorRGBA.  This happens once at construction, so once the samplers
    // are bound (i.e. cached by the software renderer), no conversion costs
    // are incurred by the interpolator.  Mipmaps, if requested, are created
    // as ColorRGBA images.
    enum { MAX_LEVELS = 16 };
    int m_iLevels;
    ColorRGBA* m_aakImage[MAX_LEVELS];
    float m_fCurrLevel, m_fDL;
    int m_iLevel0, m_iLevel1;
    ColorRGBA* m_akImage0;
    ColorRGBA* m_akImage1;
    ColorRGBA* m_akCurrImage;

    // Mipmap parameters needed by all derived classes.
    float m_fAMul0, m_fAMul1, m_fAMul2;
    float m_fBMul0, m_fBMul1, m_fBMul2;
    float m_fCMul0, m_fCMul1, m_fCMul2;
    float m_fD, m_fE, m_fF;
    bool m_bAllowMipmapping;

    // Factory creation of samplers (to hide dimensional dependence of the
    // texture images).
    typedef SoftSampler* (*Creator)(const Texture*);
    static Creator ms_aoCreator[];
};

}

#endif
