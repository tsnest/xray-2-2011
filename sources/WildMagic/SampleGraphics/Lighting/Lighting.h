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

#ifndef LIGHTING_H
#define LIGHTING_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class Lighting : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Lighting ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void CreateLights ();
    void CreatePlane ();
    void CreateSphere ();
    void UpdateEffects ();

    NodePtr m_spkScene;
    TriMeshPtr m_spkPlane, m_spkSphere;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

    // L1     =   4 combinations
    // L2     =  10 combinations
    // L3     =  20 combinations
    // L4     =  35 combinations
    // L5     =  56 combinations
    // L6     =  84 combinations
    // L7     = 120 combinations
    // L8     = 165 combinations
    // Total  = 494 combinations
    int m_iAQuantity, m_iDQuantity, m_iPQuantity, m_iSQuantity;
    LightPtr m_aspkALight[8];
    LightPtr m_aspkDLight[8];
    LightPtr m_aspkPLight[8];
    LightPtr m_aspkSLight[8];

    DefaultShaderEffectPtr m_spkDefaultEffect;
    int m_iActiveLight;
    int m_iLightQuantity;
    char m_acCaption[9];
};

WM4_REGISTER_INITIALIZE(Lighting);

#endif
