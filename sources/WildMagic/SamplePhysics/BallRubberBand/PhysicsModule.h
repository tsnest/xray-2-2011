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

#include "Wm4Vector2.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // initialize the system
    void Initialize (double dTime, double dDeltaTime,
        const Wm4::Vector2d& rkInitPos, const Wm4::Vector2d& rkInitVel);

    // take a single step of the simulation
    void Update ();

    // access the current state
    double GetTime () const;
    double GetDeltaTime () const;
    const Wm4::Vector2d& GetPosition () const;
    const Wm4::Vector2d& GetVelocity () const;
    double GetFrequency () const;

    // physical constants
    double SpringConstant;  // c
    double Mass;  // m

private:
    void Evaluate ();

    // state variables
    double m_dTime, m_dDeltaTime;
    Wm4::Vector2d m_kPosition, m_kVelocity;

    // auxiliary variables
    Wm4::Vector2d m_kInitPos;  // initial position
    double m_dFrequency;  // sqrt(c/m)
    Wm4::Vector2d m_kVelDivFreq;  // initial_velocity/frequency
};

#include "PhysicsModule.inl"

#endif
