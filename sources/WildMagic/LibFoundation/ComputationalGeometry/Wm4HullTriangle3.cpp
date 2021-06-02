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
#include "Wm4HullTriangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
HullTriangle3<Real>::HullTriangle3 (int iV0, int iV1, int iV2)
{
    V[0] = iV0;
    V[1] = iV1;
    V[2] = iV2;
    A[0] = 0;
    A[1] = 0;
    A[2] = 0;
    Sign = 0;
    Time = -1;
    OnStack = false;
}
//----------------------------------------------------------------------------
template <class Real>
int HullTriangle3<Real>::GetSign (int i, const Query3<Real>* pkQuery)
{
    if (i != Time)
    {
        Time = i;
        Sign = pkQuery->ToPlane(i,V[0],V[1],V[2]);
    }

    return Sign;
}
//----------------------------------------------------------------------------
template <class Real>
void HullTriangle3<Real>::AttachTo (HullTriangle3<Real>* pkAdj0,
    HullTriangle3<Real>* pkAdj1, HullTriangle3<Real>* pkAdj2)
{
    // assert:  The input adjacent triangles are correctly ordered.
    A[0] = pkAdj0;
    A[1] = pkAdj1;
    A[2] = pkAdj2;
}
//----------------------------------------------------------------------------
template <class Real>
int HullTriangle3<Real>::DetachFrom (int iAdj, HullTriangle3<Real>* pkAdj)
{
    assert(0 <= iAdj && iAdj < 3 && A[iAdj] == pkAdj);
    A[iAdj] = 0;
    for (int i = 0; i < 3; i++)
    {
        if (pkAdj->A[i] == this)
        {
            pkAdj->A[i] = 0;
            return i;
        }
    }
    return -1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class HullTriangle3<float>;

template WM4_FOUNDATION_ITEM
class HullTriangle3<double>;
//----------------------------------------------------------------------------
}
