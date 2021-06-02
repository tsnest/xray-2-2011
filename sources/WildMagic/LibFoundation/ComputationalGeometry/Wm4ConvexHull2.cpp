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
#include "Wm4ConvexHull2.h"
#include "Wm4Mapper2.h"
#include "Wm4Query2Filtered.h"
#include "Wm4Query2Int64.h"
#include "Wm4Query2TInteger.h"
#include "Wm4Query2TRational.h"

namespace Wm4
{

//----------------------------------------------------------------------------
template <class Real>
ConvexHull2<Real>::ConvexHull2 (int iVertexQuantity, Vector2<Real>* akVertex,
    Real fEpsilon, bool bOwner, Query::Type eQueryType)
    :
    ConvexHull<Real>(iVertexQuantity,fEpsilon,bOwner,eQueryType),
    m_kLineOrigin(Vector2<Real>::ZERO),
    m_kLineDirection(Vector2<Real>::ZERO)
{
    assert(akVertex);
    m_akVertex = akVertex;
    m_akSVertex = 0;
    m_pkQuery = 0;

    Mapper2<Real> kMapper(m_iVertexQuantity,m_akVertex,m_fEpsilon);
    if (kMapper.GetDimension() == 0)
    {
        // The values of m_iDimension, m_aiIndex, and m_aiAdjacent were
        // already initialized by the ConvexHull base class.
        return;
    }

    if (kMapper.GetDimension() == 1)
    {
        // The set is (nearly) collinear.  The caller is responsible for
        // creating a ConvexHull1 object.
        m_iDimension = 1;
        m_kLineOrigin = kMapper.GetOrigin();
        m_kLineDirection = kMapper.GetDirection(0);
        return;
    }

    m_iDimension = 2;

    int i0 = kMapper.GetExtremeIndex(0);
    int i1 = kMapper.GetExtremeIndex(1);
    int i2 = kMapper.GetExtremeIndex(2);

    m_akSVertex = WM4_NEW Vector2<Real>[m_iVertexQuantity];
    int i;

    if (eQueryType != Query::QT_RATIONAL && eQueryType != Query::QT_FILTERED)
    {
        // Transform the vertices to the square [0,1]^2.
        Vector2<Real> kMin = kMapper.GetMin();
        Real fScale = ((Real)1.0)/kMapper.GetMaxRange();
        for (i = 0; i < m_iVertexQuantity; i++)
        {
            m_akSVertex[i] = (m_akVertex[i] - kMin)*fScale;
        }

        Real fExpand;
        if (eQueryType == Query::QT_INT64)
        {
            // Scale the vertices to the square [0,2^{20}]^2 to allow use of
            // 64-bit integers.
            fExpand = (Real)(1 << 20);
            m_pkQuery = WM4_NEW Query2Int64<Real>(m_iVertexQuantity,
                m_akSVertex);
        }
        else if (eQueryType == Query::QT_INTEGER)
        {
            // Scale the vertices to the square [0,2^{24}]^2 to allow use of
            // TInteger.
            fExpand = (Real)(1 << 24);
            m_pkQuery = WM4_NEW Query2TInteger<Real>(m_iVertexQuantity,
                m_akSVertex);
        }
        else  // eQueryType == Query::QT_REAL
        {
            // No scaling for floating point.
            fExpand = (Real)1.0;
            m_pkQuery = WM4_NEW Query2<Real>(m_iVertexQuantity,m_akSVertex);
        }

        for (i = 0; i < m_iVertexQuantity; i++)
        {
            m_akSVertex[i] *= fExpand;
        }
    }
    else
    {
        // No transformation needed for exact rational arithmetic or filtered
        // predicates.
        size_t uiSize = m_iVertexQuantity*sizeof(Vector2<Real>);
        System::Memcpy(m_akSVertex,uiSize,m_akVertex,uiSize);

        if (eQueryType == Query::QT_RATIONAL)
        {
            m_pkQuery = WM4_NEW Query2TRational<Real>(m_iVertexQuantity,
                m_akSVertex);
        }
        else // eQueryType == Query::QT_FILTERED
        {
            m_pkQuery = WM4_NEW Query2Filtered<Real>(m_iVertexQuantity,
                m_akSVertex,m_fEpsilon);
        }
    }

    HullEdge2<Real>* pkE0;
    HullEdge2<Real>* pkE1;
    HullEdge2<Real>* pkE2;

    if (kMapper.GetExtremeCCW())
    {
        pkE0 = WM4_NEW HullEdge2<Real>(i0,i1);
        pkE1 = WM4_NEW HullEdge2<Real>(i1,i2);
        pkE2 = WM4_NEW HullEdge2<Real>(i2,i0);
    }
    else
    {
        pkE0 = WM4_NEW HullEdge2<Real>(i0,i2);
        pkE1 = WM4_NEW HullEdge2<Real>(i2,i1);
        pkE2 = WM4_NEW HullEdge2<Real>(i1,i0);
    }

    pkE0->Insert(pkE2,pkE1);
    pkE1->Insert(pkE0,pkE2);
    pkE2->Insert(pkE1,pkE0);

    HullEdge2<Real>* pkHull = pkE0;
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        if (!Update(pkHull,i))
        {
            pkHull->DeleteAll();
            return;
        }
    }

