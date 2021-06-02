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
// Version: 4.0.1 (2006/08/05)

#ifndef WM4COLLAPSERECORD_H
#define WM4COLLAPSERECORD_H

// Representation of an edge collapse.  The current implementation
// collapses an edge <V0,V1> by replacing V1 by V0.  No new vertices or
// surface attributes are introduced.  Moreover, the vertex array and
// connectivity array are permuted so that vertices and triangles that
// are removed first in the collapse process occur at the ends of their
// arrays.  This supports sending the vertices as static geometry to the
// hardware renderer.  The level of detail selection only needs to (1)
// change the active vertex and triangle counts and (2) update the
// connectivity array based on the m_auiIndex values.
//
// Some lighting artifacts occur if normal vectors are used.  The
// artifacts can be reduced if the normals are recalculated from the
// connectivity array each time the target record changes.  In this case
// the normal array becomes dynamic and you have to tell the renderer to
// repack each frame.
//
// The Record class can be modified to store new vertices and attributes
// for incremental updates.  For example, you might want to replace V0
// and V1 by (V0+V1)/2 and average the corresponding attributes.  In this
// case the vertex array becomes dynamic and you have to tell the
// renderer to repack each frame.

#include "Wm4GraphicsLIB.h"
#include "Wm4Stream.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM CollapseRecord
{
public:
    CollapseRecord (int iVKeep = -1, int iVThrow = -1, int iVQuantity = 0,
        int iTQuantity = 0);

    ~CollapseRecord ();

    // edge <VKeep,VThrow> collapses so that VThrow is replaced by VKeep
    int VKeep, VThrow;

    // number of vertices after edge collapse
    int VQuantity;

    // number of triangles after edge collapse
    int TQuantity;

    // connectivity array indices in [0..TQ-1] that contain VThrow
    int IQuantity;
    int* Index;
};

}

#endif
