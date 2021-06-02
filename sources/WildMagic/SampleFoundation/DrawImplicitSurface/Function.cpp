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

#include "Function.h"

// define only one of these
//#define F_SPHERE
#define F_TORUS

#ifdef F_SPHERE
//----------------------------------------------------------------------------
float F (const Vector3f& rkPos)
{
    // F(x,y,z) = x^2 + y^2 + z^2 - 1 (level surface is a sphere)
    return rkPos.SquaredLength() - 1.0f;
}
//----------------------------------------------------------------------------
Vector3f DF (const Vector3f& rkPos)
{
    // DF(x,y,z) = (2x,2y,2z)
    return 2.0f*rkPos;
}
//----------------------------------------------------------------------------
#endif

#ifdef F_TORUS
// Ro > 0 is radius from center of torus
// Ri > 0 is radius of tube of torus
// p^2 = x^2+y^2+z^2
// p^4-2*(Ro^2+Ri^2)*p^2+4*Ro^2*z^2+(Ro^2-Ri^2)^2 = 0
const float g_fRO = 1.0f;
const float g_fRI = 0.25f;
const float g_fROSqr = g_fRO*g_fRO;
const float g_fRISqr = g_fRI*g_fRI;
const float g_fSum = g_fROSqr + g_fRISqr;
const float g_fDiff = g_fROSqr - g_fRISqr;
const float g_fDiffSqr = g_fDiff*g_fDiff;
//----------------------------------------------------------------------------
float F (const Vector3f& rkPos)
{
    float fLSqr = rkPos.SquaredLength();
    return fLSqr*(fLSqr - 2.0f*g_fSum) + 4.0f*g_fROSqr*rkPos.Z()*rkPos.Z()
        + g_fDiffSqr;
}
//----------------------------------------------------------------------------
Vector3f DF (const Vector3f& rkPos)
{
    Vector3f kGradient;

    float fTmp = rkPos.SquaredLength() - g_fSum;
    kGradient.X() = 4.0f*rkPos.X()*fTmp; 
    kGradient.Y() = 4.0f*rkPos.Y()*fTmp; 
    kGradient.Z() = 4.0f*rkPos.Z()*(fTmp + 2.0f*g_fROSqr);

    return kGradient;
}
//----------------------------------------------------------------------------
#endif
