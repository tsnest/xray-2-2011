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
#include "Wm4EllipsoidGeodesic.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
EllipsoidGeodesic<Real>::EllipsoidGeodesic (Real fXExtent, Real fYExtent,
    Real fZExtent)
    :
    RiemannianGeodesic<Real>(2)
{
    m_fXExtent = fXExtent;
    m_fYExtent = fYExtent;
    m_fZExtent = fZExtent;
}
//----------------------------------------------------------------------------
template <class Real>
EllipsoidGeodesic<Real>::~EllipsoidGeodesic ()
{
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> EllipsoidGeodesic<Real>::ComputePosition (
    const GVector<Real>& rkPoint)
{
    Real fCos0 = Math<Real>::Cos(rkPoint[0]);
    Real fSin0 = Math<Real>::Sin(rkPoint[0]);
    Real fCos1 = Math<Real>::Cos(rkPoint[1]);
    Real fSin1 = Math<Real>::Sin(rkPoint[1]);

    return Vector3<Real>(
        m_fXExtent*fCos0*fSin1,
        m_fYExtent*fSin0*fSin1,
        m_fZExtent*fCos1
    );
}
//----------------------------------------------------------------------------
template <class Real>
void EllipsoidGeodesic<Real>::ComputeMetric (const GVector<Real>& rkPoint)
{
    Real fCos0 = Math<Real>::Cos(rkPoint[0]);
    Real fSin0 = Math<Real>::Sin(rkPoint[0]);
    Real fCos1 = Math<Real>::Cos(rkPoint[1]);
    Real fSin1 = Math<Real>::Sin(rkPoint[1]);

    Vector3<Real> kDer0(
        -m_fXExtent*fSin0*fSin1,
        m_fYExtent*fCos0*fSin1,
        (Real)0.0);

    Vector3<Real> kDer1(
        m_fXExtent*fCos0*fCos1,
        m_fYExtent*fSin0*fCos1,
        -m_fZExtent*fSin1);

    m_kMetric[0][0] = kDer0.Dot(kDer0);
    m_kMetric[0][1] = kDer0.Dot(kDer1);
    m_kMetric[1][0] = m_kMetric[0][1];
    m_kMetric[1][1] = kDer1.Dot(kDer1);
}
//----------------------------------------------------------------------------
template <class Real>
void EllipsoidGeodesic<Real>::ComputeChristoffel1 (
    const GVector<Real>& rkPoint)
{
    Real fCos0 = Math<Real>::Cos(rkPoint[0]);
    Real fSin0 = Math<Real>::Sin(rkPoint[0]);
    Real fCos1 = Math<Real>::Cos(rkPoint[1]);
    Real fSin1 = Math<Real>::Sin(rkPoint[1]);

    Vector3<Real> kDer0(
        -m_fXExtent*fSin0*fSin1,
        m_fYExtent*fCos0*fSin1,
        (Real)0.0);

    Vector3<Real> kDer1(
        m_fXExtent*fCos0*fCos1,
        m_fYExtent*fSin0*fCos1,
        -m_fZExtent*fSin1);

    Vector3<Real> kDer00(
        -m_fXExtent*fCos0*fSin1,
        -m_fYExtent*fSin0*fSin1,
        (Real)0.0);

    Vector3<Real> kDer01(
        -m_fXExtent*fSin0*fCos1,
        m_fYExtent*fCos0*fCos1,
        (Real)0.0);

    Vector3<Real> kDer11(
        -m_fXExtent*fCos0*fSin1,
        -m_fYExtent*fSin0*fSin1,
        -m_fZExtent*fCos1);

    m_akChristoffel1[0][0][0] = kDer00.Dot(kDer0);
    m_akChristoffel1[0][0][1] = kDer01.Dot(kDer0);
    m_akChristoffel1[0][1][0] = m_akChristoffel1[0][0][1];
    m_akChristoffel1[0][1][1] = kDer11.Dot(kDer0);

    m_akChristoffel1[1][0][0] = kDer00.Dot(kDer1);
    m_akChristoffel1[1][0][1] = kDer01.Dot(kDer1);
    m_akChristoffel1[1][1][0] = m_akChristoffel1[1][0][1];
    m_akChristoffel1[1][1][1] = kDer11.Dot(kDer1);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class EllipsoidGeodesic<float>;

template WM4_FOUNDATION_ITEM
class EllipsoidGeodesic<double>;
}
//----------------------------------------------------------------------------
