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

#ifndef SCREENPOLYGONS_H
#define SCREENPOLYGONS_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class ScreenPolygons : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    ScreenPolygons ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();

    NodePtr m_spkScene;
    CameraPtr m_spkScreenCamera;
    TriMeshPtr m_spkForePoly, m_spkMidPoly, m_spkBackPoly;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

    float m_fLinearZ, m_fDepthZ;
    double m_dAnimTime, m_dAnimTimeDelta;
};

WM4_REGISTER_INITIALIZE(ScreenPolygons);

#endif
