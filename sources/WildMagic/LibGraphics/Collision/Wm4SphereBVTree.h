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

#ifndef WM4SPHEREBVTREE_H
#define WM4SPHEREBVTREE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4BoundingVolumeTree.h"
#include "Wm4SphereBV.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM SphereBVTree : public BoundingVolumeTree
{
    WM4_DECLARE_INITIALIZE;

public:
    SphereBVTree (const TriMesh* pkMesh, int iMaxTrisPerLeaf = 1,
        bool bStoreInteriorTris = false);

protected:
    static BoundingVolume* CreateModelBound (const TriMesh* pkMesh, int i0,
        int i1, int* aiISplit, Line3f& rkLine);

    static BoundingVolume* CreateWorldBound ();
};

WM4_REGISTER_INITIALIZE(SphereBVTree);

}

#endif
