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
#include "Wm4IntrBox3Sphere3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrBox3Sphere3<Real>::IntrBox3Sphere3 (const Box3<Real>& rkBox,
    const Sphere3<Real>& rkSphere)
    :
    m_pkBox(&rkBox),
    m_pkSphere(&rkSphere)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& IntrBox3Sphere3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrBox3Sphere3<Real>::GetSphere () const
{
    return *m_pkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox3Sphere3<Real>::Test ()
{
    // Test for intersection in the coordinate system of the box by
    // transforming the sphere into that coordinate system.
    Vector3<Real> kCDiff = m_pkSphere->Center - m_pkBox->Center;

    Real fAx = Math<Real>::FAbs(kCDiff.Dot(m_pkBox->Axis[0]));
    Real fAy = Math<Real>::FAbs(kCDiff.Dot(m_pkBox->Axis[1]));
    Real fAz = Math<Real>::FAbs(kCDiff.Dot(m_pkBox->Axis[2]));
    Real fDx = fAx - m_pkBox->Extent[0];
    Real fDy = fAy - m_pkBox->Extent[1];
    Real fDz = fAz - m_pkBox->Extent[2];

    if (fAx <= m_pkBox->Extent[0])
    {
        if (fAy <= m_pkBox->Extent[1])
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // sphere center inside box
                return true;
            }
            else
            {
                // potential sphere-face intersection with face z
                return fDz <= m_pkSphere->Radius;
            }
        }
        else
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // potential sphere-face intersection with face y
                return fDy <= m_pkSphere->Radius;
            }
            else
            {
                // potential sphere-edge intersection with edge formed
                // by faces y and z
                Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
                return fDy*fDy + fDz*fDz <= fRSqr;
            }
        }
    }
    else
    {
        if (fAy <= m_pkBox->Extent[1])
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // potential sphere-face intersection with face x
                return fDx <= m_pkSphere->Radius;
            }
            else
            {
                // potential sphere-edge intersection with edge formed
                // by faces x and z
                Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
                return fDx*fDx + fDz*fDz <= fRSqr;
            }
        }
        else
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // potential sphere-edge intersection with edge formed
                // by faces x and y
                Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
                return fDx*fDx + fDy*fDy <= fRSqr;
            }
            else
            {
                // potential sphere-vertex intersection at corner formed
                // by faces x,y,z
                Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
                return fDx*fDx + fDy*fDy + fDz*fDz <= fRSqr;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox3Sphere3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    // Find intersections relative to the coordinate system of the box.
    // The sphere is transformed to the box coordinates and the velocity of
    // the sphere is relative to the box.
    Vector3<Real> kCDiff = m_pkSphere->Center - m_pkBox->Center;
    Vector3<Real> kVel = rkVelocity1 - rkVelocity0;
    Real fAx = kCDiff.Dot(m_pkBox->Axis[0]);
    Real fAy = kCDiff.Dot(m_pkBox->Axis[1]);
    Real fAz = kCDiff.Dot(m_pkBox->Axis[2]);
    Real fVx = kVel.Dot(m_pkBox->Axis[0]);
    Real fVy = kVel.Dot(m_pkBox->Axis[1]);
    Real fVz = kVel.Dot(m_pkBox->Axis[2]);

    // flip coordinate frame into the first octant
    int iSignX = 1;
    if (fAx < (Real)0.0)
    {
        fAx = -fAx;
        fVx = -fVx;
        iSignX = -1;
    }

    int iSignY = 1;
    if (fAy < (Real)0.0)
    {
        fAy = -fAy;
        fVy = -fVy;
        iSignY = -1;
    }

    int iSignZ = 1;
    if (fAz < (Real)0.0)
    {
        fAz = -fAz;
        fVz = -fVz;
        iSignZ = -1;
    }

    // intersection coordinates
    Real fIx, fIy, fIz;
    int iRetVal;

    if (fAx <= m_pkBox->Extent[0])
    {
        if (fAy <= m_pkBox->Extent[1])
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // The sphere center is inside box.  Return it as the contact
                // point, but report an "other" intersection type.
                m_fContactTime = (Real)0.0;
                m_kContactPoint = m_pkSphere->Center;
                m_iIntersectionType = IT_OTHER;
                return true;
            }
            else
            {
                // sphere above face on axis Z
                iRetVal = FindFaceRegionIntersection(m_pkBox->Extent[0], 
                    m_pkBox->Extent[1],m_pkBox->Extent[2],fAx,fAy,fAz,fVx,fVy,
                    fVz,fIx,fIy,fIz,true);     
            }
        }
        else
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // sphere above face on axis Y
                iRetVal = FindFaceRegionIntersection(m_pkBox->Extent[0], 
                    m_pkBox->Extent[2],m_pkBox->Extent[1],fAx,fAz,fAy,fVx,fVz,
                    fVy,fIx,fIz,fIy,true);   
            }
            else
            {
                // sphere is above the edge formed by faces y and z
                iRetVal = FindEdgeRegionIntersection(m_pkBox->Extent[1], 
                    m_pkBox->Extent[0],m_pkBox->Extent[2],fAy,fAx,fAz,fVy,fVx,
                    fVz,fIy,fIx,fIz,true);
            }
        }
    }
    else
    {
        if (fAy <= m_pkBox->Extent[1])
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // sphere above face on axis X
                iRetVal = FindFaceRegionIntersection(m_pkBox->Extent[1],
                    m_pkBox->Extent[2],m_pkBox->Extent[0],fAy,fAz,fAx,fVy,fVz,
                    fVx,fIy,fIz,fIx,true);
            }
            else
            {
                // sphere is above the edge formed by faces x and z
                iRetVal = FindEdgeRegionIntersection(m_pkBox->Extent[0], 
                    m_pkBox->Extent[1],m_pkBox->Extent[2],fAx,fAy,fAz,fVx,fVy,
                    fVz,fIx,fIy,fIz,true);
            }
        }
        else
        {
            if (fAz <= m_pkBox->Extent[2])
            {
                // sphere is above the edge formed by faces x and y
                iRetVal = FindEdgeRegionIntersection(m_pkBox->Extent[0], 
                    m_pkBox->Extent[2],m_pkBox->Extent[1],fAx,fAz,fAy,fVx,fVz,
                    fVy,fIx,fIz,fIy,true);
            }
            else
            {
                // sphere is above the corner formed by faces x,y,z
                iRetVal = FindVertexRegionIntersection(m_pkBox->Extent[0],
                    m_pkBox->Extent[1],m_pkBox->Extent[2],fAx,fAy,fAz,fVx,fVy,
                    fVz,fIx,fIy,fIz);
            }
        }
    }

    if (iRetVal != 0 || m_fContactTime > fTMax)
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    // calculate actual intersection (move point back into world coordinates)
    m_iIntersectionType = IT_POINT;
    m_kContactPoint = m_pkBox->Center + (iSignX*fIx)*m_pkBox->Axis[0] +
        (iSignY*fIy)*m_pkBox->Axis[1] + (iSignZ*fIz)*m_pkBox->Axis[2];
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrBox3Sphere3<Real>::GetContactPoint () const
{
    return m_kContactPoint;
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrBox3Sphere3<Real>::GetVertexIntersection (Real fDx, Real fDy,
    Real fDz, Real fVx, Real fVy, Real fVz, Real fRSqr)
{
    // Finds the time of a 3D line-sphere intersection between a line
    // P = Dt, where P = (fDx, fDy, fDz) and D = (fVx, fVy, fVz) and
    // a sphere of radius^2 fRSqr.  Note: only valid if there is, in fact,
    // an intersection.

    Real fVSqr = fVx*fVx + fVy*fVy + fVz*fVz;
    Real fDot = fDx*fVx + fDy*fVy + fDz*fVz;
    Real fDiff = fDx*fDx + fDy*fDy + fDz*fDz - fRSqr;
    Real fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(fDot*fDot-fVSqr*fDiff));
    return fDiff*fInv/((Real)1.0-fDot*fInv);
}
//----------------------------------------------------------------------------
template <class Real>
Real IntrBox3Sphere3<Real>::GetEdgeIntersection (Real fDx, Real fDz, Real fVx,
    Real fVz, Real fVSqr, Real fRSqr)
{
    // Finds the time of a 2D line-circle intersection between a line
    // P = Dt where P = (fDx,fDz) and D = (fVx, fVz) and a circle of radius^2
    // fRSqr.  Note: only valid if there is, in fact, an intersection.

    Real fDot = fVx*fDx + fVz*fDz;
    Real fDiff = fDx*fDx + fDz*fDz - fRSqr;
    Real fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(fDot*fDot-fVSqr*fDiff));
    return fDiff*fInv/((Real)1.0-fDot*fInv);
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox3Sphere3<Real>::FindFaceRegionIntersection (Real fEx, Real fEy,
    Real fEz, Real fCx, Real fCy, Real fCz, Real fVx, Real fVy, Real fVz,
    Real& rfIx, Real& rfIy, Real& rfIz, bool bAboveFace)
{
    // Returns when and whether a sphere in the region above face +Z
    // intersects face +Z or any of its vertices or edges.  The input
    // bAboveFace is true when the x and y coordinates are within the x and y
    // extents.  The function will still work if they are not, but it needs
    // to be false then, to avoid some checks that assume that x and y are
    // within the extents.  This function checks face z, and the vertex and
    // two edges that the velocity is headed towards on the face.

    // check for already intersecting if above face
    if (fCz <= fEz + m_pkSphere->Radius && bAboveFace)
    {
        m_fContactTime = (Real)0.0;
        return -1;
    }

    // check for easy out (moving away on Z axis)
    if (fVz >= (Real)0.0)
    {
        return 0;
    }

    Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;

    Real fVSqrX = fVz*fVz + fVx*fVx;
    Real fVSqrY = fVz*fVz + fVy*fVy;
    Real fDx, fDy, fDz = fCz - fEz;
    Real fCrossX, fCrossY;
    int iXSign, iYSign;

    // This determines which way the box is heading and finds the values of
    // CrossX and CrossY which are positive if the sphere center will not
    // pass through the box.  Then it is only necessary to check two edges,
    // the face and the vertex for intersection.

    if (fVx >= (Real)0.0)
    {
        iXSign = 1;
        fDx = fCx - fEx;
        fCrossX = fVx*fDz - fVz*fDx;
    }
    else
    {
        iXSign = -1;
        fDx = fCx + fEx;
        fCrossX = fVz*fDx - fVx*fDz;
    }

    if (fVy >= (Real)0.0)
    {
        iYSign = 1;
        fDy = fCy - fEy;
        fCrossY = fVy*fDz - fVz*fDy;
    }
    else
    {
        iYSign = -1;
        fDy = fCy + fEy;
        fCrossY = fVz*fDy - fVy*fDz;
    }

    // does the circle intersect along the x edge?
    if (fCrossX > m_pkSphere->Radius*fVx*iXSign)
    {
        if (fCrossX*fCrossX > fRSqr*fVSqrX)
        {
            // sphere overshoots box on the x-axis (either side)
            return 0;
        }

        // does the circle hit the y edge?
        if (fCrossY > m_pkSphere->Radius*fVy*iYSign)
        {
            // potential vertex intersection
            if (fCrossY*fCrossY > fRSqr*fVSqrY)
            {
                // sphere overshoots box on the y-axis (either side)
                return 0;
            }

            Vector3<Real> kVel(fVx,fVy,fVz);
            Vector3<Real> kD(fDx,fDy,fDz);
            Vector3<Real> kCross = kD.Cross(kVel);
            if (kCross.SquaredLength() > fRSqr*kVel.SquaredLength())
            {
                // sphere overshoots the box on the corner
                return 0;
            }

            m_fContactTime = GetVertexIntersection(fDx,fDy,fDz,fVx,fVy,fVz,
                fRSqr);
            rfIx = fEx*iXSign;
            rfIy = fEy*iYSign;
        }
        else
        {
            // x-edge intersection
            m_fContactTime = GetEdgeIntersection(fDx,fDz,fVx,fVz,fVSqrX,
                fRSqr);
            rfIx = fEx*iXSign;
            rfIy = fCy + fVy*m_fContactTime;
        }
    }
    else
    {
        // does the circle hit the y edge?
        if (fCrossY > m_pkSphere->Radius*fVy*iYSign)
        {
            // potential y-edge intersection

            if (fCrossY*fCrossY > fRSqr*fVSqrY)
            {
                // sphere overshoots box on the y-axis (either side)
                return 0;
            }

            m_fContactTime = GetEdgeIntersection(fDy,fDz,fVy,fVz,fVSqrY,
                fRSqr);
            rfIx = fCx + fVx*m_fContactTime;
            rfIy = fEy*iYSign;
        }
        else
        {
            // face intersection (easy)
            m_fContactTime = (-fDz + m_pkSphere->Radius)/fVz;
            rfIx = m_fContactTime*fVx + fCx;
            rfIy = m_fContactTime*fVy + fCy;
        }
    }

    // z coordinate of any intersection must be the face of z
    rfIz = fEz;
    return 1;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox3Sphere3<Real>::FindJustEdgeIntersection (Real fCy, Real fEx,
    Real fEy, Real fEz, Real fDx, Real fDz, Real fVx, Real fVy, Real fVz,
    Real& rfIx, Real& rfIy, Real& rfIz)
{
    // Finds the intersection of a point fDx and fDz away from an edge with
    // direction y.  The sphere is at a point fCy, and the edge is at the
    // point fEx.  Checks the edge and the vertex the velocity is heading
    // towards.

    Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
    Real fDy, fCrossZ, fCrossX;  // possible edge/vertex intersection
    int iSignY;

    // Depending on the sign of Vy, pick the vertex that the velocity is
    // heading towards on the edge, as well as creating fCrossX and fCrossZ
    // such that their sign will always be positive if the sphere center goes
    // over that edge.

    if (fVy >= (Real)0.0)
    {
        iSignY = 1;
        fDy = fCy - fEy;
        fCrossZ = fDx*fVy - fDy*fVx;
        fCrossX = fDz*fVy - fDy*fVz;
    }
    else
    {
        iSignY = -1;
        fDy = fCy + fEy;
        fCrossZ = fDy*fVx - fDx*fVy;
        fCrossX = fDy*fVz - fDz*fVy;
    }

    // Check where on edge this intersection will occur
    if (fCrossZ >= (Real)0.0 && fCrossX >= (Real)0.0
    &&  fCrossX*fCrossX + fCrossZ*fCrossZ >
        fVy*fVy*m_pkSphere->Radius*m_pkSphere->Radius)
    {
        // sphere potentially intersects with vertex
        Vector3<Real> kVel(fVx,fVy,fVz);
        Vector3<Real> kD(fDx,fDy,fDz);
        Vector3<Real> kCross = kD.Cross(kVel);
        if (kCross.SquaredLength() > fRSqr*kVel.SquaredLength())
        {
            // sphere overshoots the box on the vertex
            return 0;
        }

        // sphere actually does intersect the vertex
        m_fContactTime = GetVertexIntersection(fDx,fDy,fDz,fVx,fVy,fVz,
            fRSqr);
        rfIx = fEx;
        rfIy = iSignY*fEy;
        rfIz = fEz;
    }
    else
    {
        // sphere intersects with edge
        Real fVSqrX = fVz*fVz + fVx*fVx;
        m_fContactTime = GetEdgeIntersection(fDx,fDz,fVx,fVz,fVSqrX,fRSqr);
        rfIx = fEx;
        rfIy = fCy + m_fContactTime*fVy;
        rfIz = fEz;
    }
    return 1;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox3Sphere3<Real>::FindEdgeRegionIntersection (Real fEx, Real fEy,
    Real fEz, Real fCx, Real fCy, Real fCz, Real fVx, Real fVy, Real fVz,
    Real& rfIx, Real& rfIy, Real& rfIz, bool bAboveEdge)
{
    // Assumes the sphere center is in the region above the x and z planes.
    // The input bAboveEdge is true when the y coordinate is within the y
    // extents.  The function will still work if it is not, but it needs to be
    // false then, to avoid some checks that assume that y is within the
    // extent.  This function checks the edge that the region is above, as
    // well as does a "face region" check on the face it is heading towards.

    Real fDx = fCx - fEx;
    Real fDz = fCz - fEz;
    Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;

    if (bAboveEdge)
    {
        Real fDiff = fDx*fDx + fDz*fDz - fRSqr;
        if (fDiff <= (Real)0.0)
        {
            // circle is already intersecting the box
            m_fContactTime = (Real)0.0;
            return -1;
        }
    }

    Real fDot = fVx*fDx + fVz*fDz;
    if (fDot >= (Real)0.0)
    {
        // circle not moving towards box
        return 0;
    }

    // The value fKross splits the region of interest along the edge in the
    // middle of that region.
    Real fKross = fVz*fDx - fVx*fDz; 
    if (fKross >= (Real)0.0)
    {
        // sphere moving towards +z face
        if (fVx >= (Real)0.0)
        {
            // passed corner, moving away from box
            return 0;
        }

        // Intersection with x-z edge.  If there is trouble with objects that
        // "scrape" the surface (velocity perpendicular to face normal, and
        // point of contact with a radius direction parallel to the face
        // normal), this check may need to be a little more inclusive (small
        // tolerance due to floating point errors) as the edge check needs
        // to get "scraping" objects (as they hit the edge with the point)
        // and the face region check will not catch it because the object is
        // not moving towards the face.
        if (fKross <= -m_pkSphere->Radius*fVx)
        {
            return FindJustEdgeIntersection(fCy,fEz,fEy,fEx,fDz,fDx,fVz,fVy,
                fVx,rfIz,rfIy,rfIx);
        }

        // now, check the face of z for intersections
        return FindFaceRegionIntersection(fEx,fEy,fEz,fCx,fCy,fCz,fVx,fVy,
            fVz,rfIx,rfIy,rfIz,false);
    }
    else
    {
        // sphere moving towards +x face
        if (fVz >= (Real)0.0)
        {
            // passed corner, moving away from box
            return 0;
        }

        // Check intersection with x-z edge.  See the note above above
        // "scraping" objects.
        if (fKross >= m_pkSphere->Radius*fVz)
        {
            // possible edge/vertex intersection
            return FindJustEdgeIntersection(fCy,fEx,fEy,fEz,fDx,fDz,fVx,fVy,
                fVz,rfIx,rfIy,rfIz);
        }

        // now, check the face of x for intersections
        return FindFaceRegionIntersection(fEz,fEy,fEx,fCz,fCy,fCx,fVz,fVy,
            fVx,rfIz,rfIy,rfIx,false);
    }
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox3Sphere3<Real>::FindVertexRegionIntersection (Real fEx, Real fEy,
    Real fEz, Real fCx, Real fCy, Real fCz, Real fVx, Real fVy, Real fVz,
    Real& rfIx, Real& rfIy, Real& rfIz)
{
    // assumes the sphere is above the vertex +Ex, +Ey, +Ez

    Real fDx = fCx - fEx;
    Real fDy = fCy - fEy;
    Real fDz = fCz - fEz;
    Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
    Real fDiff = fDx*fDx + fDy*fDy + fDz*fDz - fRSqr;
    if (fDiff <= (Real)0.0)
    {
        // sphere is already intersecting the box
        m_fContactTime = (Real)0.0;
        return -1;
    }

    if (fVx*fDx + fVy*fDy + fVz*fDz >= (Real)0.0)
    {
        // sphere not moving towards box
        return 0;
    }

    // The box can be divided up into 3 regions, which simplifies checking to
    // see what the sphere hits.  The regions are divided by which edge
    // (formed by the vertex and some axis) is closest to the velocity
    // vector.
    //
    // To check if it hits the vertex, look at the edge (E) it is going
    // towards.  Create a plane formed by the other two edges (with E as the
    // plane normal) with the vertex at the origin.  The intercept along an
    // axis, in that plane, of the line formed by the sphere center and the
    // velocity as its direction, will be fCrossAxis/fVEdge.  Thus, the
    // distance from the origin to the point in the plane that that line from
    // the sphere in the velocity direction crosses will be the squared sum
    // of those two intercepts.  If that sum is less than the radius squared,
    // then the sphere will hit the vertex otherwise, it will continue on
    // past the vertex.  If it misses, since it is known which edge the box
    // is near, the find edge region test can be used.
    //
    // Also, due to the constrained conditions, only those six cases (two for
    // each region, since fCrossEdge can be + or -) of signs of fCross values
    // can occur.
    //
    // The 3rd case will also pick up cases where D = V, causing a ZERO cross.

    Real fCrossX = fVy*fDz - fVz*fDy;
    Real fCrossY = fVx*fDz - fVz*fDx;
    Real fCrossZ = fVy*fDx - fVx*fDy;
    Real fCrX2 = fCrossX*fCrossX;
    Real fCrY2 = fCrossY*fCrossY;
    Real fCrZ2 = fCrossZ*fCrossZ;
    Real fVx2 = fVx*fVx;
    Real fVy2 = fVy*fVy;
    Real fVz2 = fVz*fVz;

    // Intersection with the vertex?
    if (fCrossY < (Real)0.0
    &&  fCrossZ >= (Real)0.0
    &&  fCrY2 + fCrZ2 <= fRSqr*fVx2
    
    ||  fCrossZ < (Real)0.0
    &&  fCrossX < (Real)0.0
    &&  fCrX2 + fCrZ2 <= fRSqr*fVy2
    
    ||  fCrossY >= (Real)0.0
    &&  fCrossX >= 0
    &&  fCrX2 + fCrY2 <= fRSqr*fVz2)
    {
        // standard line-sphere intersection
        m_fContactTime = GetVertexIntersection(fDx,fDy,fDz,fVx,fVy,fVz,
            m_pkSphere->Radius*m_pkSphere->Radius);
        rfIx = m_fContactTime*fVx + fCx;
        rfIy = m_fContactTime*fVy + fCy;
        rfIz = m_fContactTime*fVz + fCz;
        return 1;
    }
    else if (fCrossY < (Real)0.0 && fCrossZ >= (Real)0.0)
    {
        // x edge region, check y,z planes
        return FindEdgeRegionIntersection(fEy,fEx,fEz,fCy,fCx,fCz,fVy,fVx,
            fVz,rfIy,rfIx,rfIz,false);
    }
    else if (fCrossZ < (Real)0.0 && fCrossX < (Real)0.0)
    {
        // y edge region, check x,z planes
        return FindEdgeRegionIntersection(fEx,fEy,fEz,fCx,fCy,fCz,fVx,fVy,
            fVz,rfIx,rfIy,rfIz,false);
    }
    else // fCrossY >= 0 && fCrossX >= 0
    {
        // z edge region, check x,y planes
        return FindEdgeRegionIntersection(fEx,fEz,fEy,fCx,fCz,fCy,fVx,fVz,
            fVy,rfIx,rfIz,rfIy,false);        
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrBox3Sphere3<float>;

template WM4_FOUNDATION_ITEM
class IntrBox3Sphere3<double>;
//----------------------------------------------------------------------------
}
