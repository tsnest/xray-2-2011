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
#include "Wm4IntrPlane3Ellipsoid3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Ellipsoid3<Real>::IntrPlane3Ellipsoid3 (const Plane3<Real>& rkPlane,
    const Ellipsoid3<Real>& rkEllipsoid)
    :
    m_pkPlane(&rkPlane),
    m_pkEllipsoid(&rkEllipsoid)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Ellipsoid3<Real>::GetPlane () const
{
    return *m_pkPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipsoid3<Real>& IntrPlane3Ellipsoid3<Real>::GetEllipsoid () const
{
    return *m_pkEllipsoid;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Ellipsoid3<Real>::Test ()
{
    Matrix3<Real> kMInverse;
    m_pkEllipsoid->GetMInverse(kMInverse);
    Real fDiscr = kMInverse.QForm(m_pkPlane->Normal,m_pkPlane->Normal);
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    Real fSDist = m_pkPlane->DistanceTo(m_pkEllipsoid->Center);
    return Math<Real>::FAbs(fSDist) <= fRoot;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Ellipsoid3<Real>::EllipsoidIsCulled () const
{
    Matrix3<Real> kMInverse;
    m_pkEllipsoid->GetMInverse(kMInverse);
    Real fDiscr = kMInverse.QForm(m_pkPlane->Normal,m_pkPlane->Normal);
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    Real fSDist = m_pkPlane->DistanceTo(m_pkEllipsoid->Center);
    return fSDist <= -fRoot;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrPlane3Ellipsoid3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
