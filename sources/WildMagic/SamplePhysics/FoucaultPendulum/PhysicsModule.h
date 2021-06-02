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
#include "Wm4Matrix3.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // initialize the differential equation solver
    void Initialize (double dTime, double dDeltaTime, double dTheta,
        double dPhi, double dThetaDot, double dPhiDot);

    double GetTime () const;
    double GetDeltaTime () const;
    double GetTheta () const;
    double GetThetaDot () const;
    double GetPhi () const;
    double GetPhiDot () const;

    // the orientation of the pendulum
    Wm4::Matrix3f GetOrientation () const;

    // take a single step of the solver
    void Update ();

    // pendulum parameters
    double AngularSpeed;  // w
    double Latitude;  // lat
    double GDivL;  // g/L

private:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime, m_adState[4], m_adAux[3];

    // ODE solver (specific solver assigned in the cpp file)
    Wm4::OdeSolverd* m_pkSolver;
    static void OdeFunction (double dTime, const double* adState,
        void* pvData, double* adFValue);
};

#include "PhysicsModule.inl"

#endif
