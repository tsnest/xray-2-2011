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
// Version: 4.6.1 (2007/09/01)

#include "Wm4FoundationPCH.h"
#include "Wm4ContPointInPolyhedron3.h"
#include "Wm4ContPointInPolygon2.h"
#include "Wm4IntrRay3Plane3.h"
#include "Wm4IntrRay3Triangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
PointInPolyhedron3<Real>::PointInPolyhedron3 (int iVQuantity,
    const Vector3<Real>* akVertex, int iFQuantity, const TriangleFace* akFace,
    int iRayQuantity, const Vector3<Real>* akDirection)
{
    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_iFQuantity = iFQuantity;
    m_akTFace = akFace;
    m_akCFace = 0;
    m_akSFace = 0;
    m_uiMethod = 0;
    m_iRayQuantity = iRayQuantity;
    m_akDirection = akDirection;
}
//----------------------------------------------------------------------------
template <class Real>
PointInPolyhedron3<Real>::PointInPolyhedron3 (int iVQuantity,
    const Vector3<Real>* akVertex, int iFQuantity, const ConvexFace* akFace,
    int iRayQuantity, const Vector3<Real>* akDirection, unsigned int uiMethod)
{
    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_iFQuantity = iFQuantity;
    m_akTFace = 0;
    m_akCFace = akFace;
    m_akSFace = 0;
    m_uiMethod = uiMethod;
    m_iRayQuantity = iRayQuantity;
    m_akDirection = akDirection;
}
//----------------------------------------------------------------------------
template <class Real>
PointInPolyhedron3<Real>::PointInPolyhedron3 (int iVQuantity,
    const Vector3<Real>* akVertex, int iFQuantity, const SimpleFace* akFace,
    int iRayQuantity, const Vector3<Real>* akDirection, unsigned int uiMethod)
{
    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_iFQuantity = iFQuantity;
    m_akTFace = 0;
    m_akCFace = 0;
    m_akSFace = akFace;
    m_uiMethod = uiMethod;
    m_iRayQuantity = iRayQuantity;
    m_akDirection = akDirection;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolyhedron3<Real>::Contains (const Vector3<Real>& rkP) const
{
    if (m_akTFace)
    {
        return ContainsT0(rkP);
    }

    if (m_akCFace)
    {
        if (m_uiMethod == 0)
        {
            return ContainsC0(rkP);
        }

        return ContainsC1C2(rkP,m_uiMethod);
    }

    if (m_akSFace)
    {
        if (m_uiMethod == 0)
        {
            return ContainsS0(rkP);
        }

        if (m_uiMethod == 1)
        {
            return ContainsS1(rkP);
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolyhedron3<Real>::FastNoIntersect (const Ray3<Real>& rkRay,
    const Plane3<Real>& rkPlane)
{
    Real fPDistance = rkPlane.Normal.Dot(rkRay.Origin) - rkPlane.Constant;
    Real fPAngle = rkPlane.Normal.Dot(rkRay.Direction);

    if (fPDistance < (Real)0.0)
    {
        // The ray origin is on the negative side of the plane.
        if (fPAngle <= (Real)0.0)
        {
            // The ray points away from the plane.
            return true;
        }
    }

    if (fPDistance > (Real)0.0)
    {
        // The ray origin is on the positive side of the plane.
        if (fPAngle >= (Real)0.0)
        {
            // The ray points away from the plane.
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolyhedron3<Real>::ContainsT0 (const Vector3<Real>& rkP) const
{
    int iInsideCount = 0;

    Triangle3<Real> kTriangle;
    Ray3<Real> kRay;
    kRay.Origin = rkP;

    for (int j = 0; j < m_iRayQuantity; j++)
    {
        kRay.Direction = m_akDirection[j];

        // Zero intersections to start with.
        bool bOdd = false;

        const TriangleFace* pkFace = m_akTFace;
        for (int i = 0; i < m_iFQuantity; i++, pkFace++)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(kRay,pkFace->Plane))
            {
                continue;
            }

            // Get the triangle vertices.
            for (int j = 0; j < 3; j++)
            {
                kTriangle.V[j] = m_akVertex[pkFace->Indices[j]];
            }

            // Test for intersection.
            if (IntrRay3Triangle3<Real>(kRay,kTriangle).Test())
            {
                // The ray intersects the triangle.
                bOdd = !bOdd;
            }
        }

        if (bOdd)
        {
            iInsideCount++;
        }
    }

    return iInsideCount > m_iRayQuantity/2;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolyhedron3<Real>::ContainsC0 (const Vector3<Real>& rkP) const
{
    int iInsideCount = 0;

    Triangle3<Real> kTriangle;
    Ray3<Real> kRay;
    kRay.Origin = rkP;

    for (int j = 0; j < m_iRayQuantity; j++)
    {
        kRay.Direction = m_akDirection[j];

        // Zero intersections to start with.
        bool bOdd = false;

        const ConvexFace* pkFace = m_akCFace;
        for (int i = 0; i < m_iFQuantity; i++, pkFace++)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(kRay,pkFace->Plane))
            {
                continue;
            }

            // Process the triangles in a trifan of the face.
            const int iNumVerticesM1 = (int)pkFace->Indices.size() - 1;
            kTriangle.V[0] = m_akVertex[pkFace->Indices[0]];
            for (int j = 1; j < iNumVerticesM1; j++)
            {
                kTriangle.V[1] = m_akVertex[pkFace->Indices[j]];
                kTriangle.V[2] = m_akVertex[pkFace->Indices[j + 1]];

                if (IntrRay3Triangle3<Real>(kRay,kTriangle).Test())
                {
                    // The ray intersects the triangle.
                    bOdd = !bOdd;
                }
            }
        }

        if (bOdd)
        {
            iInsideCount++;
        }
    }

    return iInsideCount > m_iRayQuantity/2;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolyhedron3<Real>::ContainsS0 (const Vector3<Real>& rkP) const
{
    int iInsideCount = 0;

    Triangle3<Real> kTriangle;
    Ray3<Real> kRay;
    kRay.Origin = rkP;

    for (int j = 0; j < m_iRayQuantity; j++)
    {
        kRay.Direction = m_akDirection[j];

        // Zero intersections to start with.
        bool bOdd = false;

        const SimpleFace* pkFace = m_akSFace;
        for (int i = 0; i < m_iFQuantity; i++, pkFace++)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(kRay,pkFace->Plane))
            {
                continue;
            }

            // The triangulation must exist to use it.
            const int iTQuantity = (int)pkFace->Triangles.size()/3;
            assert(iTQuantity > 0);

            // Process the triangles in a triangulation of the face.
            const int* piIndex = &pkFace->Triangles[0];
            for (int j = 0; j < iTQuantity; j++)
            {
                // Get the triangle vertices.
                for (int j = 0; j < 3; j++)
                {
                    kTriangle.V[j] = m_akVertex[*piIndex++];
                }

                // Test for intersection.
                if (IntrRay3Triangle3<Real>(kRay,kTriangle).Test())
                {
                    // The ray intersects the triangle.
                    bOdd = !bOdd;
                }
            }
        }

        if (bOdd)
        {
            iInsideCount++;
        }
    }

    return iInsideCount > m_iRayQuantity/2;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolyhedron3<Real>::ContainsC1C2 (const Vector3<Real>& rkP,
    unsigned int uiMethod) const
{
    int iInsideCount = 0;

    Ray3<Real> kRay;
    kRay.Origin = rkP;

    for (int j = 0; j < m_iRayQuantity; j++)
    {
        kRay.Direction = m_akDirection[j];

        // Zero intersections to start with.
        bool bOdd = false;

        const ConvexFace* pkFace = m_akCFace;
        for (int i = 0; i < m_iFQuantity; i++, pkFace++)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(kRay,pkFace->Plane))
            {
                continue;
            }

            // Compute the ray-plane intersection.
            IntrRay3Plane3<Real> kCalc(kRay,pkFace->Plane);
            bool bIntersects = kCalc.Find();

            // If you trigger this assertion, numerical round-off errors have
            // led to a discrepancy between FastNoIntersect and the Find()
            // result.
            assert(bIntersects);
            (void)bIntersects;  // Avoid a warning in release builds.

            Vector3<Real> kIntersect = kRay.Origin +
                kCalc.GetRayT()*kRay.Direction;

            // Get a coordinate system for the plane.  Use vertex 0 as the
            // origin.
            const Vector3<Real>& rkV0 = m_akVertex[pkFace->Indices[0]];
            Vector3<Real> kU0, kU1;
            Vector3<Real>::GenerateComplementBasis(kU0,kU1,
                pkFace->Plane.Normal);

            // Project the intersection onto the plane.
            Vector3<Real> kDiff = kIntersect - rkV0;
            Vector2<Real> kProjIntersect(kU0.Dot(kDiff),kU1.Dot(kDiff));

            // Project the face vertices onto the plane of the face.
            if (pkFace->Indices.size() > m_kProjVertices.size())
            {
                m_kProjVertices.resize(pkFace->Indices.size());
            }

            // Project the remaining vertices.  Vertex 0 is always the origin.
            const int iIQuantity = (int)pkFace->Indices.size();
            m_kProjVertices[0] = Vector2<Real>::ZERO;
            for (int j = 1; j < iIQuantity; j++)
            {
                kDiff = m_akVertex[pkFace->Indices[j]] - rkV0;
                m_kProjVertices[j][0] = kU0.Dot(kDiff);
                m_kProjVertices[j][1] = kU1.Dot(kDiff);
            }

            // Test whether the intersection point is in the convex polygon.
            PointInPolygon2<Real> kPIP((int)m_kProjVertices.size(),
                &m_kProjVertices[0]);

            if (uiMethod == 1)
            {
                if (kPIP.ContainsConvexOrderN(kProjIntersect))
                {
                    // The ray intersects the triangle.
                    bOdd = !bOdd;
                }
            }
            else
            {
                if (kPIP.ContainsConvexOrderLogN(kProjIntersect))
                {
                    // The ray intersects the triangle.
                    bOdd = !bOdd;
                }
            }
        }

        if (bOdd)
        {
            iInsideCount++;
        }
    }

    return iInsideCount > m_iRayQuantity/2;
}
//----------------------------------------------------------------------------
template <class Real>
bool PointInPolyhedron3<Real>::ContainsS1 (const Vector3<Real>& rkP) const
{
    int iInsideCount = 0;

    Ray3<Real> kRay;
    kRay.Origin = rkP;

    for (int j = 0; j < m_iRayQuantity; j++)
    {
        kRay.Direction = m_akDirection[j];

        // Zero intersections to start with.
        bool bOdd = false;

        const SimpleFace* pkFace = m_akSFace;
        for (int i = 0; i < m_iFQuantity; i++, pkFace++)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(kRay,pkFace->Plane))
            {
                continue;
            }

            // Compute the ray-plane intersection.
            IntrRay3Plane3<Real> kCalc(kRay,pkFace->Plane);
            bool bIntersects = kCalc.Find();

            // If you trigger this assertion, numerical round-off errors have
            // led to a discrepancy between FastNoIntersect and the Find()
            // result.
            assert(bIntersects);
            (void)bIntersects;  // Avoid a warning in release builds.

            Vector3<Real> kIntersect = kRay.Origin +
                kCalc.GetRayT()*kRay.Direction;

            // Get a coordinate system for the plane.  Use vertex 0 as the
            // origin.
            const Vector3<Real>& rkV0 = m_akVertex[pkFace->Indices[0]];
            Vector3<Real> kU0, kU1;
            Vector3<Real>::GenerateComplementBasis(kU0,kU1,
                pkFace->Plane.Normal);

            // Project the intersection onto the plane.
            Vector3<Real> kDiff = kIntersect - rkV0;
            Vector2<Real> kProjIntersect(kU0.Dot(kDiff),kU1.Dot(kDiff));

            // Project the face vertices onto the plane of the face.
            if (pkFace->Indices.size() > m_kProjVertices.size())
            {
                m_kProjVertices.resize(pkFace->Indices.size());
            }

            // Project the remaining vertices.  Vertex 0 is always the origin.
            const int iIQuantity = (int)pkFace->Indices.size();
            m_kProjVertices[0] = Vector2<Real>::ZERO;
            for (int j = 1; j < iIQuantity; j++)
            {
                kDiff = m_akVertex[pkFace->Indices[j]] - rkV0;
                m_kProjVertices[j][0] = kU0.Dot(kDiff);
                m_kProjVertices[j][1] = kU1.Dot(kDiff);
            }

            // Test whether the intersection point is in the convex polygon.
            PointInPolygon2<Real> kPIP((int)m_kProjVertices.size(),
                &m_kProjVertices[0]);

            if (kPIP.Contains(kProjIntersect))
            {
                // The ray intersects the triangle.
                bOdd = !bOdd;
            }
        }

        if (bOdd)
        {
            iInsideCount++;
        }
    }

    return iInsideCount > m_iRayQuantity/2;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class PointInPolyhedron3<float>;

template WM4_FOUNDATION_ITEM
class PointInPolyhedron3<double>;
//----------------------------------------------------------------------------
}
