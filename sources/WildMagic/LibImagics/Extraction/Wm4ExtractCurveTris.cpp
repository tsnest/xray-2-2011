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
// Version: 4.0.1 (2006/09/21)

#include "Wm4ImagicsPCH.h"
#include "Wm4ExtractCurveTris.h"
using namespace Wm4;

//----------------------------------------------------------------------------
ExtractCurveTris::ExtractCurveTris (int iXBound, int iYBound, int* aiData)
{
    assert(iXBound > 0 && iYBound > 0 && aiData);
    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iQuantity = iXBound*iYBound;
    m_aiData = aiData;
}
//----------------------------------------------------------------------------
void ExtractCurveTris::ExtractContour (int iLevel,
    std::vector<Vector2f>& rkVA, std::vector<EdgeKey>& rkEA)
{
    // Adjust the image so that the level set is F(x,y) = 0.
    int i;
    for (i = 0; i < m_iQuantity; i++)
    {
        m_aiData[i] -= iLevel;
    }

    // The vertices are computed as rational numbers.
    std::vector<Vertex> kVArray;

    int iXBoundM1 = m_iXBound - 1, iYBoundM1 = m_iYBound - 1;
    for (int iY = 0, iYP = 1; iY < iYBoundM1; iY++, iYP++)
    {
        int iYParity = (iY & 1);

        for (int iX = 0, iXP = 1; iX < iXBoundM1; iX++, iXP++)
        {
            int iXParity = (iX & 1);

            // Get the image values at the corners of the square.
            int i00 = iX + m_iXBound*iY;
            int i10 = i00 + 1;
            int i01 = i00 + m_iXBound;
            int i11 = i10 + m_iXBound;
            int iF00 = m_aiData[i00];
            int iF10 = m_aiData[i10];
            int iF01 = m_aiData[i01];
            int iF11 = m_aiData[i11];

            if (iXParity == iYParity)
            {
                ProcessTriangle(kVArray,rkEA,iX,iY,iF00,iX,iYP,iF01,iXP,iY,
                    iF10);

                ProcessTriangle(kVArray,rkEA,iXP,iYP,iF11,iXP,iY,iF10,iX,iYP,
                    iF01);
            }
            else
            {
                ProcessTriangle(kVArray,rkEA,iX,iYP,iF01,iXP,iYP,iF11,iX,iY,
                    iF00);

                ProcessTriangle(kVArray,rkEA,iXP,iY,iF10,iX,iY,iF00,iXP,iYP,
                    iF11);
            }
        }
    }

    // Convert the rational vertices to floating-point vertices.
    int iVQuantity = (int)kVArray.size();
    rkVA.resize(iVQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        rkVA[i][0] = ((float)kVArray[i].XNumer)/(float)kVArray[i].XDenom;
        rkVA[i][1] = ((float)kVArray[i].YNumer)/(float)kVArray[i].YDenom;
    }

    // Restore the image values.
    for (i = 0; i < m_iQuantity; i++)
    {
        m_aiData[i] += iLevel;
    }
}
//----------------------------------------------------------------------------
void ExtractCurveTris::MakeUnique (std::vector<Vector2f>& rkVA,
    std::vector<EdgeKey>& rkEA)
{
    int iVQuantity = (int)rkVA.size();
    if (iVQuantity == 0)
    {
        return;
    }

    // Use maps to generate unique storage.

    typedef std::map<Vector2f,int> VMap;
    typedef std::map<Vector2f,int>::iterator VIterator;
    VMap kVMap;
    for (int iV = 0, iNextVertex = 0; iV < iVQuantity; iV++)
    {
        std::pair<VIterator,bool> kResult = kVMap.insert(
            std::make_pair(rkVA[iV],iNextVertex));
        if (kResult.second == true)
        {
            iNextVertex++;
        }
    }

    typedef std::map<EdgeKey,int> EMap;
    typedef std::map<EdgeKey,int>::iterator EIterator;
    EMap* pkEMap = 0;
    int iE;
    VIterator pkVIter;

    int iEQuantity = (int)rkEA.size();
    if (iEQuantity)
    {
        pkEMap = WM4_NEW EMap;
        int iNextEdge = 0;
        for (iE = 0; iE < iEQuantity; iE++)
        {
            // Replace old vertex indices by new ones.
            pkVIter = kVMap.find(rkVA[rkEA[iE].V[0]]);
            assert(pkVIter != kVMap.end());
            rkEA[iE].V[0] = pkVIter->second;
            pkVIter = kVMap.find(rkVA[rkEA[iE].V[1]]);
            assert(pkVIter != kVMap.end());
            rkEA[iE].V[1] = pkVIter->second;

            // Keep only unique edges.
            std::pair<EIterator,bool> kResult = pkEMap->insert(
                std::make_pair(rkEA[iE],iNextEdge));
            if (kResult.second == true)
            {
                iNextEdge++;
            }
        }
    }

    // Pack the vertices into an array.
    iVQuantity = (int)kVMap.size();
    rkVA.resize(iVQuantity);
    for (pkVIter = kVMap.begin(); pkVIter != kVMap.end(); pkVIter++)
    {
        rkVA[pkVIter->second] = pkVIter->first;
    }

    // Pack the edges into an array.
    if (iEQuantity > 0)
    {
        iEQuantity = (int)pkEMap->size();
        rkEA.resize(iEQuantity);
        EIterator pkEIter;
        for (pkEIter = pkEMap->begin(); pkEIter != pkEMap->end(); pkEIter++)
        {
            rkEA[pkEIter->second] = pkEIter->first;
        }
        WM4_DELETE pkEMap;
    }
    else
    {
        rkEA.clear();
    }
}
//----------------------------------------------------------------------------
void ExtractCurveTris::AddVertex (std::vector<Vertex>& rkVA, int iXNumer,
    int iXDenom, int iYNumer, int iYDenom)
{
    rkVA.push_back(Vertex(iXNumer,iXDenom,iYNumer,iYDenom));
}
//----------------------------------------------------------------------------
void ExtractCurveTris::AddEdge (std::vector<Vertex>& rkVA,
    std::vector<EdgeKey>& rkEA, int iXNumer0, int iXDenom0, int iYNumer0,
    int iYDenom0, int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1)
{
    int iV0 = (int)rkVA.size(), iV1 = iV0+1;
    rkEA.push_back(EdgeKey(iV0,iV1));
    rkVA.push_back(Vertex(iXNumer0,iXDenom0,iYNumer0,iYDenom0));
    rkVA.push_back(Vertex(iXNumer1,iXDenom1,iYNumer1,iYDenom1));
}
//----------------------------------------------------------------------------
void ExtractCurveTris::ProcessTriangle (std::vector<Vertex>& rkVA,
    std::vector<EdgeKey>& rkEA, int iX0, int iY0, int iF0, int iX1, int iY1,
    int iF1, int iX2, int iY2, int iF2)
{
    int iXN0, iYN0, iXN1, iYN1, iD0, iD1;

    if (iF0 != 0)
    {
        // convert to case "+**"
        if (iF0 < 0)
        {
            iF0 = -iF0;
            iF1 = -iF1;
            iF2 = -iF2;
        }

        if (iF1 > 0)
        {
            if (iF2 > 0)
            {
                // +++
                return;
            }
            else if (iF2 < 0)
            {
                // ++-
                iD0 = iF0 - iF2;
                iXN0 = iF0*iX2 - iF2*iX0;
                iYN0 = iF0*iY2 - iF2*iY0;
                iD1 = iF1 - iF2;
                iXN1 = iF1*iX2 - iF2*iX1;
                iYN1 = iF1*iY2 - iF2*iY1;
                AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD0,iXN1,iD1,iYN1,iD1);
            }
            else
            {
                // ++0
                AddVertex(rkVA,iX2,1,iY2,1);
            }
        }
        else if (iF1 < 0)
        {
            iD0 = iF0 - iF1;
            iXN0 = iF0*iX1 - iF1*iX0;
            iYN0 = iF0*iY1 - iF1*iY0;

            if (iF2 > 0)
            {
                // +-+
                iD1 = iF2 - iF1;
                iXN1 = iF2*iX1 - iF1*iX2;
                iYN1 = iF2*iY1 - iF1*iY2;
                AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD0,iXN1,iD1,iYN1,iD1);
            }
            else if (iF2 < 0)
            {
                // +--
                iD1 = iF2 - iF0;
                iXN1 = iF2*iX0 - iF0*iX2;
                iYN1 = iF2*iY0 - iF0*iY2;
                AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD0,iXN1,iD1,iYN1,iD1);
            }
            else
            {
                // +-0
                AddEdge(rkVA,rkEA,iX2,1,iY2,1,iXN0,iD0,iYN0,iD0);
            }
        }
        else
        {
            if (iF2 > 0)
            {
                // +0+
                AddVertex(rkVA,iX1,1,iY1,1);
            }
            else if (iF2 < 0)
            {
                // +0-
                iD0 = iF2 - iF0;
                iXN0 = iF2*iX0 - iF0*iX2;
                iYN0 = iF2*iY0 - iF0*iY2;
                AddEdge(rkVA,rkEA,iX1,1,iY1,1,iXN0,iD0,iYN0,iD0);
            }
            else
            {
                // +00
                AddEdge(rkVA,rkEA,iX1,1,iY1,1,iX2,1,iY2,1);
            }
        }
    }
    else if (iF1 != 0)
    {
        // convert to case 0+*
        if (iF1 < 0)
        {
            iF1 = -iF1;
            iF2 = -iF2;
        }

        if (iF2 > 0)
        {
            // 0++
            AddVertex(rkVA,iX0,1,iY0,1);
        }
        else if (iF2 < 0)
        {
            // 0+-
            iD0 = iF1 - iF2;
            iXN0 = iF1*iX2 - iF2*iX1;
            iYN0 = iF1*iY2 - iF2*iY1;
            AddEdge(rkVA,rkEA,iX0,1,iY0,1,iXN0,iD0,iYN0,iD0);
        }
        else
        {
            // 0+0
            AddEdge(rkVA,rkEA,iX0,1,iY0,1,iX2,1,iY2,1);
        }
    }
    else if (iF2 != 0)
    {
        // cases 00+ or 00-
        AddEdge(rkVA,rkEA,iX0,1,iY0,1,iX1,1,iY1,1);
    }
    else
    {
        // case 000
        AddEdge(rkVA,rkEA,iX0,1,iY0,1,iX1,1,iY1,1);
        AddEdge(rkVA,rkEA,iX1,1,iY1,1,iX2,1,iY2,1);
        AddEdge(rkVA,rkEA,iX2,1,iY2,1,iX0,1,iY0,1);
    }
}
//----------------------------------------------------------------------------
ExtractCurveTris::Vertex::Vertex (int iXNumer, int iXDenom, int iYNumer,
    int iYDenom)
{
    if (iXDenom > 0)
    {
        XNumer = iXNumer;
        XDenom = iXDenom;
    }
    else
    {
        XNumer = -iXNumer;
        XDenom = -iXDenom;
    }

    if (iYDenom > 0)
    {
        YNumer = iYNumer;
        YDenom = iYDenom;
    }
    else
    {
        YNumer = -iYNumer;
        YDenom = -iYDenom;
    }
}
//----------------------------------------------------------------------------
