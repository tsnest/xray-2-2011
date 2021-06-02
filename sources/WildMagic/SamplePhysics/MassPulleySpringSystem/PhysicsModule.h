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

    // initialize the numerical solver
    void Initialize (double dTime, double dDeltaTime, double dY1, double dDY1,
        double dDY3);

    // take a single step of the solver
    void Update ();

    // gravitational constant
    double Gravity;

    // left mass in Figure 3.13
    double Mass1;

    // right mass in Figure 3.13
    double Mass2;

    // length of rigid wire connecting mass 1 to mass 2
    double WireLength;

    // pulley parameters
    double Mass3, Radius, Inertia;

    // spring parameters
    double SpringLength, SpringConstant;

    // accessors
    double GetInitialY1 () const;
    double GetCurrentY1 () const;
    double GetCurrentY2 () const;
    double GetCurrentY3 () const;
    double GetAngle () const;
    double GetCableFraction1 () const;
    double GetCableFraction2 () const;

private:
    // time information
    double m_dTime, m_dDeltaTime;

    // derived parameters
    double m_dAlpha, m_dBeta, m_dGamma, m_dDelta, m_dOmega, m_dGDivOmegaSqr;

    // initial conditions
    double m_dY1, m_dY2, m_dY3, m_dDY1, m_dDY2, m_dDY3;

    // solution parameters
    double m_dLPlusGDivOmegaSqr, m_dK1, m_dK2, m_dTCoeff, m_dTSqrCoeff;
    double m_dDeltaDivOmegaSqr, m_dY1Curr, m_dY2Curr, m_dY3Curr;
};

#include "PhysicsModule.inl"

#endif
