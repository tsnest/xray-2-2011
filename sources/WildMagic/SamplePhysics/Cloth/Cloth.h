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

#ifndef CLOTH_H
#define CLOTH_H

#include "Wm4WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm4;


class Cloth : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Cloth ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    // masses are located at the control points of a spline surface
    BSplineRectanglef* m_pkSpline;

    // a mass-spring system
    PhysicsModule* m_pkModule;
    void DoPhysical ();

    // scene graph
    void CreateSprings ();
    void CreateCloth ();
    void CreateScene ();
    NodePtr m_spkScene, m_spkTrnNode;
    WireframeStatePtr m_spkWireframe;
    RectangleSurfacePtr m_spkCloth;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(Cloth);

#endif
