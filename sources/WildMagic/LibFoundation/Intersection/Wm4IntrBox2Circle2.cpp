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
#include "Wm4IntrBox2Circle2.h"
#include "Wm4DistVector2Box2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrBox2Circle2<Real>::IntrBox2Circle2 (const Box2<Real>& rkBox,
    const Circle2<Real>& rkCircle)
    :
    m_pkBox(&rkBox),
    m_pkCircle(&rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Box2<Real>& IntrBox2Circle2<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrBox2Circle2<Real>::GetCircle () const
{
    return *m_pkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox2Circle2<Real>::Test ()
{
    Real fDistance = DistVector2Box2<Real>(m_pkCircle->Center,*m_pkBox).Get();
    return fDistance <= m_pkCircle->Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox2Circle2<Real>::Find (Real fTMax,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    // convert circle center to box coordinates
    Vector2<Real> kDiff = m_pkCircle->Center - m_pkBox->Center;
    Vector2<Real> kVel = rkVelocity1 - rkVelocity0;
    Real fCx = kDiff.Dot(m_pkBox->Axis[0]);
    Real fCy = kDiff.Dot(m_pkBox->Axis[1]);
    Real fVx = kVel.Dot(m_pkBox->Axis[0]);
    Real fVy = kVel.Dot(m_pkBox->Axis[1]);
    Real fEx = m_pkBox->Extent[0];
    Real fEy = m_pkBox->Extent[1];
    Real fIx, fIy;

    int iType = 0;

    if (fCx < -fEx)
    {
        if (fCy < -fEy)
        {
            // region Rmm
            iType = TestVertexRegion(fCx,fCy,fVx,fVy,fEx,fEy,fIx,fIy);
        }
        else if (fCy <= fEy)
        {
            // region Rmz
            iType = TestEdgeRegion(fCx,fCy,fVx,fVy,fEx,fEy,fIx,fIy);
        }
        else
        {
            // region Rmp
            iType = TestVertexRegion(fCx,-fCy,fVx,-fVy,fEx,fEy,fIx,fIy);
            fIy = -fIy;
        }
    }
    else if (fCx <= fEx)
    {
        if (fCy < -fEy)
        {
            // region Rzm
            iType = TestEdgeRegion(fCy,fCx,fVy,fVx,fEy,fEx,fIy,fIx);
        }
        else if (fCy <= fEy)
        {
            // region Rzz: The circle is already intersecting the box.  Use
            // the circle center as the intersection point, but let the
            // caller know of that the objects overlap by returning an "other"
            // intersection.
            m_fContactTime = (Real)0.0;
            m_kContactPoint = m_pkCircle->Center;
            m_iIntersectionType = IT_OTHER;
            return true;
        }
        else
        {
            // region Rzp
            iType = TestEdgeRegion(-fCy,fCx,-fVy,fVx,fEy,fEx,fIy,fIx);
            fIy = -fIy;
        }
    }
    else
    {
        if (fCy < -fEy)
        {
            // region Rpm
            iType = TestVertexRegion(-fCx,fCy,-fVx,fVy,fEx,fEy,fIx,fIy);
            fIx = -fIx;
        }
        else if (fCy <= fEy)
        {
            // region Rpz
            iType = TestEdgeRegion(-fCx,fCy,-fVx,fVy,fEx,fEy,fIx,fIy);
            fIx = -fIx;
        }
        else
        {
            // region Rpp
            iType = TestVertexRegion(-fCx,-fCy,-fVx,-fVy,fEx,fEy,fIx,fIy);
            fIx = -fIx;
            fIy = -fIy;
        }
    }

    if (iType != 1 || m_fContactTime > fTMax)
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    m_kContactPoint = m_pkBox->Center + fIx*m_pkBox->Axis[0] +
        fIy*m_pkBox->Axis[1];
    m_iIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrBox2Circle2<Real>::GetContactPoint () const
{
    return m_kContactPoint;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox2Circle2<Real>::TestVertexRegion (Real fCx, Real fCy, Real fVx,
    Real fVy, Real fEx, Real fEy, Real& rfIx, Real& rfIy)
{
    Real fDx = fCx + fEx;
    Real fDy = fCy + fEy;
    Real fRSqr = m_pkCircle->Radius*m_pkCircle->Radius;
    Real fDiff = fDx*fDx + fDy*fDy - fRSqr;
    if (fDiff <= (Real)0.0)
    {
        // circle is already intersecting the box
        m_fContactTime = (Real)0.0;
        return -1;
    }

    Real fDot = fVx*fDx + fVy*fDy;
    if (fDot >= (Real)0.0)
    {
        // circle not moving towards box
        return 0;
    }

    Real fDotPerp = fVx*fDy - fVy*fDx;
    Real fVSqr, fInv;

    if (fDotPerp >= (Real)0.0)
    {
        // potential contact on left edge
        if (fDotPerp <= m_pkCircle->Radius*fVy)
        {
            // lower left corner is first point of contact
            rfIx = -fEx;
            rfIy = -fEy;
            fVSqr = fVx*fVx + fVy*fVy;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            return 1;
        }

        if (fVx <= (Real)0.0)
        {
            // passed corner, moving away from box
            return 0;
        }

        fVSqr = fVx*fVx + fVy*fVy;
        fDy = fCy - fEy;
        fDotPerp = fVx*fDy - fVy*fDx;
        if (fDotPerp >= 0.0f && fDotPerp*fDotPerp > fRSqr*fVSqr)
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if (fDotPerp <= m_pkCircle->Radius*fVy)
        {
            // first contact on left edge of box
            m_fContactTime = -(fDx+m_pkCircle->Radius)/fVx;
            rfIy = fCy + m_fContactTime*fVy;
        }
        else
        {
            // first contact at upper left corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIy = fEy;
        }
    }
    else
    {
        // potential contact on bottom edge
        if (-fDotPerp <= m_pkCircle->Radius*fVx)
        {
            // lower left corner is first point of contact
            rfIx = -fEx;
            rfIy = -fEy;
            fVSqr = fVx*fVx + fVy*fVy;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            return 1;
        }

        if (fVy <= (Real)0.0)
        {
            // passed corner, moving away from box
            return 0;
        }

        fVSqr = fVx*fVx + fVy*fVy;
        fDx = fCx - fEx;
        fDotPerp = fVx*fDy - fVy*fDx;
        if (-fDotPerp >= (Real)0.0 && fDotPerp*fDotPerp > fRSqr*fVSqr)
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with y = ymin.
        rfIy = -fEy;

        if (-fDotPerp <= m_pkCircle->Radius*fVx)
        {
            // first contact on bottom edge of box
            m_fContactTime = -(fDy+m_pkCircle->Radius)/fVy;
            rfIx = fCx + m_fContactTime*fVx;
        }
        else
        {
            // first contact at lower right corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIx = fEx;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox2Circle2<Real>::TestEdgeRegion (Real fCx, Real fCy, Real fVx,
    Real fVy, Real fEx, Real fEy, Real& rfIx, Real& rfIy)
{
    Real fDx = fCx + fEx;
    Real fXSignedDist = fDx + m_pkCircle->Radius;
    if (fXSignedDist >= (Real)0.0)
    {
        // circle is already intersecting the box
        m_fContactTime = (Real)0.0;
        return -1;
    }

    if (fVx <= (Real)0.0)
    {
        // circle not moving towards box
        return 0;
    }

    Real fRSqr = m_pkCircle->Radius*m_pkCircle->Radius;
    Real fVSqr = fVx*fVx + fVy*fVy;
    Real fDy, fDot, fDotPerp, fDiff, fInv;

    if (fVy >= (Real)0.0)
    {
        fDy = fCy - fEy;
        fDotPerp = fVx*fDy - fVy*fDx;
        if (fDotPerp >= (Real)0.0 && fDotPerp*fDotPerp > fRSqr*fVSqr)
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if (fDotPerp <= m_pkCircle->Radius*fVy)
        {
            // first contact on left edge of box
            m_fContactTime = -fXSignedDist/fVx;
            rfIy = fCy + m_fContactTime*fVy;
        }
        else
        {
            // first contact at corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIy = fEy;
        }
    }
    else
    {
        fDy = fCy + fEy;
        fDotPerp = fVx*fDy - fVy*fDx;
        if (fDotPerp <= (Real)0.0 && fDotPerp*fDotPerp > fRSqr*fVSqr)
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if (fDotPerp >= m_pkCircle->Radius*fVy)
        {
            // first contact on left edge of box
            m_fContactTime = -fXSignedDist/fVx;
            rfIy = fCy + m_fContactTime*fVy;
        }
        else
        {
            // first contact at corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIy = -fEy;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrBox2Circle2<float>;

template WM4_FOUNDATION_ITEM
class IntrBox2Circle2<double>;
//----------------------------------------------------------------------------
}
