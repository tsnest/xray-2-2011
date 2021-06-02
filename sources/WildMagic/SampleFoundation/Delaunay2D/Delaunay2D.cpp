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

#include "Delaunay2D.h"

WM4_WINDOW_APPLICATION(Delaunay2D);

const int g_iSize = 512;
WindowApplication2::Color g_kWhite(255,255,255);
WindowApplication2::Color g_kGray(128,128,128);
WindowApplication2::Color g_kRed(255,0,0);
WindowApplication2::Color g_kBlue(0,0,255);
WindowApplication2::Color g_kGreen(0,255,0);
WindowApplication2::Color g_kBlack(0,0,0);

//----------------------------------------------------------------------------
Delaunay2D::Delaunay2D ()
    :
    WindowApplication2("Delaunay2D",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_pkDel1 = 0;
    m_pkDel2 = 0;
    m_iVQuantity = 0;
    m_akVertex = 0;
    m_iTQuantity = 0;
    m_aiTVertex = 0;
    m_aiTAdjacent = 0;
    m_iCurrTriX = -1;
    m_iCurrTriY = -1;
    m_iCurrentIndex = 0;
}
//----------------------------------------------------------------------------
bool Delaunay2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // generate random points and triangulate
    m_iVQuantity = 256;
    m_akVertex = WM4_NEW Vector2f[m_iVQuantity];
    for (int i = 0; i < m_iVQuantity; i++)
    {
        m_akVertex[i].X() = g_iSize*Mathf::IntervalRandom(0.125f,0.875f);
        m_akVertex[i].Y() = g_iSize*Mathf::IntervalRandom(0.125f,0.875f);

        // Comment out the last two lines and uncomment these to see
        // how the Delaunay code handles degeneracies in dimension.
        //m_akVertex[i].X() = g_iSize*Mathf::IntervalRandom(0.125f,0.875f);
        //m_akVertex[i].Y() = 2.1f*m_akVertex[i].X();
    }

    m_pkDel2 = WM4_NEW Delaunay2f(m_iVQuantity,m_akVertex,0.001f,false,
        Query::QT_INT64);

    if (m_pkDel2->GetDimension() == 2)
    {
        m_iTQuantity = m_pkDel2->GetSimplexQuantity();
        m_aiTVertex = WM4_NEW int[3*m_iTQuantity];
        size_t uiSize = 3*m_iTQuantity*sizeof(int);
        System::Memcpy(m_aiTVertex,uiSize,m_pkDel2->GetIndices(),uiSize);
        m_aiTAdjacent = WM4_NEW int[3*m_iTQuantity];
        System::Memcpy(m_aiTAdjacent,uiSize,m_pkDel2->GetAdjacencies(),
            uiSize);

        // If H is the number of hull edges and N is the number of vertices,
        // then the triangulation must have 2*N-2-H triangles and 3*N-3-H
        // edges.
        int iEQuantity = 0;
        int* aiIndex = 0;
        m_pkDel2->GetHull(iEQuantity,aiIndex);
        int iUniqueVQuantity = m_pkDel2->GetUniqueVertexQuantity();
        int iTVerify = 2*iUniqueVQuantity - 2 - iEQuantity;
        (void)iTVerify;  // avoid warning in release build
        assert(iTVerify == m_iTQuantity);
        int iEVerify = 3*iUniqueVQuantity - 3 - iEQuantity;
        (void)iEVerify;  // avoid warning about unused variable
        WM4_DELETE[] aiIndex;
    }
    else
    {
        m_pkDel1 = m_pkDel2->GetDelaunay1();
        WM4_DELETE m_pkDel2;
        m_pkDel2 = 0;
    }

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Delaunay2D::OnTerminate ()
{
    WM4_DELETE m_pkDel1;
    WM4_DELETE m_pkDel2;
    WM4_DELETE[] m_akVertex;
    WM4_DELETE[] m_aiTVertex;
    WM4_DELETE[] m_aiTAdjacent;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void Delaunay2D::OnDisplay ()
{
    ClearScreen();

    int i, iX0, iY0, iX1, iY1, iX2, iY2;
    Vector2f kV0, kV1, kV2, kP;

    if (m_pkDel2)
    {
        // draw the triangle mesh
        for (i = 0; i < m_iTQuantity; i++)
        {
            kV0 = m_akVertex[m_aiTVertex[3*i]];
            iX0 = (int)(kV0.X() + 0.5f);
            iY0 = (int)(kV0.Y() + 0.5f);

            kV1 = m_akVertex[m_aiTVertex[3*i+1]];
            iX1 = (int)(kV1.X() + 0.5f);
            iY1 = (int)(kV1.Y() + 0.5f);

            kV2 = m_akVertex[m_aiTVertex[3*i+2]];
            iX2 = (int)(kV2.X() + 0.5f);
            iY2 = (int)(kV2.Y() + 0.5f);

            DrawLine(iX0,iY0,iX1,iY1,g_kGray);
            DrawLine(iX1,iY1,iX2,iY2,g_kGray);
            DrawLine(iX2,iY2,iX0,iY0,g_kGray);
        }

        // draw hull
        int iEQuantity = 0;
        int* aiEIndex = 0;
        m_pkDel2->GetHull(iEQuantity,aiEIndex);
        for (i = 0; i < iEQuantity; i++)
        {
            kV0 = m_akVertex[aiEIndex[2*i]];
            iX0 = (int)(kV0.X() + 0.5f);
            iY0 = (int)(kV0.Y() + 0.5f);

            kV1 = m_akVertex[aiEIndex[2*i+1]];
            iX1 = (int)(kV1.X() + 0.5f);
            iY1 = (int)(kV1.Y() + 0.5f);

            DrawLine(iX0,iY0,iX1,iY1,g_kRed);
        }
        WM4_DELETE[] aiEIndex;


        // draw search path
        for (i = 0; i <= m_pkDel2->GetPathLast(); i++)
        {
            int iIndex = m_pkDel2->GetPath()[i];

            kV0 = m_akVertex[m_aiTVertex[3*iIndex]];
            kV1 = m_akVertex[m_aiTVertex[3*iIndex+1]];
            kV2 = m_akVertex[m_aiTVertex[3*iIndex+2]];

            Vector2f kCenter = (kV0+kV1+kV2)/3.0f;
            int iX = (int)(kCenter.X() + 0.5f);
            int iY = (int)(kCenter.Y() + 0.5f);
            if ( i < m_pkDel2->GetPathLast() )
                Fill(iX,iY,g_kBlue,g_kWhite);
            else
                Fill(iX,iY,g_kRed,g_kWhite);
        }

        if (m_iCurrTriX >= 0)
        {
            // draw current triangle
            Fill(m_iCurrTriX,m_iCurrTriY,g_kGreen,g_kRed);
        }
        else
        {
            // draw last edge when point is outside the hull
            int iLastV0, iLastV1, iLastVOpposite;
            m_pkDel2->GetLastEdge(iLastV0,iLastV1,iLastVOpposite);

            kV0 = m_akVertex[iLastV0];
            iX0 = (int)(kV0.X() + 0.5f);
            iY0 = (int)(kV0.Y() + 0.5f);

            kV1 = m_akVertex[iLastV1];
            iX1 = (int)(kV1.X() + 0.5f);
            iY1 = (int)(kV1.Y() + 0.5f);

            DrawLine(iX0,iY0,iX1,iY1,g_kBlack);
        }
    }
    else
    {
        const int* aiIndex = m_pkDel1->GetIndices();
        for (i = 0; i+1 < m_iVQuantity; i++)
        {
            kV0 = m_akVertex[aiIndex[i]];
            iX0 = (int)(kV0.X() + 0.5f);
            iY0 = (int)(kV0.Y() + 0.5f);

            kV1 = m_akVertex[aiIndex[i+1]];
            iX1 = (int)(kV1.X() + 0.5f);
            iY1 = (int)(kV1.Y() + 0.5f);

            DrawLine(iX0,iY0,iX1,iY1,Color(192,192,192));
        }

        for (i = 0; i < m_iVQuantity; i++)
        {
            assert(0 <= aiIndex[i] && aiIndex[i] < m_iVQuantity);
            kV0 = m_akVertex[aiIndex[i]];
            iX0 = (int)(kV0.X() + 0.5f);
            iY0 = (int)(kV0.Y() + 0.5f);

            float fW = i/(float)(m_iVQuantity-1);
            unsigned char ucR = (unsigned char)((1.0f-fW)*64.0f + fW*255.0f);
            unsigned char ucB = (unsigned char)((1.0f-fW)*255.0f + fW*64.0f);
            for (int iDY = -1; iDY <= 1; iDY++)
            {
                for (int iDX = -1; iDX <= 1; iDX++)
                {
                    SetPixel(iX0+iDX,iY0+iDY,Color(ucR,0,ucB));
                }
            }
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool Delaunay2D::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if (!m_pkDel2 || iButton != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (iState == MOUSE_DOWN)
    {
        Vector2f kP((float)iX,(float)iY);
        int i = m_pkDel2->GetContainingTriangle(kP);
        if (i >= 0)
        {
            m_iCurrTriX = iX;
            m_iCurrTriY = iY;

            float afBary[3];
            m_pkDel2->GetBarycentricSet(i,kP,afBary);
        }
        else
        {
            m_iCurrTriX = -1;
            m_iCurrTriY = -1;
        }
        OnDisplay();
    }

    return true;
}
//----------------------------------------------------------------------------
