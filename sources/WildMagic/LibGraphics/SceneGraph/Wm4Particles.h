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

#ifndef WM4PARTICLES_H
#define WM4PARTICLES_H

#include "Wm4GraphicsLIB.h"
#include "Wm4TriMesh.h"
#include "Wm4FloatArray.h"

namespace Wm4
{

class Camera;
class Culler;

class WM4_GRAPHICS_ITEM Particles : public TriMesh
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // construction and destruction
    Particles (const Attributes& rkAttr, Vector3fArray* pkLocations,
        FloatArray* pkSizes);
    virtual ~Particles ();

    // data members
    Vector3fArrayPtr Locations;
    FloatArrayPtr Sizes;
    float SizeAdjust;

    void SetActiveQuantity (int iActiveQuantity);
    int GetActiveQuantity () const;

protected:
    Particles ();

    // The particles are billboards that always face the camera.
    void GenerateParticles (const Camera* pkCamera);

    // culling
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull);

    // Allow application to specify fewer than the maximum number of vertices
    // to draw.
    int m_iActiveQuantity;
};

WM4_REGISTER_STREAM(Particles);
typedef Pointer<Particles> ParticlesPtr;
#include "Wm4Particles.inl"

}

#endif
