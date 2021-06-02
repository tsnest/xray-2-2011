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

#ifndef WM4HULLTRIANGLE3_H
#define WM4HULLTRIANGLE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Query3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM HullTriangle3
{
public:
    HullTriangle3 (int iV0, int iV1, int iV2);

    int GetSign (int i, const Query3<Real>* pkQuery);

    void AttachTo (HullTriangle3* pkAdj0, HullTriangle3* pkAdj1,
        HullTriangle3* pkAdj2);

    int DetachFrom (int iAdj, HullTriangle3* pkAdj);

    int V[3];
    HullTriangle3* A[3];
    int Sign;
    int Time;
    bool OnStack;
};

}

#endif
