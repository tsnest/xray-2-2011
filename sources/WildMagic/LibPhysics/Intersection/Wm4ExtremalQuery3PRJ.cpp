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

#include "Wm4PhysicsPCH.h"
#include "Wm4ExtremalQuery3PRJ.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
ExtremalQuery3PRJ<Real>::ExtremalQuery3PRJ (
    const ConvexPolyhedron3<Real>& rkPolytope)
    :
    ExtremalQuery3<Real>(rkPolytope)
{
    m_kCentroid = m_pkPolytope->ComputeVertexAverage();
}
//----------------------------------------------------------------------------
template <class Real>
ExtremalQuery3PRJ<Real>::~ExtremalQuery3PRJ ()
{
}
//----------------------------------------------------------------------------
template <class Real>
void ExtremalQuery3PRJ<Real>::GetExtremeVertices (
    const Vector3<Real>& rkDirection, int& riPositiveDirection,
    int& riNegativeDirection)
{
    Vector3<Real> kDiff = m_pkPolytope->GetVertex(0) - m_kCentroid;
    Real fMin = rkDirection.Dot(kDiff), fMax = fMin;
    riNegativeDirection = 0;
    riPositiveDirection = 0;

    for (int i = 1; i < m_pkPolytope->GetVQuantity(); i++)
    {
        kDiff = m_pkPolytope->GetVertex(i) - m_kCentroid;
        Real fDot = rkDirection.Dot(kDiff);
        if (fDot < fMin)
        {
            riNegativeDirection = i;
            fMin = fDot;
        }
        else if (fDot > fMax)
        {
            riPositiveDirection = i;
            fMax = fDot;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class ExtremalQuery3PRJ<float>;

template WM4_PHYSICS_ITEM
class ExtremalQuery3PRJ<double>;
//----------------------------------------------------------------------------
}
