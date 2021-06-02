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

#ifndef WM4CULLER_H
#define WM4CULLER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Camera.h"
#include "Wm4Plane3.h"
#include "Wm4Spatial.h"
#include "Wm4VisibleSet.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Culler
{
public:
    // Construction and destruction.  The first two input parameters are used
    // to create the set of potentially visible objects.  If the camera is
    // not passed to the constructor, you should set it using SetCamera before
    // calling ComputeVisibleSet.
    Culler (int iMaxQuantity = 0, int iGrowBy = 0,
        const Camera* pkCamera = 0);
    virtual ~Culler ();

    // Access to the camera, frustum copy, and potentially visible set.
    void SetCamera (const Camera* pkCamera);
    const Camera* GetCamera () const;
    void SetFrustum (const float* afFrustum);
    const float* GetFrustum () const;
    VisibleSet& GetVisibleSet ();

    // The base class behavior creates a VisibleObject from the input and
    // appends it to the end of the VisibleObject array.  Derived classes
    // may override this behavior; for example, the array might be maintained
    // as a sorted array for minimizing render state changes or it might be
    // maintained as a unique list of objects for a portal system.
    virtual void Insert (Spatial* pkObject, Effect* pkGlobalEffect);

    // Access to the stack of world culling planes.  You may push and pop
    // planes to be used in addition to the view frustum planes.  PushPlane
    // requires the input plane to be in world coordinates.
    enum { VS_MAX_PLANE_QUANTITY = 32 };
    int GetPlaneQuantity () const;
    const Plane3f* GetPlanes () const;
    void SetPlaneState (unsigned int uiPlaneState);
    unsigned int GetPlaneState () const;
    void PushPlane (const Plane3f& rkPlane);
    void PopPlane ();

    // Compare the object's world bounding volume against the culling planes.
    // Only Spatial calls this function.
    bool IsVisible (const BoundingVolume* pkWound);

    // Support for Portal::GetVisibleSet.
    bool IsVisible (int iVertexQuantity, const Vector3f* akVertex,
        bool bIgnoreNearPlane);

    // Support for BspNode::GetVisibleSet.  Determine if the view frustum is
    // fully on one side of a plane.  The "positive side" of the plane is the
    // half space to which the plane normal points.  The "negative side" is
    // the other half space.  The function returns +1 if the view frustum is
    // fully on the positive side of the plane, -1 if the view frustum is
    // fully on the negative side of the plane, or 0 if the view frustum
    // straddles the plane.  The input plane is in world coordinates and the
    // world camera coordinate system is used for the test.
    int WhichSide (const Plane3f& rkPlane) const;

    // This is the main function you should use for culling within a scene
    // graph.  Traverse the scene and construct the potentially visible set
    // relative to the world planes.
    void ComputeVisibleSet (Spatial* pkScene);

protected:
    // The input camera has information that might be needed during the
    // culling pass over the scene.
    const Camera* m_pkCamera;

    // A copy of the view frustum for the input camera.  This allows various
    // subsystems to change the frustum parameters during culling (for
    // example, the portal system) without affecting the camera, whose initial
    // state is needed by the renderer.
    float m_afFrustum[Camera::VF_QUANTITY];

    // The world culling planes corresponding to the view frustum plus any
    // additional user-defined culling planes.  The member m_uiPlaneState
    // represents bit flags to store whether or not a plane is active in the
    // culling system.  A bit of 1 means the plane is active, otherwise the
    // plane is inactive.  An active plane is compared to bounding volumes,
    // whereas an inactive plane is not.  This supports an efficient culling
    // of a hierarchy.  For example, if a node's bounding volume is inside
    // the left plane of the view frustum, then the left plane is set to
    // inactive because the children of the node are automatically all inside
    // the left plane.
    int m_iPlaneQuantity;
    Plane3f m_akPlane[VS_MAX_PLANE_QUANTITY];
    unsigned int m_uiPlaneState;

    // The potentially visible set for a call to GetVisibleSet.
    VisibleSet m_kVisible;
};

#include "Wm4Culler.inl"

}

#endif
