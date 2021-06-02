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
// Version: 4.0.1 (2006/08/30)

#include "Triangulation.h"

WM4_WINDOW_APPLICATION(Triangulation);

//----------------------------------------------------------------------------
Triangulation::Triangulation ()
    :
    WindowApplication2("Triangulation",0,0,256,256,ColorRGBA::WHITE)
{
    m_pkRoot = 0;
    m_eType = Query::QT_FILTERED;
    m_fEpsilon = 0.001f;
    m_iExample = 0;
}
//----------------------------------------------------------------------------
bool Triangulation::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    DoExample0();

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Triangulation::OnTerminate ()
{
    TriangulateEC<float>::Delete(m_pkRoot);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
bool Triangulation::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case '0':
        DoExample0();
        OnDisplay();
        return true;
    case '1':
        DoExample1();
        OnDisplay();
        return true;
    case '2':
        DoExample2();
        OnDisplay();
        return true;
    case '3':
        DoExample3();
        OnDisplay();
        return true;
    case '4':
        DoExample4();
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Triangulation::OnDisplay ()
{
    ClearScreen();

    Color kBlue(0,128,255), kBlack(0,0,0), kWhite(255,255,255);
    Color kRed(255,0,0);
    int i, i0, i1, iVQ, iX0, iY0, iX1, iY1;

    // Draw the polygon edges.
    switch (m_iExample)
    {
    case 0:
        iVQ = (int)m_kPositions.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[i0][0];
            iY0 = (int)m_kPositions[i0][1];
            iX1 = (int)m_kPositions[i1][0];
            iY1 = (int)m_kPositions[i1][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }
        break;
    case 1:
        iVQ = (int)m_kOuter.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[m_kOuter[i0]][0];
            iY0 = (int)m_kPositions[m_kOuter[i0]][1];
            iX1 = (int)m_kPositions[m_kOuter[i1]][0];
            iY1 = (int)m_kPositions[m_kOuter[i1]][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }

        iVQ = (int)m_kInner0.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[m_kInner0[i0]][0];
            iY0 = (int)m_kPositions[m_kInner0[i0]][1];
            iX1 = (int)m_kPositions[m_kInner0[i1]][0];
            iY1 = (int)m_kPositions[m_kInner0[i1]][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }
        break;
    case 2:
        iVQ = (int)m_kOuter.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[m_kOuter[i0]][0];
            iY0 = (int)m_kPositions[m_kOuter[i0]][1];
            iX1 = (int)m_kPositions[m_kOuter[i1]][0];
            iY1 = (int)m_kPositions[m_kOuter[i1]][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }

        iVQ = (int)m_kInner0.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[m_kInner0[i0]][0];
            iY0 = (int)m_kPositions[m_kInner0[i0]][1];
            iX1 = (int)m_kPositions[m_kInner0[i1]][0];
            iY1 = (int)m_kPositions[m_kInner0[i1]][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }
        break;
    case 3:
        iVQ = (int)m_kOuter.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[m_kOuter[i0]][0];
            iY0 = (int)m_kPositions[m_kOuter[i0]][1];
            iX1 = (int)m_kPositions[m_kOuter[i1]][0];
            iY1 = (int)m_kPositions[m_kOuter[i1]][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }

        iVQ = (int)m_kInner0.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[m_kInner0[i0]][0];
            iY0 = (int)m_kPositions[m_kInner0[i0]][1];
            iX1 = (int)m_kPositions[m_kInner0[i1]][0];
            iY1 = (int)m_kPositions[m_kInner0[i1]][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }

        iVQ = (int)m_kInner1.size();
        for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
        {
            iX0 = (int)m_kPositions[m_kInner1[i0]][0];
            iY0 = (int)m_kPositions[m_kInner1[i0]][1];
            iX1 = (int)m_kPositions[m_kInner1[i1]][0];
            iY1 = (int)m_kPositions[m_kInner1[i1]][1];
            DrawLine(iX0,iY0,iX1,iY1,kBlack);
        }
        break;
    case 4:
    {
        std::queue<TriangulateEC<float>::Tree*> kQueue;
        kQueue.push(m_pkRoot);
        while (kQueue.size() > 0)
        {
            TriangulateEC<float>::Tree* pkTree = kQueue.front();
            kQueue.pop();
            iVQ = (int)pkTree->Polygon.size();
            for (i0 = iVQ-1, i1 = 0; i1 < iVQ; i0 = i1++)
            {
                iX0 = (int)m_kPositions[pkTree->Polygon[i0]][0];
                iY0 = (int)m_kPositions[pkTree->Polygon[i0]][1];
                iX1 = (int)m_kPositions[pkTree->Polygon[i1]][0];
                iY1 = (int)m_kPositions[pkTree->Polygon[i1]][1];
                DrawLine(iX0,iY0,iX1,iY1,kBlack);
            }

            for (i = 0; i < (int)pkTree->Child.size(); i++)
            {
                kQueue.push(pkTree->Child[i]);
            }
        }
        break;
    }
    }

    // Flood fill the polygon inside.
    for (i = 0; i < (int)m_kFillSeed.size(); i++)
    {
        iX0 = (int)m_kFillSeed[i][0];
        iY0 = (int)m_kFillSeed[i][1];
        Fill(iX0,iY0,kBlue,kWhite);
    }

    // Draw the triangulation edges.
    int iTQuantity = (int)(m_kTriangles.size()/3);
    const int* piIndex = &m_kTriangles.front();
    for (i = 0; i < iTQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;

        iX0 = (int)m_kPositions[iV0][0];
        iY0 = (int)m_kPositions[iV0][1];
        iX1 = (int)m_kPositions[iV1][0];
        iY1 = (int)m_kPositions[iV1][1];
        DrawLine(iX0,iY0,iX1,iY1,kBlack);

        iX0 = (int)m_kPositions[iV1][0];
        iY0 = (int)m_kPositions[iV1][1];
        iX1 = (int)m_kPositions[iV2][0];
        iY1 = (int)m_kPositions[iV2][1];
        DrawLine(iX0,iY0,iX1,iY1,kBlack);

        iX0 = (int)m_kPositions[iV2][0];
        iY0 = (int)m_kPositions[iV2][1];
        iX1 = (int)m_kPositions[iV0][0];
        iY1 = (int)m_kPositions[iV0][1];
        DrawLine(iX0,iY0,iX1,iY1,kBlack);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
void Triangulation::ClearAll ()
{
    m_kPositions.clear();
    m_kOuter.clear();
    m_kInner0.clear();
    m_kInner1.clear();
    m_kInners.clear();
    TriangulateEC<float>::Delete(m_pkRoot);
    m_kFillSeed.clear();
    m_kTriangles.clear();
}
//----------------------------------------------------------------------------
void Triangulation::DoExample0 ()
{
    // Simple polygon.

    ClearAll();
    m_iExample = 0;

    m_kPositions.resize(10);
    m_kPositions[0][0] =  29.0f;  m_kPositions[0][1] = 139.0f;
    m_kPositions[1][0] =  78.0f;  m_kPositions[1][1] =  99.0f;
    m_kPositions[2][0] = 125.0f;  m_kPositions[2][1] = 141.0f;
    m_kPositions[3][0] = 164.0f;  m_kPositions[3][1] = 116.0f;
    m_kPositions[4][0] = 201.0f;  m_kPositions[4][1] = 168.0f;
    m_kPositions[5][0] = 157.0f;  m_kPositions[5][1] = 163.0f;
    m_kPositions[6][0] = 137.0f;  m_kPositions[6][1] = 200.0f;
    m_kPositions[7][0] =  98.0f;  m_kPositions[7][1] = 134.0f;
    m_kPositions[8][0] =  52.0f;  m_kPositions[8][1] = 146.0f;
    m_kPositions[9][0] =  55.0f;  m_kPositions[9][1] = 191.0f;

    m_kFillSeed.push_back(Vector2f(66.0f,128.0f));

    m_kTriangles.clear();
    TriangulateEC<float>(m_kPositions,m_eType,m_fEpsilon,m_kTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample1 ()
{
    // Polygon with one hole.  The top and bottom vertices of the outer
    // polygon are on the line containing the left edge of the inner polygon.
    // This example tests how the collinearity detection works when
    // identifying ears.

    ClearAll();
    m_iExample = 1;

    m_kPositions.resize(7);
    m_kPositions[0][0] =  64.0f;  m_kPositions[0][1] = 128.0f;
    m_kPositions[1][0] = 128.0f;  m_kPositions[1][1] =  64.0f;
    m_kPositions[2][0] = 192.0f;  m_kPositions[2][1] = 128.0f;
    m_kPositions[3][0] = 128.0f;  m_kPositions[3][1] = 192.0f;
    m_kPositions[4][0] = 160.0f;  m_kPositions[4][1] = 128.0f;
    m_kPositions[5][0] = 128.0f;  m_kPositions[5][1] =  96.0f;
    m_kPositions[6][0] = 128.0f;  m_kPositions[6][1] = 160.0f;

    m_kOuter.resize(4);
    m_kOuter[0] = 0;
    m_kOuter[1] = 1;
    m_kOuter[2] = 2;
    m_kOuter[3] = 3;
    m_kFillSeed.push_back(Vector2f(66.0f,128.0f));

    m_kInner0.resize(3);
    m_kInner0[0] = 4;
    m_kInner0[1] = 5;
    m_kInner0[2] = 6;

    TriangulateEC<float>(m_kPositions,m_eType,m_fEpsilon,m_kOuter,m_kInner0,
        m_kTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample2 ()
{
    // Polygon with one hole.

    ClearAll();
    m_iExample = 2;

    m_kPositions.resize(13);
    m_kPositions[0][0] =  29.0f;  m_kPositions[0][1] = 139.0f;
    m_kPositions[1][0] =  78.0f;  m_kPositions[1][1] =  99.0f;
    m_kPositions[2][0] = 125.0f;  m_kPositions[2][1] = 141.0f;
    m_kPositions[3][0] = 164.0f;  m_kPositions[3][1] = 116.0f;
    m_kPositions[4][0] = 201.0f;  m_kPositions[4][1] = 168.0f;
    m_kPositions[5][0] = 157.0f;  m_kPositions[5][1] = 163.0f;
    m_kPositions[6][0] = 137.0f;  m_kPositions[6][1] = 200.0f;
    m_kPositions[7][0] =  98.0f;  m_kPositions[7][1] = 134.0f;
    m_kPositions[8][0] =  52.0f;  m_kPositions[8][1] = 146.0f;
    m_kPositions[9][0] =  55.0f;  m_kPositions[9][1] = 191.0f;
    m_kPositions[10] = (m_kPositions[2]+m_kPositions[5]+m_kPositions[6])/3.0f;
    m_kPositions[11] = (m_kPositions[2]+m_kPositions[3]+m_kPositions[4])/3.0f;
    m_kPositions[12] = (m_kPositions[2]+m_kPositions[6]+m_kPositions[7])/3.0f;

    m_kOuter.resize(10);
    m_kOuter[0] = 5;
    m_kOuter[1] = 6;
    m_kOuter[2] = 7;
    m_kOuter[3] = 8;
    m_kOuter[4] = 9;
    m_kOuter[5] = 0;
    m_kOuter[6] = 1;
    m_kOuter[7] = 2;
    m_kOuter[8] = 3;
    m_kOuter[9] = 4;
    m_kFillSeed.push_back(Vector2f(31.0f,139.0f));

    m_kInner0.resize(3);
    m_kInner0[0] = 11;
    m_kInner0[1] = 12;
    m_kInner0[2] = 10;

    TriangulateEC<float>(m_kPositions,m_eType,m_fEpsilon,m_kOuter,m_kInner0,
        m_kTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample3 ()
{
    // Polygon with two holes.

    ClearAll();
    m_iExample = 3;

    m_kPositions.resize(16);
    m_kPositions[0][0] =  29.0f;  m_kPositions[0][1] = 139.0f;
    m_kPositions[1][0] =  78.0f;  m_kPositions[1][1] =  99.0f;
    m_kPositions[2][0] = 125.0f;  m_kPositions[2][1] = 141.0f;
    m_kPositions[3][0] = 164.0f;  m_kPositions[3][1] = 116.0f;
    m_kPositions[4][0] = 201.0f;  m_kPositions[4][1] = 168.0f;
    m_kPositions[5][0] = 157.0f;  m_kPositions[5][1] = 163.0f;
    m_kPositions[6][0] = 137.0f;  m_kPositions[6][1] = 200.0f;
    m_kPositions[7][0] =  98.0f;  m_kPositions[7][1] = 134.0f;
    m_kPositions[8][0] =  52.0f;  m_kPositions[8][1] = 146.0f;
    m_kPositions[9][0] =  55.0f;  m_kPositions[9][1] = 191.0f;
    m_kPositions[10] = (m_kPositions[2]+m_kPositions[5]+m_kPositions[6])/3.0f;
    m_kPositions[11] = (m_kPositions[2]+m_kPositions[3]+m_kPositions[4])/3.0f;
    m_kPositions[12] = (m_kPositions[2]+m_kPositions[6]+m_kPositions[7])/3.0f;
    m_kPositions[13] = (m_kPositions[1]+m_kPositions[0]+m_kPositions[8])/3.0f;
    m_kPositions[14] = (m_kPositions[1]+m_kPositions[8]+m_kPositions[7])/3.0f;
    m_kPositions[14][1] += 6.0f;
    m_kPositions[15] = (m_kPositions[1]+m_kPositions[7]+m_kPositions[2])/3.0f;

    m_kOuter.resize(10);
    m_kOuter[0] = 0;
    m_kOuter[1] = 1;
    m_kOuter[2] = 2;
    m_kOuter[3] = 3;
    m_kOuter[4] = 4;
    m_kOuter[5] = 5;
    m_kOuter[6] = 6;
    m_kOuter[7] = 7;
    m_kOuter[8] = 8;
    m_kOuter[9] = 9;
    m_kFillSeed.push_back(Vector2f(31.0f,139.0f));

    m_kInner0.resize(3);
    m_kInner0[0] = 11;
    m_kInner0[1] = 12;
    m_kInner0[2] = 10;
    m_kInners.push_back(&m_kInner0);

    m_kInner1.resize(3);
    m_kInner1[0] = 13;
    m_kInner1[1] = 14;
    m_kInner1[2] = 15;
    m_kInners.push_back(&m_kInner1);

    TriangulateEC<float>(m_kPositions,m_eType,m_fEpsilon,m_kOuter,m_kInners,
        m_kTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample4 ()
{
    // A tree of nested polygons.

    ClearAll();
    m_iExample = 4;

    m_kPositions.resize(43);
    m_kPositions[ 0][0] = 102.0f;  m_kPositions[ 0][1] =  15.0f;
    m_kPositions[ 1][0] = 233.0f;  m_kPositions[ 1][1] =  87.0f;
    m_kPositions[ 2][0] = 184.0f;  m_kPositions[ 2][1] = 248.0f;
    m_kPositions[ 3][0] =  33.0f;  m_kPositions[ 3][1] = 232.0f;
    m_kPositions[ 4][0] =  14.0f;  m_kPositions[ 4][1] = 128.0f;
    m_kPositions[ 5][0] = 137.0f;  m_kPositions[ 5][1] =  42.0f;
    m_kPositions[ 6][0] =  93.0f;  m_kPositions[ 6][1] =  41.0f;
    m_kPositions[ 7][0] = 137.0f;  m_kPositions[ 7][1] =  79.0f;
    m_kPositions[ 8][0] = 146.0f;  m_kPositions[ 8][1] =  66.0f;
    m_kPositions[ 9][0] = 161.0f;  m_kPositions[ 9][1] = 213.0f;
    m_kPositions[10][0] = 213.0f;  m_kPositions[10][1] = 113.0f;
    m_kPositions[11][0] = 108.0f;  m_kPositions[11][1] =  67.0f;
    m_kPositions[12][0] =  36.0f;  m_kPositions[12][1] = 153.0f;
    m_kPositions[13][0] =  89.0f;  m_kPositions[13][1] = 220.0f;
    m_kPositions[14][0] = 133.0f;  m_kPositions[14][1] = 186.0f;
    m_kPositions[15][0] = 147.0f;  m_kPositions[15][1] = 237.0f;
    m_kPositions[16][0] = 177.0f;  m_kPositions[16][1] = 237.0f;
    m_kPositions[17][0] = 184.0f;  m_kPositions[17][1] = 202.0f;
    m_kPositions[18][0] = 159.0f;  m_kPositions[18][1] = 225.0f;
    m_kPositions[19][0] =  86.0f;  m_kPositions[19][1] = 113.0f;
    m_kPositions[20][0] = 115.0f;  m_kPositions[20][1] = 118.0f;
    m_kPositions[21][0] =  98.0f;  m_kPositions[21][1] = 134.0f;
    m_kPositions[22][0] = 109.0f;  m_kPositions[22][1] = 153.0f;
    m_kPositions[23][0] =  68.0f;  m_kPositions[23][1] = 133.0f;
    m_kPositions[24][0] =  68.0f;  m_kPositions[24][1] = 156.0f;
    m_kPositions[25][0] = 115.0f;  m_kPositions[25][1] = 175.0f;
    m_kPositions[26][0] = 108.0f;  m_kPositions[26][1] = 194.0f;
    m_kPositions[27][0] =  80.0f;  m_kPositions[27][1] = 192.0f;
    m_kPositions[28][0] = 163.0f;  m_kPositions[28][1] = 108.0f;
    m_kPositions[29][0] = 185.0f;  m_kPositions[29][1] = 108.0f;
    m_kPositions[30][0] = 172.0f;  m_kPositions[30][1] = 176.0f;
    m_kPositions[31][0] =  79.0f;  m_kPositions[31][1] = 170.0f;
    m_kPositions[32][0] =  79.0f;  m_kPositions[32][1] = 179.0f;
    m_kPositions[33][0] =  88.0f;  m_kPositions[33][1] = 179.0f;
    m_kPositions[34][0] =  88.0f;  m_kPositions[34][1] = 170.0f;
    m_kPositions[35][0] =  96.0f;  m_kPositions[35][1] = 179.0f;
    m_kPositions[36][0] =  96.0f;  m_kPositions[36][1] = 187.0f;
    m_kPositions[37][0] = 103.0f;  m_kPositions[37][1] = 187.0f;
    m_kPositions[38][0] = 103.0f;  m_kPositions[38][1] = 179.0f;
    m_kPositions[39][0] = 169.0f;  m_kPositions[39][1] = 121.0f;
    m_kPositions[40][0] = 169.0f;  m_kPositions[40][1] = 131.0f;
    m_kPositions[41][0] = 178.0f;  m_kPositions[41][1] = 131.0f;
    m_kPositions[42][0] = 178.0f;  m_kPositions[42][1] = 121.0f;

    // outer0 polygon
    m_pkRoot = WM4_NEW TriangulateEC<float>::Tree;
    m_pkRoot->Polygon.resize(5);
    m_pkRoot->Polygon[0] = 0;
    m_pkRoot->Polygon[1] = 1;
    m_pkRoot->Polygon[2] = 2;
    m_pkRoot->Polygon[3] = 3;
    m_pkRoot->Polygon[4] = 4;
    m_kFillSeed.push_back(Vector2f(82.0f,69.0f));

    // inner0 polygon
    TriangulateEC<float>::Tree* pkInner0 = WM4_NEW TriangulateEC<float>::Tree;
    pkInner0->Polygon.resize(3);
    pkInner0->Polygon[0] = 5;
    pkInner0->Polygon[1] = 6;
    pkInner0->Polygon[2] = 7;
    m_pkRoot->Child.push_back(pkInner0);

    // inner1 polygon
    TriangulateEC<float>::Tree* pkInner1 = WM4_NEW TriangulateEC<float>::Tree;
    pkInner1->Polygon.resize(3);
    pkInner1->Polygon[0] = 8;
    pkInner1->Polygon[1] = 9;
    pkInner1->Polygon[2] = 10;
    m_pkRoot->Child.push_back(pkInner1);

    // inner2 polygon
    TriangulateEC<float>::Tree* pkInner2 = WM4_NEW TriangulateEC<float>::Tree;
    pkInner2->Polygon.resize(8);
    pkInner2->Polygon[0] = 11;
    pkInner2->Polygon[1] = 12;
    pkInner2->Polygon[2] = 13;
    pkInner2->Polygon[3] = 14;
    pkInner2->Polygon[4] = 15;
    pkInner2->Polygon[5] = 16;
    pkInner2->Polygon[6] = 17;
    pkInner2->Polygon[7] = 18;
    m_pkRoot->Child.push_back(pkInner2);

    // outer1 polygon (contained in inner2)
    TriangulateEC<float>::Tree* pkOuter1 = WM4_NEW TriangulateEC<float>::Tree;
    pkOuter1->Polygon.resize(5);
    pkOuter1->Polygon[0] = 19;
    pkOuter1->Polygon[1] = 20;
    pkOuter1->Polygon[2] = 21;
    pkOuter1->Polygon[3] = 22;
    pkOuter1->Polygon[4] = 23;
    pkInner2->Child.push_back(pkOuter1);
    m_kFillSeed.push_back(Vector2f(92.0f,124.0f));

    // outer2 polygon (contained in inner2)
    TriangulateEC<float>::Tree* pkOuter2 = WM4_NEW TriangulateEC<float>::Tree;
    pkOuter2->Polygon.resize(4);
    pkOuter2->Polygon[0] = 24;
    pkOuter2->Polygon[1] = 25;
    pkOuter2->Polygon[2] = 26;
    pkOuter2->Polygon[3] = 27;
    pkInner2->Child.push_back(pkOuter2);
    m_kFillSeed.push_back(Vector2f(109.0f,179.0f));

    // outer3 polygon (contained in inner1)
    TriangulateEC<float>::Tree* pkOuter3 = WM4_NEW TriangulateEC<float>::Tree;
    pkOuter3->Polygon.resize(3);
    pkOuter3->Polygon[0] = 28;
    pkOuter3->Polygon[1] = 29;
    pkOuter3->Polygon[2] = 30;
    pkInner1->Child.push_back(pkOuter3);
    m_kFillSeed.push_back(Vector2f(172.0f,139.0f));

    // inner3 polygon (contained in outer2)
    TriangulateEC<float>::Tree* pkInner3 = WM4_NEW TriangulateEC<float>::Tree;
    pkInner3->Polygon.resize(4);
    pkInner3->Polygon[0] = 31;
    pkInner3->Polygon[1] = 32;
    pkInner3->Polygon[2] = 33;
    pkInner3->Polygon[3] = 34;
    pkOuter2->Child.push_back(pkInner3);

    // inner4 polygon (contained in outer2)
    TriangulateEC<float>::Tree* pkInner4 = WM4_NEW TriangulateEC<float>::Tree;
    pkInner4->Polygon.resize(4);
    pkInner4->Polygon[0] = 35;
    pkInner4->Polygon[1] = 36;
    pkInner4->Polygon[2] = 37;
    pkInner4->Polygon[3] = 38;
    pkOuter2->Child.push_back(pkInner4);

    // inner5 polygon (contained in outer3)
    TriangulateEC<float>::Tree* pkInner5 = WM4_NEW TriangulateEC<float>::Tree;
    pkInner5->Polygon.resize(4);
    pkInner5->Polygon[0] = 39;
    pkInner5->Polygon[1] = 40;
    pkInner5->Polygon[2] = 41;
    pkInner5->Polygon[3] = 42;
    pkOuter3->Child.push_back(pkInner5);

    TriangulateEC<float>(m_kPositions,m_eType,m_fEpsilon,m_pkRoot,
        m_kTriangles);
}
//----------------------------------------------------------------------------
