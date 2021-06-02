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

#include "Wm4FoundationPCH.h"
#include "Wm4ETNonmanifoldMesh.h"
using namespace Wm4;

//----------------------------------------------------------------------------
ETNonmanifoldMesh::ETNonmanifoldMesh (ECreator oECreator, TCreator oTCreator)
{
    m_oECreator = (oECreator ? oECreator : CreateEdge);
    m_oTCreator = (oTCreator ? oTCreator : CreateTriangle);
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::~ETNonmanifoldMesh ()
{
    EMapIterator pkEIter;
    for (pkEIter = m_kEMap.begin(); pkEIter != m_kEMap.end(); pkEIter++)
    {
        WM4_DELETE pkEIter->second;
    }

    TMapIterator pkTIter;
    for (pkTIter = m_kTMap.begin(); pkTIter != m_kTMap.end(); pkTIter++)
    {
        WM4_DELETE pkTIter->second;
    }
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::EPtr ETNonmanifoldMesh::CreateEdge (int iV0, int iV1)
{
    return WM4_NEW Edge(iV0,iV1);
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::TPtr ETNonmanifoldMesh::CreateTriangle (int iV0, int iV1,
    int iV2)
{
    return WM4_NEW Triangle(iV0,iV1,iV2);
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::TPtr ETNonmanifoldMesh::InsertTriangle (int iV0, int iV1,
    int iV2)
{
    TriangleKey kTKey(iV0,iV1,iV2);
    TMapIterator pkTIter = m_kTMap.find(kTKey);
    if (pkTIter != m_kTMap.end())
    {
        // triangle already exists
        return 0;
    }

    // add new triangle
    TPtr pkT = m_oTCreator(iV0,iV1,iV2);
    m_kTMap[kTKey] = pkT;

    // add edges to mesh
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        EdgeKey kEKey(pkT->V[i0],pkT->V[i1]);
        EPtr pkE;
        EMapIterator pkEIter = m_kEMap.find(kEKey);
        if (pkEIter == m_kEMap.end())
        {
            // first time edge encountered
            pkE = m_oECreator(pkT->V[i0],pkT->V[i1]);
            m_kEMap[kEKey] = pkE;
        }
        else
        {
            // edge previously encountered and created
            pkE = pkEIter->second;
        }

        // update edge and triangle
        pkE->T.insert(pkT);
        pkT->E[i0] = pkE;
    }

    return pkT;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::RemoveTriangle (int iV0, int iV1, int iV2)
{
    TriangleKey kTKey(iV0,iV1,iV2);
    TMapIterator pkTIter = m_kTMap.find(kTKey);
    if (pkTIter == m_kTMap.end())
    {
        // triangle does not exist
        return false;
    }

    TPtr pkT = pkTIter->second;
    for (int i = 0; i < 3; i++)
    {
        // inform edges you are going away
        EPtr pkE = pkT->E[i];
        pkE->T.erase(pkT);

        // remove edge if you had the last reference to it
        if (pkE->T.size() == 0)
        {
            EdgeKey kEKey(pkE->V[0],pkE->V[1]);
            m_kEMap.erase(kEKey);
            WM4_DELETE pkE;
        }
    }

    m_kTMap.erase(kTKey);
    WM4_DELETE pkT;
    return true;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::IsManifold () const
{
    EMapCIterator pkEIter = m_kEMap.begin();
    for (/**/; pkEIter != m_kEMap.end(); pkEIter++)
    {
        if (pkEIter->second->T.size() > 2)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::IsClosed () const
{
    EMapCIterator pkEIter = m_kEMap.begin();
    for (/**/; pkEIter != m_kEMap.end(); pkEIter++)
    {
        if (pkEIter->second->T.size() != 2)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::IsConnected () const
{
    // Perform a breadth-first search to locate the connected component
    // containing the first triangle in the triangle map.
    if (m_kTMap.begin() == m_kTMap.end())
    {
        // no triangles in the mesh, by default mesh is connected
        return true;
    }

    // start search at first triangle in mesh
    std::set<Triangle*> kComponent, kBoundary;
    kBoundary.insert(m_kTMap.begin()->second);

    while (kBoundary.size() > 0)
    {
        std::set<Triangle*> kExterior;

        // process boundary triangles
        std::set<Triangle*>::iterator pkTIter = kBoundary.begin();
        for (/**/; pkTIter != kBoundary.end(); pkTIter++)
        {
            // boundary triangle is adjacent to current connected component
            TPtr pkT = *pkTIter;
            kComponent.insert(pkT);

            // locate adjacent, exterior triangles for later processing
            for (int i = 0; i < 3; i++)
            {
                EPtr pkE = pkT->E[i];
                std::set<Triangle*>::const_iterator ppkAdj = pkE->T.begin();
                for (/**/; ppkAdj != pkE->T.end(); ppkAdj++)
                {
                    if (*ppkAdj != pkT
                    &&  kComponent.find(*ppkAdj) == kComponent.end()
                    &&  kBoundary.find(*ppkAdj) == kBoundary.end())
                    {
                        kExterior.insert(*ppkAdj);
                    }
                }
            }
        }

        // exterior triangles are next in line to be processed
        kBoundary = kExterior;
    }

    return kComponent.size() == m_kTMap.size();
}
//----------------------------------------------------------------------------
void ETNonmanifoldMesh::RemoveComponent (int& riIQuantity, int* aiConnect)
{
    // Do a breadth-first search of the mesh to find connected components.
    // The input array is assumed to be large enough to hold the component.
    // (See the comments in ETNonmanifoldMesh.h for RemoveComponent.)
    riIQuantity = 0;

    if (m_kTMap.size() == 0)
    {
        return;
    }

    // find the connected component containing the first triangle in the mesh
    std::set<Triangle*> kVisited;
    kVisited.insert(m_kTMap.begin()->second);

    // traverse the connected component
    while (!kVisited.empty())
    {
        // start at the current triangle
        Triangle* pkT = *kVisited.begin();

        // add adjacent triangles to the set for recursive processing
        for (int i = 0; i < 3; i++)
        {
            EPtr pkE = pkT->E[i];
            std::set<Triangle*>::const_iterator ppkAdj = pkE->T.begin();
            for (/**/; ppkAdj != pkE->T.end(); ppkAdj++)
            {
                if (*ppkAdj != pkT)
                {
                    kVisited.insert(*ppkAdj);
                }
            }
        }

        // add triangle to connectivity array
        aiConnect[riIQuantity++] = pkT->V[0];
        aiConnect[riIQuantity++] = pkT->V[1];
        aiConnect[riIQuantity++] = pkT->V[2];

        // remove the current triangle (visited, no longer needed)
        kVisited.erase(pkT);
        RemoveTriangle(pkT->V[0],pkT->V[1],pkT->V[2]);
    }
}
//----------------------------------------------------------------------------
//void ETNonmanifoldMesh::Print (const char* acFilename)
//{
//    std::ofstream kOStr(acFilename);
//    if (!kOStr)
//    {
//        return;
//    }
//
//    // assign unique indices to the edges
//    std::map<Edge*,int> kEIndex;
//    kEIndex[0] = 0;
//    int i = 1;
//    EMapIterator pkEIter;
//    for (pkEIter = m_kEMap.begin(); pkEIter != m_kEMap.end(); pkEIter++)
//    {
//        if (pkEIter->second)
//        {
//            kEIndex[pkEIter->second] = i++;
//        }
//    }
//
//    // assign unique indices to the triangles
//    std::map<Triangle*,int> kTIndex;
//    kTIndex[0] = 0;
//    i = 1;
//    TMapIterator pkTIter;
//    for (pkTIter = m_kTMap.begin(); pkTIter != m_kTMap.end(); pkTIter++)
//    {
//        if (pkTIter->second)
//        {
//            kTIndex[pkTIter->second] = i++;
//        }
//    }
//
//    // print edges
//    kOStr << "edge quantity = " << (int)m_kEMap.size() << std::endl;
//    for (pkEIter = m_kEMap.begin(); pkEIter != m_kEMap.end(); pkEIter++)
//    {
//        Edge* pkE = pkEIter->second;
//        kOStr << 'e' << kEIndex[pkE] << " <"
//              << 'v' << pkE->V[0] << ",v" << pkE->V[1] << "; ";
//
//        std::set<Triangle*>::const_iterator ppkAdj = pkE->T.begin();
//        for (/**/; ppkAdj != pkE->T.end(); ppkAdj++)
//        {
//            kOStr << 't' << kTIndex[*ppkAdj] << ',';
//        }
//        kOStr << '>' << std::endl;
//    }
//    kOStr << std::endl;
//
//    // print triangles
//    kOStr << "triangle quantity = " << (int)m_kTMap.size() << std::endl;
//    for (pkTIter = m_kTMap.begin(); pkTIter != m_kTMap.end(); pkTIter++)
//    {
//        Triangle* pkT = pkTIter->second;
//        kOStr << 't' << kTIndex[pkT] << " <"
//              << 'v' << pkT->V[0] << ",v" << pkT->V[1] << ",v" << pkT->V[2]
//              << "; ";
//        if (pkT->E[0])
//        {
//            kOStr << 'e' << kEIndex[pkT->E[0]];
//        }
//        else
//        {
//            kOStr << '*';
//        }
//        kOStr << ',';
//        if (pkT->E[1])
//        {
//            kOStr << 'e' << kEIndex[pkT->E[1]];
//        }
//        else
//        {
//            kOStr << '*';
//        }
//        kOStr << ',';
//        if (pkT->E[2])
//        {
//            kOStr << 'e' << kEIndex[pkT->E[2]];
//        }
//        else
//        {
//            kOStr << '*';
//        }
//        kOStr << '>' << std::endl;
//    }
//    kOStr << std::endl;
//}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ETNonmanifoldMesh::Edge
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Edge::Edge (int iV0, int iV1)
{
    V[0] = iV0;
    V[1] = iV1;
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Edge::~Edge ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ETNonmanifoldMesh::Triangle
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Triangle::Triangle (int iV0, int iV1, int iV2)
{
    V[0] = iV0;
    V[1] = iV1;
    V[2] = iV2;

    for (int i = 0; i < 3; i++)
    {
        E[i] = 0;
    }
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Triangle::~Triangle ()
{
}
//----------------------------------------------------------------------------
