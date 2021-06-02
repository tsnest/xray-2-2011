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

#include "Wm4System.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // initialize the differential equation solver
    void Initialize (double dTime, double dDeltaTime, double dX, double dY,
        double dTheta, double dXDer, double dYDer, double dThetaDer);

    // access the current state
    double GetTime () const;
    double GetDeltaTime () const;
    double GetX () const;
    double GetXDer () const;
    double GetY () const;
    double GetYDer () const;
    double GetTheta () const;
    double GetThetaDer () const;

    // get rectangle corners in counterclockwise order
    void GetRectangle (
        double& rdX00, double& rdY00,
        double& rdX10, double& rdY10,
        double& rdX11, double& rdY11,
        double& rdX01, double& rdY01) const;

    // take a single step of the solver
    void Update ();

    // physical constants   // symbols used in the Game Physics book
    double MuGravity;       // mu*g
    double XLocExt;         // alpha0
    double YLocExt;         // beta0

protected:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime;
    double m_dX, m_dY, m_dTheta, m_dXDer, m_dYDer, m_dThetaDer;
    double m_dX0, m_dY0, m_dTheta0, m_dXDer0, m_dYDer0, m_dThetaDer0;
    double m_dLinVelCoeff, m_dAngVelCoeff;
};

#include "PhysicsModule.inl"

#endif
