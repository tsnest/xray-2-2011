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
#include "Wm4HullEdge2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
HullEdge2<Real>::HullEdge2 (int iV0, int iV1)
{
    V[0] = iV0;
    V[1] = iV1;
    A[0] = 0;
    A[1] = 0;
    Sign = 0;
    Time = -1;
}
//----------------------------------------------------------------------------
template <class Real>
int HullEdge2<Real>::GetSign (int i, const Query2<Real>* pkQuery)
{
    if (i != Time)
    {
        Time = i;
        Sign = pkQuery->ToLine(i,V[0],V[1]);
    }

    return Sign;
}
//----------------------------------------------------------------------------
template <class Real>
void HullEdge2<Real>::Insert (HullEdge2<Real>* pkAdj0,
    HullEdge2<Real>* pkAdj1)
{
    pkAdj0->A[1] = this;
    pkAdj1->A[0] = this;
    A[0] = pkAdj0;
    A[1] = pkAdj1;
}
//----------------------------------------------------------------------------
template <class Real>
void HullEdge2<Real>::DeleteSelf ()
{
    if (A[0])
    {
        A[0]->A[1] = 0;
    }

    if (A[1])
    {
        A[1]->A[0] = 0;
    }

    WM4_DELETE this;
}
//----------------------------------------------------------------------------
template <class Real>
void HullEdge2<Real>::DeleteAll ()
{
    HullEdge2<Real>* pkAdj = A[1];
    while (pkAdj && pkAdj != this)
    {
        HullEdge2<Real>* pkSave = pkAdj->A[1];
        WM4_DELETE pkAdj;
        pkAdj = pkSave;
    }

    assert(pkAdj == this);
    WM4_DELETE this;
}
//----------------------------------------------------------------------------
template <class Real>
void HullEdge2<Real>::GetIndices (int& riHQuantity, int*& raiHIndex)
{
    // Count the number of edge vertices and allocate the index array.
    riHQuantity = 0;
    HullEdge2<Real>* pkCurrent = this;
    do
    {
        riHQuantity++;
        pkCurrent = pkCurrent->A[1];
    }
    while (pkCurrent != this);
    raiHIndex = WM4_NEW int[riHQuantity];

    // Fill the index array.
    riHQuantity = 0;
    pkCurrent = this;
    do
    {
        raiHIndex[riHQuantity++] = pkCurrent->V[0];
        pkCurrent = pkCurrent->A[1];
    }
    while (pkCurrent != this);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class HullEdge2<float>;

template WM4_FOUNDATION_ITEM
class HullEdge2<double>;
//----------------------------------------------------------------------------
}
