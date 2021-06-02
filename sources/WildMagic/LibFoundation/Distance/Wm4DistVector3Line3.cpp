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
#include "Wm4DistVector3Line3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Line3<Real>::DistVector3Line3 (const Vector3<Real>& rkVector,
    const Line3<Real>& rkLine)
    :
    m_pkVector(&rkVector),
    m_pkLine(&rkLine)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Line3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistVector3Line3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Line3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Line3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = *m_pkVector - m_pkLine->Origin;
    m_fLineParameter = m_pkLine->Direction.Dot(kDiff);
    m_kClosestPoint0 = *m_pkVector;
    m_kClosestPoint1 = m_pkLine->Origin +
        m_fLineParameter*m_pkLine->Direction;
    kDiff = m_kClosestPoint1 - m_kClosestPoint0;
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Line3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkLine->Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_pkLine->Direction);
    return DistVector3Line3<Real>(kMVector,kMLine).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Line3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkLine->Origin + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_pkLine->Direction);
    return DistVector3Line3<Real>(kMVector,kMLine).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Line3<Real>::GetLineParameter () const
{
    return m_fLineParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Line3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Line3<double>;
//----------------------------------------------------------------------------
}
