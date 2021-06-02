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

#ifndef TERRAINS_H
#define TERRAINS_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class Terrains : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Terrains ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateSkyDome ();
    void CreateTerrain ();

    // turret-based camera motion
    virtual void MoveForward ();
    virtual void MoveBackward ();
    virtual void MoveUp ();
    virtual void MoveDown ();

    NodePtr m_spkScene;
    TerrainPtr m_spkTerrain;
    TriMeshPtr m_spkSkyDome;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

    float m_fUVBias;
    ColorRGB m_kFogColor;
    float m_fFogDensity;
    float m_fHeightAboveTerrain;
};

WM4_REGISTER_INITIALIZE(Terrains);

#endif