    pkHull->GetIndices(m_iSimplexQuantity,m_aiIndex);
    pkHull->DeleteAll();
}
//----------------------------------------------------------------------------
template <class Real>
ConvexHull2<Real>::~ConvexHull2 ()
{
    if (m_bOwner)
    {
        WM4_DELETE[] m_akVertex;
    }
    WM4_DELETE[] m_akSVertex;
    WM4_DELETE m_pkQuery;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& ConvexHull2<Real>::GetLineOrigin () const
{
    return m_kLineOrigin;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& ConvexHull2<Real>::GetLineDirection () const
{
    return m_kLineDirection;
}
//----------------------------------------------------------------------------
template <class Real>
ConvexHull1<Real>* ConvexHull2<Real>::GetConvexHull1 () const
{
    assert(m_iDimension == 1);
    if (m_iDimension != 1)
    {
        return 0;
    }

    Real* afProjection = WM4_NEW Real[m_iVertexQuantity];
    for (int i = 0; i < m_iVertexQuantity; i++)
    {
        Vector2<Real> kDiff = m_akVertex[i] - m_kLineOrigin;
        afProjection[i] = m_kLineDirection.Dot(kDiff);
    }

    return WM4_NEW ConvexHull1<Real>(m_iVertexQuantity,afProjection,
        m_fEpsilon,true,m_eQueryType);
}
//----------------------------------------------------------------------------
template <class Real>
bool ConvexHull2<Real>::Update (HullEdge2<Real>*& rpkHull, int i)
{
    // Locate an edge visible to the input point (if possible).
    HullEdge2<Real>* pkVisible = 0;
    HullEdge2<Real>* pkCurrent = rpkHull;
    do
    {
        if (pkCurrent->GetSign(i,m_pkQuery) > 0)
        {
            pkVisible = pkCurrent;
            break;
        }

        pkCurrent = pkCurrent->A[1];
    }
    while (pkCurrent != rpkHull);

    if (!pkVisible)
    {
        // The point is inside the current hull; nothing to do.
        return true;
    }

    // Remove the visible edges.
    HullEdge2<Real>* pkAdj0 = pkVisible->A[0];
    assert(pkAdj0);
    if (!pkAdj0)
    {
        return false;
    }

    HullEdge2<Real>* pkAdj1 = pkVisible->A[1];
    assert(pkAdj1);
    if (!pkAdj1)
    {
        return false;
    }

    pkVisible->DeleteSelf();

    while (pkAdj0->GetSign(i,m_pkQuery) > 0)
    {
        rpkHull = pkAdj0;
        pkAdj0 = pkAdj0->A[0];
        assert(pkAdj0);
        if (!pkAdj0)
        {
            return false;
        }

        pkAdj0->A[1]->DeleteSelf();
    }

    while (pkAdj1->GetSign(i,m_pkQuery) > 0)
    {
        rpkHull = pkAdj1;
        pkAdj1 = pkAdj1->A[1];
        assert(pkAdj1);
        if (!pkAdj1)
        {
            return false;
        }

        pkAdj1->A[0]->DeleteSelf();
    }

    // Insert the new edges formed by the input point and the end points of
    // the polyline of invisible edges.
    HullEdge2<Real>* pkEdge0 = WM4_NEW HullEdge2<Real>(pkAdj0->V[1],i);
    HullEdge2<Real>* pkEdge1 = WM4_NEW HullEdge2<Real>(i,pkAdj1->V[0]);
    pkEdge0->Insert(pkAdj0,pkEdge1);
    pkEdge1->Insert(pkEdge0,pkAdj1);
    rpkHull = pkEdge0;

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
ConvexHull2<Real>::ConvexHull2 (const char* acFilename)
    :
    ConvexHull<Real>(0,(Real)0.0,false,Query::QT_REAL)
{
    m_akVertex = 0;
    m_akSVertex = 0;
    m_pkQuery = 0;
    bool bLoaded = Load(acFilename);
    assert(bLoaded);
    (void)bLoaded;  // avoid warning in Release build
}
//----------------------------------------------------------------------------
template <class Real>
bool ConvexHull2<Real>::Load (const char* acFilename)
{
    FILE* pkIFile = System::Fopen(acFilename,"rb");
    if (!pkIFile)
    {
        return false;
    }

    ConvexHull<Real>::Load(pkIFile);

    WM4_DELETE m_pkQuery;
    WM4_DELETE[] m_akSVertex;
    if (m_bOwner)
    {
        WM4_DELETE[] m_akVertex;
    }

    m_bOwner = true;
    m_akVertex = WM4_NEW Vector2<Real>[m_iVertexQuantity];
    m_akSVertex = WM4_NEW Vector2<Real>[m_iVertexQuantity];

    size_t uiSize = sizeof(Real);
    int iVQ = 2*m_iVertexQuantity;
    if (uiSize == 4)
    {
        System::Read4le(pkIFile,iVQ,m_akVertex);
        System::Read4le(pkIFile,iVQ,m_akSVertex);
        System::Read4le(pkIFile,2,(Real*)m_kLineOrigin);
        System::Read4le(pkIFile,2,(Real*)m_kLineDirection);
    }
    else // iSize == 8
    {
        System::Read8le(pkIFile,iVQ,m_akVertex);
        System::Read8le(pkIFile,iVQ,m_akSVertex);
        System::Read8le(pkIFile,2,(Real*)m_kLineOrigin);
        System::Read8le(pkIFile,2,(Real*)m_kLineDirection);
    }

    System::Fclose(pkIFile);

    switch (m_eQueryType)
    {
    case Query::QT_INT64:
        m_pkQuery = WM4_NEW Query2Int64<Real>(m_iVertexQuantity,m_akSVertex);
        break;
    case Query::QT_INTEGER:
        m_pkQuery = WM4_NEW Query2TInteger<Real>(m_iVertexQuantity,
            m_akSVertex);
        break;
    case Query::QT_RATIONAL:
        m_pkQuery = WM4_NEW Query2TRational<Real>(m_iVertexQuantity,
            m_akSVertex);
        break;
    case Query::QT_REAL:
        m_pkQuery = WM4_NEW Query2<Real>(m_iVertexQuantity,m_akSVertex);
        break;
    case Query::QT_FILTERED:
        m_pkQuery = WM4_NEW Query2Filtered<Real>(m_iVertexQuantity,
            m_akSVertex,m_fEpsilon);
        break;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool ConvexHull2<Real>::Save (const char* acFilename) const
{
    FILE* pkOFile = System::Fopen(acFilename,"wb");
    if (!pkOFile)
    {
        return false;
    }

    ConvexHull<Real>::Save(pkOFile);

    size_t uiSize = sizeof(Real);
    int iVQ = 2*m_iVertexQuantity;
    if (uiSize == 4)
    {
        System::Write4le(pkOFile,iVQ,m_akVertex);
        System::Write4le(pkOFile,iVQ,m_akSVertex);
        System::Write4le(pkOFile,2,(const Real*)m_kLineOrigin);
        System::Write4le(pkOFile,2,(const Real*)m_kLineDirection);
    }
    else // iSize == 8
    {
        System::Write8le(pkOFile,iVQ,m_akVertex);
        System::Write8le(pkOFile,iVQ,m_akSVertex);
        System::Write8le(pkOFile,2,(const Real*)m_kLineOrigin);
        System::Write8le(pkOFile,2,(const Real*)m_kLineDirection);
    }

    System::Fclose(pkOFile);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class ConvexHull2<float>;

template WM4_FOUNDATION_ITEM
class ConvexHull2<double>;
//----------------------------------------------------------------------------
}
