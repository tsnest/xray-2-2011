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

#ifndef WM4BSPNODE_H
#define WM4BSPNODE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Node.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM BspNode : public Node
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  The base class Node has *three* children
    // and is not allowed to grow.  The first and last children (indices 0 and
    // 2) are the "positive" and "negative" children of the binary tree.  The
    // positive child corresponds to the positive side of the separating
    // plane.  The negative child corresponds to the negative side of the
    // separating plane.  The middle child slot is where additional geometry
    // may be attached such as the triangles that are coplanar with the
    // separating plane.
    BspNode ();
    BspNode (const Plane3f& rkModelPlane);
    virtual ~BspNode ();

    // These methods should be used instead of the attach/detach methods in
    // the Node base class.
    SpatialPtr AttachPositiveChild (Spatial* pkChild);
    SpatialPtr AttachCoplanarChild (Spatial* pkChild);
    SpatialPtr AttachNegativeChild (Spatial* pkChild);
    SpatialPtr DetachPositiveChild ();
    SpatialPtr DetachCoplanarChild ();
    SpatialPtr DetachNegativeChild ();
    SpatialPtr GetPositiveChild ();
    SpatialPtr GetCoplanarChild ();
    SpatialPtr GetNegativeChild ();

    // plane access
    Plane3f& ModelPlane ();
    const Plane3f& GetModelPlane () const;
    const Plane3f& GetWorldPlane () const;

    // determine the portion of the scene that contains the point
    Spatial* GetContainingNode (const Vector3f& rkPoint);

protected:
    // geometric updates
    virtual void UpdateWorldData (double dAppTime);

    // culling
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull);

    Plane3f m_kModelPlane;
    Plane3f m_kWorldPlane;
};

WM4_REGISTER_STREAM(BspNode);
typedef Pointer<BspNode> BspNodePtr;
#include "Wm4BspNode.inl"

}

#endif
