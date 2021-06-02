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
// Version: 4.0.1 (2006/08/07)

#ifndef WM4BOUNDINGVOLUME_H
#define WM4BOUNDINGVOLUME_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4Plane3.h"
#include "Wm4Ray3.h"
#include "Wm4Transformation.h"
#include "Wm4Vector3Array.h"
#include "Wm4VertexBuffer.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM BoundingVolume : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~BoundingVolume ();

    // run-time type information
    enum BVType
    {
        BV_SPHERE,
        BV_BOX,
        BV_QUANTITY
    };
    virtual int GetBVType () const = 0;

    // all bounding volumes must define a center and radius
    virtual void SetCenter (const Vector3f& rkCenter) = 0;
    virtual void SetRadius (float fRadius) = 0;
    virtual Vector3f GetCenter () const = 0;
    virtual float GetRadius () const = 0;

    // One of the derived classes has the responsibility for implementing
    // this factory function.  Our default is the WmlSphereBV class, but
    // you may change it to another.
    static BoundingVolume* Create ();

    // Compute a bounding volume that contains all the points.
    virtual void ComputeFromData (const Vector3fArray* pkVertices) = 0;
    virtual void ComputeFromData (const VertexBuffer* pkVBuffer) = 0;

    // Transform the bounding volume (model-to-world conversion).
    virtual void TransformBy (const Transformation& rkTransform,
        BoundingVolume* pkResult) = 0;

    // Determine if the bounding volume is one side of the plane, the other
    // side, or straddles the plane.  If it is on the positive side (the
    // side to which the normal points), the return value is +1.  If it is
    // on the negative side, the return value is -1.  If it straddles the
    // plane, the return value is 0.
    virtual int WhichSide (const Plane3f& rkPlane) const = 0;

    // Test for intersection of linear component and bound (points of
    // intersection not computed).  The linear component is parameterized by
    // P + t*D, where P is a point on the component and D is a unit-length
    // direction.  The interval [tmin,tmax] is
    //   line:     tmin = -Mathf::MAX_REAL, tmax = Mathf::MAX_REAL
    //   ray:      tmin = 0.0f, tmax = Mathf::MAX_REAL
    //   segment:  tmin = 0.0f, tmax > 0.0f
    virtual bool TestIntersection (const Vector3f& rkOrigin,
        const Vector3f& rkDirection, float fTMin, float fTMax) const = 0;

    // Test for intersection of the two bounds.
    virtual bool TestIntersection (const BoundingVolume* pkInput) const = 0;

    // Make a copy of the bounding volume.
    virtual void CopyFrom (const BoundingVolume* pkInput) = 0;

    // Change the current bounding volume so that it contains the input
    // bounding volume as well as its old bounding volume.
    virtual void GrowToContain (const BoundingVolume* pkInput) = 0;

    // test for containment of a point
    virtual bool Contains (const Vector3f& rkPoint) const = 0;

protected:
    BoundingVolume ();
};

WM4_REGISTER_STREAM(BoundingVolume);
typedef Pointer<BoundingVolume> BoundingVolumePtr;

}

#endif
