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
#include "Wm4IntrPlane3Lozenge3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Lozenge3<Real>::IntrPlane3Lozenge3 (const Plane3<Real>& rkPlane,
    const Lozenge3<Real>& rkLozenge)
    :
    m_pkPlane(&rkPlane),
    m_pkLozenge(&rkLozenge)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Lozenge3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrPlane3Lozenge3<Real>::GetLozenge () const
{
    return *m_pkLozenge;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Lozenge3<Real>::Test ()
{
    Real fSDistMM = m_pkPlane->DistanceTo(m_pkLozenge->Rectangle.GetMMCorner());
    Real fSDistPM = m_pkPlane->DistanceTo(m_pkLozenge->Rectangle.GetPMCorner());
    if (fSDistMM*fSDistPM <= (Real)0.0)
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    Real fSDistMP = m_pkPlane->DistanceTo(m_pkLozenge->Rectangle.GetMPCorner());
    if (fSDistMM*fSDistMP <= (Real)0.0)
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    Real fSDistPP = m_pkPlane->DistanceTo(m_pkLozenge->Rectangle.GetPPCorner());
    if (fSDistPM*fSDistPP <= (Real)0.0)
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    // The lozenge rectangle corners are all on one side of the plane.
    // The spheres centered at the corners, with radius that of the lozenge,
    // might intersect the plane.
    return Math<Real>::FAbs(fSDistMM) <= m_pkLozenge->Radius
        || Math<Real>::FAbs(fSDistPM) <= m_pkLozenge->Radius
        || Math<Real>::FAbs(fSDistMP) <= m_pkLozenge->Radius
        || Math<Real>::FAbs(fSDistPP) <= m_pkLozenge->Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Lozenge3<Real>::LozengeIsCulled () const
{
    Real fSDistMM = m_pkPlane->DistanceTo(m_pkLozenge->Rectangle.GetMMCorner());
    if (fSDistMM < (Real)0.0)
    {
        Real fSDistPM = m_pkPlane->DistanceTo(
            m_pkLozenge->Rectangle.GetPMCorner());
        if (fSDistPM < (Real)0.0)
        {
            Real fSDistMP = m_pkPlane->DistanceTo(
                m_pkLozenge->Rectangle.GetMPCorner());
            if (fSDistMP < (Real)0.0)
            {
                Real fSDistPP = m_pkPlane->DistanceTo(
                    m_pkLozenge->Rectangle.GetPPCorner());
                if (fSDistPP < (Real)0.0)
                {
                    // all four lozenge corners on negative side of plane
                    if (fSDistMM <= fSDistPM)
                    {
                        if (fSDistMM <= fSDistMP)
                        {
                            return fSDistMM <= -m_pkLozenge->Radius;
                        }
                        else
                        {
                            return fSDistMP <= -m_pkLozenge->Radius;
                        }
                    }
                    else
                    {
                        if (fSDistPM <= fSDistPP)
                        {
                            return fSDistPM <= -m_pkLozenge->Radius;
                        }
                        else
                        {
                            return fSDistPP <= -m_pkLozenge->Radius;
                        }
                    }
                }
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
class IntrPlane3Lozenge3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Lozenge3<double>;
//----------------------------------------------------------------------------
}
