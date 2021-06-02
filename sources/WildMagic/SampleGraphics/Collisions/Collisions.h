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

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class Collisions : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Collisions ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();

    NodePtr m_spkScene;
    TriMeshPtr m_spkCyln0, m_spkCyln1;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

    // collision system
    CollisionGroup* m_pkGroup;
    bool Transform (unsigned char ucKey);
    void ResetColors ();
    static void Response (CollisionRecord& rkRecord0, int iT0,
        CollisionRecord& rkRecord1, int iT1,
        Intersector<float,Vector3f>* pkIntersector);

    ImagePtr m_spkCylinderImage;
    static Vector2f ms_kBlueUV, ms_kRedUV, ms_kCyanUV, ms_kYellowUV;
};

WM4_REGISTER_INITIALIZE(Collisions);

#endif
