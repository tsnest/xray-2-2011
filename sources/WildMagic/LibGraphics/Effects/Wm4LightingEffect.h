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

#ifndef WM4LIGHTINGEFFECT_H
#define WM4LIGHTINGEFFECT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4ShaderEffect.h"
#include "Wm4MaterialState.h"
#include "Wm4Light.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM LightingEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    LightingEffect ();
    virtual ~LightingEffect ();

    // After attaching and/or detaching lights, call Configure() to activate
    // the correct shader program for the current set of lights.
    int GetLightQuantity () const;
    Light* GetLight (int i) const;
    void AttachLight (Light* pkLight);
    void DetachLight (Light* pkLight);
    void DetachAllLights ();

    // Call this function after any attaching and/or detaching lights.
    void Configure ();

    // Enable and disable lights for multipass drawing.  The first pass is
    // responsible for handling the emissive lighting.
    virtual void SetGlobalState (int iPass, Renderer* pkRenderer,
        bool bPrimaryEffect);
    virtual void RestoreGlobalState (int iPass, Renderer* pkRenderer,
        bool bPrimaryEffect);

protected:
    ColorRGB m_kSaveEmissive;
    std::vector<LightPtr> m_kLights;
};

WM4_REGISTER_STREAM(LightingEffect);
typedef Pointer<LightingEffect> LightingEffectPtr;
#include "Wm4LightingEffect.inl"

}

#endif
