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
#include "Wm4DistVector3Frustum3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Frustum3<Real>::DistVector3Frustum3 (const Vector3<Real>& rkVector,
    const Frustum3<Real>& rkFrustum)
    :
    m_pkVector(&rkVector),
    m_pkFrustum(&rkFrustum)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Frustum3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Frustum3<Real>& DistVector3Frustum3<Real>::GetFrustum () const
{
    return *m_pkFrustum;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::GetSquared ()
{
    // compute coordinates of point with respect to frustum coordinate system
    Vector3<Real> kDiff = *m_pkVector - m_pkFrustum->Origin;
    Vector3<Real> kTest = Vector3<Real>(
        kDiff.Dot(m_pkFrustum->RVector),
        kDiff.Dot(m_pkFrustum->UVector),
        kDiff.Dot(m_pkFrustum->DVector));

    // perform calculations in octant with nonnegative R and U coordinates
    bool bRSignChange;
    if (kTest.X() < (Real)0.0)
    {
        bRSignChange = true;
        kTest.X() = -kTest.X();
    }
    else
    {
        bRSignChange = false;
    }

    bool bUSignChange;
    if (kTest.Y() < (Real)0.0)
    {
        bUSignChange = true;
        kTest.Y() = -kTest.Y();
    }
    else
    {
        bUSignChange = false;
    }

    // frustum derived parameters
    Real fRMin = m_pkFrustum->RBound;
    Real fRMax = m_pkFrustum->GetDRatio()*fRMin;
    Real fUMin = m_pkFrustum->UBound;
    Real fUMax = m_pkFrustum->GetDRatio()*fUMin;
    Real fDMin = m_pkFrustum->DMin;
    Real fDMax = m_pkFrustum->DMax;
    Real fRMinSqr = fRMin*fRMin;
    Real fUMinSqr = fUMin*fUMin;
    Real fDMinSqr = fDMin*fDMin;
    Real fMinRDDot = fRMinSqr + fDMinSqr;
    Real fMinUDDot = fUMinSqr + fDMinSqr;
    Real fMinRUDDot = fRMinSqr + fMinUDDot;
    Real fMaxRDDot = m_pkFrustum->GetDRatio()*fMinRDDot;
    Real fMaxUDDot = m_pkFrustum->GetDRatio()*fMinUDDot;
    Real fMaxRUDDot = m_pkFrustum->GetDRatio()*fMinRUDDot;

    // Algorithm computes closest point in all cases by determining in which
    // Voronoi region of the vertices, edges, and faces of the frustum that
    // the test point lives.
    Vector3<Real> kClosest;
    Real fRDot, fUDot, fRDDot, fUDDot, fRUDDot, fREdgeDot, fUEdgeDot, fT;
    if (kTest.Z() >= fDMax)
    {
        if (kTest.X() <= fRMax)
        {
            if (kTest.Y() <= fUMax)
            {
                // F-face
                kClosest.X() = kTest.X();
                kClosest.Y() = kTest.Y();
                kClosest.Z() = fDMax;
            }
            else
            {
                // UF-edge
                kClosest.X() = kTest.X();
                kClosest.Y() = fUMax;
                kClosest.Z() = fDMax;
            }
        }
        else
        {
            if (kTest.Y() <= fUMax)
            {
                // LF-edge
                kClosest.X() = fRMax;
                kClosest.Y() = kTest.Y();
                kClosest.Z() = fDMax;
            }
            else
            {
                // LUF-vertex
                kClosest.X() = fRMax;
                kClosest.Y() = fUMax;
                kClosest.Z() = fDMax;
            }
        }
    }
    else if (kTest.Z() <= fDMin)
    {
        if (kTest.X() <= fRMin)
        {
            if (kTest.Y() <= fUMin)
            {
                // N-face
                kClosest.X() = kTest.X();
                kClosest.Y() = kTest.Y();
                kClosest.Z() = fDMin;
            }
            else
            {
                fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                if (fUDDot >= fMaxUDDot)
                {
                    // UF-edge
                    kClosest.X() = kTest.X();
                    kClosest.Y() = fUMax;
                    kClosest.Z() = fDMax;
                }
                else if (fUDDot >= fMinUDDot)
                {
                    // U-face
                    fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                    fT = fUDot/fMinUDDot;
                    kClosest.X() = kTest.X();
                    kClosest.Y() = kTest.Y() - fT*fDMin;
                    kClosest.Z() = kTest.Z() + fT*fUMin;
                }
                else
                {
                    // UN-edge
                    kClosest.X() = kTest.X();
                    kClosest.Y() = fUMin;
                    kClosest.Z() = fDMin;
                }
            }
        }
        else
        {
            if (kTest.Y() <= fUMin)
            {
                fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                if (fRDDot >= fMaxRDDot)
                {
                    // LF-edge
                    kClosest.X() = fRMax;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = fDMax;
                }
                else if (fRDDot >= fMinRDDot)
                {
                    // L-face
                    fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                    fT = fRDot/fMinRDDot;
                    kClosest.X() = kTest.X() - fT*fDMin;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = kTest.Z() + fT*fRMin;
                }
                else
                {
                    // LN-edge
                    kClosest.X() = fRMin;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = fDMin;
                }
            }
            else
            {
                fRUDDot = fRMin*kTest.X() + fUMin*kTest.Y() + fDMin*kTest.Z();
                fREdgeDot = fUMin*fRUDDot - fMinRUDDot*kTest.Y();
                if (fREdgeDot >= (Real)0.0)
                {
                    fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                    if (fRDDot >= fMaxRDDot)
                    {
                        // LF-edge
                        kClosest.X() = fRMax;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = fDMax;
                    }
                    else if (fRDDot >= fMinRDDot)
                    {
                        // L-face
                        fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                        fT = fRDot/fMinRDDot;
                        kClosest.X() = kTest.X() - fT*fDMin;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = kTest.Z() + fT*fRMin;
                    }
                    else
                    {
                        // LN-edge
                        kClosest.X() = fRMin;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = fDMin;
                    }
                }
                else
                {
                    fUEdgeDot = fRMin*fRUDDot - fMinRUDDot*kTest.X();
                    if (fUEdgeDot >= (Real)0.0)
                    {
                        fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                        if (fUDDot >= fMaxUDDot)
                        {
                            // UF-edge
                            kClosest.X() = kTest.X();
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else if (fUDDot >= fMinUDDot)
                        {
                            // U-face
                            fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                            fT = fUDot/fMinUDDot;
                            kClosest.X() = kTest.X();
                            kClosest.Y() = kTest.Y() - fT*fDMin;
                            kClosest.Z() = kTest.Z() + fT*fUMin;
                        }
                        else
                        {
                            // UN-edge
                            kClosest.X() = kTest.X();
                            kClosest.Y() = fUMin;
                            kClosest.Z() = fDMin;
                        }
                    }
                    else
                    {
                        if (fRUDDot >= fMaxRUDDot)
                        {
                            // LUF-vertex
                            kClosest.X() = fRMax;
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else if (fRUDDot >= fMinRUDDot)
                        {
                            // LU-edge
                            fT = fRUDDot/fMinRUDDot;
                            kClosest.X() = fT*fRMin;
                            kClosest.Y() = fT*fUMin;
                            kClosest.Z() = fT*fDMin;
                        }
                        else
                        {
                            // LUN-vertex
                            kClosest.X() = fRMin;
                            kClosest.Y() = fUMin;
                            kClosest.Z() = fDMin;
                        }
                    }
                }
            }
        }
    }
    else
    {
        fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
        fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
        if (fRDot <= (Real)0.0)
        {
            if (fUDot <= (Real)0.0)
            {
                // point inside frustum
                kClosest = kTest;
            }
            else
            {
                fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                if (fUDDot >= fMaxUDDot)
                {
                    // UF-edge
                    kClosest.X() = kTest.X();
                    kClosest.Y() = fUMax;
                    kClosest.Z() = fDMax;
                }
                else
                {
                    // U-face
                    fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                    fT = fUDot/fMinUDDot;
                    kClosest.X() = kTest.X();
                    kClosest.Y() = kTest.Y() - fT*fDMin;
                    kClosest.Z() = kTest.Z() + fT*fUMin;
                }
            }
        }
        else
        {
            if (fUDot <= (Real)0.0)
            {
                fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                if (fRDDot >= fMaxRDDot)
                {
                    // LF-edge
                    kClosest.X() = fRMax;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = fDMax;
                }
                else
                {
                    // L-face
                    fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                    fT = fRDot/fMinRDDot;
                    kClosest.X() = kTest.X() - fT*fDMin;
                    kClosest.Y() = kTest.Y();
                    kClosest.Z() = kTest.Z() + fT*fRMin;
                }
            }
            else
            {
                fRUDDot = fRMin*kTest.X() + fUMin*kTest.Y() + fDMin*kTest.Z();
                fREdgeDot = fUMin*fRUDDot - fMinRUDDot*kTest.Y();
                if (fREdgeDot >= (Real)0.0)
                {
                    fRDDot = fRMin*kTest.X() + fDMin*kTest.Z();
                    if (fRDDot >= fMaxRDDot)
                    {
                        // LF-edge
                        kClosest.X() = fRMax;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = fDMax;
                    }
                    else // assert( fRDDot >= fMinRDDot ) from geometry
                    {
                        // L-face
                        fRDot = fDMin*kTest.X() - fRMin*kTest.Z();
                        fT = fRDot/fMinRDDot;
                        kClosest.X() = kTest.X() - fT*fDMin;
                        kClosest.Y() = kTest.Y();
                        kClosest.Z() = kTest.Z() + fT*fRMin;
                    }
                }
                else
                {
                    fUEdgeDot = fRMin*fRUDDot - fMinRUDDot*kTest.X();
                    if (fUEdgeDot >= (Real)0.0)
                    {
                        fUDDot = fUMin*kTest.Y() + fDMin*kTest.Z();
                        if (fUDDot >= fMaxUDDot)
                        {
                            // UF-edge
                            kClosest.X() = kTest.X();
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else // assert( fUDDot >= fMinUDDot ) from geometry
                        {
                            // U-face
                            fUDot = fDMin*kTest.Y() - fUMin*kTest.Z();
                            fT = fUDot/fMinUDDot;
                            kClosest.X() = kTest.X();
                            kClosest.Y() = kTest.Y() - fT*fDMin;
                            kClosest.Z() = kTest.Z() + fT*fUMin;
                        }
                    }
                    else
                    {
                        if (fRUDDot >= fMaxRUDDot)
                        {
                            // LUF-vertex
                            kClosest.X() = fRMax;
                            kClosest.Y() = fUMax;
                            kClosest.Z() = fDMax;
                        }
                        else // assert( fRUDDot >= fMinRUDDot ) from geometry
                        {
                            // LU-edge
                            fT = fRUDDot/fMinRUDDot;
                            kClosest.X() = fT*fRMin;
                            kClosest.Y() = fT*fUMin;
                            kClosest.Z() = fT*fDMin;
                        }
                    }
                }
            }
        }
    }

    kDiff = kTest - kClosest;

    // convert back to original quadrant
    if (bRSignChange)
    {
        kClosest.X() = -kClosest.X();
    }

    if (bUSignChange)
    {
        kClosest.Y() = -kClosest.Y();
    }

    m_kClosestPoint0 = *m_pkVector;

    // convert back to original coordinates
    m_kClosestPoint1 = m_pkFrustum->Origin +
        kClosest.X()*m_pkFrustum->RVector +
        kClosest.Y()*m_pkFrustum->UVector +
        kClosest.Z()*m_pkFrustum->DVector;

    // compute and return squared distance
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkFrustum->Origin + fT*rkVelocity1;
    Frustum3<Real> kMFrustum(kMOrigin,m_pkFrustum->DVector,
        m_pkFrustum->UVector,m_pkFrustum->RVector,m_pkFrustum->DMin,
        m_pkFrustum->DMax,m_pkFrustum->UBound,m_pkFrustum->RBound);
    return DistVector3Frustum3<Real>(kMVector,kMFrustum).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Frustum3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Vector3<Real> kMOrigin = m_pkFrustum->Origin + fT*rkVelocity1;
    Frustum3<Real> kMFrustum(kMOrigin,m_pkFrustum->DVector,
        m_pkFrustum->UVector,m_pkFrustum->RVector,m_pkFrustum->DMin,
        m_pkFrustum->DMax,m_pkFrustum->UBound,m_pkFrustum->RBound);
    return DistVector3Frustum3<Real>(kMVector,kMFrustum).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Frustum3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Frustum3<double>;
//----------------------------------------------------------------------------
}
