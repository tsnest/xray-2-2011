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
// Version: 4.0.0 (2006/06/28)

#ifndef WM4CONVEXHULL2_H
#define WM4CONVEXHULL2_H

#include "Wm4FoundationLIB.h"
#include "Wm4ConvexHull1.h"
#include "Wm4HullEdge2.h"
#include "Wm4Query2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM ConvexHull2 : public ConvexHull<Real>
{
public:
    // The input to the constructor is the array of vertices whose convex hull
    // is required.  If you want ConvexHull2 to delete the vertices during
    // destruction, set bOwner to 'true'.  Otherwise, you own the vertices and
    // must delete them yourself.
    //
    // You have a choice of speed versus accuracy.  The fastest choice is
    // Query::QT_INT64, but it gives up a lot of precision, scaling the points
    // to [0,2^{20}]^3.  The choice Query::QT_INTEGER gives up less precision,
    // scaling the points to [0,2^{24}]^3.  The choice Query::QT_RATIONAL uses
    // exact arithmetic, but is the slowest choice.  The choice Query::QT_REAL
    // uses floating-point arithmetic, but is not robust in all cases.

    ConvexHull2 (int iVertexQuantity, Vector2<Real>* akVertex, Real fEpsilon,
        bool bOwner, Query::Type eQueryType);
    virtual ~ConvexHull2 ();

    // If GetDimension() returns 1, then the points lie on a line.  You must
    // create a ConvexHull1 object using the function provided.
    const Vector2<Real>& GetLineOrigin () const;
    const Vector2<Real>& GetLineDirection () const;
    ConvexHull1<Real>* GetConvexHull1 () const;

private:
    using ConvexHull<Real>::m_eQueryType;
    using ConvexHull<Real>::m_iVertexQuantity;
    using ConvexHull<Real>::m_iDimension;
    using ConvexHull<Real>::m_iSimplexQuantity;
    using ConvexHull<Real>::m_aiIndex;
    using ConvexHull<Real>::m_fEpsilon;
    using ConvexHull<Real>::m_bOwner;

    // Support for streaming to/from disk.
    ConvexHull2 (const char* acFilename);
    bool Load (const char* acFilename);
    bool Save (const char* acFilename) const;

    bool Update (HullEdge2<Real>*& rpkHull, int i);

    // The input points.
    Vector2<Real>* m_akVertex;

    // Support for robust queries.
    Vector2<Real>* m_akSVertex;
    Query2<Real>* m_pkQuery;

    // The line of containment if the dimension is 1.
    Vector2<Real> m_kLineOrigin, m_kLineDirection;
};

typedef ConvexHull2<float> ConvexHull2f;
typedef ConvexHull2<double> ConvexHull2d;

}

#endif
