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
// Version: 4.0.0 (2006/10/09)

#ifndef NONITERATIVEEIGENSOLVER_H
#define NONITERATIVEEIGENSOLVER_H

#include "Wm4ConsoleApplication.h"
using namespace Wm4;

class NoniterativeEigensolver : public ConsoleApplication
{
    WM4_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

private:
    void SolveAndMeasure (const Matrix3f& rkA, float& rfIError,
        float& rfNError, float& rfIDet, float& rfNDet);

    // Data for iterative method.
    float m_afIEValue[3];
    Vector3f m_akIEVector[3];

    // Data for noniterative method.
    float m_afNEValue[3];
    Vector3f m_akNEVector[3];
};

WM4_REGISTER_INITIALIZE(NoniterativeEigensolver);

#endif
