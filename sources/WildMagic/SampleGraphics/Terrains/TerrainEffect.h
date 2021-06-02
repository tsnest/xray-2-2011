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
// Version: 4.0.2 (2008/02/18)

#ifndef TERRAINEFFECT_H
#define TERRAINEFFECT_H

#include "Wm4ShaderEffect.h"

namespace Wm4
{

class TerrainEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM_WITH_POSTLINK;

public:
    // The fog factor is computed to be f = exp(-(d*z)^2), where d is the
    // fog density and z is the z-component of the view-space vertex position.
    TerrainEffect (const char* acBaseName, const char* acDetailName,
        const ColorRGB& rkFogColor, float fFogDensity);
    virtual ~TerrainEffect ();

    void SetFogColor (const ColorRGB& rkFogColor);
    ColorRGB GetFogColor () const;

    void SetFogDensity (float fFogDensity);
    float GetFogDensity () const;

protected:
    // streaming
    TerrainEffect ();

    // The fog color is stored in the first three components (R = 0, G = 1,
    // and B = 2).  The fog density is stored in the last component.
    float m_afFogColorDensity[4];
};

WM4_REGISTER_STREAM(TerrainEffect);
typedef Pointer<TerrainEffect> TerrainEffectPtr;

#include "TerrainEffect.inl"

}

#endif
