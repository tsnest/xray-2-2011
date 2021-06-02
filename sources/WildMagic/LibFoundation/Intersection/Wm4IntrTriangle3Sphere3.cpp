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
#include "Wm4IntrTriangle3Sphere3.h"
#include "Wm4DistVector3Triangle3.h"
#include "Wm4IntrSegment3Sphere3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrTriangle3Sphere3<Real>::IntrTriangle3Sphere3 (
    const Triangle3<Real>& rkTriangle, const Sphere3<Real>& rkSphere)
    :
    m_pkTriangle(&rkTriangle),
    m_pkSphere(&rkSphere)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrTriangle3Sphere3<Real>::GetTriangle () const
{
    return *m_pkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrTriangle3Sphere3<Real>::GetSphere () const
{
    return *m_pkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Sphere3<Real>::Test ()
{
    DistVector3Triangle3<Real> kCalc(m_pkSphere->Center,*m_pkTriangle);
    Real fSqrDist = kCalc.GetSquared();
    Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
    return fSqrDist <= fRSqr;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Sphere3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    // Get the triangle vertices.
    const Vector3<Real>* akV = m_pkTriangle->V;

    // Get the triangle edges.
    Vector3<Real> akE[3] =
    {
        akV[1] - akV[0],
        akV[2] - akV[1], 
        akV[0] - akV[2]
    };

    // Get the triangle normal.
    Vector3<Real> kN = akE[1].Cross(akE[0]);

    // Sphere center projection on triangle normal.
    Real fNdC = kN.Dot(m_pkSphere->Center);

    // Radius projected length in normal direction.  This defers the square
    // root to normalize kN until absolutely needed.
    Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
    Real fNormRadiusSqr = kN.SquaredLength()*fRSqr;

    // Triangle projection on triangle normal.
    Real fNdT = kN.Dot(akV[0]);

    // Distance from sphere to triangle along the normal.
    Real fDist = fNdC - fNdT;
    
    // Normals for the plane formed by edge i and the triangle normal.
    Vector3<Real> akExN[3] =
    {
        akE[0].Cross(kN),
        akE[1].Cross(kN),
        akE[2].Cross(kN)
    };

    Segment3<Real> kSeg;

    if (fDist*fDist <= fNormRadiusSqr)
    {
        // Sphere currently intersects the plane of the triangle.

        // See which edges the sphere center is inside/outside.
        bool bInside[3];
        for (int i = 0; i < 3; i++ )
        {
            bInside[i] =
                (akExN[i].Dot(m_pkSphere->Center) >= akExN[i].Dot(akV[i]));
        }

        if (bInside[0])
        {
            if (bInside[1])
            {
                if (bInside[2])
                {
                    // Triangle inside sphere.
                    return false;
                }
                else // !bInside[2]
                {
                    // Potential intersection with edge <V2,V0>.
                    kSeg = Segment3<Real>(akV[2],akV[0]);
                    IntrSegment3Sphere3<Real> kCalc(kSeg,*m_pkSphere);
                    if (kCalc.Find(fTMax,rkVelocity0,rkVelocity1))
                    {
                        m_fContactTime = kCalc.GetContactTime();
                        m_kPoint = kCalc.GetPoint(0);
                    }
                    return false;
                }
            }
            else // !bInside[1]
            {
                if (bInside[2])
                {
                    // Potential intersection with edge <V1,V2>.
                    kSeg = Segment3<Real>(akV[1],akV[2]);
                    IntrSegment3Sphere3<Real> kCalc(kSeg,*m_pkSphere);
                    if (kCalc.Find(fTMax,rkVelocity0,rkVelocity1))
                    {
                        m_fContactTime = kCalc.GetContactTime();
                        m_kPoint = kCalc.GetPoint(0);
                    }
                    return false;
                }
                else // !bInside[2]
                {
                    // Potential intersection with edges <V1,V2>, <V2,V0>.
                    return FindTriangleSphereCoplanarIntersection(2,akV,
                        akExN[2],akE[2],fTMax,rkVelocity0,rkVelocity1);
                }            
            }
        } 
        else // !bInside[0]
        {
            if (bInside[1])
            {
                if (bInside[2])
                {
                    // Potential intersection with edge <V0,V1>.
                    kSeg = Segment3<Real>(akV[0],akV[1]);
                    IntrSegment3Sphere3<Real> kCalc(kSeg,*m_pkSphere);
                    if (kCalc.Find(fTMax,rkVelocity0,rkVelocity1))
                    {
                        m_fContactTime = kCalc.GetContactTime();
                        m_kPoint = kCalc.GetPoint(0);
                    }
                    return false;
                }
                else // !bInside[2]
                {
                    // Potential intersection with edges <V2,V0>, <V0,V1>.
                    return FindTriangleSphereCoplanarIntersection(0,akV,
                        akExN[0],akE[0],fTMax,rkVelocity0,rkVelocity1);
                }
            }
            else // !bInside[1]
            {
                if (bInside[2])
                {
                    // Potential intersection with edges <V0,V1>, <V1,V2>.
                    return FindTriangleSphereCoplanarIntersection(1,akV,
                        akExN[1],akE[1],fTMax,rkVelocity0,rkVelocity1);
                }
                else // !bInside[2]
                {
                    // We should not get here.
                    assert(false);
                    return false;
                }            
            }
        }
    }
    else
    {
        // Sphere does not currently intersect the plane of the triangle.

        // Sphere moving, triangle stationary.
        Vector3<Real> kVel = rkVelocity1 - rkVelocity0;

        // Find point of intersection of the sphere and the triangle
        // plane.  Where this point occurs on the plane relative to the
        // triangle determines the potential kind of intersection.

        kN.Normalize();

        // Point on sphere we care about intersecting the triangle plane.
        Vector3<Real> kSpherePoint;

        // On which side of the triangle is the sphere?
        if (fNdC > fNdT)
        {
            // Positive side.
            if (kVel.Dot(kN) >= (Real)0)
            {
                // Moving away, easy out.
                return false;
            }

            kSpherePoint = m_pkSphere->Center - m_pkSphere->Radius*kN;
        }
        else
        {
            // Negative side.
            if (kVel.Dot(kN) <= (Real)0)
            {
                // Moving away, easy out.
                return false;
            }

            kSpherePoint = m_pkSphere->Center + m_pkSphere->Radius*kN;
        }

        // Find intersection of velocity ray and triangle plane.

        // Project ray and plane onto the plane normal.
        Real fPlane = kN.Dot(akV[0]);
        Real fPoint = kN.Dot(kSpherePoint);
        Real fVel = kN.Dot(kVel);
        Real fTime = (fPlane - fPoint)/fVel;

        // Where this intersects.
        Vector3<Real> kIntrPoint = kSpherePoint + fTime*kVel;

        // See which edges this intersection point is inside/outside.
        bool bInside[3];
        for (int i = 0; i < 3; i++)
        {
            bInside[i] = (akExN[i].Dot(kIntrPoint) >=  akExN[i].Dot(akV[i]));
        }

        if (bInside[0])
        {
            if (bInside[1])
            {
                if (bInside[2])
                {
                    // Intersects face at time fTime.
                    if (fTime > fTMax)
                    {
                        // Intersection after tMax.
                        return false;
                    }
                    else
                    {
                        // kIntrPoint is the point in space, assuming that 
                        // TriVel is 0.  Re-adjust the point to where it 
                        // should be, were it not.
                        m_fContactTime = fTime;
                        m_kPoint = kIntrPoint + fTime*rkVelocity0;
                        return true;
                    }
                }
                else // !bInside[2]
                {
                    // Potential intersection with edge <V2,V0>.
                    kSeg = Segment3<Real>(akV[2],akV[0]);
                    IntrSegment3Sphere3<Real> kCalc(kSeg,*m_pkSphere);
                    if (kCalc.Find(fTMax,rkVelocity0,rkVelocity1))
                    {
                        m_fContactTime = kCalc.GetContactTime();
                        m_kPoint = kCalc.GetPoint(0);
                    }
                    return false;
                }
            }
            else // !bInside[1]
            {
                if (bInside[2])
                {
                    // Potential intersection with edge <V1,V2>.
                    kSeg = Segment3<Real>(akV[1],akV[2]);
                    IntrSegment3Sphere3<Real> kCalc(kSeg,*m_pkSphere);
                    if (kCalc.Find(fTMax,rkVelocity0,rkVelocity1))
                    {
                        m_fContactTime = kCalc.GetContactTime();
                        m_kPoint = kCalc.GetPoint(0);
                    }
                    return false;
                }
                else // !bInside[2]
                {
                    // Potential intersection with vertex V2.
                    return FindSphereVertexIntersection(akV[2],fTMax,
                        rkVelocity1,rkVelocity0);
                }            
            }
        } 
        else // !bInside[0]
        {
            if (bInside[1])
            {
                if (bInside[2])
                {
                    // Potential intersection with edge <V0,V1>.
                    kSeg = Segment3<Real>(akV[0],akV[1]);
                    IntrSegment3Sphere3<Real> kCalc(kSeg,*m_pkSphere);
                    if (kCalc.Find(fTMax,rkVelocity0,rkVelocity1))
                    {
                        m_fContactTime = kCalc.GetContactTime();
                        m_kPoint = kCalc.GetPoint(0);
                    }
                    return false;
                }
                else // !bInside[2]
                {
                    // Potential intersection with vertex V0.
                    return FindSphereVertexIntersection(akV[0],fTMax,
                        rkVelocity1,rkVelocity0);
                }
            }
            else // !bInside[1]
            {
                if (bInside[2])
                {
                    // Potential intersection with vertex V1.
                    return FindSphereVertexIntersection(akV[1],fTMax,
                        rkVelocity1,rkVelocity0);
                }
                else // !bInside[2]
                {
                    // We should not get here.
                    assert(false);
                    return false;
                }            
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrTriangle3Sphere3<Real>::GetPoint () const
{
    return m_kPoint;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Sphere3<Real>::FindTriangleSphereCoplanarIntersection (
    int iVertex, const Vector3<Real> akV[3], const Vector3<Real>& rkSideNorm,
    const Vector3<Real>& rkSide, Real fTMax, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    // iVertex is the "hinge" vertex that the two potential edges that can
    // be intersected by the sphere connect to, and it indexes into akV.
    //
    // rkSideNorm is the normal of the plane formed by (iVertex,iVertex+1)
    // and the tri norm, passed so as not to recalculate

    // Check for intersections at time 0.
    Vector3<Real> kDist = akV[iVertex] - m_pkSphere->Center;
    if (kDist.SquaredLength() < m_pkSphere->Radius*m_pkSphere->Radius)
    {
        // Already intersecting that vertex.
        m_fContactTime = (Real)0;
        return false;
    }

    // Tri stationary, sphere moving.
    Vector3<Real> kVel = rkVelocity1 - rkVelocity0;

    // Check for easy out.
    if (kVel.Dot(kDist) <= (Real)0)
    {
        // Moving away.
        return false;
    }

    // Find intersection of velocity ray and side normal.

    // Project ray and plane onto the plane normal.
    Real fPlane = rkSideNorm.Dot(akV[iVertex]);
    Real fCenter = rkSideNorm.Dot(m_pkSphere->Center);
    Real fVel = rkSideNorm.Dot(kVel);
    Real fFactor = (fPlane - fCenter)/fVel;
    Vector3<Real> kPoint = m_pkSphere->Center + fFactor*kVel;

    // Find which side of the hinge vertex this lies by projecting both the
    // vertex and this new point onto the triangle edge (the same edge whose
    // "normal" was used to find this point).
    Real fVertex = rkSide.Dot(akV[iVertex]);
    Real fPoint = rkSide.Dot(kPoint);
    Vector3<Real> kEnd0 = akV[iVertex], kEnd1;
    if (fPoint >= fVertex)
    {
        // Intersection with edge (iVertex,iVertex+1).
        kEnd1 = akV[(iVertex+1) % 3];
    }
    else
    {
        // Intersection with edge (iVertex-1,iVertex).
        if (iVertex != 0)
        {
            kEnd1 = akV[iVertex-1];
        }
        else
        {
            kEnd1 = akV[2];
        }
    }
    Segment3<Real> kSeg(kEnd0,kEnd1);

    // This could be either an sphere-edge or a sphere-vertex intersection
    // (this test isn't enough to differentiate), so use the full-on
    // line-sphere test.
    IntrSegment3Sphere3<Real> kCalc(kSeg,*m_pkSphere);
    if (kCalc.Find(fTMax,rkVelocity0,rkVelocity1))
    {
        m_fContactTime = kCalc.GetContactTime();
        m_kPoint = kCalc.GetPoint(0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Sphere3<Real>::FindSphereVertexIntersection (
    const Vector3<Real>& rkVertex, Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    // Finds the time and place (and possible occurrence it may miss) of an
    // intersection between a sphere of fRadius at rkOrigin moving in rkDir
    // towards a vertex at rkVertex.

    Vector3<Real> kVel = rkVelocity1 - rkVelocity0;
    Vector3<Real> kD = m_pkSphere->Center - rkVertex;
    Vector3<Real> kCross = kD.Cross(kVel);
    Real fRSqr = m_pkSphere->Radius*m_pkSphere->Radius;
    Real fVSqr = kVel.SquaredLength();

    if (kCross.SquaredLength() > fRSqr*fVSqr)
    {
        // The ray overshoots the sphere.
        return false;
    }

    // Find the time of intersection.
    Real fDot = kD.Dot(kVel);
    Real fDiff = kD.SquaredLength() - fRSqr;
    Real fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(fDot*fDot-fVSqr*fDiff));

    m_fContactTime = fDiff*fInv/((Real)1 - fDot*fInv);
    if (m_fContactTime > fTMax)
    {
        // The intersection occurs after max time.
        return false;
    }

    // The intersection is a triangle vertex.
    m_kPoint = rkVertex + m_fContactTime*rkVelocity0;
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrTriangle3Sphere3<float>;

template WM4_FOUNDATION_ITEM
class IntrTriangle3Sphere3<double>;
//----------------------------------------------------------------------------
}
