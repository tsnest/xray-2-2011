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
// Version: 4.0.1 (2006/10/18)

#ifndef SIMPLEPENDULUM_H
#define SIMPLEPENDULUM_H

#include "Wm4ConsoleApplication.h"
#include "Wm4Imagics.h"
using namespace Wm4;

class SimplePendulum : public ConsoleApplication
{
    WM4_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

protected:
    enum { SIZE = 512 };

    static float* ExplicitEuler (float fX0, float fY0, float fH);
    static float* ImplicitEuler (float fX0, float fY0, float fH);
    static float* RungeKutta (float fX0, float fY0, float fH);
    static float* LeapFrog (float fX0, float fY0, float fH);
    void SolveMethod (float* (*oMethod)(float,float,float),
        const char* acOutIm, const char* acOutTxt);

    void Stiff1 ();
    float F0 (float fT, float fX, float fY);
    float F1 (float fT, float fX, float fY);
    void Stiff2True ();
    void Stiff2Approximate ();

    static void DrawPixel (int iX, int iY);
    static ImageRGB82D* ms_pkImage;
    static unsigned int ms_uiColor;
    static float ms_fK;
};

WM4_REGISTER_INITIALIZE(SimplePendulum);

#endif
