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
#include "Wm4Bisect3.h"
#include "Wm4Math.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
Bisect3<Real>::Bisect3 (Function oF, Function oG, Function oH, int iMaxLevel,
    Real fTolerance)
{
    m_oF = oF;
    m_oG = oG;
    m_oH = oH;
    m_iMaxLevel = iMaxLevel;
    m_iLevel = 0;
    m_fTolerance = fTolerance;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect3<Real>::ZeroTest (Real fX, Real fY, Real fZ, Real& rfF, Real& rfG,
    Real& rfH, Real& rfXRoot, Real& rfYRoot, Real& rfZRoot)
{
    rfF = m_oF(fX,fY,fZ);
    rfG = m_oG(fX,fY,fZ);
    rfH = m_oH(fX,fY,fZ);

    if (Math<Real>::FAbs(rfF) <= m_fTolerance
    &&  Math<Real>::FAbs(rfG) <= m_fTolerance
    &&  Math<Real>::FAbs(rfH) <= m_fTolerance)
    {
        rfXRoot = fX;
        rfYRoot = fY;
        rfZRoot = fZ;
        m_iLevel--;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
typename Bisect3<Real>::BisectNode* Bisect3<Real>::AddNode (Real fX, Real fY,
    Real fZ, Real fF, Real fG, Real fH)
{
    BisectNode* pkTemp = WM4_NEW BisectNode;
    pkTemp->X = fX;
    pkTemp->Y = fY;
    pkTemp->Z = fZ;
    pkTemp->F = fF;
    pkTemp->G = fG;
    pkTemp->H = fH;
    return pkTemp;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect3<Real>::Bisect (Real fX0, Real fY0, Real fZ0, Real fX1,
    Real fY1, Real fZ1, Real& rfXRoot, Real& rfYRoot, Real& rfZRoot)
{
    // test eight corner values
    if (ZeroTest(fX0,fY0,fZ0,m_fF000,m_fG000,m_fH000,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    if (ZeroTest(fX1,fY0,fZ0,m_fF100,m_fG100,m_fH100,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    if (ZeroTest(fX0,fY1,fZ0,m_fF010,m_fG010,m_fH010,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    if (ZeroTest(fX1,fY1,fZ0,m_fF110,m_fG110,m_fH110,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    if (ZeroTest(fX0,fY0,fZ1,m_fF001,m_fG001,m_fH001,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    if (ZeroTest(fX1,fY0,fZ1,m_fF101,m_fG101,m_fH101,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    if (ZeroTest(fX0,fY1,fZ1,m_fF011,m_fG011,m_fH011,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    if (ZeroTest(fX1,fY1,fZ1,m_fF111,m_fG111,m_fH111,rfXRoot,rfYRoot,rfZRoot))
    {
        return true;
    }

    // build initial oct

    // add pkN000
    m_pkGraph = WM4_NEW BisectNode;
    m_pkGraph->X = fX0;
    m_pkGraph->Y = fY0;
    m_pkGraph->Z = fZ0;
    m_pkGraph->F = m_fF000;
    m_pkGraph->G = m_fG000;
    m_pkGraph->H = m_fH000;

    // add pkN100
    BisectNode* pkTemp = AddNode(fX1,fY0,fZ0,m_fF100,m_fG100,m_fH100);
    pkTemp->XNext = 0;
    m_pkGraph->XNext = pkTemp;

    // add pkN010
    pkTemp = AddNode(fX0,fY1,fZ0,m_fF010,m_fG010,m_fH010);
    pkTemp->YNext = 0;
    m_pkGraph->YNext = pkTemp;

    // add pkN110
    AddNode(fX1,fY1,fZ0,m_fF110,m_fG110,m_fH110);
    pkTemp->XNext = 0;
    pkTemp->YNext = 0;
    m_pkGraph->XNext->YNext = pkTemp;
    m_pkGraph->YNext->XNext = pkTemp;

    // add pkN001
    pkTemp = AddNode(fX0,fY1,fZ1,m_fF001,m_fG001,m_fH001);
    pkTemp->ZNext = 0;
    m_pkGraph->ZNext = pkTemp;

    // add pkN101
    pkTemp = AddNode(fX1,fY0,fZ1,m_fF101,m_fG101,m_fH101);
    pkTemp->XNext = 0;
    pkTemp->ZNext = 0;
    m_pkGraph->XNext->ZNext = pkTemp;
    m_pkGraph->ZNext->XNext = pkTemp;

    // add pkN011
    pkTemp = AddNode(fX0,fY1,fZ1,m_fF011,m_fG011,m_fH011);
    pkTemp->YNext = 0;
    pkTemp->ZNext = 0;
    m_pkGraph->YNext->ZNext = pkTemp;
    m_pkGraph->ZNext->YNext = pkTemp;

    // add pkN111
    pkTemp = AddNode(fX1,fY1,fZ1,m_fF111,m_fG111,m_fH111);
    m_pkGraph->XNext->YNext->ZNext = pkTemp;
    m_pkGraph->YNext->XNext->ZNext = pkTemp;
    m_pkGraph->XNext->ZNext->YNext = pkTemp;

    bool bResult = BisectRecurse(m_pkGraph);
    if (bResult)
    {
        rfXRoot = m_fXRoot;
        rfYRoot = m_fYRoot;
        rfZRoot = m_fZRoot;
    }

    // remove remaining oct from m_pkGraph
    WM4_DELETE m_pkGraph->XNext->YNext->ZNext;
    WM4_DELETE m_pkGraph->XNext->ZNext;
    WM4_DELETE m_pkGraph->YNext->ZNext;
    WM4_DELETE m_pkGraph->ZNext;
    WM4_DELETE m_pkGraph->XNext->YNext;
    WM4_DELETE m_pkGraph->XNext;
    WM4_DELETE m_pkGraph->YNext;
    WM4_DELETE m_pkGraph;

    return bResult;
}
//----------------------------------------------------------------------------
template <class Real>
bool Bisect3<Real>::BisectRecurse (BisectNode* pkN000)
{
    if (++m_iLevel == m_iMaxLevel)
    {
        m_iLevel--;
        return false;
    }

    BisectNode* pkN100 = pkN000->XNext;
    BisectNode* pkN010 = pkN000->YNext;
    BisectNode* pkN110 = pkN100->YNext;
    BisectNode* pkN001 = pkN000->ZNext;
    BisectNode* pkN101 = pkN001->XNext;
    BisectNode* pkN011 = pkN001->YNext;
    BisectNode* pkN111 = pkN101->YNext;

    m_iNetSign = (int)(
        Math<Real>::Sign(pkN000->F) +
        Math<Real>::Sign(pkN010->F) +
        Math<Real>::Sign(pkN100->F) +
        Math<Real>::Sign(pkN110->F) +
        Math<Real>::Sign(pkN001->F) +
        Math<Real>::Sign(pkN011->F) +
        Math<Real>::Sign(pkN101->F) +
        Math<Real>::Sign(pkN111->F));

    if (abs(m_iNetSign) == 8)
    {
        // F has same sign at corners
        m_iLevel--;
        return false;
    }

    m_iNetSign = (int)(
        Math<Real>::Sign(pkN000->G) +
        Math<Real>::Sign(pkN010->G) +
        Math<Real>::Sign(pkN100->G) +
        Math<Real>::Sign(pkN110->G) +
        Math<Real>::Sign(pkN001->G) +
        Math<Real>::Sign(pkN011->G) +
        Math<Real>::Sign(pkN101->G) +
        Math<Real>::Sign(pkN111->G));

    if (abs(m_iNetSign) == 8)
    {
        // G has same sign at corners
        m_iLevel--;
        return false;
    }

    m_iNetSign = (int)(
        Math<Real>::Sign(pkN000->H) +
        Math<Real>::Sign(pkN010->H) +
        Math<Real>::Sign(pkN100->H) +
        Math<Real>::Sign(pkN110->H) +
        Math<Real>::Sign(pkN001->H) +
        Math<Real>::Sign(pkN011->H) +
        Math<Real>::Sign(pkN101->H) +
        Math<Real>::Sign(pkN111->H));

    if (abs(m_iNetSign) == 8)
    {
        // H has same sign at corners
        m_iLevel--;
        return false;
    }

    // bisect the oct
    m_fX0 = pkN000->X;
    m_fY0 = pkN000->Y;
    m_fZ0 = pkN000->Z;
    m_fX1 = pkN111->X;
    m_fY1 = pkN111->Y;
    m_fZ1 = pkN111->Z;
    m_fXm = 0.5f*(m_fX0+m_fX1);
    m_fYm = 0.5f*(m_fY0+m_fY1);
    m_fZm = 0.5f*(m_fZ0+m_fZ1);

    // edge 011,111
    if (ZeroTest(m_fXm,m_fY1,m_fZ1,m_fFm11,m_fGm11,m_fHm11,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 101,111
    if (ZeroTest(m_fX1,m_fYm,m_fZ1,m_fF1m1,m_fG1m1,m_fH1m1,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 110,111
    if (ZeroTest(m_fX1,m_fY1,m_fZm,m_fF11m,m_fG11m,m_fH11m,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 010,110
    if (ZeroTest(m_fXm,m_fY1,m_fZ0,m_fFm10,m_fGm10,m_fHm10,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 100,110
    if (ZeroTest(m_fX1,m_fYm,m_fZ0,m_fF1m0,m_fG1m0,m_fH1m0,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 100,101
    if (ZeroTest(m_fX1,m_fY0,m_fZm,m_fF10m,m_fG10m,m_fH10m,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 001,101
    if (ZeroTest(m_fXm,m_fY0,m_fZ1,m_fFm01,m_fGm01,m_fHm01,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 001,011
    if (ZeroTest(m_fX0,m_fYm,m_fZ1,m_fF0m1,m_fG0m1,m_fH0m1,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 010,011
    if (ZeroTest(m_fX0,m_fY1,m_fZm,m_fF01m,m_fG01m,m_fH01m,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 000,100
    if (ZeroTest(m_fXm,m_fY0,m_fZ0,m_fFm00,m_fGm00,m_fHm00,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 000,010
    if (ZeroTest(m_fX0,m_fYm,m_fZ0,m_fF0m0,m_fG0m0,m_fH0m0,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 000,001
    if (ZeroTest(m_fX0,m_fY0,m_fZm,m_fF00m,m_fG00m,m_fH00m,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // face 110,100,101,111
    if (ZeroTest(m_fX1,m_fYm,m_fZm,m_fF1mm,m_fG1mm,m_fH1mm,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // face 010,110,111,011
    if (ZeroTest(m_fXm,m_fY1,m_fZm,m_fFm1m,m_fGm1m,m_fHm1m,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // face 001,101,111,011
    if (ZeroTest(m_fXm,m_fYm,m_fZ1,m_fFmm1,m_fGmm1,m_fHmm1,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // face 000,010,011,001
    if (ZeroTest(m_fX0,m_fYm,m_fZm,m_fF0mm,m_fG0mm,m_fH0mm,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // face 000,100,001,101
    if (ZeroTest(m_fXm,m_fY0,m_fZm,m_fFm0m,m_fGm0m,m_fHm0m,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // face 000,100,110,010
    if (ZeroTest(m_fXm,m_fYm,m_fZ0,m_fFmm0,m_fGmm0,m_fHmm0,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // center
    if (ZeroTest(m_fXm,m_fYm,m_fZm,m_fFmmm,m_fGmmm,m_fHmmm,m_fXRoot,m_fYRoot,
        m_fZRoot))
    {
        return true;
    }

    // edge 011,111
    BisectNode* pkTemp = AddNode(m_fXm,m_fY1,m_fZ1,m_fFm11,m_fGm11,m_fHm11);
    pkTemp->XNext = pkN111;
    pkTemp->YNext = 0;
    pkTemp->ZNext = 0;
    pkN011->XNext = pkTemp;

    // edge 101,111
    pkTemp = AddNode(m_fX1,m_fYm,m_fZ1,m_fF1m1,m_fG1m1,m_fH1m1);
    pkTemp->XNext = 0;
    pkTemp->YNext = pkN111;
    pkTemp->ZNext = 0;
    pkN101->YNext = pkTemp;

    // edge 110,111
    pkTemp = AddNode(m_fX1,m_fY1,m_fZm,m_fF11m,m_fG11m,m_fH11m);
    pkTemp->XNext = 0;
    pkTemp->YNext = 0;
    pkTemp->ZNext = pkN111;
    pkN110->ZNext = pkTemp;

    // edge 010,110
    pkTemp = AddNode(m_fXm,m_fY1,m_fZ0,m_fFm10,m_fGm10,m_fHm10);
    pkTemp->XNext = pkN110;
    pkTemp->YNext = 0;
    pkN010->XNext = pkTemp;

    // edge 100,110
    pkTemp = AddNode(m_fX1,m_fYm,m_fZ1,m_fF1m0,m_fG1m0,m_fH1m0);
    pkTemp->XNext = 0;
    pkTemp->YNext = pkN110;
    pkN100->YNext = pkTemp;

    // edge 100,101
    pkTemp = AddNode(m_fX1,m_fY0,m_fZm,m_fF10m,m_fG10m,m_fH10m);
    pkTemp->XNext = 0;
    pkTemp->ZNext = pkN101;
    pkN100->ZNext = pkTemp;

    // edge 001,101
    pkTemp = AddNode(m_fXm,m_fY0,m_fZ1,m_fFm01,m_fGm01,m_fHm01);
    pkTemp->XNext = pkN101;
    pkTemp->ZNext = 0;
    pkN001->XNext = pkTemp;

    // edge 001,011
    pkTemp = AddNode(m_fX0,m_fYm,m_fZ1,m_fF0m1,m_fG0m1,m_fH0m1);
    pkTemp->YNext = pkN011;
    pkTemp->ZNext = 0;
    pkN001->YNext = pkTemp;

    // edge 010,011
    pkTemp = AddNode(m_fX0,m_fY1,m_fZm,m_fF01m,m_fG01m,m_fH01m);
    pkTemp->YNext = 0;
    pkTemp->ZNext = pkN011;
    pkN010->ZNext = pkTemp;

    // edge 000,100
    pkTemp = AddNode(m_fXm,m_fY0,m_fZ0,m_fFm00,m_fGm00,m_fHm00);
    pkTemp->XNext = pkN100;
    pkN000->XNext = pkTemp;

    // edge 000,010
    pkTemp = AddNode(m_fX0,m_fYm,m_fZ0,m_fF0m0,m_fG0m0,m_fH0m0);
    pkTemp->YNext = pkN010;
    pkN000->YNext = pkTemp;

    // edge 000,001
    pkTemp = AddNode(m_fX0,m_fY0,m_fZm,m_fF00m,m_fG00m,m_fH00m);
    pkTemp->ZNext = pkN001;
    pkN000->ZNext = pkTemp;

    // face 110,100,101,111
    pkTemp = AddNode(m_fX1,m_fYm,m_fZm,m_fF11m,m_fG11m,m_fH11m);
    pkTemp->XNext = 0;
    pkTemp->YNext = pkN110->ZNext;
    pkTemp->ZNext = pkN101->YNext;
    pkN100->YNext->ZNext = pkTemp;
    pkN100->ZNext->YNext = pkTemp;

    // face 010,110,111,011
    pkTemp = AddNode(m_fXm,m_fY1,m_fZm,m_fFm1m,m_fGm1m,m_fHm1m);
    pkTemp->XNext = pkN110->ZNext;
    pkTemp->YNext = 0;
    pkTemp->ZNext = pkN011->XNext;
    pkN010->XNext->ZNext = pkTemp;
    pkN010->ZNext->XNext = pkTemp;

    // face 001,101,111,011
    pkTemp = AddNode(m_fXm,m_fYm,m_fZ1,m_fFmm1,m_fGmm1,m_fHmm1);
    pkTemp->XNext = pkN101->YNext;
    pkTemp->YNext = pkN011->XNext;
    pkTemp->ZNext = 0;
    pkN001->XNext->YNext = pkTemp;
    pkN001->YNext->XNext = pkTemp;

    // face 000,010,011,001
    pkTemp = AddNode(m_fX0,m_fYm,m_fZm,m_fF0mm,m_fG0mm,m_fH0mm);
    pkTemp->YNext = pkN010->ZNext;
    pkTemp->ZNext = pkN001->YNext;
    pkN000->YNext->ZNext = pkTemp;
    pkN000->ZNext->YNext = pkTemp;

    // face 000,100,001,101
    pkTemp = AddNode(m_fXm,m_fY0,m_fZm,m_fFm0m,m_fGm0m,m_fHm0m);
    pkTemp->XNext = pkN100->ZNext;
    pkTemp->ZNext = pkN001->XNext;
    pkN000->XNext->ZNext = pkTemp;
    pkN000->ZNext->XNext = pkTemp;

    // face 000,100,110,010
    pkTemp = AddNode(m_fXm,m_fYm,m_fZ0,m_fFmm0,m_fGmm0,m_fHmm0);
    pkTemp->XNext = pkN100->YNext;
    pkTemp->YNext = pkN010->XNext;
    pkN000->XNext->YNext = pkTemp;
    pkN000->YNext->XNext = pkTemp;

    // center
    pkTemp = AddNode(m_fXm,m_fYm,m_fZm,m_fFmmm,m_fGmmm,m_fHmmm);
    pkTemp->XNext = pkN100->YNext->ZNext;
    pkTemp->YNext = pkN010->XNext->ZNext;
    pkTemp->ZNext = pkN001->XNext->YNext;
    pkN000->XNext->YNext->ZNext = pkTemp;
    pkN000->XNext->ZNext->YNext = pkTemp;
    pkN000->YNext->ZNext->XNext = pkTemp;

    // Search the subocts for roots
    bool bResult =
        BisectRecurse(pkN000) ||
        BisectRecurse(pkN000->XNext) ||
        BisectRecurse(pkN000->YNext) ||
        BisectRecurse(pkN000->XNext->YNext) ||
        BisectRecurse(pkN000->ZNext) ||
        BisectRecurse(pkN000->ZNext->XNext) ||
        BisectRecurse(pkN000->ZNext->YNext) ||
        BisectRecurse(pkN000->ZNext->XNext->YNext);

    // entire suboct check failed, remove the nodes that were added

    // center
    WM4_DELETE pkN000->XNext->YNext->ZNext;

    // faces
    WM4_DELETE pkN000->XNext->YNext;
    WM4_DELETE pkN000->YNext->ZNext;
    WM4_DELETE pkN000->XNext->ZNext;
    WM4_DELETE pkN001->XNext->YNext;
    WM4_DELETE pkN010->XNext->ZNext;
    WM4_DELETE pkN100->YNext->ZNext;

    // edges
    WM4_DELETE pkN000->XNext;  pkN000->XNext = pkN100;
    WM4_DELETE pkN000->YNext;  pkN000->YNext = pkN010;
    WM4_DELETE pkN000->ZNext;  pkN000->ZNext = pkN001;
    WM4_DELETE pkN001->YNext;  pkN001->YNext = pkN011;
    WM4_DELETE pkN001->XNext;  pkN001->XNext = pkN101;
    WM4_DELETE pkN010->ZNext;  pkN010->ZNext = pkN011;
    WM4_DELETE pkN100->ZNext;  pkN100->ZNext = pkN101;
    WM4_DELETE pkN010->XNext;  pkN010->XNext = pkN110;
    WM4_DELETE pkN100->YNext;  pkN100->YNext = pkN110;
    WM4_DELETE pkN011->XNext;  pkN011->XNext = pkN111;
    WM4_DELETE pkN101->YNext;  pkN101->YNext = pkN111;
    WM4_DELETE pkN110->ZNext;  pkN110->ZNext = pkN111;

    m_iLevel--;
    return bResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class Bisect3<float>;

template WM4_FOUNDATION_ITEM
class Bisect3<double>;
//----------------------------------------------------------------------------
}
