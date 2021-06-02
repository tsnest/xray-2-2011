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

#ifndef FLOWINGSKIRT_H
#define FLOWINGSKIRT_H

#include "Wm4WindowApplication3.h"
#include "Wm4BSplineCurve3.h"
using namespace Wm4;

class FlowingSkirt : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    FlowingSkirt ();
    virtual ~FlowingSkirt ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateSkirt ();
    void UpdateSkirt ();
    void ModifyCurves ();

    // the scene graph
    NodePtr m_spkScene, m_spkTrnNode;
    TriMeshPtr m_spkSkirt;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

    // the skirt is a generalized Bezier cylinder
    int m_iNumCtrl, m_iDegree;
    float m_fATop, m_fBTop, m_fABot, m_fBBot;
    BSplineCurve3f* m_pkSkirtTop;
    BSplineCurve3f* m_pkSkirtBot;
    float* m_afFrequency;
};

WM4_REGISTER_INITIALIZE(FlowingSkirt);

#endif
