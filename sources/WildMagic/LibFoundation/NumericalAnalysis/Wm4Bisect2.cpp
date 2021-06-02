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
#include "Wm4Bisect2.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Bisect2<Real>::Bisect2 (Function oF, Function oG, int iMaxLevel,
    Real fTolerance)
{
    m_oF = oF;
    m_oG = oG;
    m_iMaxLevel = iMaxLevel;
    m_iLevel = 0;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect2<Real>::ZeroTest (Real fX, Real fY, Real& rfF, Real& rfG,
    Real& rfXRoot, Real& rfYRoot)
{
    rfF = m_oF(fX,fY);
    rfG = m_oG(fX,fY);

    if (Math<Real>::FAbs(rfF) <= m_fTolerance
    &&  Math<Real>::FAbs(rfG) <= m_fTolerance)
    {
        rfXRoot = fX;
        rfYRoot = fY;
        m_iLevel--;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
typename Bisect2<Real>::BisectNode* Bisect2<Real>::AddNode (Real fX, Real fY,
    Real fF, Real fG)
{
    BisectNode* pkTemp = WM4_NEW BisectNode;
    pkTemp->X = fX;
    pkTemp->Y = fY;
    pkTemp->F = fF;
    pkTemp->G = fG;
    return pkTemp;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect2<Real>::Bisect (Real fX0, Real fY0, Real fX1, Real fY1,
    Real& rfXRoot, Real& rfYRoot)
{
    // test four corner values
    if (ZeroTest(fX0,fY0,m_fF00,m_fG00,rfXRoot,rfYRoot))
        return true;

    if (ZeroTest(fX1,fY0,m_fF10,m_fG10,rfXRoot,rfYRoot))
        return true;

    if (ZeroTest(fX0,fY1,m_fF01,m_fG01,rfXRoot,rfYRoot))
        return true;

    if (ZeroTest(fX1,fY1,m_fF11,m_fG11,rfXRoot,rfYRoot))
        return true;

    // build initial quad

    // add pkN00
    m_pkGraph = WM4_NEW BisectNode;
    m_pkGraph->X = fX0;
    m_pkGraph->Y = fY0;
    m_pkGraph->F = m_fF00;
    m_pkGraph->G = m_fG00;

    // add pkN10
    BisectNode* pkTemp = AddNode(fX1,fY0,m_fF10,m_fG10);
    pkTemp->XNext = 0;
    m_pkGraph->XNext = pkTemp;

    // add pkN01
    pkTemp = AddNode(fX0,fY1,m_fF01,m_fG01);
    pkTemp->YNext = 0;
    m_pkGraph->YNext = pkTemp;

    // add pkN11
    pkTemp = AddNode(fX1,fY1,m_fF11,m_fG11);
    pkTemp->XNext = 0;
    pkTemp->YNext = 0;
    m_pkGraph->XNext->YNext = pkTemp;
    m_pkGraph->YNext->XNext = pkTemp;

    m_iLevel = 0;
    bool bResult = BisectRecurse(m_pkGraph);
    if (bResult)
    {
        rfXRoot = m_fXRoot;
        rfYRoot = m_fYRoot;
    }

    // remove remaining quad from m_pkGraph
    WM4_DELETE m_pkGraph->XNext->YNext;
    WM4_DELETE m_pkGraph->XNext;
    WM4_DELETE m_pkGraph->YNext;
    WM4_DELETE m_pkGraph;

    return bResult;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect2<Real>::BisectRecurse (BisectNode* pkN00)
{
    if (++m_iLevel == m_iMaxLevel)
    {
        m_iLevel--;
        return false;
    }

    BisectNode* pkN10 = pkN00->XNext;
    BisectNode* pkN11 = pkN10->YNext;
    BisectNode* pkN01 = pkN00->YNext;

    m_iNetSign = (int)(
        Math<Real>::Sign(pkN00->F) +
        Math<Real>::Sign(pkN01->F) +
        Math<Real>::Sign(pkN10->F) +
        Math<Real>::Sign(pkN11->F));

    if (abs(m_iNetSign) == 4)
    {
        // F has same sign at corners
        m_iLevel--;
        return false;
    }

    m_iNetSign = (int)(
        Math<Real>::Sign(pkN00->G) +
        Math<Real>::Sign(pkN01->G) +
        Math<Real>::Sign(pkN10->G) +
        Math<Real>::Sign(pkN11->G));

    if ( abs(m_iNetSign) == 4 )
    {
        // G has same sign at corners
        m_iLevel--;
        return false;
    }

    // bisect the quad
    m_fX0 = pkN00->X;
    m_fY0 = pkN00->Y;
    m_fX1 = pkN11->X;
    m_fY1 = pkN11->Y;
    m_fXm = ((Real)0.5)*(m_fX0+m_fX1);
    m_fYm = ((Real)0.5)*(m_fY0+m_fY1);

    // right edge 10,11
    if (ZeroTest(m_fX1,m_fYm,m_fF1m,m_fG1m,m_fXRoot,m_fYRoot))
        return true;
    
    // bottom edge 01,11
    if (ZeroTest(m_fXm,m_fY1,m_fFm1,m_fGm1,m_fXRoot,m_fYRoot))
        return true;

    // top edge 00,10
    if (ZeroTest(m_fXm,m_fY0,m_fFm0,m_fGm0,m_fXRoot,m_fYRoot))
        return true;

    // left edge 00,01
    if (ZeroTest(m_fX0,m_fYm,m_fF0m,m_fG0m,m_fXRoot,m_fYRoot))
        return true;

    // center
    if (ZeroTest(m_fXm,m_fYm,m_fFmm,m_fGmm,m_fXRoot,m_fYRoot))
        return true;

    // right bisector
    BisectNode* pkTemp = AddNode(m_fX1,m_fYm,m_fF1m,m_fG1m);
    pkTemp->XNext = 0;
    pkTemp->YNext = pkN11;
    pkN10->YNext = pkTemp;

    // bottom bisector
    pkTemp = AddNode(m_fXm,m_fY1,m_fFm1,m_fGm1);
    pkTemp->XNext = pkN11;
    pkTemp->YNext = 0;
    pkN01->XNext = pkTemp;

    // top bisector
    pkTemp = AddNode(m_fXm,m_fY0,m_fFm0,m_fGm0);
    pkTemp->XNext = pkN10;
    pkN00->XNext = pkTemp;

    // left bisector
    pkTemp = AddNode(m_fX0,m_fYm,m_fF0m,m_fG0m);
    pkTemp->YNext = pkN01;
    pkN00->YNext = pkTemp;

    // middle bisector
    pkTemp = AddNode(m_fXm,m_fYm,m_fFmm,m_fGmm);
    pkTemp->XNext = pkN10->YNext;
    pkTemp->YNext = pkN01->XNext;
    pkN00->XNext->YNext = pkTemp;
    pkN00->YNext->XNext = pkTemp;

    // Search the subquads for roots
    bool bResult =
        BisectRecurse(pkN00) ||
        BisectRecurse(pkN00->XNext) ||
        BisectRecurse(pkN00->YNext) ||
        BisectRecurse(pkN00->XNext->YNext);

    // entire subquad check failed, remove the nodes that were added

    // center
    WM4_DELETE pkN00->XNext->YNext;

    // edges
    WM4_DELETE pkN00->XNext;
    pkN00->XNext = pkN10;
    WM4_DELETE pkN00->YNext;
    pkN00->YNext = pkN01;
    WM4_DELETE pkN01->XNext;
    pkN01->XNext = pkN11;
    WM4_DELETE pkN10->YNext;
    pkN10->YNext = pkN11;

    m_iLevel--;
    return bResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class Bisect2<float>;

template WM4_FOUNDATION_ITEM
class Bisect2<double>;
//----------------------------------------------------------------------------
}
