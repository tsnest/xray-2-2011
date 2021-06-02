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

#ifndef WM4PLANARSHADOWEFFECT_H
#define WM4PLANARSHADOWEFFECT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Effect.h"
#include "Wm4Light.h"
#include "Wm4TriMesh.h"
#include "Wm4Matrix4.h"
#include "Wm4MaterialEffect.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM PlanarShadowEffect : public Effect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    PlanarShadowEffect (int iQuantity);
    virtual ~PlanarShadowEffect ();

    virtual void Draw (Renderer* pkRenderer, Spatial* pkGlobalObject,
        int iMin, int iMax, VisibleObject* akVisible);

    // member access
    int GetQuantity () const;
    void SetPlane (int i, TriMeshPtr spkPlane);
    TriMeshPtr GetPlane (int i) const;
    void SetProjector (int i, LightPtr spkProjector);
    LightPtr GetProjector (int i) const;
    void SetShadowColor (int i, const ColorRGBA& rkShadowColor);
    const ColorRGBA& GetShadowColor (int i) const;

protected:
    PlanarShadowEffect ();

    bool GetProjectionMatrix (int i,
        const BoundingVolume* pkGlobalObjectWorldBound,
        Matrix4f& rkProjection);

    int m_iQuantity;
    TriMeshPtr* m_aspkPlane;
    LightPtr* m_aspkProjector;
    ColorRGBA* m_akShadowColor;

    // Temporary render state for drawing.
    AlphaStatePtr m_spkAState;
    MaterialStatePtr m_spkMState;
    StencilStatePtr m_spkSState;
    ZBufferStatePtr m_spkZState;
    MaterialEffectPtr m_spkMEffect;
};

WM4_REGISTER_STREAM(PlanarShadowEffect);
typedef Pointer<PlanarShadowEffect> PlanarShadowEffectPtr;
#include "Wm4PlanarShadowEffect.inl"

}

#endif
