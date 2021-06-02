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
// Version: 4.0.1 (2006/09/25)

#include "Polysolid2D.h"

WM4_WINDOW_APPLICATION(Polysolid2D);

const int g_iSize = 256;

//----------------------------------------------------------------------------
Polysolid2D::Polysolid2D ()
    :
    WindowApplication2("Polysolid2D",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_pkActive = 0;
    m_pkPoly0 = 0;
    m_pkPoly1 = 0;
}
//----------------------------------------------------------------------------
bool Polysolid2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    m_iChoice = 0;
    m_pkPoly0 = ConstructInvertedEll();
    m_pkPoly1 = ConstructPentagon();
    DoBoolean();

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Polysolid2D::OnTerminate ()
{
    WM4_DELETE m_pkPoly0;
    WM4_DELETE m_pkPoly1;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void Polysolid2D::OnDisplay ()
{
    ClearScreen();

    DrawPolySolid(*m_pkPoly0,Color(255,0,0));
    DrawPolySolid(*m_pkPoly1,Color(0,255,0));
    if (m_pkActive)
    {
        DrawPolySolid(*m_pkActive,Color(0,0,255));
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool Polysolid2D::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'n':
    case 'N':
        WM4_DELETE m_pkPoly0;
        WM4_DELETE m_pkPoly1;
        m_pkActive = 0;

        m_iChoice = (m_iChoice+1) % 3;
        switch (m_iChoice)
        {
        case 0:
            m_pkPoly0 = ConstructInvertedEll();
            m_pkPoly1 = ConstructPentagon();
            break;
        case 1:
            m_pkPoly0 = ConstructSquare();
            m_pkPoly1 = ConstructSShape();
            break;
        case 2:
            m_pkPoly0 = ConstructPolyWithHoles();
            m_pkPoly1 = ConstructPentagon();
            break;
        }
        DoBoolean();
        break;

    case 'p':
    case 'P':
        m_pkActive = 0;
        break;
    case 'u':
    case 'U':
        m_pkActive = &m_kUnion;
        break;
    case 'i':
    case 'I':
        m_pkActive = &m_kIntersection;
        break;
    case 'd':
    case 'D':
        m_pkActive = &m_kDiff01;
        break;
    case 'e':
    case 'E':
        m_pkActive = &m_kDiff10;
        break;
    case 'x':
    case 'X':
        m_pkActive = &m_kXor;
        break;
    }

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
Polysolid2* Polysolid2D::ConstructInvertedEll ()
{
    Polysolid2* pkPoly = WM4_NEW Polysolid2;

    RPoint2 kStart, kR0, kR1;

    kR0.X() = kStart.X() = g_iSize/8;
    kR0.Y() = kStart.Y() = g_iSize/8;
    kR1.X() = kR0.X();
    kR1.Y() = 7*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 7*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = 5*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 5*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = 6*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 2*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = 3*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 3*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1 = kStart;
    pkPoly->InsertEdge(kR0,kR1);

    return pkPoly;
}
//---------------------------------------------------------------------------
Polysolid2* Polysolid2D::ConstructPentagon ()
{
    Polysolid2* pkPoly = WM4_NEW Polysolid2;

    double dAngle = -2.0*Mathd::PI/5.0;
    double dRadius = 0.35*g_iSize;
    double dXCenter = g_iSize/2.0, dYCenter = g_iSize/2.0;

    RPoint2 kStart, kR0, kR1;
    kR0.X() = kStart.X() = (int)(dXCenter+dRadius);
    kR0.Y() = kStart.Y() = (int)(dYCenter);
    kR1.X() = (int)(dXCenter + dRadius*Mathd::Cos(dAngle));
    kR1.Y() = (int)(dYCenter + dRadius*Mathd::Sin(dAngle));
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = (int)(dXCenter + dRadius*Mathd::Cos(2.0*dAngle));
    kR1.Y() = (int)(dYCenter + dRadius*Mathd::Sin(2.0*dAngle));
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = (int)(dXCenter + dRadius*Mathd::Cos(3.0*dAngle));
    kR1.Y() = (int)(dYCenter + dRadius*Mathd::Sin(3.0*dAngle));
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = (int)(dXCenter + dRadius*Mathd::Cos(4.0*dAngle));
    kR1.Y() = (int)(dYCenter + dRadius*Mathd::Sin(4.0*dAngle));
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1 = kStart;
    pkPoly->InsertEdge(kR0,kR1);

    return pkPoly;
}
//---------------------------------------------------------------------------
Polysolid2* Polysolid2D::ConstructSquare ()
{
    Polysolid2* pkPoly = WM4_NEW Polysolid2;

    RPoint2 kR0, kR1;

    kR0.X() = 2*g_iSize/8;
    kR0.Y() = 2*g_iSize/8;
    kR1.X() = kR0.X();
    kR1.Y() = 6*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 6*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = 2*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 2*g_iSize/8;
    kR1.Y() = 2*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    return pkPoly;
}
//---------------------------------------------------------------------------
Polysolid2* Polysolid2D::ConstructSShape ()
{
    Polysolid2* pkPoly = WM4_NEW Polysolid2;

    RPoint2 kR0, kR1;

    kR0.X() = 6*g_iSize/8;
    kR0.Y() = (int)(2.5*g_iSize/8);
    kR1.X() = kR0.X();
    kR1.Y() = 3*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = (int)(6.5*g_iSize/8);
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = (int)(3.25*g_iSize/8);
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 5*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = (int)(5.5*g_iSize/8);
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 6*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = (int)(4.75*g_iSize/8);
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = (int)(5.5*g_iSize/8);
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = 4*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 7*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = (int)(2.5*g_iSize/8);
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 6*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    return pkPoly;
}
//---------------------------------------------------------------------------
Polysolid2* Polysolid2D::ConstructPolyWithHoles ()
{
    Polysolid2* pkPoly = WM4_NEW Polysolid2;

    RPoint2 kR0, kR1;

    // outer boundary
    kR0.X() = g_iSize/8;
    kR0.Y() = g_iSize/8;
    kR1.X() = kR0.X();
    kR1.Y() = 7*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 7*g_iSize/8;
    kR1.Y() = g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = g_iSize/8;
    kR1.Y() = g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    // inner hole
    kR0.X() = 2*g_iSize/8;
    kR0.Y() = 3*g_iSize/16;
    kR1.X() = 3*g_iSize/8;
    kR1.Y() = kR0.Y();
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = kR0.X();
    kR1.Y() = 3*g_iSize/8;
    pkPoly->InsertEdge(kR0,kR1);

    kR0 = kR1;
    kR1.X() = 2*g_iSize/8;
    kR1.Y() = 3*g_iSize/16;
    pkPoly->InsertEdge(kR0,kR1);

    return pkPoly;
}
//---------------------------------------------------------------------------
void Polysolid2D::DrawPolySolid (Polysolid2& rkPoly, Color kColor)
{
    const Polysolid2::ESet& rkESet = rkPoly.GetEdges();
    Polysolid2::ESet::const_iterator pkIter;
    for (pkIter = rkESet.begin(); pkIter != rkESet.end(); pkIter++)
    {
        const Polysolid2::Vertex* pkV0 = pkIter->GetVertex(0);
        const Polysolid2::Vertex* pkV1 = pkIter->GetVertex(1);
        const RPoint2& rkPosition0 = pkV0->Position;
        const RPoint2& rkPosition1 = pkV1->Position;

        float fRX0, fRY0, fRX1, fRY1;
        rkPosition0.X().ConvertTo(fRX0);
        rkPosition0.Y().ConvertTo(fRY0);
        rkPosition1.X().ConvertTo(fRX1);
        rkPosition1.Y().ConvertTo(fRY1);

        int iX0 = (int)fRX0;
        int iY0 = g_iSize-1-(int)fRY0;
        int iX1 = (int)fRX1;
        int iY1 = g_iSize-1-(int)fRY1;

        DrawLine(iX0,iY0,iX1,iY1,kColor);
    }
}
//---------------------------------------------------------------------------
void Polysolid2D::DoBoolean ()
{
    Polysolid2& P = *m_pkPoly0;
    Polysolid2& Q = *m_pkPoly1;

    m_kIntersection = P & Q;
    m_kUnion        = P | Q;
    m_kDiff01       = P - Q;
    m_kDiff10       = Q - P;
    m_kXor          = P ^ Q;
}
//----------------------------------------------------------------------------
