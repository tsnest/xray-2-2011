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
// Version: 4.0.0 (2006/09/06)

//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real>::RealVBase ()
{
    // The data is uninitialized for performance in array construction.
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real>::RealVBase (Real fV0, Real fV1, Real fV2, Real fV3)
{
    m_afTuple[0] = fV0;
    m_afTuple[1] = fV1;
    m_afTuple[2] = fV2;
    m_afTuple[3] = fV3;
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real>::RealVBase (const RealVBase& rkVector)
{
    m_afTuple[0] = rkVector.m_afTuple[0];
    m_afTuple[1] = rkVector.m_afTuple[1];
    m_afTuple[2] = rkVector.m_afTuple[2];
    m_afTuple[3] = rkVector.m_afTuple[3];
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real>::~RealVBase ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real>& RealVBase<Real>::operator= (const RealVBase& rkVector)
{
    m_afTuple[0] = rkVector.m_afTuple[0];
    m_afTuple[1] = rkVector.m_afTuple[1];
    m_afTuple[2] = rkVector.m_afTuple[2];
    m_afTuple[3] = rkVector.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real>::operator const Real* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real>::operator Real* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <typename Real>
Real RealVBase<Real>::operator[] (int i) const
{
    // Assert:  0 <= i < N, where N in {1,2,3,4} depending on which derived
    // class object is making the call.
    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real& RealVBase<Real>::operator[] (int i)
{
    // Assert:  0 <= i < N, where N in {1,2,3,4} depending on which derived
    // class object is making the call.
    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void RealVBase<Real>::Set (int i0, const RealVBase& rkVector)
{
    // Assert:  0 <= i0 < N, where N in {1,2,3,4} depending on which derived
    // class object is making the call.
    m_afTuple[i0] = rkVector[0];
}
//----------------------------------------------------------------------------
template <typename Real>
void RealVBase<Real>::Set (int i0, int i1, const RealVBase& rkVector)
{
    // Assert:  0 <= i0 < N, 0 <= i1 < N, where N in {2,3,4} depending on
    // which derived class object is making the call.  Also, i0 and i1 must
    // be distinct indices.
    m_afTuple[i0] = rkVector[0];
    m_afTuple[i1] = rkVector[1];
}
//----------------------------------------------------------------------------
template <typename Real>
void RealVBase<Real>::Set (int i0, int i1, int i2, const RealVBase& rkVector)
{
    // Assert:  0 <= i0 < N, 0 <= i1 < N, 0 <= i2 < N, where N in {3,4}
    // depending on which derived class object is making the call.  Also,
    // i0, i1, and i2 must be distinct indices.
    m_afTuple[i0] = rkVector[0];
    m_afTuple[i1] = rkVector[1];
    m_afTuple[i2] = rkVector[2];
}
//----------------------------------------------------------------------------
template <typename Real>
void RealVBase<Real>::Set (int i0, int i1, int i2, int i3,
    const RealVBase& rkVector)
{
    // Assert:  0 <= i0 < 4, 0 <= i1 < 4, 0 <= i2 < 4, 0 <= i3 < 4.  Also,
    // i0, i1, i2, and i3 must be distinct indices.
    m_afTuple[i0] = rkVector[0];
    m_afTuple[i1] = rkVector[1];
    m_afTuple[i2] = rkVector[2];
    m_afTuple[i3] = rkVector[3];
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real> RealVBase<Real>::Get (int i0) const
{
    // Assert:  0 <= i0 < N, where N in {1,2,3,4} depending on which derived
    // class object is making the call.
    RealVBase<Real> kResult;
    kResult.m_afTuple[0] = m_afTuple[i0];
    kResult.m_afTuple[1] = (Real)0.0;
    kResult.m_afTuple[2] = (Real)0.0;
    kResult.m_afTuple[3] = (Real)0.0;
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real> RealVBase<Real>::Get (int i0, int i1) const
{
    // Assert:  0 <= i0 < N, 0 <= i1 < N, where N in {2,3,4} depending on
    // which derived class object is making the call.  The indices i0 and i1
    // are not required to be distinct.
    RealVBase<Real> kResult;
    kResult.m_afTuple[0] = m_afTuple[i0];
    kResult.m_afTuple[1] = m_afTuple[i1];
    kResult.m_afTuple[2] = (Real)0.0;
    kResult.m_afTuple[3] = (Real)0.0;
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real> RealVBase<Real>::Get (int i0, int i1, int i2) const
{
    // Assert:  0 <= i0 < N, 0 <= i1 < N, 0 <= i2 < N, where N in {3,4}
    // depending on which derived class object is making the call.  The
    // indices i0, i1, and i2 are not required to be distinct.
    RealVBase<Real> kResult;
    kResult.m_afTuple[0] = m_afTuple[i0];
    kResult.m_afTuple[1] = m_afTuple[i1];
    kResult.m_afTuple[2] = m_afTuple[i2];
    kResult.m_afTuple[3] = (Real)0.0;
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real>
RealVBase<Real> RealVBase<Real>::Get (int i0, int i1, int i2, int i3) const
{
    // Assert:  0 <= i0 < 4, 0 <= i1 < 4, 0 <= i2 < 4, 0 <= i3 < 4.  The
    // indices i0, i1, i2, and i3 are not required to be distinct.
    RealVBase<Real> kResult;
    kResult.m_afTuple[0] = m_afTuple[i0];
    kResult.m_afTuple[1] = m_afTuple[i1];
    kResult.m_afTuple[2] = m_afTuple[i2];
    kResult.m_afTuple[3] = m_afTuple[i3];
    return kResult;
}
//----------------------------------------------------------------------------
