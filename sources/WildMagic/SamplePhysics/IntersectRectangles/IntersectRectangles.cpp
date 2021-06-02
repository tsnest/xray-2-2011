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

#include "IntersectRectangles.h"

WM4_WINDOW_APPLICATION(IntersectRectangles);

const int g_iSize = 256;

// #define SINGLE_STEP

//----------------------------------------------------------------------------
IntersectRectangles::IntersectRectangles ()
    :
    WindowApplication2("IntersectingRectangles",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_bMouseDown = false;
    m_pkIR = 0;
    m_fLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool IntersectRectangles::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    for (int i = 0; i < 16; i++)
    {
        float fXMin = Mathf::IntervalRandom(0.125f*g_iSize,0.875f*g_iSize);
        float fXMax = fXMin + Mathf::IntervalRandom(4.0f,32.0f);
        float fYMin = Mathf::IntervalRandom(0.125f*g_iSize,0.875f*g_iSize);
        float fYMax = fYMin + Mathf::IntervalRandom(4.0f,32.0f);
        m_kRects.push_back(AxisAlignedBox2f(fXMin,fXMax,fYMin,fYMax));
    }

    m_pkIR = WM4_NEW IntersectingRectanglesf(m_kRects);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void IntersectRectangles::OnTerminate ()
{
    WM4_DELETE m_pkIR;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectRectangles::OnIdle ()
{
#ifndef SINGLE_STEP
    float fCurrIdle = (float)System::GetTime();
    float fDiff = fCurrIdle - m_fLastIdle;
    if (fDiff >= 1.0f/30.0f)
    {
        ModifyRectangles();
        OnDisplay();
        m_fLastIdle = fCurrIdle;
    }
#endif
}
//----------------------------------------------------------------------------
void IntersectRectangles::OnDisplay ()
{
    ClearScreen();
    DrawRectangles();
    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool IntersectRectangles::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

#ifdef SINGLE_STEP
    switch (ucKey)
    {
    case 'g':
    case 'G':
        ModifyRectangles();
        OnDisplay();
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
void IntersectRectangles::ModifyRectangles ()
{
    for (int i = 0; i < (int)m_kRects.size(); i++)
    {
        AxisAlignedBox2f kRect = m_kRects[i];

        float fDX = Mathf::IntervalRandom(-4.0f,4.0f);
        if (kRect.Min[0]+fDX >= 0.0f && kRect.Max[0]+fDX < g_iSize)
        {
            kRect.Min[0] += fDX;
            kRect.Max[0] += fDX;
        }

        float fDY = Mathf::IntervalRandom(-4.0f,4.0f);
        if (kRect.Min[1]+fDY >= 0.0f && kRect.Max[1]+fDY < g_iSize)
        {
            kRect.Min[1] += fDY;
            kRect.Max[1] += fDY;
        }

        m_pkIR->SetRectangle(i,kRect);
    }

    m_pkIR->Update();
}
//----------------------------------------------------------------------------
void IntersectRectangles::DrawRectangles ()
{
    Color kGray(192,192,192), kBlack(0,0,0), kRed(255,0,0);
    int i, iXMin, iXMax, iYMin, iYMax;
    for (i = 0; i < (int)m_kRects.size(); i++)
    {
        const AxisAlignedBox2f& rkRect = m_kRects[i];
        iXMin = (int)rkRect.Min[0];
        iXMax = (int)rkRect.Max[0];
        iYMin = (int)rkRect.Min[1];
        iYMax = (int)rkRect.Max[1];
        DrawRectangle(iXMin,iYMin,iXMax,iYMax,kGray,true);
        DrawRectangle(iXMin,iYMin,iXMax,iYMax,kBlack);
    }

    const std::set<EdgeKey>& rkOverlap = m_pkIR->GetOverlap();
    std::set<EdgeKey>::const_iterator pkIter = rkOverlap.begin();
    for (/**/; pkIter != rkOverlap.end(); pkIter++)
    {
        int i0 = pkIter->V[0];
        int i1 = pkIter->V[1];
        const AxisAlignedBox2f& rkR0 = m_kRects[i0];
        const AxisAlignedBox2f& rkR1 = m_kRects[i1];
        AxisAlignedBox2f kIntr;
        if (rkR0.FindIntersection(rkR1,kIntr))
        {
            iXMin = (int)kIntr.Min[0];
            iXMax = (int)kIntr.Max[0];
            iYMin = (int)kIntr.Min[1];
            iYMax = (int)kIntr.Max[1];
            DrawRectangle(iXMin,iYMin,iXMax,iYMax,kRed,true);
            DrawRectangle(iXMin,iYMin,iXMax,iYMax,kBlack);
        }
    }
}
//----------------------------------------------------------------------------
