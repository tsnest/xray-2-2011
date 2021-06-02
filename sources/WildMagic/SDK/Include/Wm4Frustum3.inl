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

//----------------------------------------------------------------------------
template <class Real>
Frustum3<Real>::Frustum3 ()
    :
    Origin(Vector3<Real>::ZERO),
    DVector(-Vector3<Real>::UNIT_Z),
    UVector(Vector3<Real>::UNIT_Y),
    RVector(Vector3<Real>::UNIT_X)
{
    DMin = (Real)1.0;
    DMax = (Real)2.0;
    UBound = (Real)1.0;
    RBound = (Real)1.0;

    Update();
}
//----------------------------------------------------------------------------
template <class Real>
Frustum3<Real>::Frustum3 (const Vector3<Real>& rkOrigin,
    const Vector3<Real>& rkDVector, const Vector3<Real>& rkUVector,
    const Vector3<Real>& rkRVector, Real fDMin, Real fDMax, Real fUBound,
    Real fRBound)
    :
    Origin(rkOrigin),
    DVector(rkDVector),
    UVector(rkUVector),
    RVector(rkRVector)
{
    DMin = fDMin;
    DMax = fDMax;
    UBound = fUBound;
    RBound = fRBound;

    Update();
}
//----------------------------------------------------------------------------
template <class Real>
void Frustum3<Real>::Update ()
{
    m_fDRatio = DMax/DMin;
    m_fMTwoUF = ((Real)-2.0)*UBound*DMax;
    m_fMTwoRF = ((Real)-2.0)*RBound*DMax;
}
//----------------------------------------------------------------------------
template <class Real>
Real Frustum3<Real>::GetDRatio () const
{
    return m_fDRatio;
}
//----------------------------------------------------------------------------
template <class Real>
Real Frustum3<Real>::GetMTwoUF () const
{
    return m_fMTwoUF;
}
//----------------------------------------------------------------------------
template <class Real>
Real Frustum3<Real>::GetMTwoRF () const
{
    return m_fMTwoRF;
}
//----------------------------------------------------------------------------
template <class Real>
void Frustum3<Real>::ComputeVertices (Vector3<Real> akVertex[8]) const
{
    Vector3<Real> kDScaled = DMin*DVector;
    Vector3<Real> kUScaled = UBound*UVector;
    Vector3<Real> kRScaled = RBound*RVector;

    akVertex[0] = kDScaled - kUScaled - kRScaled;
    akVertex[1] = kDScaled - kUScaled + kRScaled;
    akVertex[2] = kDScaled + kUScaled + kRScaled;
    akVertex[3] = kDScaled + kUScaled - kRScaled;

    for (int i = 0, ip = 4; i < 4; i++, ip++)
    {
        akVertex[ip] = Origin + m_fDRatio*akVertex[i];
        akVertex[i] += Origin;
    }
}
//----------------------------------------------------------------------------
