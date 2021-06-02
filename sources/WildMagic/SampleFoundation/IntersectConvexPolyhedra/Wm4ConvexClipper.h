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

#ifndef WM4CONVEXCLIPPER_H
#define WM4CONVEXCLIPPER_H

#include "Wm4Plane3.h"

namespace Wm4
{

template <class Real> class ConvexPolyhedron;
template <class Real> class Plane3;

template <class Real>
class ConvexClipper
{
public:
    class Vertex
    {
    public:
        Vertex ()
        {
            // m_kPoint and m_iOccurs are uninitialized
            m_fDistance = (Real)0;
            m_bVisible = true;
        }

        Vector3<Real> m_kPoint;
        Real m_fDistance;
        int m_iOccurs;
        bool m_bVisible;
    };

    class Edge
    {
    public:
        Edge ()
        {
            // m_aiVertex and m_aiFace are uninitialized
            m_bVisible = true;
        }

        int m_aiVertex[2];
        int m_aiFace[2];
        bool m_bVisible;
    };

    class Face
    {
    public:
        Face ()
        {
            // m_kPlane is uninitialized, m_akEdge is empty
            m_bVisible = true;
        }

        Plane3<Real> m_kPlane;
        std::set<int> m_akEdge;
        bool m_bVisible;
    };

    // Construction.
    ConvexClipper (const ConvexPolyhedron<Real>& rkPoly,
        Real fEpsilon = Math<Real>::ZERO_TOLERANCE);

    // Discard the portion of the mesh on the negative side of the plane.
    // This function is valid for any manifold triangle mesh (at most two
    // triangles shared per edge).
    int Clip (const Plane3<Real>& rkPlane);

    // convert back to a convex polyhedron
    void Convert (ConvexPolyhedron<Real>& rkPoly);

    // for debugging
    bool Print (const char* acFilename) const;

protected:
    // support for post-processing faces
    class EdgePlus
    {
    public:
        EdgePlus ()
        {
        }

        EdgePlus (int iE, const Edge& rkE)
        {
            m_iE = iE;
            m_iF0 = rkE.m_aiFace[0];
            m_iF1 = rkE.m_aiFace[1];

            if (rkE.m_aiVertex[0] < rkE.m_aiVertex[1])
            {
                m_iV0 = rkE.m_aiVertex[0];
                m_iV1 = rkE.m_aiVertex[1];
            }
            else
            {
                m_iV0 = rkE.m_aiVertex[1];
                m_iV1 = rkE.m_aiVertex[0];
            }
        }

        bool operator< (const EdgePlus& rkE) const
        {
            if (m_iV1 < rkE.m_iV1)
            {
                return true;
            }

            if (m_iV1 == rkE.m_iV1)
            {
                return m_iV0 < rkE.m_iV0;
            }

            return false;
        }

        bool operator== (const EdgePlus& rkE) const
        {
            return m_iV0 == rkE.m_iV0 && m_iV1 == rkE.m_iV1;
        }

        bool operator!= (const EdgePlus& rkE) const
        {
            return m_iV0 != rkE.m_iV0 || m_iV1 != rkE.m_iV1;
        }

        int m_iE, m_iV0, m_iV1, m_iF0, m_iF1;
    };

    void PostProcess (int iNF, Face& rkNF);

    bool GetOpenPolyline (Face& rkF, int& riVStart, int& riVFinal);
    void OrderVertices (Face& rkF, std::vector<int>& raiOrdered);
    void GetTriangles (std::vector<int>& raiConnect,
        std::vector<Plane3<Real> >& rakPlane);

    std::vector<Vertex> m_akVertex;
    std::vector<Edge> m_akEdge;
    std::vector<Face> m_akFace;
    Real m_fEpsilon;
};

typedef ConvexClipper<float> ConvexClipperf;
typedef ConvexClipper<double> ConvexClipperd;

}

#endif
