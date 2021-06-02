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

#ifndef WATERDROPFORMATION_H
#define WATERDROPFORMATION_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class WaterDropFormation : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    WaterDropFormation ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    // scene graph
    void CreatePlane ();
    void CreateWall ();
    void CreateWaterRoot ();

    NodePtr m_spkScene, m_spkTrnNode, m_spkWaterRoot;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkPlane, m_spkWall;
    RevolutionSurfacePtr m_spkWaterSurface, m_spkWaterDrop;
    TextureEffectPtr m_spkWater;
    Culler m_kCuller;

    // water sphere curves and simulation parameters
    void Configuration0 ();  // water surface
    void Configuration1 ();  // split into water surface and water drop
    void DoPhysical1 ();
    void DoPhysical2 ();
    void DoPhysical3 ();
    void DoPhysical ();
    void DoVisual ();

    NURBSCurve2f* m_pkSpline;
    NURBSCurve2f* m_pkCircle;
    Vector2f* m_akCtrlPoint;
    Vector2f* m_akTarget;
    float m_fSimTime, m_fSimDelta, m_fLastSeconds;
};

WM4_REGISTER_INITIALIZE(WaterDropFormation);

#endif
