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

#ifndef WM4HULLEDGE2_H
#define WM4HULLEDGE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Query2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM HullEdge2
{
public:
    HullEdge2 (int iV0, int iV1);

    int GetSign (int i, const Query2<Real>* pkQuery);

    void Insert (HullEdge2* pkAdj0, HullEdge2* pkAdj1);
    void DeleteSelf ();
    void DeleteAll ();

    void GetIndices (int& riHQuantity, int*& raiHIndex);

    int V[2];
    HullEdge2* A[2];
    int Sign;
    int Time;
};

}

#endif
