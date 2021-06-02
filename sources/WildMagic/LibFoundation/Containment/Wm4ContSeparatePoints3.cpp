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

#include "Wm4FoundationPCH.h"
#include "Wm4ContSeparatePoints3.h"
#include "Wm4ConvexHull3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
SeparatePoints3<Real>::SeparatePoints3 (int iQuantity0,
    const Vector3<Real>* akVertex0, int iQuantity1,
    const Vector3<Real>* akVertex1, Plane3<Real>& rkSeprPlane)
{
    // construct convex hull of point set 0
    ConvexHull3<Real> kHull0(iQuantity0,(Vector3<Real>*)akVertex0,0.001f,
        false,Query::QT_INT64);

    // code does not currently handle point/segment/polygon hull
    assert(kHull0.GetDimension() == 3);
    if (kHull0.GetDimension() < 3)
    {
        return;
    }

    int iTQuantity0 = kHull0.GetSimplexQuantity();
    const int* aiIndex0 = kHull0.GetIndices();

    // construct convex hull of point set 1
    ConvexHull3<Real> kHull1(iQuantity1,(Vector3<Real>*)akVertex1,0.001f,
        false,Query::QT_INT64);

    // code does not currently handle point/segment/polygon hull
    assert(kHull1.GetDimension() == 3);
    if (kHull1.GetDimension() < 3)
    {
        return;
    }

    int iTQuantity1 = kHull1.GetSimplexQuantity();
    const int* aiIndex1 = kHull1.GetIndices();

    // test faces of hull 0 for possible separation of points
    int i, i0, i1, i2, iSide0, iSide1;
    Vector3<Real> kDiff0, kDiff1;
    for (i = 0; i < iTQuantity0; i++)
    {
        // lookup face (assert: i0 != i1 && i0 != i2 && i1 != i2)
        i0 = aiIndex0[3*i  ];
        i1 = aiIndex0[3*i+1];
        i2 = aiIndex0[3*i+2];

        // compute potential separating plane (assert: normal != (0,0,0))
        rkSeprPlane = Plane3<Real>(akVertex0[i0],akVertex0[i1],akVertex0[i2]);

        // determine if hull 1 is on same side of plane
        iSide1 = OnSameSide(rkSeprPlane,iTQuantity1,aiIndex1,akVertex1);

        if (iSide1)
        {
            // determine which side of plane hull 0 lies
            iSide0 = WhichSide(rkSeprPlane,iTQuantity0,aiIndex0,akVertex0);
            if (iSide0*iSide1 <= 0)  // plane separates hulls
            {
                m_bSeparated = true;
                return;
            }
        }
    }

    // test faces of hull 1 for possible separation of points
    for (i = 0; i < iTQuantity1; i++)
    {
        // lookup edge (assert: i0 != i1 && i0 != i2 && i1 != i2)
        i0 = aiIndex1[3*i  ];
        i1 = aiIndex1[3*i+1];
        i2 = aiIndex1[3*i+2];

        // compute perpendicular to face (assert: normal != (0,0,0))
        rkSeprPlane = Plane3<Real>(akVertex1[i0],akVertex1[i1],akVertex1[i2]);

        // determine if hull 0 is on same side of plane
        iSide0 = OnSameSide(rkSeprPlane,iTQuantity0,aiIndex0,akVertex0);
        if (iSide0)
        {
            // determine which side of plane hull 1 lies
            iSide1 = WhichSide(rkSeprPlane,iTQuantity1,aiIndex1,akVertex1);
            if (iSide0*iSide1 <= 0)  // plane separates hulls
            {
                m_bSeparated = true;
                return;
            }
        }
    }

    // build edge set for hull 0
    std::set<std::pair<int,int> > kESet0;
    for (i = 0; i < iTQuantity0; i++)
    {
        // lookup face (assert: i0 != i1 && i0 != i2 && i1 != i2)
        i0 = aiIndex0[3*i  ];
        i1 = aiIndex0[3*i+1];
        i2 = aiIndex0[3*i+2];
        kESet0.insert(std::make_pair(i0,i1));
        kESet0.insert(std::make_pair(i0,i2));
        kESet0.insert(std::make_pair(i1,i2));
    }

    // build edge list for hull 1
    std::set<std::pair<int,int> > kESet1;
    for (i = 0; i < iTQuantity1; i++)
    {
        // lookup face (assert: i0 != i1 && i0 != i2 && i1 != i2)
        i0 = aiIndex1[3*i  ];
        i1 = aiIndex1[3*i+1];
        i2 = aiIndex1[3*i+2];
        kESet1.insert(std::make_pair(i0,i1));
        kESet1.insert(std::make_pair(i0,i2));
        kESet1.insert(std::make_pair(i1,i2));
    }

    // Test planes whose normals are cross products of two edges, one from
    // each hull.
    std::set<std::pair<int,int> >::iterator pkE0, pkE1;
    for (pkE0 = kESet0.begin(); pkE0 != kESet0.end(); pkE0++)
    {
        // get edge
        kDiff0 = akVertex0[pkE0->second] - akVertex0[pkE0->first];

        for (pkE1 = kESet1.begin(); pkE1 != kESet1.end(); pkE1++)
        {
            kDiff1 = akVertex1[pkE1->second] - akVertex1[pkE1->first];

            // compute potential separating plane
            rkSeprPlane.Normal = kDiff0.UnitCross(kDiff1);
            rkSeprPlane.Constant = rkSeprPlane.Normal.Dot(
                akVertex0[pkE0->first]);

            // determine if hull 0 is on same side of plane
            iSide0 = OnSameSide(rkSeprPlane,iTQuantity0,aiIndex0,
                akVertex0);
            iSide1 = OnSameSide(rkSeprPlane,iTQuantity1,aiIndex1,
                akVertex1);

            if (iSide0*iSide1 < 0)  // plane separates hulls
            {
                m_bSeparated = true;
                return;
            }
        }
    }

    m_bSeparated = false;
}
//----------------------------------------------------------------------------
template <class Real>
SeparatePoints3<Real>::operator bool ()
{
    return m_bSeparated;
}
//----------------------------------------------------------------------------
template <class Real>
int SeparatePoints3<Real>::OnSameSide (const Plane3<Real>& rkPlane,
    int iTriangleQuantity, const int* aiIndex, const Vector3<Real>* akPoint)
{
    // test if all points on same side of plane (nx,ny,nz)*(x,y,z) = c
    int iPosSide = 0, iNegSide = 0;

    for (int iT = 0; iT < iTriangleQuantity; iT++)
    {
        for (int i = 0; i < 3; i++)
        {
            int iV = aiIndex[3*iT+i];;
            Real fC0 = rkPlane.Normal.Dot(akPoint[iV]);
            if (fC0 > rkPlane.Constant + Math<Real>::ZERO_TOLERANCE)
            {
                iPosSide++;
            }
            else if (fC0 < rkPlane.Constant - Math<Real>::ZERO_TOLERANCE)
            {
                iNegSide++;
            }
            
            if (iPosSide && iNegSide)
            {
                // plane splits point set
                return 0;
            }
        }
    }

    return (iPosSide ? +1 : -1);
}
//----------------------------------------------------------------------------
template <class Real>
int SeparatePoints3<Real>::WhichSide (const Plane3<Real>& rkPlane,
    int iTriangleQuantity, const int* aiIndex, const Vector3<Real>* akPoint)
{
    // establish which side of plane hull is on
    for (int iT = 0; iT < iTriangleQuantity; iT++)
    {
        for (int i = 0; i < 3; i++)
        {
            int iV = aiIndex[3*iT+i];
            Real fC0 = rkPlane.Normal.Dot(akPoint[iV]);
            if (fC0 > rkPlane.Constant + Math<Real>::ZERO_TOLERANCE)
            {
                // positive side
                return +1;
            }
            if (fC0 < rkPlane.Constant - Math<Real>::ZERO_TOLERANCE)
            {
                // negative side
                return -1;
            }
        }
    }

    // hull is effectively collinear
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class SeparatePoints3<float>;

template WM4_FOUNDATION_ITEM
class SeparatePoints3<double>;
//----------------------------------------------------------------------------
}
