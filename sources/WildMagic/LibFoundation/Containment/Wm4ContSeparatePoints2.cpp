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
#include "Wm4ContSeparatePoints2.h"
#include "Wm4ConvexHull2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
SeparatePoints2<Real>::SeparatePoints2 (int iQuantity0,
    const Vector2<Real>* akVertex0, int iQuantity1,
    const Vector2<Real>* akVertex1, Line2<Real>& rkSeprLine)
{
    // construct convex hull of point set 0
    ConvexHull2<Real> kHull0(iQuantity0,(Vector2<Real>*)akVertex0,0.001f,
        false,Query::QT_INT64);
    assert(kHull0.GetDimension() == 2);
    int iEdgeQuantity0 = kHull0.GetSimplexQuantity();
    const int* aiEdge0 = kHull0.GetIndices();

    // construct convex hull of point set 1
    ConvexHull2<Real> kHull1(iQuantity1,(Vector2<Real>*)akVertex1,0.001f,
        false,Query::QT_INT64);
    assert(kHull1.GetDimension() == 2);
    int iEdgeQuantity1 = kHull1.GetSimplexQuantity();
    const int* aiEdge1 = kHull1.GetIndices();

    // test edges of hull 0 for possible separation of points
    int j0, j1, iI0, iI1, iSide0, iSide1;
    Vector2<Real> kLineNormal;
    Real fLineConstant;
    for (j1 = 0, j0 = iEdgeQuantity0-1; j1 < iEdgeQuantity0; j0 = j1++)
    {
        // lookup edge (assert: iI0 != iI1 )
        iI0 = aiEdge0[j0];
        iI1 = aiEdge0[j1];

        // compute potential separating line (assert: (xNor,yNor) != (0,0))
        rkSeprLine.Origin = akVertex0[iI0];
        rkSeprLine.Direction = akVertex0[iI1] - akVertex0[iI0];
        rkSeprLine.Direction.Normalize();
        kLineNormal = rkSeprLine.Direction.Perp();
        fLineConstant = kLineNormal.Dot(rkSeprLine.Origin);

        // determine if hull 1 is on same side of line
        iSide1 = OnSameSide(kLineNormal,fLineConstant,iEdgeQuantity1,aiEdge1,
            akVertex1);

        if (iSide1)
        {
            // determine which side of line hull 0 lies
            iSide0 = WhichSide(kLineNormal,fLineConstant,iEdgeQuantity0,
                aiEdge0,akVertex0);

            if (iSide0*iSide1 <= 0)  // line separates hulls
            {
                m_bSeparated = true;
                return;
            }
        }
    }

    // test edges of hull 1 for possible separation of points
    for (j1 = 0, j0 = iEdgeQuantity1-1; j1 < iEdgeQuantity1; j0 = j1++)
    {
        // lookup edge (assert: iI0 != iI1 )
        iI0 = aiEdge1[j0];
        iI1 = aiEdge1[j1];

        // compute perpendicular to edge (assert: (xNor,yNor) != (0,0))
        rkSeprLine.Origin = akVertex1[iI0];
        rkSeprLine.Direction = akVertex1[iI1] - akVertex1[iI0];
        rkSeprLine.Direction.Normalize();
        kLineNormal = rkSeprLine.Direction.Perp();
        fLineConstant = kLineNormal.Dot(rkSeprLine.Origin);

        // determine if hull 0 is on same side of line
        iSide0 = OnSameSide(kLineNormal,fLineConstant,iEdgeQuantity0,aiEdge0,
            akVertex0);

        if (iSide0)
        {
            // determine which side of line hull 1 lies
            iSide1 = WhichSide(kLineNormal,fLineConstant,iEdgeQuantity1,
                aiEdge1,akVertex1);

            if (iSide0*iSide1 <= 0)  // line separates hulls
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
SeparatePoints2<Real>::operator bool ()
{
    return m_bSeparated;
}
//----------------------------------------------------------------------------
template <class Real>
int SeparatePoints2<Real>::OnSameSide (const Vector2<Real>& rkLineNormal,
    Real fLineConstant, int iEdgeQuantity, const int* aiEdge,
    const Vector2<Real>* akPoint)
{
    // test if all points on same side of line Dot(N,X) = c
    Real fC0;
    int iPosSide = 0, iNegSide = 0;

    for (int i1 = 0, i0 = iEdgeQuantity-1; i1 < iEdgeQuantity; i0 = i1++)
    {
        fC0 = rkLineNormal.Dot(akPoint[aiEdge[i0]]);
        if (fC0 > fLineConstant + Math<Real>::ZERO_TOLERANCE)
        {
            iPosSide++;
        }
        else if (fC0 < fLineConstant - Math<Real>::ZERO_TOLERANCE)
        {
            iNegSide++;
        }
        
        if (iPosSide && iNegSide)
        {
            // line splits point set
            return 0;
        }

        fC0 = rkLineNormal.Dot(akPoint[aiEdge[i1]]);
        if (fC0 > fLineConstant + Math<Real>::ZERO_TOLERANCE)
        {
            iPosSide++;
        }
        else if (fC0 < fLineConstant - Math<Real>::ZERO_TOLERANCE)
        {
            iNegSide++;
        }
        
        if (iPosSide && iNegSide)
        {
            // line splits point set
            return 0;
        }
    }

    return (iPosSide ? +1 : -1);
}
//----------------------------------------------------------------------------
template <class Real>
int SeparatePoints2<Real>::WhichSide (const Vector2<Real>& rkLineNormal,
    Real fLineConstant, int iEdgeQuantity, const int* aiEdge,
    const Vector2<Real>* akPoint)
{
    // establish which side of line hull is on
    Real fC0;
    for (int i1 = 0, i0 = iEdgeQuantity-1; i1 < iEdgeQuantity; i0 = i1++)
    {
        fC0 = rkLineNormal.Dot(akPoint[aiEdge[i0]]);
        if (fC0 > fLineConstant + Math<Real>::ZERO_TOLERANCE)
        {
            // hull on positive side
            return +1;
        }
        if (fC0 < fLineConstant - Math<Real>::ZERO_TOLERANCE)
        {
            // hull on negative side
            return -1;
        }

        fC0 = rkLineNormal.Dot(akPoint[aiEdge[i1]]);
        if (fC0 > fLineConstant + Math<Real>::ZERO_TOLERANCE)
        {
            // hull on positive side
            return +1;
        }
        if (fC0 < fLineConstant - Math<Real>::ZERO_TOLERANCE)
        {
            // hull on negative side
            return -1;
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
class SeparatePoints2<float>;

template WM4_FOUNDATION_ITEM
class SeparatePoints2<double>;
//----------------------------------------------------------------------------
}
