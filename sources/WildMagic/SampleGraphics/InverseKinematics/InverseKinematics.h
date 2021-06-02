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

#ifndef INVERSEKINEMATICS_H
#define INVERSEKINEMATICS_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class InverseKinematics : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    InverseKinematics ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMotion (int iButton, int iX, int iY,
        unsigned int uiModifiers);

protected:
    void CreateScene ();
    TriMesh* CreateCube ();
    Polyline* CreateRod ();
    TriMesh* CreateGround ();
    void UpdateRod ();
    bool Transform (unsigned char ucKey);

    NodePtr m_spkScene, m_spkIKSystem, m_spkGoal, m_spkJoint0, m_spkJoint1;
    WireframeStatePtr m_spkWireframe;
    PolylinePtr m_spkRod;
    VertexColor3EffectPtr m_spkVCEffect;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(InverseKinematics);

#endif
