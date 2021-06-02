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

#ifndef SHADOWMAPEFFECT_H
#define SHADOWMAPEFFECT_H

#include "Wm4ShaderEffect.h"
#include "Wm4Camera.h"
#include "Wm4Texture.h"

namespace Wm4
{

class ShadowMapEffect : public Effect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    ShadowMapEffect (Camera* pkProjector,
        const std::string& rkProjectionImage, int iDepthWidth,
        int iDepthHeight, float fDepthBias);

    virtual ~ShadowMapEffect ();

    virtual void Draw (Renderer* pkRenderer, Spatial* pkGlobalObject,
        int iMin, int iMax, VisibleObject* akVisible);

    void SetDepthBias (float fDepthBias);
    float GetDepthBias () const;

protected:
    ShadowMapEffect ();

    CameraPtr m_spkProjector;
    ShaderEffectPtr m_spkDepthEffect;
    FrameBuffer* m_pkDepthBuffer;
    ImagePtr m_spkDepthImage;
    ShaderEffectPtr m_spkShadowEffect;
    Texture* m_pkDepthTexture;  // For testing (copy depth to system memory).

    // The depth bias is stored at index 0.  The other array values are
    // unused.
    float m_afDepthBias[4];
};

WM4_REGISTER_STREAM(ShadowMapEffect);
typedef Pointer<ShadowMapEffect> ShadowMapEffectPtr;
#include "ShadowMapEffect.inl"

}

#endif
