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
// Version: 4.0.1 (2006/09/24)

#include "ConvexHull2D.h"

WM4_WINDOW_APPLICATION(ConvexHull2D);

const int g_iSize = 512;

//----------------------------------------------------------------------------
ConvexHull2D::ConvexHull2D ()
    :
    WindowApplication2("ConvexHull2D",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f)),
    m_fEpsilon(0.001f)
{
    m_eQueryType = Query::QT_REAL;
    m_pkHull = 0;
    m_iVQuantity = 0;
    m_akVertex = 0;
}
//----------------------------------------------------------------------------
bool ConvexHull2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    GenerateHull2D();
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void ConvexHull2D::OnTerminate ()
{
    WM4_DELETE m_pkHull;
    WM4_DELETE[] m_akVertex;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void ConvexHull2D::OnDisplay ()
{
    ClearScreen();

    Color kBlack(0,0,0), kGray(128,128,128), kBlue(0,0,255);

    int iDim = m_pkHull->GetDimension();
    int iQuantity = m_pkHull->GetSimplexQuantity();
    const int* aiIndex = m_pkHull->GetIndices();

    int i0, i1, iX0, iY0, iX1, iY1;
    Vector2f kV0, kV1;

    if (iDim == 0)
    {
        // draw point
        kV0 = m_akVertex[0];
        iX0 = UnitToScreen(kV0.X());
        iY0 = UnitToScreen(kV0.Y());
        SetPixel(iX0,iY0,kGray);
    }
    else if (iDim == 1)
    {
        // draw line segment
        kV0 = m_akVertex[aiIndex[0]];
        iX0 = UnitToScreen(kV0.X());
        iY0 = UnitToScreen(kV0.Y());

        kV1 = m_akVertex[aiIndex[1]];
        iX1 = UnitToScreen(kV1.X());
        iY1 = UnitToScreen(kV1.Y());

        DrawLine(iX0,iY0,iX1,iY1,kGray);
    }
    else
    {
        // draw convex polygon
        for (i0 = iQuantity-1, i1 = 0; i1 < iQuantity; i0 = i1++)
        {
            kV0 = m_akVertex[aiIndex[i0]];
            iX0 = UnitToScreen(kV0.X());
            iY0 = UnitToScreen(kV0.Y());

            kV1 = m_akVertex[aiIndex[i1]];
            iX1 = UnitToScreen(kV1.X());
            iY1 = UnitToScreen(kV1.Y());

            DrawLine(iX0,iY0,iX1,iY1,kGray);
        }
    }

    // draw input points
    for (i0 = 0; i0 < m_iVQuantity; i0++)
    {
        kV0 = m_akVertex[i0];
        iX0 = UnitToScreen(kV0.X());
        iY0 = UnitToScreen(kV0.Y());
        SetThickPixel(iX0,iY0,1,kBlue);
    }

    // draw hull vertices
    if (aiIndex)
    {
        for (i0 = 0; i0 < iQuantity; i0++)
        {
            kV0 = m_akVertex[aiIndex[i0]];
            iX0 = UnitToScreen(kV0.X());
            iY0 = UnitToScreen(kV0.Y());
            SetThickPixel(iX0,iY0,1,kBlack);
        }
    }
    else
    {
        kV0 = m_akVertex[0];
        iX0 = UnitToScreen(kV0.X());
        iY0 = UnitToScreen(kV0.Y());
        SetThickPixel(iX0,iY0,1,kBlack);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
void ConvexHull2D::ScreenOverlay ()
{
    const size_t uiSize = 512;
    char acMsg[uiSize];

    switch (m_eQueryType)
    {
    case Query::QT_INT64:
        System::Strcpy(acMsg,uiSize,"query type = INT64");
        break;
    case Query::QT_INTEGER:
        System::Strcpy(acMsg,uiSize,"query type = INTEGER");
        break;
    case Query::QT_RATIONAL:
        System::Strcpy(acMsg,uiSize,"query type = RATIONAL");
        break;
    case Query::QT_REAL:
        System::Strcpy(acMsg,uiSize,"query type = REAL");
        break;
    case Query::QT_FILTERED:
        System::Strcpy(acMsg,uiSize,"query type = FILTERED");
        break;
    default:
        assert(false);
        break;
    }

    m_pkRenderer->Draw(8,16,ColorRGBA::BLACK,acMsg);
}
//----------------------------------------------------------------------------
bool ConvexHull2D::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    // Generate points that are nearly the same point.
    case '0':
        GenerateHull0D();
        OnDisplay();
        return true;

    // Generate points that are nearly collinear.
    case '1':
        GenerateHull1D();
        OnDisplay();
        return true;

    // Generate points that have a convex polygon hull.
    case '2':
        GenerateHull2D();
        OnDisplay();
        return true;

    // Lots of collinear points that lead to a convex polygon hull.
    case 'l':
    case 'L':
        GenerateHullManyCollinear();
        OnDisplay();
        return true;

    // query type INT64
    case 'n':
    case 'N':
        m_eQueryType = Query::QT_INT64;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type INTEGER
    case 'i':
    case 'I':
        m_eQueryType = Query::QT_INTEGER;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type RATIONAL
    case 'r':
    case 'R':
        m_eQueryType = Query::QT_RATIONAL;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type REAL (float)
    case 'f':
    case 'F':
        m_eQueryType = Query::QT_REAL;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type FILTERED
    case 'c':
    case 'C':
        m_eQueryType = Query::QT_FILTERED;
        RegenerateHull();
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHull0D ()
{
    m_iVQuantity = 8;
    WM4_DELETE[] m_akVertex;
    m_akVertex = WM4_NEW Vector2f[m_iVQuantity];
    m_akVertex[0].X() = Mathf::UnitRandom();
    m_akVertex[0].Y() = Mathf::UnitRandom();
    for (int i = 1; i < m_iVQuantity; i++)
    {
        float fSign = (Mathf::SymmetricRandom() > 0.0f ? 1.0f : -1.0f);
        m_akVertex[i].X() = m_akVertex[0].X() + fSign*0.00001f;
        m_akVertex[i].Y() = m_akVertex[0].Y() + fSign*0.00001f;
    }

    RegenerateHull();
    assert(m_pkHull->GetDimension() == 0);
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHull1D ()
{
    m_iVQuantity = 32;
    WM4_DELETE[] m_akVertex;
    m_akVertex = WM4_NEW Vector2f[m_iVQuantity];
    int iVQm1 = m_iVQuantity - 1;
    m_akVertex[0].X() = Mathf::UnitRandom();
    m_akVertex[0].Y() = Mathf::UnitRandom();
    m_akVertex[iVQm1].X() = Mathf::UnitRandom();
    m_akVertex[iVQm1].Y() = Mathf::UnitRandom();
    for (int i = 1; i < iVQm1; i++)
    {
        float fSign = (Mathf::SymmetricRandom() > 0.0f ? 1.0f : -1.0f);
        float fT = Mathf::UnitRandom();
        m_akVertex[i] = (1.0f-fT)*m_akVertex[0] + fT*m_akVertex[iVQm1];
        m_akVertex[i].X() += fSign*0.00001f;
        m_akVertex[i].Y() += fSign*0.00001f;
    }

    RegenerateHull();
    assert(m_pkHull->GetDimension() == 1);
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHull2D ()
{
    // generate random points
    m_iVQuantity = 256;
    WM4_DELETE[] m_akVertex;
    m_akVertex = WM4_NEW Vector2f[m_iVQuantity];
    for (int i = 0; i < m_iVQuantity; i++)
    {
        m_akVertex[i].X() = Mathf::UnitRandom();
        m_akVertex[i].Y() = Mathf::UnitRandom();
    }

    RegenerateHull();
    assert(m_pkHull->GetDimension() == 2);
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHullManyCollinear ()
{
    // generate a lot of nearly collinear points
    m_iVQuantity = 128;
    WM4_DELETE[] m_akVertex;
    m_akVertex = WM4_NEW Vector2f[m_iVQuantity];

    Vector2f kCenter(0.5f,0.5f);
    Vector2f kU0(Mathf::SymmetricRandom(),Mathf::SymmetricRandom());
    kU0.Normalize();
    Vector2f kU1 = kU0.Perp();
    float fE0 = 0.5f*Mathf::UnitRandom();
    float fE1 = 0.5f*Mathf::UnitRandom();

    float fT;
    int i;
    for (i = 0; i < m_iVQuantity/4; i++)
    {
        fT = i/(m_iVQuantity/4.0f);
        m_akVertex[i] = kCenter - fE0*kU0 - fE1*kU1 +
            2.0f*fE0*fT*kU0;
    }
    for (i = 0; i < m_iVQuantity/4; i++)
    {
        fT = i/(m_iVQuantity/4.0f);
        m_akVertex[i+m_iVQuantity/4] = kCenter + fE0*kU0 - fE1*kU1 +
            2.0f*fE1*fT*kU1;
    }
    for (i = 0; i < m_iVQuantity/4; i++)
    {
        fT = i/(m_iVQuantity/4.0f);
        m_akVertex[i+m_iVQuantity/2] = kCenter + fE0*kU0 + fE1*kU1 -
            2.0f*fE0*fT*kU0;
    }
    for (i = 0; i < m_iVQuantity/4; i++)
    {
        fT = i/(m_iVQuantity/4.0f);
        m_akVertex[i+3*m_iVQuantity/4] = kCenter - fE0*kU0 + fE1*kU1 -
            2.0f*fE1*fT*kU1;
    }

    RegenerateHull();
    assert(m_pkHull->GetDimension() == 2);
}
//----------------------------------------------------------------------------
void ConvexHull2D::RegenerateHull ()
{
    WM4_DELETE m_pkHull;
    m_pkHull = WM4_NEW ConvexHull2f(m_iVQuantity,m_akVertex,m_fEpsilon,false,
        m_eQueryType);

    if (m_pkHull->GetDimension() == 1)
    {
        ConvexHull2f* pkSave = (ConvexHull2f*)m_pkHull;
        m_pkHull = pkSave->GetConvexHull1();
        WM4_DELETE pkSave;
    }
}
//----------------------------------------------------------------------------
int ConvexHull2D::UnitToScreen (float fValue)
{
    return (int)(g_iSize*(0.25f+0.5f*fValue));
}
//----------------------------------------------------------------------------
