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
// Version: 4.6.0 (2007/09/22)

#ifndef DYNAMICTEXTURES_H
#define DYNAMICTEXTURES_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class DynamicTextures : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    DynamicTextures ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    Vector2f Evaluate (const Vector2f& rkParam) const;
    void UpdateTexture ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    DynamicTexturePtr m_spkDTexture;
    TextureEffectPtr m_spkEffect;
    Culler m_kCuller;

    int m_iQuantity;
    int* m_aiPermutation;
    int* m_aiInversePermutation;
    int m_iCurrent, m_iDelta;
    bool m_bReverse;
    bool m_bIsDirect3D;
};

WM4_REGISTER_INITIALIZE(DynamicTextures);

#endif
