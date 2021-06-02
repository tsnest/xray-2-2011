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

#ifndef HELIXTUBESURFACE_H
#define HELIXTUBESURFACE_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class HelixTubeSurface : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    HelixTubeSurface ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnSpecialKeyDown (int iKey, int iX, int iY);

protected:
    MultipleCurve3f* CreateCurve ();
    static float Radial (float fT);
    virtual bool MoveCamera ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;
    MultipleCurve3f* m_pkCurve;
    float m_fMinCurveTime, m_fMaxCurveTime, m_fCurvePeriod;
    float m_fCurveTime, m_fDeltaTime;
};

WM4_REGISTER_INITIALIZE(HelixTubeSurface);

#endif
