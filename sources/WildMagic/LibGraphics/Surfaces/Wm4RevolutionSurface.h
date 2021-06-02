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

#ifndef WM4REVOLUTIONSURFACE_H
#define WM4REVOLUTIONSURFACE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4TriMesh.h"
#include "Wm4Curve2.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM RevolutionSurface : public TriMesh
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // The axis of revolution is the z-axis.  You can obtain arbitrary axes
    // after the fact by applying translations and rotations to the object.
    // The curve of revolution is (x(t),z(t)) with t in [tmin,tmax] and
    // z(t) > 0.  It is also assumed to be non-self-intersecting.  When the
    // curve is open, three cases to consider:  If z(tmin) and z(tmax) are
    // both positive, the surface topology is that of a cylinder (use
    // REV_CYLINDER_TOPOLOGY).  If exactly one of z(tmin) or z(tmax) is zero,
    // the surface topology is that of a disk (use REV_DISK_TOPOLOGY).  If
    // z(tmin) and z(tmax) are both zero, the surface topology is that of a
    // sphere (use REV_SPHERE_TOPOLOGY).  When the curve of revolution is
    // closed so that (x(tmin),z(tmin)) and (x(tmax),z(tmax)) are the same
    // point, the surface topology is that of a torus.  For now, rather than
    // having the surface object determine the type of curve, use the
    // Topology enumerated values to provide this information.

    enum TopologyType
    {
        REV_DISK_TOPOLOGY,
        REV_CYLINDER_TOPOLOGY,
        REV_SPHERE_TOPOLOGY,
        REV_TORUS_TOPOLOGY,
        MAX_TOPOLOGY_TYPES
    };

    // Construction and destruction.  The caller is responsible for deleting
    // the input curve.
    RevolutionSurface (Curve2f* pkCurve, float fXCenter,
        TopologyType eTopology, int iCurveSamples, int iRadialSamples,
        const Attributes& rkAttr, bool bSampleByArcLength, bool bOutsideView);

    virtual ~RevolutionSurface ();

    // member access
    int GetCurveSamples () const;
    int GetRadialSamples () const;
    void SetCurve (Curve2f* pkCurve);
    const Curve2f* GetCurve () const;
    TopologyType GetTopology () const;
    bool& SampleByArcLength ();
    bool GetSampleByArcLength () const;

    // Modify vertices and normals when the curve itself changes over time.
    // You are responsible for maintaining the topology of the curve.  For
    // example, if your constructor input was REV_SPHERE_TOPOLOGY, you should
    // not change the curve to produce a non-spherical topology.
    void UpdateSurface ();

protected:
    RevolutionSurface ();

    void ComputeSampleData ();
    void UpdateDisk ();
    void UpdateCylinder ();
    void UpdateSphere ();
    void UpdateTorus ();

    Curve2f* m_pkCurve;
    float m_fXCenter;
    TopologyType m_eTopology;
    int m_iCurveSamples, m_iRadialSamples;
    float* m_afSin;
    float* m_afCos;
    Vector3f* m_akSample;
    bool m_bSampleByArcLength;

private:
    static const char* ms_aacTopology[MAX_TOPOLOGY_TYPES];
};

WM4_REGISTER_STREAM(RevolutionSurface);
typedef Pointer<RevolutionSurface> RevolutionSurfacePtr;
#include "Wm4RevolutionSurface.inl"

}

#endif
