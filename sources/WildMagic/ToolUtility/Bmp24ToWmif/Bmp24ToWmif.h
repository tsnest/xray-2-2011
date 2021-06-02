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
// Version: 4.0.1 (2008/11/14)

#ifndef BMP24TOWMIF_H
#define BMP24TOWMIF_H

#include "Wm4ConsoleApplication.h"
using namespace Wm4;

class Bmp24ToWmif : public ConsoleApplication
{
    WM4_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

protected:
    void Usage ();

    int GetImage (const char* acBMPName, int& riWidth, int& riHeight,
        unsigned char*& raucData, bool& rbIs24Bit);
};

WM4_REGISTER_INITIALIZE(Bmp24ToWmif);

#endif
