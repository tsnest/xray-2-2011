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

#ifndef WM4SURFACEPATCH_H
#define WM4SURFACEPATCH_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4Vector3.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM SurfacePatch : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~SurfacePatch ();

    // The parametric domain is either rectangular or triangular.  Valid (u,v)
    // values for a rectangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax
    // Valid (u,v) values for a triangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax,
    //   (vmax-vmin)*(u-umin)+(umax-umin)*(v-vmax) <= 0
    float GetUMin () const;
    float GetUMax () const;
    float GetVMin () const;
    float GetVMax () const;
    bool IsRectangular () const;

    // position and derivatives up to second order
    virtual Vector3f P (float fU, float fV) const = 0;
    virtual Vector3f PU (float fU, float fV) const = 0;
    virtual Vector3f PV (float fU, float fV) const = 0;
    virtual Vector3f PUU (float fU, float fV) const = 0;
    virtual Vector3f PUV (float fU, float fV) const = 0;
    virtual Vector3f PVV (float fU, float fV) const = 0;

    // Compute a coordinate frame.  The set {T0,T1,N} is a right-handed
    // orthonormal set.
    void GetFrame (float fU, float fV, Vector3f& rkPosition,
        Vector3f& rkTangent0, Vector3f& rkTangent1, Vector3f& rkNormal) const;

    // Differential geometric quantities.  The returned scalars are the
    // principal curvatures and the returned vectors are the corresponding
    // principal directions.
    void ComputePrincipalCurvatureInfo (float fU, float fV, float& rfCurv0,
        float& rfCurv1, Vector3f& rkDir0, Vector3f& rkDir1);

protected:
    SurfacePatch (float fUMin, float fUMax, float fVMin, float fVMax,
        bool bRectangular);
    SurfacePatch ();

    float m_fUMin, m_fUMax, m_fVMin, m_fVMax;
    bool m_bRectangular;
};

WM4_REGISTER_STREAM(SurfacePatch);
typedef Pointer<SurfacePatch> SurfacePatchPtr;
#include "Wm4SurfacePatch.inl"

}

#endif
