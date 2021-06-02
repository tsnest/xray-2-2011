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

#ifndef DELAUNAY3D_H
#define DELAUNAY3D_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class Delaunay3D : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Delaunay3D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    TriMesh* CreateSphere () const;
    TriMesh* CreateTetra (int iIndex) const;
    void ChangeTetraStatus (int iIndex, const ColorRGBA& rkColor,
        bool bWireframe);
    void ChangeLastTetraStatus (int iIndex, int iVOpposite,
        const ColorRGBA& rkColor, const ColorRGBA& rkOppositeColor);
    void DoSearch ();

    NodePtr m_spkScene;
    Culler m_kCuller;
    Delaunay3f* m_pkDel;
    Vector3f m_kMin, m_kMax;
};

WM4_REGISTER_INITIALIZE(Delaunay3D);

#endif
