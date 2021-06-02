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

#ifndef WM4CONVEXHULL3_H
#define WM4CONVEXHULL3_H

#include "Wm4FoundationLIB.h"
#include "Wm4ConvexHull1.h"
#include "Wm4ConvexHull2.h"
#include "Wm4HullTriangle3.h"
#include "Wm4Query3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM ConvexHull3 : public ConvexHull<Real>
{
public:
    // The input to the constructor is the array of vertices whose convex hull
    // is required.  If you want ConvexHull3 to delete the vertices during
    // destruction, set bOwner to 'true'.  Otherwise, you own the vertices and
    // must delete them yourself.
    //
    // You have a choice of speed versus accuracy.  The fastest choice is
    // Query::QT_INT64, but it gives up a lot of precision, scaling the points
    // to [0,2^{20}]^3.  The choice Query::QT_INTEGER gives up less precision,
    // scaling the points to [0,2^{24}]^3.  The choice Query::QT_RATIONAL uses
    // exact arithmetic, but is the slowest choice.  The choice Query::QT_REAL
    // uses floating-point arithmetic, but is not robust in all cases.

    ConvexHull3 (int iVertexQuantity, Vector3<Real>* akVertex, Real fEpsilon,
        bool bOwner, Query::Type eQueryType);
    virtual ~ConvexHull3 ();

    // If GetDimension() returns 1, then the points lie on a line.  You must
    // create a ConvexHull1 object using the function provided.
    const Vector3<Real>& GetLineOrigin () const;
    const Vector3<Real>& GetLineDirection () const;
    ConvexHull1<Real>* GetConvexHull1 () const;

    // If GetDimension() returns 2, then the points lie on a plane.  The plane
    // has two direction vectors (inputs 0 or 1).  You must create a
    // ConvexHull2 object using the function provided.
    const Vector3<Real>& GetPlaneOrigin () const;
    const Vector3<Real>& GetPlaneDirection (int i) const;
    ConvexHull2<Real>* GetConvexHull2 () const;

    // Support for streaming to/from disk.
    ConvexHull3 (const char* acFilename);
    bool Load (const char* acFilename);
    bool Save (const char* acFilename) const;

private:
    using ConvexHull<Real>::m_eQueryType;
    using ConvexHull<Real>::m_iVertexQuantity;
    using ConvexHull<Real>::m_iDimension;
    using ConvexHull<Real>::m_iSimplexQuantity;
    using ConvexHull<Real>::m_aiIndex;
    using ConvexHull<Real>::m_fEpsilon;
    using ConvexHull<Real>::m_bOwner;

    bool Update (int i);
    void ExtractIndices ();
    void DeleteHull ();

    // The input points.
    Vector3<Real>* m_akVertex;

    // Support for robust queries.
    Vector3<Real>* m_akSVertex;
    Query3<Real>* m_pkQuery;

    // The line of containment if the dimension is 1.
    Vector3<Real> m_kLineOrigin, m_kLineDirection;

    // The plane of containment if the dimension is 2.
    Vector3<Real> m_kPlaneOrigin, m_akPlaneDirection[2];

    // The current hull.
    std::set<HullTriangle3<Real>*> m_kHull;

    class TerminatorData
    {
    public:
        TerminatorData (int iV0 = -1, int iV1 = -1, int iNullIndex = -1,
            HullTriangle3<Real>* pkTri = 0)
        {
            V[0] = iV0;
            V[1] = iV1;
            NullIndex = iNullIndex;
            Tri = pkTri;
        }

        int V[2];
        int NullIndex;
        HullTriangle3<Real>* Tri;
    };
};

typedef ConvexHull3<float> ConvexHull3f;
typedef ConvexHull3<double> ConvexHull3d;

}

#endif
