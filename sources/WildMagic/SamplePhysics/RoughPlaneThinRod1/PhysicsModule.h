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
    void Initialize (double dTime, double dDeltaTime, double dX, double dY,
        double dTheta, double dXDot, double dYDot, double dThetaDot);

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

    // physical constants   // symbols used in the Game Physics book
    double Length;          // L1 = L2 = L/2
    double MassDensity;     // delta0
    double Friction;        // c

protected:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime;
    double m_adState[6], m_adAux[9];
    double m_dHalfLength;

    // integrands for the generalized forces
    static double FXIntegrand (double dL, void* pvData);
    static double FYIntegrand (double dL, void* pvData);
    static double FThetaIntegrand (double dL, void* pvData);

    // ODE solver (specific solver assigned in the cpp file)
    Wm4::OdeSolverd* m_pkSolver;
    static void OdeFunction (double dTime, const double* adState,
        void* pvData, double* adFValue);

    // integration parameter
    static int ms_iOrder;
};

#include "PhysicsModule.inl"

#endif
