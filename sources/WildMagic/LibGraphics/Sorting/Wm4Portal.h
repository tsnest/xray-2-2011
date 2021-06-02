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

#ifndef WM4PORTAL_H
#define WM4PORTAL_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Spatial.h"

namespace Wm4
{

class ConvexRegion;
class Culler;

class WM4_GRAPHICS_ITEM Portal : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // The portal is a unidirectional connector between two regions.  The
    // vertices of the portal must satisfy the following constraints:
    // 1. They must form a planar simple polygon (quantity >= 3 is implied).
    //    The polygon does not have to be convex.
    // 2. They must be counterclockwise ordered when looking through the
    //    portal to the adjacent region.
    // 3. They must be in the model space coordinates for the region that
    //    contains the portal.

    // Construction and destruction.  Portal accepts responsibility for
    // deleting the input array.  The model vertices must be counterclockwise
    // oriented in the model plane when viewed from the side of the plane to
    // which the plane normal points.
    Portal (int iVQuantity, Vector3f* akModelVertex,
        const Plane3f& rkModelPlane, ConvexRegion* pkAdjacentRegion,
        bool bOpen);

    virtual ~Portal ();

    // member access
    ConvexRegion*& AdjacentRegion ();
    bool& Open ();

protected:
    // streaming
    Portal ();

    // updates
    friend class ConvexRegion;
    void UpdateWorldData (const Transformation& rkWorld);

    // culling
    bool ReducedFrustum (const Culler& rkCuller, float afReducedFrustum[6]);
    void GetVisibleSet (Culler& rkCuller, bool bNoCull);

    // portal vertices and their planes
    int m_iVQuantity;
    Vector3f* m_akModelVertex;
    Vector3f* m_akWorldVertex;
    Plane3f m_kModelPlane, m_kWorldPlane;

    // region to which portal leads
    ConvexRegion* m_pkAdjacentRegion;

    // portals can be open or closed
    bool m_bOpen;
};

WM4_REGISTER_STREAM(Portal);
typedef Pointer<Portal> PortalPtr;
#include "Wm4Portal.inl"

}

#endif
