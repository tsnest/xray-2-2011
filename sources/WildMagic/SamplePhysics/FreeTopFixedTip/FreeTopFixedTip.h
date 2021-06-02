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

#ifndef FREETOPFIXEDTIP_H
#define FREETOPFIXEDTIP_H

#include "Wm4WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm4;

class FreeTopFixedTip : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    FreeTopFixedTip ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    // scene graph
    void CreateScene ();
    TriMesh* CreateFloor ();
    TriMesh* CreateTop ();
    Polyline* CreateAxisTop ();
    Polyline* CreateAxisVertical ();

    NodePtr m_spkScene, m_spkTopRoot;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;
    float m_fMaxPhi;

    // physics
    void DoPhysical ();
    PhysicsModule m_kModule;

    // controlled frame rate
    float m_fLastIdle;
};

WM4_REGISTER_INITIALIZE(FreeTopFixedTip);

#endif
