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

#ifndef BLENDEDTERRAINEFFECT_H
#define BLENDEDTERRAINEFFECT_H

#include "Wm4ShaderEffect.h"

namespace Wm4
{

class BlendedTerrainEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM_WITH_POSTLINK;

public:
    BlendedTerrainEffect (const char* acGrassName, const char* acStoneName,
        const char* acBlendName, const char* acCloudName);
    virtual ~BlendedTerrainEffect ();

    // for the vertex program
    void SetFlowDirection (const Vector2f& rkFlowDirection);
    Vector2f GetFlowDirection () const;

    // for the pixel program
    void SetPowerFactor (float fPowerFactor);
    float GetPowerFactor () const;

protected:
    // streaming
    BlendedTerrainEffect ();

    // The flow direction is stored in locations 0 and 1.  The others are
    // unused.
    float m_afFlowDirection[4];

    // The power factor is stored in location 0.  The others are unused.
    float m_afPowerFactor[4];
};

WM4_REGISTER_STREAM(BlendedTerrainEffect);
typedef Pointer<BlendedTerrainEffect> BlendedTerrainEffectPtr;

#include "BlendedTerrainEffect.inl"

}

#endif
