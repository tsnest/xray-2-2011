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
#include "Wm4DistVector3Tetrahedron3.h"
#include "Wm4DistVector3Triangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Tetrahedron3<Real>::DistVector3Tetrahedron3 (
    const Vector3<Real>& rkVector, const Tetrahedron3<Real>& rkTetrahedron)
    :
    m_pkVector(&rkVector),
    m_pkTetrahedron(&rkTetrahedron)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Tetrahedron3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Tetrahedron3<Real>& DistVector3Tetrahedron3<Real>::GetTetrahedron ()
    const
{
    return *m_pkTetrahedron;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Tetrahedron3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Tetrahedron3<Real>::GetSquared ()
{
    // Construct the planes for the faces of the tetrahedron.  The normals
    // are outer pointing, but specified not to be unit length.  We only need
    // to know sidedness of the query point, so we will save cycles by not
    // computing unit-length normals.
    Plane3<Real> akPlane[4];
    m_pkTetrahedron->GetPlanes(akPlane,false);

    // Determine which faces are visible to the query point.  Only these
    // need to be processed by point-to-triangle distance queries.
    Real fMinSqrDistance = Math<Real>::MAX_REAL;
    Vector3<Real> kMinTetraClosest = Vector3<Real>::ZERO;
    for (int i = 0; i < 4; i++)
    {
        if (akPlane[i].WhichSide(*m_pkVector) >= 0)
        {
            int aiIndex[3] = { 0, 0, 0 };
            m_pkTetrahedron->GetFaceIndices(i,aiIndex);
            Triangle3<Real> kTri(
                m_pkTetrahedron->V[aiIndex[0]],
                m_pkTetrahedron->V[aiIndex[1]],
                m_pkTetrahedron->V[aiIndex[2]]);

            DistVector3Triangle3<Real> kDQuery(*m_pkVector,kTri);
            Real fSqrDistance = kDQuery.GetSquared();
            if (fSqrDistance < fMinSqrDistance)
            {
                fMinSqrDistance = fSqrDistance;
                kMinTetraClosest = kDQuery.GetClosestPoint1();
            }
        }
    }

    m_kClosestPoint0 = *m_pkVector;
    if (fMinSqrDistance != Math<Real>::MAX_REAL)
    {
        // The query point is outside the "solid" tetrahedron.
        m_kClosestPoint1 = kMinTetraClosest;
    }
    else
    {
        // The query point is inside the "solid" tetrahedron.  Report a zero
        // distance.  The closest points are identical.
        fMinSqrDistance = (Real)0.0;
        m_kClosestPoint1 = *m_pkVector;
    }

    return fMinSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Tetrahedron3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_pkTetrahedron->V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_pkTetrahedron->V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_pkTetrahedron->V[2] + fT*rkVelocity1;
    Vector3<Real> kMV3 = m_pkTetrahedron->V[3] + fT*rkVelocity1;
    Tetrahedron3<Real> kMTetrahedron(kMV0,kMV1,kMV2,kMV3);
    return DistVector3Tetrahedron3<Real>(kMVector,kMTetrahedron).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Tetrahedron3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMV0 = m_pkTetrahedron->V[0] + fT*rkVelocity1;
    Vector3<Real> kMV1 = m_pkTetrahedron->V[1] + fT*rkVelocity1;
    Vector3<Real> kMV2 = m_pkTetrahedron->V[2] + fT*rkVelocity1;
    Vector3<Real> kMV3 = m_pkTetrahedron->V[3] + fT*rkVelocity1;
    Tetrahedron3<Real> kMTetrahedron(kMV0,kMV1,kMV2,kMV3);
    return DistVector3Tetrahedron3<Real>(kMVector,kMTetrahedron).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Tetrahedron3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Tetrahedron3<double>;
//----------------------------------------------------------------------------
}
