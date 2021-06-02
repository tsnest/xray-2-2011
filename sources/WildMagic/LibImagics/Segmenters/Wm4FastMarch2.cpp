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
// Version: 4.0.0 (2006/09/21)

#include "Wm4ImagicsPCH.h"
#include "Wm4FastMarch2.h"
using namespace Wm4;

//----------------------------------------------------------------------------
FastMarch2::FastMarch2 (int iXBound, int iYBound, float fXSpacing,
    float fYSpacing, const float* afSpeed, const std::vector<int>& rkSeeds)
    :
    FastMarch(iXBound*iYBound,afSpeed,rkSeeds)
{
    Initialize(iXBound,iYBound,fXSpacing,fYSpacing);
}
//----------------------------------------------------------------------------
FastMarch2::FastMarch2 (int iXBound, int iYBound, float fXSpacing,
    float fYSpacing, const float fSpeed, const std::vector<int>& rkSeeds)
    :
    FastMarch(iXBound*iYBound,fSpeed,rkSeeds)
{
    Initialize(iXBound,iYBound,fXSpacing,fYSpacing);
}
//----------------------------------------------------------------------------
FastMarch2::~FastMarch2 ()
{
}
//----------------------------------------------------------------------------
void FastMarch2::Initialize (int iXBound, int iYBound, float fXSpacing,
    float fYSpacing)
{
    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iXBm1 = m_iXBound - 1;
    m_iYBm1 = m_iYBound - 1;
    m_fXSpacing = fXSpacing;
    m_fYSpacing = fYSpacing;
    m_fInvXSpacing = 1.0f/fXSpacing;
    m_fInvYSpacing = 1.0f/fYSpacing;

    // Boundary pixels are marked as zero speed to allow us to avoid having
    // to process the boundary pixels separately during the iteration.
    int iX, iY, i;

    // vertex (0,0)
    i = Index(0,0);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (xmax,0)
    i = Index(m_iXBm1,0);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (0,ymax)
    i = Index(0,m_iYBm1);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (xmax,ymax)
    i = Index(m_iXBm1,m_iYBm1);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // edges (x,0) and (x,ymax)
    for (iX = 0; iX < m_iXBound; iX++)
    {
        i = Index(iX,0);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(iX,m_iYBm1);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // edges (0,y) and (xmax,y)
    for (iY = 0; iY < m_iYBound; iY++)
    {
        i = Index(0,iY);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(m_iXBm1,iY);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // Compute the first batch of trial pixels.  These are pixels a grid
    // distance of one away from the seed pixels.
    for (iY = 1; iY < m_iYBm1; iY++)
    {
        for (iX = 1; iX < m_iXBm1; iX++)
        {
            i = Index(iX,iY);
            if (IsFar(i))
            {
                if ((IsValid(i-1) && !IsTrial(i-1))
                ||  (IsValid(i+1) && !IsTrial(i+1))
                ||  (IsValid(i-m_iXBound) && !IsTrial(i-m_iXBound))
                ||  (IsValid(i+m_iXBound) && !IsTrial(i+m_iXBound)))
                {
                    ComputeTime(i);
                    m_apkTrial[i] = m_kHeap.Insert(i,m_afTime[i]);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void FastMarch2::GetBoundary (std::vector<int>& rkBoundary) const
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        if (IsValid(i) && !IsTrial(i))
        {
            if (IsTrial(i-1)
            ||  IsTrial(i+1)
            ||  IsTrial(i-m_iXBound)
            ||  IsTrial(i+m_iXBound))
            {
                rkBoundary.push_back(i);
            }
        }
    }
}
//----------------------------------------------------------------------------
bool FastMarch2::IsBoundary (int i) const
{
    if (IsValid(i) && !IsTrial(i))
    {
        if (IsTrial(i-1)
        ||  IsTrial(i+1)
        ||  IsTrial(i-m_iXBound)
        ||  IsTrial(i+m_iXBound))
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void FastMarch2::Iterate ()
{
    // Remove the minimum trial value from the heap.
    int i;
    float fValue;
    m_kHeap.Remove(i,fValue);

    // Promote the trial value to a known value.  The value was negative but
    // is now nonnegative (the heap stores only nonnegative numbers).
    assert(IsTrial(i) && m_afTime[i] == fValue);
    m_apkTrial[i] = 0;

    // All trial pixels must be updated.  All far neighbors must become trial
    // pixels.
    int iM1 = i - 1;
    if (IsTrial(iM1))
    {
        ComputeTime(iM1);
        m_kHeap.Update(m_apkTrial[iM1],m_afTime[iM1]);
    }
    else if (IsFar(iM1))
    {
        ComputeTime(iM1);
        m_apkTrial[iM1] = m_kHeap.Insert(iM1,m_afTime[iM1]);
    }

    int iP1 = i + 1;
    if (IsTrial(iP1))
    {
        ComputeTime(iP1);
        m_kHeap.Update(m_apkTrial[iP1],m_afTime[iP1]);
    }
    else if (IsFar(iP1))
    {
        ComputeTime(iP1);
        m_apkTrial[iP1] = m_kHeap.Insert(iP1,m_afTime[iP1]);
    }

    int iMXB = i - m_iXBound;
    if (IsTrial(iMXB))
    {
        ComputeTime(iMXB);
        m_kHeap.Update(m_apkTrial[iMXB],m_afTime[iMXB]);
    }
    else if (IsFar(iMXB))
    {
        ComputeTime(iMXB);
        m_apkTrial[iMXB] = m_kHeap.Insert(iMXB,m_afTime[iMXB]);
    }

    int iPXB = i + m_iXBound;
    if (IsTrial(iPXB))
    {
        ComputeTime(iPXB);
        m_kHeap.Update(m_apkTrial[iPXB],m_afTime[iPXB]);
    }
    else if (IsFar(iPXB))
    {
        ComputeTime(iPXB);
        m_apkTrial[iPXB] = m_kHeap.Insert(iPXB,m_afTime[iPXB]);
    }
}
//----------------------------------------------------------------------------
void FastMarch2::ComputeTime (int i)
{
    bool bHasXTerm;
    float fXConst;
    if (IsValid(i-1))
    {
        bHasXTerm = true;
        fXConst = m_afTime[i-1];
        if (IsValid(i+1))
        {
            if (m_afTime[i+1] < fXConst)
            {
                fXConst = m_afTime[i+1];
            }
        }
    }
    else if (IsValid(i+1))
    {
        bHasXTerm = true;
        fXConst = m_afTime[i+1];
    }
    else
    {
        bHasXTerm = false;
        fXConst = 0.0f;
    }

    bool bHasYTerm;
    float fYConst;
    if (IsValid(i-m_iXBound))
    {
        bHasYTerm = true;
        fYConst = m_afTime[i-m_iXBound];
        if (IsValid(i+m_iXBound))
        {
            if (m_afTime[i+m_iXBound] < fYConst)
            {
                fYConst = m_afTime[i+m_iXBound];
            }
        }
    }
    else if (IsValid(i+m_iXBound))
    {
        bHasYTerm = true;
        fYConst = m_afTime[i+m_iXBound];
    }
    else
    {
        bHasYTerm = false;
        fYConst = 0.0f;
    }

    if (bHasXTerm)
    {
        if (bHasYTerm)
        {
            float fSum = fXConst + fYConst;
            float fDif = fXConst - fYConst;
            float fDiscr = 2.0f*m_afInvSpeed[i]*m_afInvSpeed[i] - fDif*fDif;
            if (fDiscr >= 0.0f)
            {
                // The quadratic equation has a real-valued solution.  Choose
                // the largest positive root for the crossing time.
                m_afTime[i] = 0.5f*(fSum + Mathf::Sqrt(fDiscr));
            }
            else
            {
                // The quadratic equation does not have a real-valued
                // solution.  This can happen when the speed is so large
                // that the time gradient has very small length, which means
                // that the time has not changed significantly from the
                // neighbors to the current pixel.  Just choose the maximum
                // time of the neighbors (Is there a better choice?).
                m_afTime[i] = (fDif >= 0.0f ? fXConst : fYConst);
            }
        }
        else
        {
            // The equation is linear.
            m_afTime[i] = m_afInvSpeed[i] + fXConst;
        }
    }
    else if (bHasYTerm)
    {
        // The equation is linear.
        m_afTime[i] = m_afInvSpeed[i] + fYConst;
    }
    else
    {
        // The pixel must have at least one known neighbor.
        assert(false);
    }
}
//----------------------------------------------------------------------------
