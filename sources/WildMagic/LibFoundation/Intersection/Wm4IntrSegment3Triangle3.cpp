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
// Version: 4.0.2 (2008/09/15)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrSegment3Triangle3.h"
#include "Wm4IntrUtility3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment3Triangle3<Real>::IntrSegment3Triangle3 (
    const Segment3<Real>& rkSegment, const Triangle3<Real>& rkTriangle)
    :
    m_pkSegment(&rkSegment),
    m_pkTriangle(&rkTriangle)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& IntrSegment3Triangle3<Real>::GetSegment () const
{
    return *m_pkSegment;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrSegment3Triangle3<Real>::GetTriangle () const
{
    return *m_pkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Triangle3<Real>::Test ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_pkSegment->Origin - m_pkTriangle->V[0];
    Vector3<Real> kEdge1 = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    Vector3<Real> kEdge2 = m_pkTriangle->V[2] - m_pkTriangle->V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = segment direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_pkSegment->Direction.Dot(kNormal);
    Real fSign;
    if (fDdN > Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)1.0;
    }
    else if (fDdN < -Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Segment and triangle are parallel, call it a "no intersection"
        // even if the segment does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_pkSegment->Direction.Dot(kDiff.Cross(kEdge2));
    if (fDdQxE2 >= (Real)0.0)
    {
        Real fDdE1xQ = fSign*m_pkSegment->Direction.Dot(kEdge1.Cross(kDiff));
        if (fDdE1xQ >= (Real)0.0)
        {
            if (fDdQxE2 + fDdE1xQ <= fDdN)
            {
                // line intersects triangle, check if segment does
                Real fQdN = -fSign*kDiff.Dot(kNormal);
                Real fExtDdN = m_pkSegment->Extent*fDdN;
                if (-fExtDdN <= fQdN && fQdN <= fExtDdN)
                {
                    // segment intersects triangle
                    return true;
                }
                // else: |t| > extent, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Triangle3<Real>::Find ()
{
    // compute the offset origin, edges, and normal
    Vector3<Real> kDiff = m_pkSegment->Origin - m_pkTriangle->V[0];
    Vector3<Real> kEdge1 = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    Vector3<Real> kEdge2 = m_pkTriangle->V[2] - m_pkTriangle->V[0];
    Vector3<Real> kNormal = kEdge1.Cross(kEdge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = segment direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real fDdN = m_pkSegment->Direction.Dot(kNormal);
    Real fSign;
    if (fDdN > Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)1.0;
    }
    else if (fDdN < -Math<Real>::ZERO_TOLERANCE)
    {
        fSign = (Real)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Segment and triangle are parallel, call it a "no intersection"
        // even if the segment does intersect.
        return false;
    }

    Real fDdQxE2 = fSign*m_pkSegment->Direction.Dot(kDiff.Cross(kEdge2));
    if (fDdQxE2 >= (Real)0.0)
    {
        Real fDdE1xQ = fSign*m_pkSegment->Direction.Dot(kEdge1.Cross(kDiff));
        if (fDdE1xQ >= (Real)0.0)
        {
            if (fDdQxE2 + fDdE1xQ <= fDdN)
            {
                // line intersects triangle, check if segment does
                Real fQdN = -fSign*kDiff.Dot(kNormal);
                Real fExtDdN = m_pkSegment->Extent*fDdN;
                if (-fExtDdN <= fQdN && fQdN <= fExtDdN)
                {
                    // segment intersects triangle
                    Real fInv = ((Real)1.0)/fDdN;
                    m_fSegmentT = fQdN*fInv;
                    m_fTriB1 = fDdQxE2*fInv;
                    m_fTriB2 = fDdE1xQ*fInv;
                    m_fTriB0 = (Real)1.0 - m_fTriB1 - m_fTriB2;
                    return true;
                }
                // else: |t| > extent, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSegment3Triangle3<Real>::GetSegmentT () const
{
    return m_fSegmentT;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSegment3Triangle3<Real>::GetTriB0 () const
{
    return m_fTriB0;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSegment3Triangle3<Real>::GetTriB1 () const
{
    return m_fTriB1;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrSegment3Triangle3<Real>::GetTriB2 () const
{
    return m_fTriB2;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Triangle3<Real>::Test (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    m_iQuantity = 0;

    // Get the endpoints of the segment.
    Vector3<Real> akSegment[2] =
    {
        m_pkSegment->GetNegEnd(),
        m_pkSegment->GetPosEnd()
    };

    // Get the triangle edges.
    Vector3<Real> kEdge0 = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    Vector3<Real> kEdge1 = m_pkTriangle->V[2] - m_pkTriangle->V[0];

    // Get the triangle velocity relative to the segment.
    Vector3<Real> kVelocity = rkVelocity1 - rkVelocity0;

    m_fContactTime = (Real)0;
    Real fTLast = Math<Real>::MAX_REAL;

    // Test tri-normal.
    Vector3<Real> kNormV = kEdge0.Cross(kEdge1);
    if (!IntrAxis<Real>::Test(kNormV,akSegment,*m_pkTriangle,kVelocity,
        fTMax,m_fContactTime,fTLast))
    {
        return false;
    }

    // Test whether the segment is parallel to the triangle, effectively the
    // test:  sin(Angle(NormV,DirU)) > 1-epsilon
    Vector3<Real> kDirU = akSegment[1] - akSegment[0];
    Vector3<Real> kNormU = kNormV.Cross(kDirU);
    Real fDirUSqrLen = kDirU.SquaredLength();
    Real fNorUSqrLen = kNormU.SquaredLength();
    Real fNorVSqrLen = kNormV.SquaredLength();
    Real fOmEpsilon = (Real)1 - Math<Real>::ZERO_TOLERANCE;

    int i0, i1;
    Vector3<Real> kAxis;

    if (fNorUSqrLen > fOmEpsilon*fNorVSqrLen*fDirUSqrLen)  // parallel
    {
        // Test tri-normal cross seg-direction.
        if (!IntrAxis<Real>::Test(kNormU,akSegment,*m_pkTriangle,kVelocity,
            fTMax,m_fContactTime,fTLast))
        {
            return false;
        }

        // Test tri-normal cross tri-edges.
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            kAxis = kNormV.Cross(m_pkTriangle->V[i1] - m_pkTriangle->V[i0]);
            if (!IntrAxis<Real>::Test(kAxis,akSegment,*m_pkTriangle,
                kVelocity,fTMax,m_fContactTime,fTLast))
            {
                return false;
            }
        }
    }
    else  // not parallel
    {
        // Test seg-direction cross tri-edges.
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            kAxis = kDirU.Cross(m_pkTriangle->V[i1] - m_pkTriangle->V[i0]);
            if (!IntrAxis<Real>::Test(kAxis,akSegment,*m_pkTriangle,
                kVelocity,fTMax,m_fContactTime,fTLast))
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment3Triangle3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    m_iQuantity = 0;
    m_iIntersectionType = IT_EMPTY;

    // Get the endpoints of the segment.
    Vector3<Real> akSegment[2] =
    {
        m_pkSegment->GetNegEnd(),
        m_pkSegment->GetPosEnd()
    };

    // Get the triangle edges.
    Vector3<Real> kEdge0 = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    Vector3<Real> kEdge1 = m_pkTriangle->V[2] - m_pkTriangle->V[0];

    // Get the triangle velocity relative to the segment.
    Vector3<Real> kVelocity = rkVelocity1 - rkVelocity0;

    m_fContactTime = (Real)0;
    Real fTLast = Math<Real>::MAX_REAL;

    // Test tri-normal.
    Vector3<Real> kNormV = kEdge0.Cross(kEdge1);
    IntrConfiguration<Real> kSegContact, kTriContact;
    int eSide;
    if (!IntrAxis<Real>::Find(kNormV,akSegment,*m_pkTriangle,kVelocity,
        fTMax,m_fContactTime,fTLast,eSide,kSegContact,kTriContact))
    {
        return false;
    }

    // Test whether the segment is parallel to the triangle, effectively the
    // test:  sin(Angle(NormV,DirU)) > 1-epsilon
    Vector3<Real> kDirU = akSegment[1] - akSegment[0];
    Vector3<Real> kNormU = kNormV.Cross(kDirU);
    Real fDirUSqrLen = kDirU.SquaredLength();
    Real fNorUSqrLen = kNormU.SquaredLength();
    Real fNorVSqrLen = kNormV.SquaredLength();
    Real fOmEpsilon = (Real)1 - Math<Real>::ZERO_TOLERANCE;

    int i0, i1;
    Vector3<Real> kAxis;

    if (fNorUSqrLen > fOmEpsilon*fNorVSqrLen*fDirUSqrLen)  // parallel
    {
        // Find tri-normal cross seg-direction.
        if (!IntrAxis<Real>::Find(kNormU,akSegment,*m_pkTriangle,kVelocity,
            fTMax,m_fContactTime,fTLast,eSide,kSegContact,kTriContact))
        {
            return false;
        }

        // Find tri-normal cross tri-edges.
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            kAxis = kNormV.Cross(m_pkTriangle->V[i1] - m_pkTriangle->V[i0]);
            if (!IntrAxis<Real>::Find(kAxis,akSegment,*m_pkTriangle,
                kVelocity,fTMax,m_fContactTime,fTLast,eSide,kSegContact,
                kTriContact))
            {
                return false;
            }
        }
    } 
    else 
    {
        // test seg-direction cross tri-edges
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            kDirU.Cross(m_pkTriangle->V[i1] - m_pkTriangle->V[i0]);
            if (!IntrAxis<Real>::Find(kAxis,akSegment,*m_pkTriangle,
                kVelocity,fTMax,m_fContactTime,fTLast,eSide,kSegContact,
                kTriContact))
            {
                return false;
            }
        }
    }

    if (m_fContactTime < (Real)0)
    {
        return false;
    }

    FindContactSet<Real>(akSegment,*m_pkTriangle,eSide,kSegContact,
        kTriContact,rkVelocity0,rkVelocity1,m_fContactTime,m_iQuantity,
        m_akPoint);

    if (m_iQuantity == 1)
    {
        m_iIntersectionType = IT_POINT;
    }
    else
    {
        m_iIntersectionType = IT_SEGMENT;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment3Triangle3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrSegment3Triangle3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrSegment3Triangle3<float>;

template WM4_FOUNDATION_ITEM
class IntrSegment3Triangle3<double>;
//----------------------------------------------------------------------------
}
