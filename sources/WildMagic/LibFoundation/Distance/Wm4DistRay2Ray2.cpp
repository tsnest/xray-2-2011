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
// Version: 4.0.1 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4DistRay2Ray2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistRay2Ray2<Real>::DistRay2Ray2 (const Ray2<Real>& rkRay0,
    const Ray2<Real>& rkRay1)
    :
    m_pkRay0(&rkRay0),
    m_pkRay1(&rkRay1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray2<Real>& DistRay2Ray2<Real>::GetRay0 () const
{
    return *m_pkRay0;
}
//----------------------------------------------------------------------------
template <class Real>
const Ray2<Real>& DistRay2Ray2<Real>::GetRay1 () const
{
    return *m_pkRay1;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Ray2<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Ray2<Real>::GetSquared ()
{
    Vector2<Real> kDiff = m_pkRay0->Origin - m_pkRay1->Origin;
    Real fA01 = -m_pkRay0->Direction.Dot(m_pkRay1->Direction);
    Real fB0 = kDiff.Dot(m_pkRay0->Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist;

    if (fDet >= Math<Real>::ZERO_TOLERANCE)
    {
        // rays are not parallel
        fB1 = -kDiff.Dot(m_pkRay1->Direction);
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;

        if (fS0 >= (Real)0.0)
        {
            if (fS1 >= (Real)0.0)  // region 0 (interior)
            {
                // minimum at two interior points of rays
                Real fInvDet = ((Real)1.0)/fDet;
                fS0 *= fInvDet;
                fS1 *= fInvDet;
                fSqrDist = (Real)0.0;
            }
            else  // region 3 (side)
            {
                fS1 = (Real)0.0;
                if (fB0 >= (Real)0.0)
                {
                    fS0 = (Real)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fS0 = -fB0;
                    fSqrDist = fB0*fS0+fC;
                }
            }
        }
        else
        {
            if (fS1 >= (Real)0.0)  // region 1 (side)
            {
                fS0 = (Real)0.0;
                if (fB1 >= (Real)0.0)
                {
                    fS1 = (Real)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fS1 = -fB1;
                    fSqrDist = fB1*fS1+fC;
                }
            }
            else  // region 2 (corner)
            {
                if (fB0 < (Real)0.0)
                {
                    fS0 = -fB0;
                    fS1 = (Real)0.0;
                    fSqrDist = fB0*fS0+fC;
                }
                else
                {
                    fS0 = (Real)0.0;
                    if (fB1 >= (Real)0.0)
                    {
                        fS1 = (Real)0.0;
                        fSqrDist = fC;
                    }
                    else
                    {
                        fS1 = -fB1;
                        fSqrDist = fB1*fS1+fC;
                    }
                }
            }
        }
    }
    else
    {
        // rays are parallel
        if (fA01 > (Real)0.0)
        {
            // opposite direction vectors
            fS1 = (Real)0.0;
            if (fB0 >= (Real)0.0)
            {
                fS0 = (Real)0.0;
                fSqrDist = fC;
            }
            else
            {
                fS0 = -fB0;
                fSqrDist = fB0*fS0+fC;
            }
        }
        else
        {
            // same direction vectors
            if (fB0 >= (Real)0.0)
            {
                fB1 = -kDiff.Dot(m_pkRay1->Direction);
                fS0 = (Real)0.0;
                fS1 = -fB1;
                fSqrDist = fB1*fS1+fC;
            }
            else
            {
                fS0 = -fB0;
                fS1 = (Real)0.0;
                fSqrDist = fB0*fS0+fC;
            }
        }
    }

    m_kClosestPoint0 = m_pkRay0->Origin + fS0*m_pkRay0->Direction;
    m_kClosestPoint1 = m_pkRay1->Origin + fS1*m_pkRay1->Direction;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Ray2<Real>::Get (Real fT, const Vector2<Real>& rkVelocity0,
    const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMOrigin0 = m_pkRay0->Origin + fT*rkVelocity0;
    Vector2<Real> kMOrigin1 = m_pkRay1->Origin + fT*rkVelocity1;
    Ray2<Real> kMRay0(kMOrigin0,m_pkRay0->Direction);
    Ray2<Real> kMRay1(kMOrigin1,m_pkRay1->Direction);
    return DistRay2Ray2<Real>(kMRay0,kMRay1).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistRay2Ray2<Real>::GetSquared (Real fT,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    Vector2<Real> kMOrigin0 = m_pkRay0->Origin + fT*rkVelocity0;
    Vector2<Real> kMOrigin1 = m_pkRay1->Origin + fT*rkVelocity1;
    Ray2<Real> kMRay0(kMOrigin0,m_pkRay0->Direction);
    Ray2<Real> kMRay1(kMOrigin1,m_pkRay1->Direction);
    return DistRay2Ray2<Real>(kMRay0,kMRay1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistRay2Ray2<float>;

template WM4_FOUNDATION_ITEM
class DistRay2Ray2<double>;
//----------------------------------------------------------------------------
}
