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

#ifndef BOUNCINGBALL_H
#define BOUNCINGBALL_H

#include "Wm4WindowApplication3.h"
#include "DeformableBall.h"
using namespace Wm4;

class BouncingBall : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    BouncingBall ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateBall ();
    void CreateFloor ();
    void CreateWall ();
    void DoPhysical ();
    void DoVisual ();

    // representation of body
    DeformableBall* m_pkBall;

    // simulated clock
    float m_fSimTime, m_fSimDelta;

    // the scene graph
    NodePtr m_spkScene, m_spkBall;
    TriMeshPtr m_spkFloor, m_spkWall;
    WireframeStatePtr m_spkWireframeState;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(BouncingBall);

#endif
