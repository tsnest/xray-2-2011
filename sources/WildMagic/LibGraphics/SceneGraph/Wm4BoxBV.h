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

#ifndef WM4BOXBV_H
#define WM4BOXBV_H

#include "Wm4GraphicsLIB.h"
#include "Wm4BoundingVolume.h"
#include "Wm4Box3.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM BoxBV : public BoundingVolume
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.
    BoxBV ();  // center (0,0,0), axes (1,0,0),(0,1,0),(0,0,1), extents 1,1,1
    BoxBV (const Box3f& rkBox);
    virtual ~BoxBV ();

    virtual int GetBVType () const;

    // all bounding volumes must define a center and radius
    virtual void SetCenter (const Vector3f& rkCenter);
    virtual void SetRadius (float fRadius);
    virtual Vector3f GetCenter () const;
    virtual float GetRadius () const;

    Box3f& Box ();
    const Box3f& GetBox () const;

    // Compute a box that contains all the points.
    virtual void ComputeFromData (const Vector3fArray* pkVertices);
    virtual void ComputeFromData (const VertexBuffer* pkVBuffer);

    // Transform the box (model-to-world conversion).
    virtual void TransformBy (const Transformation& rkTransform,
        BoundingVolume* pkResult);

    // Determine if the bounding volume is one side of the plane, the other
    // side, or straddles the plane.  If it is on the positive side (the
    // side to which the normal points), the return value is +1.  If it is
    // on the negative side, the return value is -1.  If it straddles the
    // plane, the return value is 0.
    virtual int WhichSide (const Plane3f& rkPlane) const;

    // Test for intersection of linear component and bound (points of
    // intersection not computed).  The linear component is parameterized by
    // P + t*D, where P is a point on the component and D is a unit-length
    // direction.  The interval [tmin,tmax] is
    //   line:     tmin = -Mathf::MAX_REAL, tmax = Mathf::MAX_REAL
    //   ray:      tmin = 0.0f, tmax = Mathf::MAX_REAL
    //   segment:  tmin = 0.0f, tmax > 0.0f
    virtual bool TestIntersection (const Vector3f& rkOrigin,
        const Vector3f& rkDirection, float fTMin, float fTMax) const;

    // Test for intersection of the two bounds.
    virtual bool TestIntersection (const BoundingVolume* pkInput) const;

    // Make a copy of the bounding volume.
    virtual void CopyFrom (const BoundingVolume* pkInput);

    // Change the current box so that it contains itself and the input.
    virtual void GrowToContain (const BoundingVolume* pkInput);

    // test for containment of a point
    virtual bool Contains (const Vector3f& rkPoint) const;

protected:
    Box3f m_kBox;
};

#include "Wm4BoxBV.inl"

WM4_REGISTER_STREAM(BoxBV);
typedef Pointer<BoxBV> BoxBVPtr;

}

#endif
