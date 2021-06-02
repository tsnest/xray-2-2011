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
// Version: 4.0.2 (2008/08/17)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrPlane3Cylinder3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Cylinder3<Real>::IntrPlane3Cylinder3 (const Plane3<Real>& rkPlane,
    const Cylinder3<Real>& rkCylinder)
    :
    m_pkPlane(&rkPlane),
    m_pkCylinder(&rkCylinder)
{
    m_iType = PC_EMPTY_SET;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Cylinder3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Cylinder3<Real>& IntrPlane3Cylinder3<Real>::GetCylinder () const
{
    return *m_pkCylinder;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Cylinder3<Real>::Test ()
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = m_pkPlane->DistanceTo(m_pkCylinder->Segment.Origin);
    Real fAbsNdW = Math<Real>::FAbs(m_pkPlane->Normal.Dot(
        m_pkCylinder->Segment.Direction));
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1.0
        - fAbsNdW*fAbsNdW));
    Real fTerm = m_pkCylinder->Radius*fRoot +
        ((Real)0.5)*m_pkCylinder->Height*fAbsNdW;

    // intersection occurs if and only if 0 is in the interval [min,max]
    return Math<Real>::FAbs(fSDist) <= fTerm;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Cylinder3<Real>::Find ()
{
    Real fSDist = m_pkPlane->DistanceTo(m_pkCylinder->Segment.Origin);
    Vector3<Real> kCenter =  m_pkCylinder->Segment.Origin -
        fSDist*m_pkPlane->Normal;
    Real fCosTheta = m_pkCylinder->Segment.Direction.Dot(m_pkPlane->Normal);
    Real fAbsCosTheta = Math<Real>::FAbs(fCosTheta);

    if (fAbsCosTheta > (Real)0)
    {
        // The cylinder axis intersects the plane in a unique point.
        if (fAbsCosTheta < (Real)1)
        {
            m_iType = PC_ELLIPSE;
            m_kEllipse.Normal = m_pkPlane->Normal;
            m_kEllipse.Center = kCenter -
                (fSDist/fCosTheta)*m_pkCylinder->Segment.Direction;
            m_kEllipse.Major = m_pkCylinder->Segment.Direction -
                fCosTheta*m_pkPlane->Normal;
            m_kEllipse.Minor = m_pkPlane->Normal.Cross(m_kEllipse.Major);
            m_kEllipse.MajorLength = m_pkCylinder->Radius/fAbsCosTheta;
            m_kEllipse.MinorLength = m_pkCylinder->Radius;
            m_kEllipse.Major.Normalize();
            m_kEllipse.Minor.Normalize();
            return true;
        }
        else
        {
            m_iType = PC_CIRCLE;
            m_kCircle.N = m_pkPlane->Normal;
            m_kCircle.Center = kCenter;
            m_kCircle.Radius = m_pkCylinder->Radius;
            return true;
        }
    }
    else
    {
        // The cylinder is parallel to the plane.
        Real fAbsSDist = Math<Real>::FAbs(fSDist);
        if (fAbsSDist < m_pkCylinder->Radius)
        {
            m_iType = PC_TWO_LINES;

            Vector3<Real> kOffset = m_pkCylinder->Segment.Direction.Cross(
                m_pkPlane->Normal);
            Real fExtent = Math<Real>::Sqrt(
                m_pkCylinder->Radius*m_pkCylinder->Radius - fSDist*fSDist);

            m_kLine0.Origin = kCenter - fExtent*kOffset;
            m_kLine0.Direction = m_pkCylinder->Segment.Direction;
            m_kLine1.Origin = kCenter + fExtent*kOffset;
            m_kLine1.Direction = m_pkCylinder->Segment.Direction;
            return true;
        }
        else if (fAbsSDist == m_pkCylinder->Radius)
        {
            m_iType = PC_ONE_LINE;
            m_kLine0.Origin = kCenter;
            m_kLine0.Direction = m_pkCylinder->Segment.Direction;
            return true;
        }
        else
        {
            m_iType = PC_EMPTY_SET;
            return false;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Cylinder3<Real>::CylinderIsCulled () const
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = m_pkPlane->DistanceTo(m_pkCylinder->Segment.Origin);
    Real fAbsNdW = Math<Real>::FAbs(m_pkPlane->Normal.Dot(
        m_pkCylinder->Segment.Direction));
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1.0
        - fAbsNdW*fAbsNdW));
    Real fTerm = m_pkCylinder->Radius*fRoot +
        ((Real)0.5)*m_pkCylinder->Height*fAbsNdW;

    // culling occurs if and only if max <= 0
    return fSDist + fTerm <= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrPlane3Cylinder3<Real>::GetType () const
{
    return m_iType;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrPlane3Cylinder3<Real>::GetOneLine (Line3<Real>& rkLine) const
{
    rkLine = m_kLine0;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrPlane3Cylinder3<Real>::GetTwoLines (Line3<Real>& rkLine0,
    Line3<Real>& rkLine1) const
{
    rkLine0 = m_kLine0;
    rkLine1 = m_kLine1;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrPlane3Cylinder3<Real>::GetCircle (Circle3<Real>& rkCircle) const
{
    rkCircle = m_kCircle;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrPlane3Cylinder3<Real>::GetEllipse (Ellipse3<Real>& rkEllipse) const
{
    rkEllipse = m_kEllipse;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrPlane3Cylinder3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Cylinder3<double>;
//----------------------------------------------------------------------------
}
