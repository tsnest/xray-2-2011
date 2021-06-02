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

#ifndef POLYHEDRONDISTANCE_H
#define POLYHEDRONDISTANCE_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class PolyhedronDistance : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    PolyhedronDistance ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    TriMesh* CreateTetra (float fSize, bool bBlack);
    Polyline* CreateLine ();
    TriMesh* CreatePlane ();
    void UpdateLine ();
    void InitialState ();
    bool Transform (unsigned char ucKey);

    // representation of bodies
    TTuple<3,int>* m_akFace;
    TriMesh* m_apkTetra[4];
    VertexBuffer* m_apkVBuffer[2];
    Vector3f m_akTV[2];
    Matrix3f m_akTM[2];

    // display variables
    float m_fSeparation;
    float m_fEdgeLength;
    float m_fSmall;

    // Offsets during calculation with LCPPolyDist to ensure that all of the
    // vertices are in the first octant.
    float m_fOffset;
 
    // the scene graph
    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    PolylinePtr m_aspkLine[2];
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(PolyhedronDistance);

#endif
