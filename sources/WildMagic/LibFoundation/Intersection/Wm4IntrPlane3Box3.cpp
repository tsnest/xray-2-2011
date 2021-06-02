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
#include "Wm4IntrPlane3Box3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Box3<Real>::IntrPlane3Box3 (const Plane3<Real>& rkPlane,
    const Box3<Real>& rkBox)
    :
    m_pkPlane(&rkPlane),
    m_pkBox(&rkBox)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Box3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrPlane3Box3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Box3<Real>::Test ()
{
    Real afTmp[3] =
    {
        m_pkBox->Extent[0]*(m_pkPlane->Normal.Dot(m_pkBox->Axis[0])),
        m_pkBox->Extent[1]*(m_pkPlane->Normal.Dot(m_pkBox->Axis[1])),
        m_pkBox->Extent[2]*(m_pkPlane->Normal.Dot(m_pkBox->Axis[2]))
    };

    Real fRadius = Math<Real>::FAbs(afTmp[0]) + Math<Real>::FAbs(afTmp[1]) +
        Math<Real>::FAbs(afTmp[2]);

    Real fSignedDistance = m_pkPlane->DistanceTo(m_pkBox->Center);
    return Math<Real>::FAbs(fSignedDistance) <= fRadius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Box3<Real>::BoxIsCulled () const
{
    Real afTmp[3] =
    {
        m_pkBox->Extent[0]*(m_pkPlane->Normal.Dot(m_pkBox->Axis[0])),
        m_pkBox->Extent[1]*(m_pkPlane->Normal.Dot(m_pkBox->Axis[1])),
        m_pkBox->Extent[2]*(m_pkPlane->Normal.Dot(m_pkBox->Axis[2]))
    };

    Real fRadius = Math<Real>::FAbs(afTmp[0]) + Math<Real>::FAbs(afTmp[1]) +
        Math<Real>::FAbs(afTmp[2]);

    Real fSignedDistance = m_pkPlane->DistanceTo(m_pkBox->Center);
    return fSignedDistance <= -fRadius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrPlane3Box3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Box3<double>;
//----------------------------------------------------------------------------
}
