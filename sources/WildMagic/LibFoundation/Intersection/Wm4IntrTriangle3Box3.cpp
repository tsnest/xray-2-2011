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
// Version: 4.7.0 (2008/09/15)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrTriangle3Box3.h"
#include "Wm4DistVector3Triangle3.h"
#include "Wm4IntrSegment3Box3.h"
#include "Wm4IntrUtility3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrTriangle3Box3<Real>::IntrTriangle3Box3 (
    const Triangle3<Real>& rkTriangle, const Box3<Real>& rkBox)
    :
    m_pkTriangle(&rkTriangle),
    m_pkBox(&rkBox)
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrTriangle3Box3<Real>::GetTriangle () const
{
    return *m_pkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrTriangle3Box3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Box3<Real>::Test ()
{
    Real fMin0, fMax0, fMin1, fMax1;
    Vector3<Real> kD, akE[3];

    // Test direction of triangle normal.
    akE[0] = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    akE[1] = m_pkTriangle->V[2] - m_pkTriangle->V[0];
    kD = akE[0].Cross(akE[1]);
    fMin0 = kD.Dot(m_pkTriangle->V[0]);
    fMax0 = fMin0;
    IntrAxis<Real>::GetProjection(kD,*m_pkBox,fMin1,fMax1);
    if (fMax1 < fMin0 || fMax0 < fMin1)
    {
        return false;
    }

    // Test direction of box faces.
    for (int i = 0; i < 3; i++)
    {
        kD = m_pkBox->Axis[i];
        IntrAxis<Real>::GetProjection(kD,*m_pkTriangle,fMin0,fMax0);
        Real fDdC = kD.Dot(m_pkBox->Center);
        fMin1 = fDdC - m_pkBox->Extent[i];
        fMax1 = fDdC + m_pkBox->Extent[i];
        if (fMax1 < fMin0 || fMax0 < fMin1)
        {
            return false;
        }
    }

    // Test direction of triangle-box edge cross products.
    akE[2] = akE[1] - akE[0];
    for (int i0 = 0; i0 < 3; i0++)
    {
        for (int i1 = 0; i1 < 3; i1++)
        {
            kD = akE[i0].Cross(m_pkBox->Axis[i1]);
            IntrAxis<Real>::GetProjection(kD,*m_pkTriangle,fMin0,fMax0);
            IntrAxis<Real>::GetProjection(kD,*m_pkBox,fMin1,fMax1);
            if (fMax1 < fMin0 || fMax0 < fMin1)
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Box3<Real>::Find ()
{
    // Start with the triangle and clip it against each face of the box.
    m_iQuantity = 3;
    for (int i = 0; i < 3; i++)
    {
        m_akPoint[i] = m_pkTriangle->V[i];
    }

    for (int iDir = -1; iDir <= 1; iDir += 2)
    {
        for (int iSide = 0; iSide < 3; iSide++)
        {
            Vector3<Real> kInnerNormal = ((Real)iDir)*m_pkBox->Axis[iSide];
            Real fConstant = kInnerNormal.Dot(m_pkBox->Center) -
                m_pkBox->Extent[iSide];
            ClipConvexPolygonAgainstPlane(kInnerNormal,fConstant,
                m_iQuantity,m_akPoint);
        }
    }

    return m_iQuantity > 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Box3<Real>::Test (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    // Process as if triangle is stationary, box is moving.
    Vector3<Real> kVelocity = rkVelocity1 - rkVelocity0;
    m_fContactTime = (Real)0;
    Real fTLast = Math<Real>::MAX_REAL;

    // Test direction of triangle normal.
    Vector3<Real> akE[3] =
    {
        m_pkTriangle->V[1] - m_pkTriangle->V[0],
        m_pkTriangle->V[2] - m_pkTriangle->V[0],
        m_pkTriangle->V[2] - m_pkTriangle->V[1]
    };
    Vector3<Real> kD = akE[0].Cross(akE[1]);
    if (!IntrAxis<Real>::Test(kD,*m_pkTriangle,*m_pkBox,kVelocity,fTMax,
        m_fContactTime,fTLast))
    {
        return false;
    }

    // Test direction of box faces.
    for (int i = 0; i < 3; i++)
    {
        kD = m_pkBox->Axis[i];
        if (!IntrAxis<Real>::Test(kD,*m_pkTriangle,*m_pkBox,kVelocity,fTMax,
            m_fContactTime,fTLast))
        {
            return false;
        }
    }

    // test direction of triangle-box edge cross products
    for (int i0 = 0; i0 < 3; i0++)
    {
        for (int i1 = 0; i1 < 3; i1++)
        {
            kD = akE[i0].Cross(m_pkBox->Axis[i1]);
            if (!IntrAxis<Real>::Test(kD,*m_pkTriangle,*m_pkBox,kVelocity,
                fTMax,m_fContactTime,fTLast))
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Box3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    // Process as if triangle is stationary, box is moving.
    Vector3<Real> kVelocity = rkVelocity1 - rkVelocity0;

    m_fContactTime = (Real)0;
    Real fTLast = Math<Real>::MAX_REAL;

    int eSide = IntrConfiguration<Real>::NONE;
    IntrConfiguration<Real> kTriContact, kBoxContact;

    // Test tri-normal.
    Vector3<Real> akE[3] =
    {
        m_pkTriangle->V[1] - m_pkTriangle->V[0],
        m_pkTriangle->V[2] - m_pkTriangle->V[1],
        m_pkTriangle->V[0] - m_pkTriangle->V[2]
    };

    Vector3<Real> kTriNorm = akE[0].Cross(akE[1]);
    if (!IntrAxis<Real>::Find(kTriNorm,*m_pkTriangle,*m_pkBox,kVelocity,fTMax,
        m_fContactTime,fTLast,eSide,kTriContact,kBoxContact))
    {
        return false;
    }

    Vector3<Real> kAxis;
    int iCoplanar = -1; // triangle coplanar to none of the box normals
    int i0;
    for (i0 = 0; i0 < 3; i0++)
    {
        kAxis = m_pkBox->Axis[i0];
        if (!IntrAxis<Real>::Find(kAxis,*m_pkTriangle,*m_pkBox,kVelocity,
            fTMax,m_fContactTime,fTLast,eSide,kTriContact,kBoxContact))
        {
            return false;
        }

        // Test if axis is parallel to triangle normal.  The test is:
        // sin(Angle(normal,axis)) < epsilon
        Real fNdA = kTriNorm.Dot(kAxis);
        Real fNdN = kTriNorm.SquaredLength();
        Real fAdA = kAxis.SquaredLength();
        Real fSin = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 -
            fNdA*fNdA/(fNdN*fAdA)));
        if (fSin < Math<Real>::ZERO_TOLERANCE)
        {
            iCoplanar = i0;
        }
    }

    if (iCoplanar == -1)
    {
        // Test triedges cross boxfaces.
        for (i0 = 0; i0 < 3; i0++ )
        {
            for (int i1 = 0; i1 < 3; i1++ )
            {
                kAxis = akE[i0].Cross(m_pkBox->Axis[i1]);
                if (!IntrAxis<Real>::Find(kAxis,*m_pkTriangle,*m_pkBox,
                    kVelocity,fTMax,m_fContactTime,fTLast,eSide,kTriContact,
                    kBoxContact))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        // Test triedges cross coplanar box axis.
        for (i0 = 0; i0 < 3; i0++)
        {
            kAxis = akE[i0].Cross(kTriNorm);
            if (!IntrAxis<Real>::Find(kAxis,*m_pkTriangle,*m_pkBox,kVelocity,
                fTMax,m_fContactTime,fTLast,eSide,kTriContact,kBoxContact))
            {
                return false;
            }
        }
    }

    // Test velocity cross box faces.
    for (i0 = 0; i0 < 3; i0++)
    {
        kAxis = kVelocity.Cross(m_pkBox->Axis[i0]);
        if (!IntrAxis<Real>::Find(kAxis,*m_pkTriangle,*m_pkBox,kVelocity,
            fTMax,m_fContactTime,fTLast,eSide,kTriContact,kBoxContact))
        {
            return false;
        }
    }

    if (m_fContactTime < (Real)0 || eSide == IntrConfiguration<Real>::NONE)
    {
        return false;
    }

    FindContactSet<Real>(*m_pkTriangle,*m_pkBox,eSide,kTriContact,
        kBoxContact,rkVelocity0,rkVelocity1,m_fContactTime,m_iQuantity,
        m_akPoint);

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrTriangle3Box3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrTriangle3Box3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrTriangle3Box3<float>;

template WM4_FOUNDATION_ITEM
class IntrTriangle3Box3<double>;
//----------------------------------------------------------------------------
}
