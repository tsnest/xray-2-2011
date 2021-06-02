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

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Wm4MassSpringSurface.h"

class PhysicsModule : public Wm4::MassSpringSurface3f
{
public:
    // Construction and destruction.
    //
    // Gravity is controlled by the input rkGravity.
    //
    // Mass-spring systems tend to exhibit "stiffness" (in the sense of
    // numerical stability).  To remedy this problem, a small amount of
    // viscous friction is added to the external force.  This term is
    // of the form -fViscosity*velocity where fViscosity > 0.
    //
    // The initial wind force is specified by the caller.  The wind remains
    // in effect throughout the simulation.  To simulate oscillatory behavior
    // locally, random forces are applied at each mass in the direction
    // perpendicular to the plane of the wind and gravity vectors.  The
    // amplitudes are sinusoidal.  The phases are randomly generated.

    PhysicsModule (int iRows, int iCols, float fStep,
        const Wm4::Vector3f& rkGravity, const Wm4::Vector3f& rkWind,
        float fViscosity, float fAmplitude);

    virtual ~PhysicsModule ();

    // Acceleration is due to forces of gravitation, wind, and viscous
    // friction.  The wind forces are randomly generated.
    virtual Wm4::Vector3f ExternalAcceleration (int i, float fTime,
        const Wm4::Vector3f* akPosition, const Wm4::Vector3f* akVelocity);

protected:
    Wm4::Vector3f m_kGravity, m_kWind;
    float m_fViscosity;

    // sinusoidal forces
    float m_fAmplitude;
    float* m_afPhase;
    Wm4::Vector3f m_kDirection;
};

#endif
