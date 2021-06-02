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
#include "Wm4ContPointInPolygon2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
PointInPolygon2<Real>::PointInPolygon2 (int iQuantity,
    const Vector2<Real>* akVertex)
{
    m_iQuantity = iQuantity;
    m_akVertex = akVertex;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolygon2<Real>::Contains (const Vector2<Real>& rkP) const
{
    bool bInside = false;
    for (int i = 0, j = m_iQuantity-1; i < m_iQuantity; j = i++)
    {
        const Vector2<Real>& rkU0 = m_akVertex[i];
        const Vector2<Real>& rkU1 = m_akVertex[j];
        Real fRHS, fLHS;

        if (rkP.Y() < rkU1.Y())  // U1 above ray
        {
            if (rkU0.Y() <= rkP.Y())  // U0 on or below ray
            {
                fLHS = (rkP.Y()-rkU0.Y())*(rkU1.X()-rkU0.X());
                fRHS = (rkP.X()-rkU0.X())*(rkU1.Y()-rkU0.Y());
                if (fLHS > fRHS)
                {
                    bInside = !bInside;
                }
            }
        }
        else if (rkP.Y() < rkU0.Y())  // U1 on or below ray, U0 above ray
        {
            fLHS = (rkP.Y()-rkU0.Y())*(rkU1.X()-rkU0.X());
            fRHS = (rkP.X()-rkU0.X())*(rkU1.Y()-rkU0.Y());
            if (fLHS < fRHS)
            {
                bInside = !bInside;
            }
        }
    }
    return bInside;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolygon2<Real>::ContainsConvexOrderN (const Vector2<Real>& rkP)
    const
{
    for (int i1 = 0, i0 = m_iQuantity-1; i1 < m_iQuantity; i0 = i1++)
    {
        Real fNx = m_akVertex[i1].Y() - m_akVertex[i0].Y();
        Real fNy = m_akVertex[i0].X() - m_akVertex[i1].X();
        Real fDx = rkP.X() - m_akVertex[i0].X();
        Real fDy = rkP.Y() - m_akVertex[i0].Y();
        if (fNx*fDx + fNy*fDy > (Real)0.0)
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolygon2<Real>::ContainsConvexOrderLogN (const Vector2<Real>& rkP)
    const
{
    return SubContainsPoint(rkP,0,0);
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolygon2<Real>::ContainsQuadrilateral (
    const Vector2<Real>& rkP) const
{
    if (m_iQuantity != 4)
        return false;

    Real fNx = m_akVertex[2].Y() - m_akVertex[0].Y();
    Real fNy = m_akVertex[0].X() - m_akVertex[2].X();
    Real fDx = rkP.X() - m_akVertex[0].X();
    Real fDy = rkP.Y() - m_akVertex[0].Y();

    if (fNx*fDx + fNy*fDy > (Real)0.0)
    {
        // P potentially in <V0,V1,V2>
        fNx = m_akVertex[1].Y() - m_akVertex[0].Y();
        fNy = m_akVertex[0].X() - m_akVertex[1].X();
        if (fNx*fDx + fNy*fDy > (Real)0.0)
        {
            return false;
        }

        fNx = m_akVertex[2].Y() - m_akVertex[1].Y();
        fNy = m_akVertex[1].X() - m_akVertex[2].X();
        fDx = rkP.X() - m_akVertex[1].X();
        fDy = rkP.Y() - m_akVertex[1].Y();
        if (fNx*fDx + fNy*fDy > (Real)0.0)
        {
            return false;
        }
    }
    else
    {
        // P potentially in <V0,V2,V3>
        fNx = m_akVertex[0].Y() - m_akVertex[3].Y();
        fNy = m_akVertex[3].X() - m_akVertex[0].X();
        if (fNx*fDx + fNy*fDy > (Real)0.0)
        {
            return false;
        }

        fNx = m_akVertex[3].Y() - m_akVertex[2].Y();
        fNy = m_akVertex[2].X() - m_akVertex[3].X();
        fDx = rkP.X() - m_akVertex[3].X();
        fDy = rkP.Y() - m_akVertex[3].Y();
        if (fNx*fDx + fNy*fDy > (Real)0.0)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolygon2<Real>::SubContainsPoint (const Vector2<Real>& rkP,
    int i0, int i1) const
{
    Real fNx, fNy, fDx, fDy;

    int iDiff = i1 - i0;
    if (iDiff == 1 || (iDiff < 0 && iDiff+m_iQuantity == 1))
    {
        fNx = m_akVertex[i1].Y() - m_akVertex[i0].Y();
        fNy = m_akVertex[i0].X() - m_akVertex[i1].X();
        fDx = rkP.X() - m_akVertex[i0].X();
        fDy = rkP.Y() - m_akVertex[i0].Y();
        return fNx*fDx + fNy*fDy <= (Real)0.0;
    }

    // bisect the index range
    int iMid;
    if (i0 < i1)
    {
        iMid = (i0 + i1) >> 1;
    }
    else
    {
        iMid = ((i0 + i1 + m_iQuantity) >> 1);
        if (iMid >= m_iQuantity)
        {
            iMid -= m_iQuantity;
        }
    }

    // determine which side of the splitting line contains the point
    fNx = m_akVertex[iMid].Y() - m_akVertex[i0].Y();
    fNy = m_akVertex[i0].X() - m_akVertex[iMid].X();
    fDx = rkP.X() - m_akVertex[i0].X();
    fDy = rkP.Y() - m_akVertex[i0].Y();
    if (fNx*fDx + fNy*fDy > (Real)0.0)
    {
        // P potentially in <V(i0),V(i0+1),...,V(mid-1),V(mid)>
        return SubContainsPoint(rkP,i0,iMid);
    }
    else
    {
        // P potentially in <V(mid),V(mid+1),...,V(i1-1),V(i1)>
        return SubContainsPoint(rkP,iMid,i1);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class PointInPolygon2<float>;

template WM4_FOUNDATION_ITEM
class PointInPolygon2<double>;
//----------------------------------------------------------------------------
}
