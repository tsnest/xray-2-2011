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
#include "Wm4FastMarch3.h"
using namespace Wm4;

const float FastMarch3::ms_fOneThird = 1.0f/3.0f;

//----------------------------------------------------------------------------
FastMarch3::FastMarch3 (int iXBound, int iYBound, int iZBound,
    float fXSpacing, float fYSpacing, float fZSpacing, const float* afSpeed,
    const std::vector<int>& rkSeeds)
    :
    FastMarch(iXBound*iYBound*iZBound,afSpeed,rkSeeds)
{
    Initialize(iXBound,iYBound,iZBound,fXSpacing,fYSpacing,fZSpacing);
}
//----------------------------------------------------------------------------
FastMarch3::FastMarch3 (int iXBound, int iYBound, int iZBound,
    float fXSpacing, float fYSpacing, float fZSpacing, const float fSpeed,
    const std::vector<int>& rkSeeds)
    :
    FastMarch(iXBound*iYBound*iZBound,fSpeed,rkSeeds)
{
    Initialize(iXBound,iYBound,iZBound,fXSpacing,fYSpacing,fZSpacing);
}
//----------------------------------------------------------------------------
FastMarch3::~FastMarch3 ()
{
}
//----------------------------------------------------------------------------
void FastMarch3::Initialize (int iXBound, int iYBound, int iZBound,
    float fXSpacing, float fYSpacing, float fZSpacing)
{
    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iZBound = iZBound;
    m_iXYBound = iXBound*iYBound;
    m_iXBm1 = m_iXBound - 1;
    m_iYBm1 = m_iYBound - 1;
    m_iZBm1 = m_iZBound - 1;
    m_fXSpacing = fXSpacing;
    m_fYSpacing = fYSpacing;
    m_fZSpacing = fZSpacing;
    m_fInvXSpacing = 1.0f/fXSpacing;
    m_fInvYSpacing = 1.0f/fYSpacing;
    m_fInvZSpacing = 1.0f/fZSpacing;

    // Boundary pixels are marked as zero speed to allow us to avoid having
    // to process the boundary pixels separately during the iteration.
    int iX, iY, iZ, i;

    // vertex (0,0,0)
    i = Index(0,0,0);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (xmax,0,0)
    i = Index(m_iXBm1,0,0);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (0,ymax,0)
    i = Index(0,m_iYBm1,0);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (xmax,ymax,0)
    i = Index(m_iXBm1,m_iYBm1,0);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (0,0,zmax)
    i = Index(0,0,m_iZBm1);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (xmax,0,zmax)
    i = Index(m_iXBm1,0,m_iZBm1);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (0,ymax,zmax)
    i = Index(0,m_iYBm1,m_iZBm1);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // vertex (xmax,ymax,zmax)
    i = Index(m_iXBm1,m_iYBm1,m_iZBm1);
    m_afInvSpeed[i] = Mathf::MAX_REAL;
    m_afTime[i] = -Mathf::MAX_REAL;

    // edges (x,0,0) and (x,ymax,0)
    for (iX = 0; iX < m_iXBound; iX++)
    {
        i = Index(iX,0,0);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(iX,m_iYBm1,0);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // edges (0,y,0) and (xmax,y,0)
    for (iY = 0; iY < m_iYBound; iY++)
    {
        i = Index(0,iY,0);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(m_iXBm1,iY,0);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // edges (x,0,zmax) and (x,ymax,zmax)
    for (iX = 0; iX < m_iXBound; iX++)
    {
        i = Index(iX,0,m_iZBm1);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(iX,m_iYBm1,m_iZBm1);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // edges (0,y,zmax) and (xmax,y,zmax)
    for (iY = 0; iY < m_iYBound; iY++)
    {
        i = Index(0,iY,m_iZBm1);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(m_iXBm1,iY,m_iZBm1);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // edges (0,0,z) and (xmax,0,z)
    for (iZ = 0; iZ < m_iZBound; iZ++)
    {
        i = Index(0,0,iZ);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(m_iXBm1,0,iZ);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // edges (0,ymax,z) and (xmax,ymax,z)
    for (iZ = 0; iZ < m_iZBound; iZ++)
    {
        i = Index(0,m_iYBm1,iZ);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
        i = Index(m_iXBm1,m_iYBm1,iZ);
        m_afInvSpeed[i] = Mathf::MAX_REAL;
        m_afTime[i] = -Mathf::MAX_REAL;
    }

    // Compute the first batch of trial pixels.  These are pixels a grid
    // distance of one away from the seed pixels.
    for (iZ = 1; iZ < m_iZBm1; iZ++)
    {
        for (iY = 1; iY < m_iYBm1; iY++)
        {
            for (iX = 1; iX < m_iXBm1; iX++)
            {
                i = Index(iX,iY,iZ);
                if (IsFar(i))
                {
                    if ((IsValid(i-1) && !IsTrial(i-1))
                    ||  (IsValid(i+1) && !IsTrial(i+1))
                    ||  (IsValid(i-m_iXBound) && !IsTrial(i-m_iXBound))
                    ||  (IsValid(i+m_iXBound) && !IsTrial(i+m_iXBound))
                    ||  (IsValid(i-m_iXYBound) && !IsTrial(i-m_iXYBound))
                    ||  (IsValid(i+m_iXYBound) && !IsTrial(i+m_iXYBound)))
                    {
                        ComputeTime(i);
                        m_apkTrial[i] = m_kHeap.Insert(i,m_afTime[i]);
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void FastMarch3::GetBoundary (std::vector<int>& rkBoundary) const
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        if (IsValid(i) && !IsTrial(i))
        {
            if (IsTrial(i-1)
            ||  IsTrial(i+1)
            ||  IsTrial(i-m_iXBound)
            ||  IsTrial(i+m_iXBound)
            ||  IsTrial(i-m_iXYBound)
            ||  IsTrial(i+m_iXYBound))
            {
                rkBoundary.push_back(i);
            }
        }
    }
}
//----------------------------------------------------------------------------
bool FastMarch3::IsBoundary (int i) const
{
    if (IsValid(i) && !IsTrial(i))
    {
        if (IsTrial(i-1)
        ||  IsTrial(i+1)
        ||  IsTrial(i-m_iXBound)
        ||  IsTrial(i+m_iXBound)
        ||  IsTrial(i-m_iXYBound)
        ||  IsTrial(i+m_iXYBound))
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void FastMarch3::Iterate ()
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

    int iMXYB = i - m_iXYBound;
    if (IsTrial(iMXYB))
    {
        ComputeTime(iMXYB);
        m_kHeap.Update(m_apkTrial[iMXYB],m_afTime[iMXYB]);
    }
    else if (IsFar(iMXYB))
    {
        ComputeTime(iMXYB);
        m_apkTrial[iMXYB] = m_kHeap.Insert(iMXYB,m_afTime[iMXYB]);
    }

    int iPXYB = i + m_iXYBound;
    if (IsTrial(iPXYB))
    {
        ComputeTime(iPXYB);
        m_kHeap.Update(m_apkTrial[iPXYB],m_afTime[iPXYB]);
    }
    else if (IsFar(iPXYB))
    {
        ComputeTime(iPXYB);
        m_apkTrial[iPXYB] = m_kHeap.Insert(iPXYB,m_afTime[iPXYB]);
    }
}
//----------------------------------------------------------------------------
void FastMarch3::ComputeTime (int i)
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

    bool bHasZTerm;
    float fZConst;
    if (IsValid(i-m_iXYBound))
    {
        bHasZTerm = true;
        fZConst = m_afTime[i-m_iXYBound];
        if (IsValid(i+m_iXYBound))
        {
            if (m_afTime[i+m_iXYBound] < fZConst)
            {
                fZConst = m_afTime[i+m_iXYBound];
            }
        }
    }
    else if (IsValid(i+m_iXYBound))
    {
        bHasZTerm = true;
        fZConst = m_afTime[i+m_iXYBound];
    }
    else
    {
        bHasZTerm = false;
        fZConst = 0.0f;
    }

    float fSum, fDif, fDiscr;

    if (bHasXTerm)
    {
        if (bHasYTerm)
        {
            if (bHasZTerm)
            {
                // xyz
                fSum = fXConst + fYConst + fZConst;
                fDiscr = 3.0f*m_afInvSpeed[i]*m_afInvSpeed[i];
                fDif = fXConst - fYConst;
                fDiscr -= fDif*fDif;
                fDif = fXConst - fZConst;
                fDiscr -= fDif*fDif;
                fDif = fYConst - fZConst;
                fDiscr -= fDif*fDif;
                if (fDiscr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    m_afTime[i] = ms_fOneThird*(fSum + Mathf::Sqrt(fDiscr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    m_afTime[i] = fXConst;
                    if (fYConst > m_afTime[i])
                    {
                        m_afTime[i] = fYConst;
                    }
                    if (fZConst > m_afTime[i])
                    {
                        m_afTime[i] = fZConst;
                    }
                }
            }
            else
            {
                // xy
                fSum = fXConst + fYConst;
                fDif = fXConst - fYConst;
                fDiscr = 2.0f*m_afInvSpeed[i]*m_afInvSpeed[i] - fDif*fDif;
                if (fDiscr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    m_afTime[i] = 0.5f*(fSum + Mathf::Sqrt(fDiscr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    m_afTime[i] = (fDif >= 0.0f ? fXConst : fYConst);
                }
            }
        }
        else
        {
            if (bHasZTerm)
            {
                // xz
                fSum = fXConst + fZConst;
                fDif = fXConst - fZConst;
                fDiscr = 2.0f*m_afInvSpeed[i]*m_afInvSpeed[i] - fDif*fDif;
                if (fDiscr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    m_afTime[i] = 0.5f*(fSum + Mathf::Sqrt(fDiscr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    m_afTime[i] = (fDif >= 0.0f ? fXConst : fZConst);
                }
            }
            else
            {
                // x
                m_afTime[i] = m_afInvSpeed[i] + fXConst;
            }
        }
    }
    else
    {
        if (bHasYTerm)
        {
            if (bHasZTerm)
            {
                // yz
                fSum = fYConst + fZConst;
                fDif = fYConst - fZConst;
                fDiscr = 2.0f*m_afInvSpeed[i]*m_afInvSpeed[i] - fDif*fDif;
                if (fDiscr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    m_afTime[i] = 0.5f*(fSum + Mathf::Sqrt(fDiscr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    m_afTime[i] = (fDif >= 0.0f ? fYConst : fZConst);
                }
            }
            else
            {
                // y
                m_afTime[i] = m_afInvSpeed[i] + fYConst;
            }
        }
        else
        {
            if (bHasZTerm)
            {
                // z
                m_afTime[i] = m_afInvSpeed[i] + fZConst;
            }
            else
            {
                // No terms, which cannot happen.  The voxel must have at
                // least one valid neighbor.
                assert(false);
            }
        }
    }
}
//----------------------------------------------------------------------------
