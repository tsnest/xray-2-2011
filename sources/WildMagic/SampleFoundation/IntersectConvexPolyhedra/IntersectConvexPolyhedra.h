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
// Version: 4.7.0 (2008/09/15)

#ifndef INTERSECTCONVEXPOLYHEDRA_H
#define INTERSECTCONVEXPOLYHEDRA_H

#include "Wm4WindowApplication3.h"
#include "Wm4ConvexPolyhedron.h"
using namespace Wm4;

class IntersectConvexPolyhedra : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    IntersectConvexPolyhedra ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void ComputeIntersection ();

    NodePtr m_spkScene;
    TriMeshPtr m_spkMeshPoly0, m_spkMeshPoly1, m_spkMeshIntersection;
    VertexColor3EffectPtr m_spkEffect;
    Culler m_kCuller;
    Attributes m_kAttr;

    ConvexPolyhedronf m_kWorldPoly0, m_kWorldPoly1, m_kIntersection;
};

WM4_REGISTER_INITIALIZE(IntersectConvexPolyhedra);

#endif
