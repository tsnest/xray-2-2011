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
// Version: 4.0.3 (2009/01/23)

#ifndef WM4CAMERA_H
#define WM4CAMERA_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"

namespace Wm4
{

class Renderer;

class WM4_GRAPHICS_ITEM Camera : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.
    Camera ();
    virtual ~Camera ();

    // The camera frame is always in world coordinates.
    //   default location  E = (0,0,0)
    //   default direction D = (0,0,-1)
    //   default up        U = (0,1,0)
    //   default right     R = (1,0,0)
    void SetFrame (const Vector3f& rkLocation, const Vector3f& rkDVector,
        const Vector3f& rkUVector, const Vector3f& rkRVector);
    void SetLocation (const Vector3f& rkLocation);
    void SetAxes (const Vector3f& rkDVector, const Vector3f& rkUVector,
        const Vector3f& rkRVector);
    Vector3f GetLocation () const;
    Vector3f GetDVector () const;
    Vector3f GetUVector () const;
    Vector3f GetRVector () const;

    // The view frustum has parameters [rmin,rmax], [umin,umax], and
    // [dmin,dmax].  The interval [rmin,rmax] is measured in the right
    // direction R.  These are the "left" and "right" frustum values.  The
    // interval [umin,umax] is measured in the up direction U.  These are
    // the "bottom" and "top" values.  The interval [dmin,dmax] is measured
    // in the view direction D.  These are the "near" and "far" values.
    // The frustum values are stored in an array with the following mappings:
    enum
    {
        VF_DMIN     = 0,  // near
        VF_DMAX     = 1,  // far
        VF_UMIN     = 2,  // bottom
        VF_UMAX     = 3,  // top
        VF_RMIN     = 4,  // left
        VF_RMAX     = 5,  // right
        VF_QUANTITY = 6
    };

    // Set the view frustum.  The interval [rmin,rmax] is measured in the
    // right direction R.  These are the "left" and "right" frustum values.
    // The interval [umin,umax] is measured in the up direction U.  These are
    // the "bottom" and "top" values.  The interval [dmin,dmax] is measured
    // in the view direction D.  These are the "near" and "far" values.
    void SetFrustum (float fRMin, float fRMax, float fUMin, float fUMax,
        float fDMin, float fDMax);

    // Set all the view frustum values simultaneously.  TO DO:  Currently,
    // these must be in the order compatible with the VF_* constants.  That
    // is:  dmin, dmax, umin, umax, rmin, rmax.  This is confusing because
    // the previous SetFrustum uses a different order.  Make them consistent.
    void SetFrustum (const float* afFrustum);

    // Set a symmetric view frustum (umin = -umax, rmin = -rmax) using a field
    // of view in the "up" direction and an aspect ratio "width/height".  This
    // call is the equivalent of gluPerspective in OpenGL.  As such, the field
    // of view in this function must be specified in degrees and be in the
    // interval (0,180).
    void SetFrustum (float fUpFovDegrees, float fAspectRatio, float fDMin,
        float fDMax);

    // Get the view frustum.
    void GetFrustum (float& rfRMin, float& rfRMax, float& rfUMin,
        float& rfUMax, float& rfDMin, float& rfDMax) const;

    // Get all the view frustum values simultaneously.
    const float* GetFrustum () const;

    // Get the parameters for a symmetric view frustum.  The return value is
    // 'true' iff the current frustum is symmetric, in which case the output
    // parameters are valid.
    bool GetFrustum (float& rfUpFovDegrees, float& rfAspectRatio,
        float& rfDMin, float& rfDMax) const;

    // Get the individual frustum values.
    float GetDMin () const;
    float GetDMax () const;
    float GetUMin () const;
    float GetUMax () const;
    float GetRMin () const;
    float GetRMax () const;

    // Allow for orthogonal cameras as well as perspective cameras.  The
    // default is perspective (value is 'true').  Set to 'false' for an
    // orthogonal camera.  TO DO.  Stream this member.
    bool Perspective;

    // The viewport is contained in [0,W]x[0,H], where W is the window width
    // and H is the window height.  If the input width and height are zero,
    // the integer values for the viewport are all set to zero and the
    // floating-point viewport is [0,1]x[0,1].
    void SetViewport (int iLeft, int iRight, int iTop, int iBottom,
        int iWidth, int iHeight);

    // If the currently stored width and height are positive, then this call
    // returns all the stored integer values.  If the width and height are
    // zero, the returned values are computed
    void GetViewport (int& riLeft, int& riRight, int& riTop, int& riBottom,
        int& riWidth, int& riHeight);

    // Get the normalized viewport in [0,1]x[0,1].  These are ratios of the
    // integer-valued left/right with width and bottom/top with height.
    void GetViewport (float& rfLeft, float& rfRight, float& rfTop,
        float& rfBottom);

    // The depth range is contained in [0,1].
    void SetDepthRange (float fNear, float fFar);
    void GetDepthRange (float& rfNear, float& rfFar);

    // Mouse picking support.  The (x,y) input point is in left-handed screen
    // coordinates (what you usually read from the windowing system).  The
    // function returns 'true' if and only if the input point is located in
    // the current viewport.  When 'true', the origin and direction values
    // are valid and are in world coordinates.  The direction vector is unit
    // length.
    bool GetPickRay (int iX, int iY, int iWidth, int iHeight,
        Vector3f& rkOrigin, Vector3f& rkDirection) const;

protected:
    // The world coordinate frame.
    Vector3f m_kLocation, m_kDVector, m_kUVector, m_kRVector;

    // The view frustum, stored in order as near, far, bottom, top, left,
    // and right.
    float m_afFrustum[VF_QUANTITY];

    // The viewport in integer units.  The left and right values are in [0,W],
    // where W is the window width.  The bottom and top values are in [0,H],
    // where H is the window height.
    int m_iWidth, m_iHeight;
    int m_iPortL, m_iPortR, m_iPortB, m_iPortT;

    // The viewport in fractional units in [0,1].
    float m_fPortL, m_fPortR, m_fPortT, m_fPortB;

    // The depth range in [0,1].
    float m_fPortN, m_fPortF;

    // The renderer to which this camera has been attached.  The camera is
    // considered to be active if this pointer is not null.  By necessity, a
    // camera cannot be attached to multiple renderers, but a camera may be
    // shared by renderers as long as only one renderer at a time uses the
    // camera.  The renderer is responsible for setting this Camera member.
    friend class Renderer;
    Renderer* m_pkRenderer;
};

WM4_REGISTER_STREAM(Camera);
typedef Pointer<Camera> CameraPtr;
#include "Wm4Camera.inl"

}

#endif
