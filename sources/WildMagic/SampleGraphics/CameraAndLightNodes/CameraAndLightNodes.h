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
// Version: 4.0.1 (2006/07/30)

#ifndef CAMERAANDLIGHTNODES_H
#define CAMERAANDLIGHTNODES_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class CameraAndLightNodes : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    CameraAndLightNodes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual void MoveForward ();
    virtual void MoveBackward ();
    virtual void TurnLeft ();
    virtual void TurnRight ();
    virtual void MoveUp ();
    virtual void MoveDown ();
    virtual void LookUp ();
    virtual void LookDown ();

protected:
    void CreateScene ();
    TriMesh* CreateGround ();
    TriMesh* CreateLightTarget (Light* pkLight);
    Node* CreateLightFixture (LightPtr& rspkAdjustableLight);
    void CreateScreenPolygon ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframeState;
    CameraNodePtr m_spkCNode;
    CameraPtr m_spkScreenCamera;
    TriMeshPtr m_spkSky;
    LightPtr m_spkAdjustableLight0, m_spkAdjustableLight1;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(CameraAndLightNodes);

#endif
