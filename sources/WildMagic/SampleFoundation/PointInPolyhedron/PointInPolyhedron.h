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
// Version: 4.6.0 (2007/08/20)

#ifndef POINTINPOLYHEDRON_H
#define POINTINPOLYHEDRON_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class PointInPolyhedron : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    PointInPolyhedron ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void CreateQuery (TriMesh* pkMesh);
    void DeleteQuery ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    PolypointPtr m_spkPoints;
    Culler m_kCuller;

    PointInPolyhedron3f* m_pkQuery;
    PointInPolyhedron3f::TriangleFace* m_akTFace;
    PointInPolyhedron3f::ConvexFace* m_akCFace;
    PointInPolyhedron3f::SimpleFace* m_akSFace;
    int m_iRayQuantity;
    Vector3f* m_akRayDirection;
};

WM4_REGISTER_INITIALIZE(PointInPolyhedron);

#endif
