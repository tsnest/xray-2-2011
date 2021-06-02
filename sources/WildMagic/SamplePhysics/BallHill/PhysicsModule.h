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
    void Initialize (double dTime, double dDeltaTime, double dY1, double dY2,
        double dY1Dot, double dY2Dot);

    double GetTime () const;
    double GetDeltaTime () const;
    double GetY1 () const;
    double GetY1Dot () const;
    double GetY2 () const;
    double GetY2Dot () const;

    // get the ball center and incremental rotation (after update)
    void GetData (Wm4::Vector3f& rkCenter, Wm4::Matrix3f& rkIncrRot) const;

    // compute paraboloid height from an xy-plane position
    float GetHeight (float fX, float fY) const;

    // take a single step of the solver
    void Update ();

    // physical constants
    double Gravity;

    // paraboloid parameters
    double A1, A2, A3;

    // ball radius
    double Radius;

private:
    // The paraboloid is x3 = a3 - (x1/a1)^2 - (x2/a2)^2.  The equations of
    // motion are:
    //   x1"+(4*x1/a1^2)*((x1*x1"+(x1')^2)/a1^2+(x2*x2"+(x2')^2)/a2^2)
    //     = 2*g*x1/a1^2
    //   x2"+(4*x2/a2^2)*((x1*x1"+(x1')^2)/a1^2+(x2*x2"+(x2')^2)/a2^2)
    //     = 2*g*x2/a2^2
    // Make the change of variables y1 = x1/a2 and y2 = x2/a2.  The equations
    // of motion are:
    //   a1^2*y1"+4*y1*(y1*y1"+(y1')^2+y2*y2"+(y2')^2) = 2g*y1
    //   a2^2*y2"+4*y2*(y1*y1"+(y1')^2+y2*y2"+(y2')^2) = 2g*y2
    // The second derivatives y1" and y2" can be solved algebraically:
    //  +   +   +                       +^{-1} +                             +
    //  |y1"| = |a1^2+4*y1^2 4*y1*y2    |      |2*g*y1-4*y1*((y1')^2+(y2')^2)|
    //  |y2"|   |4*y1*y2     a2^2+4*y2^2|      |2*g*y2-4*y2*((y1')^2+(y2')^2)|
    //  +   +   +                       +      +                             +
    //
    // The four state variables for the RK4 solver.
    //
    // state[0] = y1
    // state[1] = y1'
    // state[2] = y2
    // state[3] = y2'
    //
    // Auxiliary variables that the caller of the RK4 Update function must
    // set before passing to the update.
    //
    // aux[0] = a1^2
    // aux[1] = a2^2
    // aux[2] = g
    // aux[3] = storage for return value of DY2Dot, calculated in DY1Dot

    // state and auxiliary variables
    double m_dTime, m_dDeltaTime, m_adState[4], m_adAux[3];

    // ODE solver (specific solver assigned in the cpp file)
    Wm4::OdeSolverd* m_pkSolver;
    static void OdeFunction (double dTime, const double* adState,
        void* pvData, double* adFValue);
};

#include "PhysicsModule.inl"

#endif
