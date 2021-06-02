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
#include "Wm4ExtractCurveSquares.h"
using namespace Wm4;

//----------------------------------------------------------------------------
ExtractCurveSquares::ExtractCurveSquares (int iXBound, int iYBound,
    int* aiData)
{
    assert(iXBound > 0 && iYBound > 0 && aiData);
    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iQuantity = iXBound*iYBound;
    m_aiData = aiData;
}
//----------------------------------------------------------------------------
void ExtractCurveSquares::ExtractContour (int iLevel,
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
        for (int iX = 0, iXP = 1; iX < iXBoundM1; iX++, iXP++)
        {
            // Get the image values at the corners of the square.
            int i00 = iX + m_iXBound*iY;
            int i10 = i00 + 1;
            int i01 = i00 + m_iXBound;
            int i11 = i10 + m_iXBound;
            int iF00 = m_aiData[i00];
            int iF10 = m_aiData[i10];
            int iF01 = m_aiData[i01];
            int iF11 = m_aiData[i11];

            // Construct the vertices and edges of the level curve in the
            // square.
            ProcessSquare(kVArray,rkEA,iX,iXP,iY,iYP,iF00,iF10,iF11,iF01);
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
void ExtractCurveSquares::MakeUnique (std::vector<Vector2f>& rkVA,
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
void ExtractCurveSquares::AddVertex (std::vector<Vertex>& rkVA, int iXNumer,
    int iXDenom, int iYNumer, int iYDenom)
{
    rkVA.push_back(Vertex(iXNumer,iXDenom,iYNumer,iYDenom));
}
//----------------------------------------------------------------------------
void ExtractCurveSquares::AddEdge (std::vector<Vertex>& rkVA,
    std::vector<EdgeKey>& rkEA, int iXNumer0, int iXDenom0, int iYNumer0,
    int iYDenom0, int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1)
{
    int iV0 = (int)rkVA.size(), iV1 = iV0+1;
    rkEA.push_back(EdgeKey(iV0,iV1));
    rkVA.push_back(Vertex(iXNumer0,iXDenom0,iYNumer0,iYDenom0));
    rkVA.push_back(Vertex(iXNumer1,iXDenom1,iYNumer1,iYDenom1));
}
//----------------------------------------------------------------------------
void ExtractCurveSquares::ProcessSquare (std::vector<Vertex>& rkVA,
    std::vector<EdgeKey>& rkEA, int iX, int iXP, int iY, int iYP, int iF00,
    int iF10, int iF11, int iF01)
{
    int iXN0, iYN0, iXN1, iYN1, iD0, iD1, iD2, iD3, iDet;

    if (iF00 != 0)
    {
        // convert to case "+***"
        if (iF00 < 0)
        {
            iF00 = -iF00;
            iF10 = -iF10;
            iF11 = -iF11;
            iF01 = -iF01;
        }

        if (iF10 > 0)
        {
            if (iF11 > 0)
            {
                if (iF01 > 0)
                {
                    // ++++
                    return;
                }
                else if (iF01 < 0)
                {
                    // +++-
                    iD0 = iF11 - iF01;
                    iXN0 = iF11*iX - iF01*iXP;
                    iD1 = iF00 - iF01;
                    iYN1 = iF00*iYP - iF01*iY;
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iX,1,iYN1,iD1);
                }
                else
                {
                    // +++0
                    AddVertex(rkVA,iX,1,iYP,1);
                }
            }
            else if (iF11 < 0)
            {
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;

                if (iF01 > 0)
                {
                    // ++-+
                    iD1 = iF01 - iF11;
                    iXN1 = iF01*iXP - iF11*iX;
                    AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iXN1,iD1,iYP,1);
                }
                else if (iF01 < 0)
                {
                    // ++--
                    iD1 = iF01 - iF00;
                    iYN1 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iX,1,iYN1,iD1,iXP,1,iYN0,iD0);
                }
                else
                {
                    // ++-0
                    AddEdge(rkVA,rkEA,iX,1,iYP,1,iXP,1,iYN0,iD0);
                }
            }
            else
            {
                if (iF01 > 0)
                {
                    // ++0+
                    AddVertex(rkVA,iXP,1,iYP,1);
                }
                else if (iF01 < 0)
                {
                    // ++0-
                    iD0 = iF01 - iF00;
                    iYN0 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYN0,iD0);
                }
                else
                {
                    // ++00
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
                }
            }
        }
        else if (iF10 < 0)
        {
            iD0 = iF00 - iF10;
            iXN0 = iF00*iXP - iF10*iX;

            if (iF11 > 0)
            {
                iD1 = iF11 - iF10;
                iYN1 = iF11*iY - iF10*iYP;

                if (iF01 > 0)
                {
                    // +-++
                    AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iYN1,iD1);
                }
                else if (iF01 < 0)
                {
                    // +-+-
                    iD3 = iF11 - iF01;
                    iXN1 = iF11*iX - iF01*iXP;
                    iD2 = iF01 - iF00;
                    iYN0 = iF01*iY - iF00*iYP;

                    if (iD0*iD3 > 0)
                    {
                        iDet = iXN1*iD0 - iXN0*iD3;
                    }
                    else
                    {
                        iDet = iXN0*iD3 - iXN1*iD0;
                    }

                    if (iDet > 0)
                    {
                        AddEdge(rkVA,rkEA,iXN1,iD3,iYP,1,iXP,1,iYN1,iD1);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iX,1,iYN0,iD2);
                    }
                    else if (iDet < 0)
                    {
                        AddEdge(rkVA,rkEA,iXN1,iD3,iYP,1,iX,1,iYN0,iD2);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iYN1,iD1);
                    }
                    else
                    {
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iXN0,iD0,iY,1);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iXN0,iD0,iYP,1);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iX,1,iYN0,iD2);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iXP,1,iYN0,iD2);
                    }
                }
                else
                {
                    // +-+0
                    AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iYN1,iD1);
                    AddVertex(rkVA,iX,1,iYP,1);
                }
            }
            else if (iF11 < 0)
            {
                if (iF01 > 0)
                {
                    // +--+
                    iD1 = iF11 - iF01;
                    iXN1 = iF11*iX - iF01*iXP;
                    AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXN1,iD1,iYP,1);
                }
                else if (iF01 < 0)
                {
                    // +---
                    iD1 = iF01 - iF00;
                    iYN1 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iX,1,iYN1,iD1,iXN0,iD0,iY,1);
                }
                else
                {
                    // +--0
                    AddEdge(rkVA,rkEA,iX,1,iYP,1,iXN0,iD0,iY,1);
                }
            }
            else
            {
                if (iF01 > 0)
                {
                    // +-0+
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iXN0,iD0,iY,1);
                }
                else if (iF01 < 0)
                {
                    // +-0-
                    iD1 = iF01 - iF00;
                    iYN1 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iX,1,iYN1,iD1,iXN0,iD0,iY,1);
                    AddVertex(rkVA,iXP,1,iYP,1);
                }
                else
                {
                    // +-00
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iXN0,iD0,iYP,1);
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iX,1,iYP,1);
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iXN0,iD0,iY,1);
                }
            }
        }
        else
        {
            if (iF11 > 0)
            {
                if (iF01 > 0)
                {
                    // +0++
                    AddVertex(rkVA,iXP,1,iY,1);
                }
                else if (iF01 < 0)
                {
                    // +0+-
                    iD0 = iF11 - iF01;
                    iXN0 = iF11*iX - iF01*iXP;
                    iD1 = iF00 - iF01;
                    iYN1 = iF00*iYP - iF01*iY;
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iX,1,iYN1,iD1);
                    AddVertex(rkVA,iXP,1,iY,1);
                }
                else
                {
                    // +0+0
                    AddVertex(rkVA,iXP,1,iY,1);
                    AddVertex(rkVA,iX,1,iYP,1);
                }
            }
            else if (iF11 < 0)
            {
                if (iF01 > 0)
                {
                    // +0-+
                    iD0 = iF11 - iF01;
                    iXN0 = iF11*iX - iF01*iXP;
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iXN0,iD0,iYP,1);
                }
                else if (iF01 < 0)
                {
                    // +0--
                    iD0 = iF01 - iF00;
                    iYN0 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iX,1,iYN0,iD0);
                }
                else
                {
                    // +0-0
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iX,1,iYP,1);
                }
            }
            else
            {
                if (iF01 > 0)
                {
                    // +00+
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
                }
                else if (iF01 < 0)
                {
                    // +00-
                    iD0 = iF00 - iF01;
                    iYN0 = iF00*iYP - iF01*iY;
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYN0,iD0);
                    AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iXP,1,iYP,1);
                    AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iX,1,iYN0,iD0);
                }
                else
                {
                    // +000
                    AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
                    AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
                }
            }
        }
    }
    else if (iF10 != 0)
    {
        // convert to case 0+**
        if (iF10 < 0)
        {
            iF10 = -iF10;
            iF11 = -iF11;
            iF01 = -iF01;
        }

        if (iF11 > 0)
        {
            if (iF01 > 0)
            {
                // 0+++
                AddVertex(rkVA,iX,1,iY,1);
            }
            else if (iF01 < 0)
            {
                // 0++-
                iD0 = iF11 - iF01;
                iXN0 = iF11*iX - iF01*iXP;
                AddEdge(rkVA,rkEA,iX,1,iY,1,iXN0,iD0,iYP,1);
            }
            else
            {
                // 0++0
                AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
            }
        }
        else if (iF11 < 0)
        {
            if (iF01 > 0)
            {
                // 0+-+
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;
                iD1 = iF01 - iF11;
                iXN1 = iF01*iXP - iF11*iX;
                AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iXN1,iD1,iYP,1);
                AddVertex(rkVA,iX,1,iY,1);
            }
            else if (iF01 < 0)
            {
                // 0+--
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;
                AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iYN0,iD0);
            }
            else
            {
                // 0+-0
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;
                AddEdge(rkVA,rkEA,iX,1,iY,1,iX,1,iYN0,iD0);
                AddEdge(rkVA,rkEA,iX,1,iYN0,iD0,iX,1,iYP,1);
                AddEdge(rkVA,rkEA,iX,1,iYN0,iD0,iXP,1,iYN0,iD0);
            }
        }
        else
        {
            if (iF01 > 0)
            {
                // 0+0+
                AddVertex(rkVA,iX,1,iY,1);
                AddVertex(rkVA,iXP,1,iYP,1);
            }
            else if (iF01 < 0)
            {
                // 0+0-
                AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iYP,1);
            }
            else
            {
                // 0+00
                AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
                AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
            }
        }
    }
    else if (iF11 != 0)
    {
        // convert to case 00+*
        if (iF11 < 0)
        {
            iF11 = -iF11;
            iF01 = -iF01;
        }

        if (iF01 > 0)
        {
            // 00++
            AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
        }
        else if (iF01 < 0)
        {
            // 00+-
            iD0 = iF01 - iF11;
            iXN0 = iF01*iXP - iF11*iX;
            AddEdge(rkVA,rkEA,iX,1,iY,1,iXN0,iD0,iY,1);
            AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iY,1);
            AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXN0,iD0,iYP,1);
        }
        else
        {
            // 00+0
            AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
            AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
        }
    }
    else if (iF01 != 0)
    {
        // cases 000+ or 000-
        AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
        AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
    }
    else
    {
        // case 0000
        AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
        AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
        AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
        AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
    }
}
//----------------------------------------------------------------------------
ExtractCurveSquares::Vertex::Vertex (int iXNumer, int iXDenom, int iYNumer,
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
