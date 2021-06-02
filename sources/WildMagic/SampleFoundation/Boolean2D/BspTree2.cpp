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

#include "BspTree2.h"
#include "Polygon2.h"
#include <list>

//----------------------------------------------------------------------------
BspTree2::BspTree2 (Polygon2& rkPoly, const EArray& rkInput)
{
    assert(rkInput.size() > 0);

    // construct splitting line from first edge
    Vector2d kL0 = rkPoly.m_kVArray[rkInput[0].i0];
    Vector2d kL1 = rkPoly.m_kVArray[rkInput[0].i1];

    // add edge to coincident list
    m_kCoincident.push_back(rkInput[0]);

    // split remaining edges
    EArray kPosArray, kNegArray;
    int iMax = (int)rkInput.size();
    for (int i = 1; i < iMax; i++)
    {
        int iV0 = rkInput[i].i0;
        int iV1 = rkInput[i].i1;
        Vector2d kV0 = rkPoly.m_kVArray[iV0];
        Vector2d kV1 = rkPoly.m_kVArray[iV1];

        Vector2d kI;
        int iVMid;

        switch (Classify(kL0,kL1,kV0,kV1,kI))
        {
            case TRANSVERSE_POSITIVE:
                // modify edge <V0,V1> to <V0,I> and add new edge <I,V1>
                iVMid = rkPoly.InsertVertex(kI);
                rkPoly.SplitEdge(iV0,iV1,iVMid);
                kPosArray.push_back(Edge2(iVMid,iV1));
                kNegArray.push_back(Edge2(iV0,iVMid));
                break;
            case TRANSVERSE_NEGATIVE:
                // modify edge <V0,V1> to <V0,I> and add new edge <I,V1>
                iVMid = rkPoly.InsertVertex(kI);
                rkPoly.SplitEdge(iV0,iV1,iVMid);
                kPosArray.push_back(Edge2(iV0,iVMid));
                kNegArray.push_back(Edge2(iVMid,iV1));
                break;
            case ALL_POSITIVE:
                kPosArray.push_back(rkInput[i]);
                break;
            case ALL_NEGATIVE:
                kNegArray.push_back(rkInput[i]);
                break;
            default:  // COINCIDENT
                m_kCoincident.push_back(rkInput[i]);
        }
    }

    if (kPosArray.size() > 0)
    {
        m_pkPosChild = WM4_NEW BspTree2(rkPoly,kPosArray);
    }
    else
    {
        m_pkPosChild = 0;
    }

    if (kNegArray.size() > 0)
    {
        m_pkNegChild = WM4_NEW BspTree2(rkPoly,kNegArray);
    }
    else
    {
        m_pkNegChild = 0;
    }
}
//----------------------------------------------------------------------------
BspTree2::~BspTree2 ()
{
    WM4_DELETE m_pkPosChild;
    WM4_DELETE m_pkNegChild;
}
//----------------------------------------------------------------------------
BspTree2* BspTree2::GetCopy () const
{
    BspTree2* pkTree = WM4_NEW BspTree2;

    pkTree->m_kCoincident = m_kCoincident;

    if (m_pkPosChild)
    {
        pkTree->m_pkPosChild = m_pkPosChild->GetCopy();
    }
    else
    {
        pkTree->m_pkPosChild = 0;
    }

    if (m_pkNegChild)
    {
        pkTree->m_pkNegChild = m_pkNegChild->GetCopy();
    }
    else
    {
        pkTree->m_pkNegChild = 0;
    }

    return pkTree;
}
//----------------------------------------------------------------------------
void BspTree2::Negate ()
{
    // reverse coincident edge directions
    for (int i = 0; i < (int)m_kCoincident.size(); i++)
    {
        Edge2& rkE = m_kCoincident[i];
        int iSave = rkE.i0;
        rkE.i0 = rkE.i1;
        rkE.i1 = iSave;
    }

    // swap positive and negative subtrees
    BspTree2* pkSave = m_pkPosChild;
    m_pkPosChild = m_pkNegChild;
    m_pkNegChild = pkSave;

    if (m_pkPosChild)
    {
        m_pkPosChild->Negate();
    }

    if (m_pkNegChild)
    {
        m_pkNegChild->Negate();
    }
}
//----------------------------------------------------------------------------
int BspTree2::Classify (const Vector2d& rkL0, const Vector2d& rkL1,
    const Vector2d& rkV0, const Vector2d& rkV1, Vector2d& rkI) const
{
    // for numerical round-off error handling
    const double dEpsilon0 = 0.00001;
    const double dEpsilon1 = 0.99999;

    Vector2d kDir = rkL1 - rkL0;
    Vector2d kNor = kDir.Perp();
    Vector2d kDiff0 = rkV0 - rkL0;
    Vector2d kDiff1 = rkV1 - rkL0;

    double dD0 = kNor.Dot(kDiff0);
    double dD1 = kNor.Dot(kDiff1);

    if (dD0*dD1 < 0.0)
    {
        // Edge <V0,V1> transversely crosses line.  Compute point of
        // intersection I = V0+t*(V1-V0).
        double dT = dD0/(dD0 - dD1);
        if (dT > dEpsilon0)
        {
            if (dT < dEpsilon1)
            {
                rkI = rkV0 + dT*(rkV1-rkV0);
                if (dD1 > 0.0)
                {
                    return TRANSVERSE_POSITIVE;
                }
                else
                {
                    return TRANSVERSE_NEGATIVE;
                }
            }
            else
            {
                // T is effectively 1 (numerical round-off issue), so
                // set d1 = 0 and go on to other cases.
                dD1 = 0.0;
            }
        }
        else
        {
            // T is effectively 0 (numerical round-off issue), so
            // set d0 = 0 and go on to other cases.
            dD0 = 0.0;
        }
    }

    if (dD0 > 0.0 || dD1 > 0.0)
    {
        // edge on positive side of line
        return ALL_POSITIVE;
    }

    if (dD0 < 0.0 || dD1 < 0.0)
    {
        // edge on negative side of line
        return ALL_NEGATIVE;
    }

    return COINCIDENT;
}
//----------------------------------------------------------------------------
void BspTree2::GetPosPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
    const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
    Polygon2& rkCoSame, Polygon2& rkCoDiff) const
{
    if (m_pkPosChild)
    {
        m_pkPosChild->GetPartition(rkPoly,rkV0,rkV1,rkPos,rkNeg,rkCoSame,
            rkCoDiff);
    }
    else
    {
        int i0 = rkPos.InsertVertex(rkV0);
        int i1 = rkPos.InsertVertex(rkV1);
        rkPos.InsertEdge(Edge2(i0,i1));
    }
}
//----------------------------------------------------------------------------
void BspTree2::GetNegPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
    const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
    Polygon2& rkCoSame, Polygon2& rkCoDiff) const
{
    if (m_pkNegChild)
    {
        m_pkNegChild->GetPartition(rkPoly,rkV0,rkV1,rkPos,rkNeg,rkCoSame,
            rkCoDiff);
    }
    else
    {
        int i0 = rkNeg.InsertVertex(rkV0);
        int i1 = rkNeg.InsertVertex(rkV1);
        rkNeg.InsertEdge(Edge2(i0,i1));
    }
}
//----------------------------------------------------------------------------
void BspTree2::GetCoPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
    const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
    Polygon2& rkCoSame, Polygon2& rkCoDiff) const
{
    const double dEpsilon = 0.00001;

    // Segment the line containing V0 and V1 by the coincident intervals that
    // intersect <V0,V1>.
    Vector2d kDir = rkV1 - rkV0;
    double dTMax = kDir.Dot(kDir);

    Vector2d kL0, kL1;
    double dT0, dT1;
    bool bSameDir;

    list<Interval> kList;
    list<Interval>::iterator pkI;

    for (int i = 0; i < (int)m_kCoincident.size(); i++)
    {
        kL0 = rkPoly.m_kVArray[m_kCoincident[i].i0];
        kL1 = rkPoly.m_kVArray[m_kCoincident[i].i1];

        dT0 = kDir.Dot(kL0-rkV0);
        if (Mathd::FAbs(dT0) <= dEpsilon)
        {
            dT0 = 0.0;
        }
        else if (Mathd::FAbs(dT0-dTMax) <= dEpsilon)
        {
            dT0 = dTMax;
        }

        dT1 = kDir.Dot(kL1-rkV0);
        if (Mathd::FAbs(dT1) <= dEpsilon)
        {
            dT1 = 0.0;
        }
        else if (Mathd::FAbs(dT1-dTMax) <= dEpsilon)
        {
            dT1 = dTMax;
        }

        bSameDir = (dT1 > dT0);
        if (!bSameDir)
        {
            double fSave = dT0;
            dT0 = dT1;
            dT1 = fSave;
        }

        if (dT1 > 0.0 && dT0 < dTMax)
        {
            if (kList.empty())
            {
                kList.push_front(Interval(dT0,dT1,bSameDir,true));
            }
            else
            {
                for (pkI = kList.begin(); pkI != kList.end(); pkI++)
                {
                    if (Mathd::FAbs(dT1 - pkI->m_dT0) <= dEpsilon)
                    {
                        dT1 = pkI->m_dT0;
                    }

                    if (dT1 <= pkI->m_dT0)
                    {
                        // [t0,t1] is on the left of [I.t0,I.t1]
                        kList.insert(pkI,Interval(dT0,dT1,bSameDir,true));
                        break;
                    }

                    // Theoretically, the intervals are disjoint or intersect
                    // only at an end point.  The assert makes sure that
                    // [t0,t1] is to the right of [I.t0,I.t1].
                    if (Mathd::FAbs(dT0 - pkI->m_dT1) <= dEpsilon)
                    {
                        dT0 = pkI->m_dT1;
                    }

                    assert(dT0 >= pkI->m_dT1);

                    list<Interval>::iterator pkLast = kList.end();
                    pkLast--;
                    if (pkI == pkLast)
                    {
                        kList.push_back(Interval(dT0,dT1,bSameDir,true));
                        break;
                    }
                }
            }
        }
    }

    if (kList.empty())
    {
        GetPosPartition(rkPoly,rkV0,rkV1,rkPos,rkNeg,rkCoSame,rkCoDiff);
        GetNegPartition(rkPoly,rkV0,rkV1,rkPos,rkNeg,rkCoSame,rkCoDiff);
        return;
    }

    // Insert outside intervals between the touching intervals.  It is
    // possible that two touching intervals are adjacent, so this is not just
    // a simple alternation of touching and outside intervals.
    Interval& rkFront = kList.front();
    if (rkFront.m_dT0 > 0.0)
    {
        kList.push_front(Interval(0.0,rkFront.m_dT0,rkFront.m_bSameDir,
            false));
    }
    else
    {
        rkFront.m_dT0 = 0.0;
    }

    Interval& rkBack = kList.back();
    if (rkBack.m_dT1 < dTMax)
    {
        kList.push_back(Interval(rkBack.m_dT1,dTMax,rkBack.m_bSameDir,false));
    }
    else
    {
        rkBack.m_dT1 = dTMax;
    }

    list<Interval>::iterator pkI0 = kList.begin();
    list<Interval>::iterator pkI1 = kList.begin();
    pkI1++;
    for (/**/; pkI1 != kList.end(); pkI0++, pkI1++)
    {
        dT0 = pkI0->m_dT1;
        dT1 = pkI1->m_dT0;
        if (dT1 - dT0 > dEpsilon)
        {
            pkI0 = kList.insert(pkI1,Interval(dT0,dT1,true,false));
        }
    }

    // process the segmentation
    double dInvTMax = 1.0/dTMax;
    dT0 = kList.front().m_dT0*dInvTMax;
    kL1 = rkV0 + kList.front().m_dT0*dInvTMax*kDir;
    for (pkI = kList.begin(); pkI != kList.end(); pkI++)
    {
        kL0 = kL1;
        dT1 = pkI->m_dT1*dInvTMax;
        kL1 = rkV0 + pkI->m_dT1*dInvTMax*kDir;

        if (pkI->m_bTouching)
        {
            Edge2 kE;
            if ( pkI->m_bSameDir )
            {
                kE.i0 = rkCoSame.InsertVertex(kL0);
                kE.i1 = rkCoSame.InsertVertex(kL1);
                if (kE.i0 != kE.i1)
                {
                    rkCoSame.InsertEdge(kE);
                }
            }
            else
            {
                kE.i0 = rkCoDiff.InsertVertex(kL1);
                kE.i1 = rkCoDiff.InsertVertex(kL0);
                if (kE.i0 != kE.i1)
                {
                    rkCoDiff.InsertEdge(kE);
                }
            }
        }
        else
        {
            GetPosPartition(rkPoly,kL0,kL1,rkPos,rkNeg,rkCoSame,rkCoDiff);
            GetNegPartition(rkPoly,kL0,kL1,rkPos,rkNeg,rkCoSame,rkCoDiff);
        }
    }
}
//----------------------------------------------------------------------------
void BspTree2::GetPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
    const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
    Polygon2& rkCoSame, Polygon2& rkCoDiff) const
{
    // construct splitting line from first coincident edge
    Vector2d kL0 = rkPoly.m_kVArray[m_kCoincident[0].i0];
    Vector2d kL1 = rkPoly.m_kVArray[m_kCoincident[0].i1];

    Vector2d kI;

    switch (Classify(kL0,kL1,rkV0,rkV1,kI))
    {
    case TRANSVERSE_POSITIVE:
        GetPosPartition(rkPoly,kI,rkV1,rkPos,rkNeg,rkCoSame,rkCoDiff);
        GetNegPartition(rkPoly,rkV0,kI,rkPos,rkNeg,rkCoSame,rkCoDiff);
        break;
    case TRANSVERSE_NEGATIVE:
        GetPosPartition(rkPoly,rkV0,kI,rkPos,rkNeg,rkCoSame,rkCoDiff);
        GetNegPartition(rkPoly,kI,rkV1,rkPos,rkNeg,rkCoSame,rkCoDiff);
        break;
    case ALL_POSITIVE:
        GetPosPartition(rkPoly,rkV0,rkV1,rkPos,rkNeg,rkCoSame,rkCoDiff);
        break;
    case ALL_NEGATIVE:
        GetNegPartition(rkPoly,rkV0,rkV1,rkPos,rkNeg,rkCoSame,rkCoDiff);
        break;
    default:  // COINCIDENT
        GetCoPartition(rkPoly,rkV0,rkV1,rkPos,rkNeg,rkCoSame,rkCoDiff);
    }
}
//----------------------------------------------------------------------------
int BspTree2::Classify (const Vector2d& rkL0, const Vector2d& rkL1,
    const Vector2d& rkV) const
{
    // for numerical round-off error handling
    const double dEpsilon = 0.00001;

    Vector2d kDir = rkL1 - rkL0;
    Vector2d kNor = kDir.Perp();
    Vector2d kDiff = rkV - rkL0;
    double dC = kNor.Dot(kDiff);

    if (dC > dEpsilon)
    {
        return ALL_POSITIVE;
    }

    if (dC < -dEpsilon)
    {
        return ALL_NEGATIVE;
    }

    return COINCIDENT;
}
//----------------------------------------------------------------------------
int BspTree2::CoPointLocation (const Polygon2& rkPoly, const Vector2d& rkV)
    const
{
    // for numerical round-off error handling
    const double dEpsilon = 0.00001;

    for (int i = 0; i < (int)m_kCoincident.size(); i++)
    {
        Vector2d kL0 = rkPoly.m_kVArray[m_kCoincident[i].i0];
        Vector2d kL1 = rkPoly.m_kVArray[m_kCoincident[i].i1];
        Vector2d kDir = kL1 - kL0;
        Vector2d kDiff = rkV - kL0;
        double dTMax = kDir.Dot(kDir);
        double dT = kDir.Dot(kDiff);

        if (-dEpsilon <= dT && dT <= dTMax+dEpsilon)
        {
            return 0;
        }
    }

    // does not matter which subtree you use
    if (m_pkPosChild)
    {
        return m_pkPosChild->PointLocation(rkPoly,rkV);
    }

    if (m_pkNegChild)
    {
        return m_pkNegChild->PointLocation(rkPoly,rkV);
    }

    return 0;
}
//----------------------------------------------------------------------------
int BspTree2::PointLocation (const Polygon2& rkPoly, const Vector2d& rkV)
    const
{
    // construct splitting line from first coincident edge
    Vector2d kL0 = rkPoly.m_kVArray[m_kCoincident[0].i0];
    Vector2d kL1 = rkPoly.m_kVArray[m_kCoincident[0].i1];

    switch (Classify(kL0,kL1,rkV))
    {
    case ALL_POSITIVE:
        if (m_pkPosChild)
        {
            return m_pkPosChild->PointLocation(rkPoly,rkV);
        }
        else
        {
            return 1;
        }
    case ALL_NEGATIVE:
        if (m_pkNegChild)
        {
            return m_pkNegChild->PointLocation(rkPoly,rkV);
        }
        else
        {
            return -1;
        }
    default:  // COINCIDENT
        return CoPointLocation(rkPoly,rkV);
    }
}
//----------------------------------------------------------------------------
void BspTree2::Indent (ofstream& rkOStr, int iQuantity)
{
    for (int i = 0; i < iQuantity; i++)
    {
        rkOStr << ' ';
    }
}
//----------------------------------------------------------------------------
void BspTree2::Print (ofstream& rkOStr, int iLevel, char cType)
{
    for (int i = 0; i < (int)m_kCoincident.size(); i++)
    {
        Indent(rkOStr,4*iLevel);
        rkOStr << cType << " <" << m_kCoincident[i].i0 << ',' <<
            m_kCoincident[i].i1 << ">" << endl;
    }

    if (m_pkPosChild)
    {
        m_pkPosChild->Print(rkOStr,iLevel+1,'p');
    }

    if (m_pkNegChild)
    {
        m_pkNegChild->Print(rkOStr,iLevel+1,'n');
    }
}
//----------------------------------------------------------------------------
