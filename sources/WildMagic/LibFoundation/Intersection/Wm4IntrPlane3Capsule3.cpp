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
#include "Wm4IntrPlane3Capsule3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Capsule3<Real>::IntrPlane3Capsule3 (const Plane3<Real>& rkPlane,
    const Capsule3<Real>& rkCapsule)
    :
    m_pkPlane(&rkPlane),
    m_pkCapsule(&rkCapsule)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Capsule3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrPlane3Capsule3<Real>::GetCapsule () const
{
    return *m_pkCapsule;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Capsule3<Real>::Test ()
{
    Real fPDist = m_pkPlane->DistanceTo(m_pkCapsule->Segment.GetPosEnd());
    Real fNDist = m_pkPlane->DistanceTo(m_pkCapsule->Segment.GetNegEnd());
    if (fPDist*fNDist <= (Real)0.0)
    {
        // capsule segment end points on opposite sides of the plane
        return true;
    }

    // End points on same side of plane, but the end point spheres (with
    // radius of the capsule) might intersect the plane.
    return Math<Real>::FAbs(fPDist) <= m_pkCapsule->Radius
        || Math<Real>::FAbs(fNDist) <= m_pkCapsule->Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Capsule3<Real>::CapsuleIsCulled () const
{
    Real fPDist = m_pkPlane->DistanceTo(m_pkCapsule->Segment.GetPosEnd());
    if (fPDist < (Real)0.0)
    {
        Real fNDist = m_pkPlane->DistanceTo(m_pkCapsule->Segment.GetNegEnd());
        if (fNDist < (Real)0.0)
        {
            if (fPDist <= fNDist)
            {
                return fPDist <= -m_pkCapsule->Radius;
            }
            else
            {
                return fNDist <= -m_pkCapsule->Radius;
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrPlane3Capsule3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Capsule3<double>;
//----------------------------------------------------------------------------
}
