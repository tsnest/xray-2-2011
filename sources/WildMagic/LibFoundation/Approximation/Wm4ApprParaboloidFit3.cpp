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

#include "Wm4FoundationPCH.h"
#include "Wm4ApprParaboloidFit3.h"
#include "Wm4LinearSystem.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
bool ParaboloidFit3 (int iQuantity, const Vector3<Real>* akPoint,
    Real afCoeff[6])
{
    // allocate linear system (matrix is zeroed initially)
    GMatrix<Real> kMat(6,6);
    Real afRHS[6];
    memset(afRHS,0,6*sizeof(Real));

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX2 = akPoint[i].X()*akPoint[i].X();
        Real fXY = akPoint[i].X()*akPoint[i].Y();
        Real fY2 = akPoint[i].Y()*akPoint[i].Y();
        Real fZX = akPoint[i].Z()*akPoint[i].X();
        Real fZY = akPoint[i].Z()*akPoint[i].Y();
        Real fX3 = akPoint[i].X()*fX2;
        Real fX2Y = fX2*akPoint[i].Y();
        Real fXY2 = akPoint[i].X()*fY2;
        Real fY3 = akPoint[i].Y()*fY2;
        Real fZX2 = akPoint[i].Z()*fX2;
        Real fZXY = akPoint[i].Z()*fXY;
        Real fZY2 = akPoint[i].Z()*fY2;
        Real fX4 = fX2*fX2;
        Real fX3Y = fX3*akPoint[i].Y();
        Real fX2Y2 = fX2*fY2;
        Real fXY3 = akPoint[i].X()*fY3;
        Real fY4 = fY2*fY2;

        kMat[0][0] += fX4;
        kMat[0][1] += fX3Y;
        kMat[0][2] += fX2Y2;
        kMat[0][3] += fX3;
        kMat[0][4] += fX2Y;
        kMat[0][5] += fX2;
        kMat[1][2] += fXY3;
        kMat[1][4] += fXY2;
        kMat[1][5] += fXY;
        kMat[2][2] += fY4;
        kMat[2][4] += fY3;
        kMat[2][5] += fY2;
        kMat[3][3] += fX2;
        kMat[3][5] += akPoint[i].X();
        kMat[4][5] += akPoint[i].Y();

        afRHS[0] += fZX2;
        afRHS[1] += fZXY;
        afRHS[2] += fZY2;
        afRHS[3] += fZX;
        afRHS[4] += fZY;
        afRHS[5] += akPoint[i].Z();
    }

    kMat[1][0] = kMat[0][1];
    kMat[1][1] = kMat[0][2];
    kMat[1][3] = kMat[0][4];
    kMat[2][0] = kMat[0][2];
    kMat[2][1] = kMat[1][2];
    kMat[2][3] = kMat[1][4];
    kMat[3][0] = kMat[0][3];
    kMat[3][1] = kMat[1][3];
    kMat[3][2] = kMat[2][3];
    kMat[3][4] = kMat[1][5];
    kMat[4][0] = kMat[0][4];
    kMat[4][1] = kMat[1][4];
    kMat[4][2] = kMat[2][4];
    kMat[4][3] = kMat[3][4];
    kMat[4][4] = kMat[2][5];
    kMat[5][0] = kMat[0][5];
    kMat[5][1] = kMat[1][5];
    kMat[5][2] = kMat[2][5];
    kMat[5][3] = kMat[3][5];
    kMat[5][4] = kMat[4][5];
    kMat[5][5] = (Real)iQuantity;

    return LinearSystem<Real>().Solve(kMat,afRHS,afCoeff);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
bool ParaboloidFit3<float> (int, const Vector3<float>*, float[6]);

template WM4_FOUNDATION_ITEM
bool ParaboloidFit3<double> (int, const Vector3<double>*, double[6]);
//----------------------------------------------------------------------------
}
