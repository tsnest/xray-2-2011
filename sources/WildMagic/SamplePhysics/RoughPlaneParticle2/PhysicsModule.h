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

#include "Wm4OdeSolver.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // initialize the differential equation solver
    void Initialize (double dTime, double dDeltaTime, double dX1, double dY1,
        double dX2, double dY2, double dXDot, double dYDot, double dThetaDot);

    // access the current state
    double GetTime () const;
    double GetDeltaTime () const;
    double GetX () const;
    double GetXDot () const;
    double GetY () const;
    double GetYDot () const;
    double GetTheta () const;
    double GetThetaDot () const;
    void Get (double& rdX1, double& rdY1, double& rdX2, double& rdY2) const;

    // take a single step of the solver
    void Update ();

    // physical constants // symbols used in the Game Physics book
    double Gravity;               // g
    double Mass1, Mass2;          // m1, m2
    double Friction1, Friction2;  // c1, c2

protected:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime;
    double m_adState[6], m_adAux[7];
    double m_dLength1, m_dLength2;

    // ODE solver (specific solver assigned in the cpp file)
    Wm4::OdeSolverd* m_pkSolver;
    static void OdeFunction (double dTime, const double* adState,
        void* pvData, double* adFValue);
};

#include "PhysicsModule.inl"

#endif
