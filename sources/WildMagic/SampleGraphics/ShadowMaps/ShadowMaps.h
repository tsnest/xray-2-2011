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
// Version: 4.0.1 (2008/10/13)

#ifndef SHADOWMAPS_H
#define SHADOWMAPS_H

#include "Wm4WindowApplication3.h"
#include "ShadowMapEffect.h"
using namespace Wm4;

class ShadowMaps : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    ShadowMaps ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void CreateGround ();
    void CreateTorus ();
    void CreateGlobe ();
    void CreateFlashlight ();

    NodePtr m_spkScene, m_spkShadowMapNode, m_spkFlashlight;
    TriMeshPtr m_spkGround, m_spkTorus, m_spkGlobe, m_spkLightSphere;
    LightPtr m_spkLight;
    WireframeStatePtr m_spkWireframe;
    ShadowMapEffectPtr m_spkSMEffect;
    Culler m_kCuller;

    const char* m_acCaption;
    static const char* ms_aacCaptions[3];
};

WM4_REGISTER_INITIALIZE(ShadowMaps);

#endif
