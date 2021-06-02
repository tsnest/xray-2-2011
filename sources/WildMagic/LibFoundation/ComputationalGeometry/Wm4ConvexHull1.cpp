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
#include "Wm4ConvexHull1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
ConvexHull1<Real>::ConvexHull1 (int iVertexQuantity, Real* afVertex,
    Real fEpsilon, bool bOwner, Query::Type eQueryType)
    :
    ConvexHull<Real>(iVertexQuantity,fEpsilon,bOwner,eQueryType)
{
    assert(afVertex);
    m_afVertex = afVertex;

    std::vector<SortedVertex> kArray(m_iVertexQuantity);
    int i;
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        kArray[i].Value = m_afVertex[i];
        kArray[i].Index = i;
    }
    std::sort(kArray.begin(),kArray.end());

    Real fRange = kArray[m_iVertexQuantity-1].Value - kArray[0].Value;
    if (fRange >= m_fEpsilon)
    {
        m_iDimension = 1;
        m_iSimplexQuantity = 2;
        m_aiIndex = WM4_NEW int[2];
        m_aiIndex[0] = kArray[0].Index;
        m_aiIndex[1] = kArray[m_iVertexQuantity-1].Index;
    }
}
//----------------------------------------------------------------------------
template <class Real>
ConvexHull1<Real>::~ConvexHull1 ()
{
    if (m_bOwner)
    {
        WM4_DELETE[] m_afVertex;
    }
}
//----------------------------------------------------------------------------
template <class Real>
const Real* ConvexHull1<Real>::GetVertices () const
{
    return m_afVertex;
}
//----------------------------------------------------------------------------
template <class Real>
ConvexHull1<Real>::ConvexHull1 (const char* acFilename)
    :
    ConvexHull<Real>(0,(Real)0.0,false,Query::QT_REAL)
{
    m_afVertex = 0;
    bool bLoaded = Load(acFilename);
    assert(bLoaded);
    (void)bLoaded;  // avoid warning in Release build
}
//----------------------------------------------------------------------------
template <class Real>
bool ConvexHull1<Real>::Load (const char* acFilename)
{
    FILE* pkIFile = System::Fopen(acFilename,"rb");
    if (!pkIFile)
    {
        return false;
    }

    ConvexHull<Real>::Load(pkIFile);

    if (m_bOwner)
    {
        WM4_DELETE[] m_afVertex;
    }

    m_bOwner = true;
    m_afVertex = WM4_NEW Real[m_iVertexQuantity];

    size_t uiSize = sizeof(Real);
    if (uiSize == 4)
    {
        System::Read4le(pkIFile,m_iVertexQuantity,m_afVertex);
    }
    else // uiSize == 8
    {
        System::Read8le(pkIFile,m_iVertexQuantity,m_afVertex);
    }

    System::Fclose(pkIFile);
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool ConvexHull1<Real>::Save (const char* acFilename) const
{
    FILE* pkOFile = System::Fopen(acFilename,"wb");
    if (!pkOFile)
    {
        return false;
    }

    ConvexHull<Real>::Save(pkOFile);

    size_t uiSize = sizeof(Real);
    if (uiSize == 4)
    {
        System::Write4le(pkOFile,m_iVertexQuantity,m_afVertex);
    }
    else // uiSize == 8
    {
        System::Write8le(pkOFile,m_iVertexQuantity,m_afVertex);
    }

    System::Fclose(pkOFile);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class ConvexHull1<float>;

template WM4_FOUNDATION_ITEM
class ConvexHull1<double>;
//----------------------------------------------------------------------------
}
