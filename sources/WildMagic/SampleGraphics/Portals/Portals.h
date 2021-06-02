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

#ifndef PORTALS_H
#define PORTALS_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class Portals : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Portals ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateCenterCube (const std::string& rkFloor,
        const std::string& rkCeiling, const std::string& rkWall,
        const std::string& rkPicture0, const std::string& rkPicture1,
        const std::string& rkPicture2, const std::string& rkPicture3,
        Node*& rpkCube, Portal**& rapkPortal);

    void CreateAxisConnector (const std::string& rkFloor,
        const std::string& rkCeiling, const std::string& rkWall,
        Node*& rpkCube, Portal**& rapkPortal);

    void CreateEndCube (const std::string& rkFloor,
        const std::string& rkCeiling, const std::string& rkWall,
        Node*& rpkCube, Portal**& rapkPortal);

    void CreateDiagonalConnector (const std::string& rkFloor,
        const std::string& rkCeiling, const std::string& rkWall,
        Node*& rpkCube, Portal**& rapkPortal);

    TriMesh* CreateOutside ();
    ConvexRegionManager* CreateBspTree ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(Portals);

#endif
