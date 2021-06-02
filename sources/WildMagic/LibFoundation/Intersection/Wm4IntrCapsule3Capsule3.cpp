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
#include "Wm4IntrCapsule3Capsule3.h"
#include "Wm4DistSegment3Segment3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrCapsule3Capsule3<Real>::IntrCapsule3Capsule3 (
    const Capsule3<Real>& rkCapsule0, const Capsule3<Real>& rkCapsule1)
    :
    m_pkCapsule0(&rkCapsule0),
    m_pkCapsule1(&rkCapsule1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrCapsule3Capsule3<Real>::GetCapsule0 () const
{
    return *m_pkCapsule0;
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrCapsule3Capsule3<Real>::GetCapsule1 () const
{
    return *m_pkCapsule1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrCapsule3Capsule3<Real>::Test ()
{
    Real fDistance = DistSegment3Segment3<Real>(m_pkCapsule0->Segment,
        m_pkCapsule1->Segment).Get();
    Real fRSum = m_pkCapsule0->Radius + m_pkCapsule1->Radius;
    return fDistance <= fRSum;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrCapsule3Capsule3<float>;

template WM4_FOUNDATION_ITEM
class IntrCapsule3Capsule3<double>;
//----------------------------------------------------------------------------
}
