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

#ifndef CREATENORMALMAP_H
#define CREATENORMALMAP_H

#include "Wm4ConsoleApplication.h"
using namespace Wm4;

class CreateNormalMap : public ConsoleApplication
{
    WM4_DECLARE_INITIALIZE;

public:
    virtual int Main (int iQuantity, char** apcArgument);

protected:
    void Usage ();

    int GetImage (const char* acBMPName, int& riWidth, int& riHeight,
        unsigned char*& raucData);

    void SaveImage (const char* acBMPName, int iWidth, int iHeight,
        unsigned char* aucData);

    unsigned char* GenerateNormals (int iXMax, int iYMax,
        float* afHeight, float fXScale, float fYScale);
};

WM4_REGISTER_INITIALIZE(CreateNormalMap);

#endif
