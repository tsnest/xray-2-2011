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
// Version: 4.0.1 (2008/02/18)

#ifndef BLENDEDTERRAIN_H
#define BLENDEDTERRAIN_H

#include "Wm4WindowApplication3.h"
#include "BlendedTerrainEffect.h"
using namespace Wm4;

#define USE_BTEFFECT

class BlendedTerrain : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    BlendedTerrain ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void CreateSkyDome ();
    void CreateHeightField ();
    void UpdateClouds ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkHeightField, m_spkSkyDome;
    ImagePtr m_spkHeight, m_spkBlend;
    Culler m_kCuller;

    float m_fFlowDelta, m_fZAngle, m_fZDeltaAngle;

#ifdef USE_BTEFFECT
    BlendedTerrainEffectPtr m_spkEffect;
#else
    float m_afFlowDirection[4];
    float m_afPowerFactor[4];
#endif
};

WM4_REGISTER_INITIALIZE(BlendedTerrain);

#endif
