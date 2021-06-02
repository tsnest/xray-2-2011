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

#ifndef REFRACTION_H
#define REFRACTION_H

#include "Wm4WindowApplication3.h"
#include "RefractionEffect.h"
using namespace Wm4;

class Refraction : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Refraction ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();

    NodePtr m_spkScene, m_spkTrnNode;
    RefractionEffectPtr m_spkEffect;
    CameraPtr m_spkScreenCamera;
    TriMeshPtr m_spkScreenPolygon;
    Culler m_kCuller;

    bool m_bReflection;
};

WM4_REGISTER_INITIALIZE(Refraction);

#endif
