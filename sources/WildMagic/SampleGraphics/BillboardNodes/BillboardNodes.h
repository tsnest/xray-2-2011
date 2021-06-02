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
// Version: 4.0.2 (2009/01/23)

#ifndef BILLBOARDNODES_H
#define BILLBOARDNODES_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

//#define DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
#define DEMONSTRATE_POST_PROJECTION_REFLECTION

class BillboardNodes : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    BillboardNodes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();

    NodePtr m_spkScene;
    BillboardNodePtr m_spkBillboard0, m_spkBillboard1;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

#ifdef DEMONSTRATE_POST_PROJECTION_REFLECTION
    CullStatePtr m_spkCullState;
#endif

#ifdef DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
    void CreateScreenObjects ();
    CameraPtr m_spkScreenCamera;
    TriMeshPtr m_spkScreenRectangle;
    TriMeshPtr m_spkTorus;
#endif
};

WM4_REGISTER_INITIALIZE(BillboardNodes);

#endif
