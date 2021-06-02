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

#ifndef WM4TUBESURFACE_H
#define WM4TUBESURFACE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Curve3.h"
#include "Wm4TriMesh.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM TubeSurface : public TriMesh
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    typedef float (*RadialFunction)(float);

    // Construction and destruction.  TubeSurface accepts responsibility
    // for deleting the input curve.  If rkUpVector is not the zero vector,
    // it will be used as 'up' in the frame calculations.  If it is the zero
    // vector, the Frenet frame will be used.  If bWantColors is 'true',
    // the vertex colors are allocated and set to black.  The application
    // needs to assign colors as needed.  If either of pkUVMin or pkUVMax is
    // not null, both must be not null.  In this case, texture coordinates are
    // generated for the surface.
    TubeSurface (Curve3f* pkMedial, RadialFunction oRadial, bool bClosed,
        const Vector3f& rkUpVector, int iMedialSamples, int iSliceSamples,
        const Attributes& rkAttr, bool bSampleByArcLength, bool bInsideView,
        const Vector2f* pkUVMin, const Vector2f* pkUVMax);

    virtual ~TubeSurface ();

    // member access
    Curve3f*& Medial ();
    const Curve3f* GetMedial () const;
    RadialFunction& Radial ();
    RadialFunction GetRadial () const;
    Vector3f& UpVector ();
    const Vector3f& GetUpVector () const;
    int GetSliceSamples () const;

    // Generate vertices for the end slices.  These are useful when you build
    // an open tube and want to attach meshes at the ends to close the tube.
    // The input array must have size (at least) S+1 where S is the number
    // returned by GetSliceSamples.  Function GetTMinSlice is used to access
    // the slice corresponding to the medial curve evaluated at its domain
    // minimum, tmin.  Function GetTMaxSlice accesses the slice for the
    // domain maximum, tmax.  If the curve is closed, the slices are the same.
    void GetTMinSlice (Vector3f* akSlice);
    void GetTMaxSlice (Vector3f* akSlice);

    // If the medial curve is modified, for example if it is control point
    // based and the control points are modified, then you should call this
    // update function to recompute the tube surface geometry.
    void UpdateSurface ();

protected:
    TubeSurface ();

    // tessellation support
    int Index (int iS, int iM);
    void ComputeSinCos ();
    void ComputeVertices ();
    void ComputeNormals ();
    void ComputeUVs (const Vector2f& rkUVMin, const Vector2f& rkUVMax);
    void ComputeIndices (bool bInsideView);

    Curve3f* m_pkMedial;
    RadialFunction m_oRadial;
    int m_iMedialSamples, m_iSliceSamples;
    Vector3f m_kUpVector;
    float* m_afSin;
    float* m_afCos;
    bool m_bClosed, m_bSampleByArcLength;
};

WM4_REGISTER_STREAM(TubeSurface);
typedef Pointer<TubeSurface> TubeSurfacePtr;
#include "Wm4TubeSurface.inl"

}

#endif
