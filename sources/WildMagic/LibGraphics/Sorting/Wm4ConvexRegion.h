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

#ifndef WM4CONVEXREGION_H
#define WM4CONVEXREGION_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Node.h"

namespace Wm4
{

class Portal;

class WM4_GRAPHICS_ITEM ConvexRegion : public Node
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  ConvexRegion accepts responsibility
    // for deleting the input array.
    ConvexRegion (int iPQuantity, Portal** apkPortal);
    virtual ~ConvexRegion ();

    // portal access
    int GetPortalQuantity () const;
    Portal* GetPortal (int i) const;

protected:
    // streaming
    ConvexRegion ();

    // geometric updates
    virtual void UpdateWorldData (double dAppTime);

    // portals of the region (these are not set up to be shared)
    int m_iPQuantity;
    Portal** m_apkPortal;

    // for region graph traversal
    bool m_bVisited;

// internal use
public:
    // Culling.  ConvexRegionManager starts the region graph traversal
    // with the region containing the eye point.  Portal continues the
    // traversal.  Should you decide not to use a convex region manager and
    // track which region the eyepoint is in using your own system, you will
    // need to make sure that convex region that contains the eyepoint is
    // the first one visited during a depth-first traversal of the scene.
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull);
};

WM4_REGISTER_STREAM(ConvexRegion);
typedef Pointer<ConvexRegion> ConvexRegionPtr;
#include "Wm4ConvexRegion.inl"

}

#endif
