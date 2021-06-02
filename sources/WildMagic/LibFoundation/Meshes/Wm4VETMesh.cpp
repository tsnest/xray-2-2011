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
// Version: 4.4.0 (2007/06/07)

#include "Wm4FoundationPCH.h"
#include "Wm4VETMesh.h"
using namespace Wm4;
using namespace std;

//----------------------------------------------------------------------------
VETMesh::VETMesh ()
{
}
//----------------------------------------------------------------------------
VETMesh::~VETMesh ()
{
}
//----------------------------------------------------------------------------
void VETMesh::InsertTriangle (int iV0, int iV1, int iV2)
{
    Triangle kT(iV0,iV1,iV2);
    Edge kE0(iV0,iV1), kE1(iV1,iV2), kE2(iV2,iV0);

    // Insert the triangle.
    pair<MTIter,bool> kRT = m_kTMap.insert(make_pair(kT,TriangleAttribute()));

    // Insert the vertices.
    pair<MVIter,bool> kRV0 = m_kVMap.insert(make_pair(iV0,VertexAttribute()));
    kRV0.first->second.ESet.Insert(kE0);
    kRV0.first->second.ESet.Insert(kE2);
    kRV0.first->second.TSet.Insert(kT);

    pair<MVIter,bool> kRV1 = m_kVMap.insert(make_pair(iV1,VertexAttribute()));
    kRV1.first->second.ESet.Insert(kE0);
    kRV1.first->second.ESet.Insert(kE1);
    kRV1.first->second.TSet.Insert(kT);

    pair<MVIter,bool> kRV2 = m_kVMap.insert(make_pair(iV2,VertexAttribute()));
    kRV2.first->second.ESet.Insert(kE1);
    kRV2.first->second.ESet.Insert(kE2);
    kRV2.first->second.TSet.Insert(kT);

    // Insert the edges.
    pair<MEIter,bool> kRE0 = m_kEMap.insert(make_pair(kE0,EdgeAttribute()));
    kRE0.first->second.TSet.Insert(kT);

    pair<MEIter,bool> kRE1 = m_kEMap.insert(make_pair(kE1,EdgeAttribute()));
    kRE1.first->second.TSet.Insert(kT);

    pair<MEIter,bool> kRE2 = m_kEMap.insert(make_pair(kE2,EdgeAttribute()));
    kRE2.first->second.TSet.Insert(kT);

    // Notify derived classes that mesh components have been inserted.  The
    // notification occurs here to make sure the derived classes have access
    // to the current state of the mesh after the triangle insertion.
    OnVertexInsert(iV0,kRV0.second,kRV0.first->second.Data);
    OnVertexInsert(iV1,kRV1.second,kRV1.first->second.Data);
    OnVertexInsert(iV2,kRV2.second,kRV2.first->second.Data);
    OnEdgeInsert(kE0,kRE0.second,kRE0.first->second.Data);
    OnEdgeInsert(kE1,kRE1.second,kRE1.first->second.Data);
    OnEdgeInsert(kE2,kRE2.second,kRE2.first->second.Data);
    OnTriangleInsert(kT,kRT.second,kRT.first->second.Data);
}
//----------------------------------------------------------------------------
void VETMesh::InsertTriangle (const Triangle& rkT)
{
    InsertTriangle(rkT.V[0],rkT.V[1],rkT.V[2]);
}
//----------------------------------------------------------------------------
void VETMesh::RemoveTriangle (int iV0, int iV1, int iV2)
{
    // Remove the triangle.
    Triangle kT(iV0,iV1,iV2);
    MTIter pkT = m_kTMap.find(kT);
    if (pkT == m_kTMap.end())
    {
        // The triangle does not exist, nothing to do.
        return;
    }

    // Update the edges.
    Edge kE0(iV0,iV1), kE1(iV1,iV2), kE2(iV2,iV0);

    MEIter pkE0 = m_kEMap.find(kE0);
    assert(pkE0 != m_kEMap.end());
    pkE0->second.TSet.Remove(kT);

    MEIter pkE1 = m_kEMap.find(kE1);
    assert(pkE1 != m_kEMap.end());
    pkE1->second.TSet.Remove(kT);

    MEIter pkE2 = m_kEMap.find(kE2);
    assert(pkE2 != m_kEMap.end());
    pkE2->second.TSet.Remove(kT);

    // Update the vertices.
    MVIter pkV0 = m_kVMap.find(iV0);
    assert(pkV0 != m_kVMap.end());
    pkV0->second.TSet.Remove(kT);

    MVIter pkV1 = m_kVMap.find(iV1);
    assert(pkV1 != m_kVMap.end());
    pkV1->second.TSet.Remove(kT);

    MVIter pkV2 = m_kVMap.find(iV2);
    assert(pkV2 != m_kVMap.end());
    pkV2->second.TSet.Remove(kT);

    if (pkE0->second.TSet.GetQuantity() == 0)
    {
        pkV0->second.ESet.Remove(kE0);
        pkV1->second.ESet.Remove(kE0);
    }

    if (pkE1->second.TSet.GetQuantity() == 0)
    {
        pkV1->second.ESet.Remove(kE1);
        pkV2->second.ESet.Remove(kE1);
    }

    if (pkE2->second.TSet.GetQuantity() == 0)
    {
        pkV0->second.ESet.Remove(kE2);
        pkV2->second.ESet.Remove(kE2);
    }

    // Notify derived classes that mesh components are about to be destroyed.
    // The notification occurs here to make sure the derived classes have
    // access to the current state of the mesh before the triangle removal.

    bool bDestroy = pkV0->second.ESet.GetQuantity() == 0 &&
        pkV0->second.TSet.GetQuantity() == 0;
    OnVertexRemove(iV0,bDestroy,pkV0->second.Data);
    if (bDestroy)
    {
        m_kVMap.erase(iV0);
    }

    bDestroy = pkV1->second.ESet.GetQuantity() == 0 &&
        pkV1->second.TSet.GetQuantity() == 0;
    OnVertexRemove(iV1,bDestroy,pkV1->second.Data);
    if (bDestroy)
    {
        m_kVMap.erase(iV1);
    }

    bDestroy = pkV2->second.ESet.GetQuantity() == 0 &&
        pkV2->second.TSet.GetQuantity() == 0;
    OnVertexRemove(iV2,bDestroy,pkV2->second.Data);
    if (bDestroy)
    {
        m_kVMap.erase(iV2);
    }

    bDestroy = pkE0->second.TSet.GetQuantity() == 0;
    OnEdgeRemove(kE0,bDestroy,pkE0->second.Data);
    if (bDestroy)
    {
        m_kEMap.erase(kE0);
    }

    bDestroy = pkE1->second.TSet.GetQuantity() == 0;
    OnEdgeRemove(kE1,bDestroy,pkE1->second.Data);
    if (bDestroy)
    {
        m_kEMap.erase(kE1);
    }

    bDestroy = pkE2->second.TSet.GetQuantity() == 0;
    OnEdgeRemove(kE2,bDestroy,pkE2->second.Data);
    if (bDestroy)
    {
        m_kEMap.erase(kE2);
    }

    OnTriangleRemove(kT,true,pkT->second.Data);
    m_kTMap.erase(kT);
}
//----------------------------------------------------------------------------
void VETMesh::RemoveTriangle (const Triangle& rkT)
{
    RemoveTriangle(rkT.V[0],rkT.V[1],rkT.V[2]);
}
//----------------------------------------------------------------------------
void VETMesh::RemoveAllTriangles ()
{
    MTIter pkT = m_kTMap.begin();
    while (pkT != m_kTMap.end())
    {
        int iV0 = pkT->first.V[0];
        int iV1 = pkT->first.V[1];
        int iV2 = pkT->first.V[2];
        pkT++;

        RemoveTriangle(iV0,iV1,iV2);
    }
}
//----------------------------------------------------------------------------
//void VETMesh::Print (const char* acFilename) const
//{
//    ofstream kOStr(acFilename);
//    int i;
//
//    // Print the vertices.
//    kOStr << "vertex quantity = " << (int)m_kVMap.size() << endl;
//    for (MVCIter pkVM = m_kVMap.begin(); pkVM != m_kVMap.end(); pkVM++)
//    {
//        kOStr << "v<" << pkVM->first << "> : e ";
//
//        const TSmallUnorderedSet<Edge>& rkESet = pkVM->second.ESet;
//        for (i = 0; i < rkESet.GetQuantity(); i++)
//        {
//            kOStr << '<' << rkESet[i].V[0]
//                  << ',' << rkESet[i].V[1]
//                  << "> ";
//        }
//
//        kOStr << ": t ";
//        const TSmallUnorderedSet<Triangle>& rkTSet = pkVM->second.TSet;
//        for (i = 0; i < rkTSet.GetQuantity(); i++)
//        {
//            kOStr << '<' << rkTSet[i].V[0]
//                  << ',' << rkTSet[i].V[1]
//                  << ',' << rkTSet[i].V[2]
//                  << "> ";
//        }
//        kOStr << endl;
//    }
//    kOStr << endl;
//
//    // Print the edges.
//    kOStr << "edge quantity = " << (int)m_kEMap.size() << endl;
//    for (MECIter pkEM = m_kEMap.begin(); pkEM != m_kEMap.end(); pkEM++)
//    {
//        kOStr << "e<" << pkEM->first.V[0] << ',' << pkEM->first.V[1];
//        kOStr << "> : t ";
//        const TSmallUnorderedSet<Triangle>& rkTSet = pkEM->second.TSet;
//        for (i = 0; i < rkTSet.GetQuantity(); i++)
//        {
//            kOStr << '<' << rkTSet[i].V[0]
//                  << ',' << rkTSet[i].V[1]
//                  << ',' << rkTSet[i].V[2]
//                  << "> ";
//        }
//        kOStr << endl;
//    }
//    kOStr << endl;
//
//    // Print the triangles.
//    kOStr << "triangle quantity = " << (int)m_kTMap.size() << endl;
//    for (MTCIter pkTM = m_kTMap.begin(); pkTM != m_kTMap.end(); pkTM++)
//    {
//        kOStr << "t<" << pkTM->first.V[0] << ',' << pkTM->first.V[1];
//        kOStr << ',' << pkTM->first.V[2]  << ">" << endl;
//    }
//    kOStr << endl;
//}
//----------------------------------------------------------------------------
void VETMesh::GetVertices (set<int>& rkVSet) const
{
    rkVSet.clear();
    for (MVCIter pkV = m_kVMap.begin(); pkV != m_kVMap.end(); pkV++)
    {
        rkVSet.insert(pkV->first);
    }
}
//----------------------------------------------------------------------------
void* VETMesh::GetData (int iV)
{
    MVIter pkV = m_kVMap.find(iV);
    return (pkV != m_kVMap.end() ? pkV->second.Data : 0);
}
//----------------------------------------------------------------------------
const TSmallUnorderedSet<VETMesh::Edge>* VETMesh::GetEdges (int iV) const
{
    MVCIter pkV = m_kVMap.find(iV);
    return (pkV != m_kVMap.end() ? &pkV->second.ESet : 0);
}
//----------------------------------------------------------------------------
const TSmallUnorderedSet<VETMesh::Triangle>* VETMesh::GetTriangles (int iV)
    const
{
    MVCIter pkV = m_kVMap.find(iV);
    return (pkV != m_kVMap.end() ? &pkV->second.TSet : 0);
}
//----------------------------------------------------------------------------
void VETMesh::GetEdges (set<Edge>& rkESet) const
{
    rkESet.clear();
    for (MECIter pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
    {
        rkESet.insert(pkE->first);
    }
}
//----------------------------------------------------------------------------
void* VETMesh::GetData (int iV0, int iV1)
{
    MEIter pkE = m_kEMap.find(Edge(iV0,iV1));
    return (pkE != m_kEMap.end() ? pkE->second.Data : 0);
}
//----------------------------------------------------------------------------
void* VETMesh::GetData (const Edge& rkE)
{
    return GetData(rkE.V[0],rkE.V[1]);
}
//----------------------------------------------------------------------------
const TSmallUnorderedSet<VETMesh::Triangle>* VETMesh::GetTriangles (int iV0,
    int iV1) const
{
    MECIter pkE = m_kEMap.find(Edge(iV0,iV1));
    return (pkE != m_kEMap.end() ? &pkE->second.TSet : 0);
}
//----------------------------------------------------------------------------
void VETMesh::GetTriangles (set<Triangle>& rkTSet) const
{
    rkTSet.clear();
    for (MTCIter pkT = m_kTMap.begin(); pkT != m_kTMap.end(); pkT++)
    {
        rkTSet.insert(pkT->first);
    }
}
//----------------------------------------------------------------------------
void* VETMesh::GetData (int iV0, int iV1, int iV2)
{
    MTIter pkT = m_kTMap.find(Triangle(iV0,iV1,iV2));
    return (pkT != m_kTMap.end() ? pkT->second.Data : 0);
}
//----------------------------------------------------------------------------
void* VETMesh::GetData (const Triangle& rkT)
{
    return GetData(rkT.V[0],rkT.V[1],rkT.V[2]);
}
//----------------------------------------------------------------------------
bool VETMesh::IsManifold () const
{
    for (MECIter pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
    {
        if (pkE->second.TSet.GetQuantity() > 2)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool VETMesh::IsClosed () const
{
    for (MECIter pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
    {
        if (pkE->second.TSet.GetQuantity() != 2)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool VETMesh::IsConnected () const
{
    // Do a depth-first search of the mesh.  It is connected if and only if
    // all of the triangles are visited on a single search.

    int iTSize = (int)m_kTMap.size();
    if (iTSize == 0)
    {
        return true;
    }

    // For marking visited triangles during the traversal.
    map<Triangle,bool> kVisitedMap;
    MTCIter pkT;
    for (pkT = m_kTMap.begin(); pkT != m_kTMap.end(); pkT++)
    {
        kVisitedMap.insert(make_pair(pkT->first,false));
    }

    // Start the traversal at any triangle in the mesh.
    stack<Triangle> kStack;
    kStack.push(m_kTMap.begin()->first);
    map<Triangle,bool>::iterator pkVI =
        kVisitedMap.find(m_kTMap.begin()->first);
    assert(pkVI != kVisitedMap.end());
    pkVI->second = true;
    iTSize--;

    while (!kStack.empty())
    {
        // Start at the current triangle.
        Triangle kT = kStack.top();
        kStack.pop();

        for (int i = 0; i < 3; i++)
        {
            // Get an edge of the current triangle.
            MECIter pkE = m_kEMap.find(Edge(kT.V[i],kT.V[(i+1)%3]));

            // Visit each adjacent triangle.
            const TSmallUnorderedSet<Triangle>& rkTSet = pkE->second.TSet;
            for (int j = 0; j < rkTSet.GetQuantity(); j++)
            {
                const Triangle& rkTAdj = rkTSet[j];
                pkVI = kVisitedMap.find(rkTAdj);
                assert(pkVI != kVisitedMap.end());
                if (pkVI->second == false)
                {
                    // This adjacent triangle not yet visited.
                    kStack.push(rkTAdj);
                    pkVI->second = true;
                    iTSize--;
                }
            }
        }
    }

    return iTSize == 0;
}
//----------------------------------------------------------------------------
void VETMesh::GetComponents (vector<VETMesh*>& rkComponents)
{
    // Do a depth-first search of the mesh to find connected components.
    int iTSize = (int)m_kTMap.size();
    if (iTSize == 0)
    {
        return;
    }

    // For marking visited triangles during the traversal.
    map<Triangle,bool> kVisitedMap;
    MTCIter pkT;
    for (pkT = m_kTMap.begin(); pkT != m_kTMap.end(); pkT++)
    {
        kVisitedMap.insert(make_pair(pkT->first,false));
    }

    while (iTSize > 0)
    {
        // Find an unvisited triangle in the mesh.
        stack<Triangle> kStack;
        map<Triangle,bool>::iterator pkVI = kVisitedMap.begin();
        while (pkVI != kVisitedMap.end())
        {
            if (pkVI->second == false)
            {
                kStack.push(pkVI->first);
                pkVI->second = true;
                iTSize--;
                break;
            }
            pkVI++;
        }

        // Traverse the connected component of the starting triangle.
        VETMesh* pkComponent = Create();
        while (!kStack.empty())
        {
            // Start at the current triangle.
            Triangle kT = kStack.top();
            kStack.pop();
            pkComponent->InsertTriangle(kT);

            for (int i = 0; i < 3; i++)
            {
                // Get an edge of the current triangle.
                Edge kE(kT.V[i],kT.V[(i+1)%3]);
                MECIter pkE = m_kEMap.find(kE);

                // Visit each adjacent triangle.
                const TSmallUnorderedSet<Triangle>& rkTSet = pkE->second.TSet;
                for (int j = 0; j < rkTSet.GetQuantity(); j++)
                {
                    const Triangle& rkTAdj = rkTSet[j];
                    pkVI = kVisitedMap.find(rkTAdj);
                    assert(pkVI != kVisitedMap.end());
                    if (pkVI->second == false)
                    {
                        // This adjacent triangle not yet visited.
                        kStack.push(rkTAdj);
                        pkVI->second = true;
                        iTSize--;
                    }
                }
            }
        }
        rkComponents.push_back(pkComponent);
    }
}
//----------------------------------------------------------------------------
void VETMesh::GetComponents (vector<int>& rkIndex, int*& raiConnect)
{
    rkIndex.clear();

    // Do a depth-first search of the mesh to find connected components.
    int iTSize = (int)m_kTMap.size();
    if (iTSize == 0)
    {
        raiConnect = 0;
        return;
    }

    int iIQuantity = 3*iTSize;
    int iIndex = 0;
    raiConnect = new int[iIQuantity];

    // For marking visited triangles during the traversal.
    map<Triangle,bool> kVisitedMap;
    MTCIter pkT;
    for (pkT = m_kTMap.begin(); pkT != m_kTMap.end(); pkT++)
    {
        kVisitedMap.insert(make_pair(pkT->first,false));
    }

    while (iTSize > 0)
    {
        // Find an unvisited triangle in the mesh.
        stack<Triangle> kStack;
        map<Triangle,bool>::iterator pkVI = kVisitedMap.begin();
        while (pkVI != kVisitedMap.end())
        {
            if (pkVI->second == false)
            {
                kStack.push(pkVI->first);
                pkVI->second = true;
                iTSize--;
                break;
            }
            pkVI++;
        }

        // Traverse the connected component of the starting triangle.
        VETMesh* pkComponent = Create();
        while (!kStack.empty())
        {
            // Start at the current triangle.
            Triangle kT = kStack.top();
            kStack.pop();
            pkComponent->InsertTriangle(kT);

            for (int i = 0; i < 3; i++)
            {
                // Get an edge of the current triangle.
                Edge kE(kT.V[i],kT.V[(i+1)%3]);
                MECIter pkE = m_kEMap.find(kE);

                // Visit each adjacent triangle.
                const TSmallUnorderedSet<Triangle>& rkTSet = pkE->second.TSet;
                for (int j = 0; j < rkTSet.GetQuantity(); j++)
                {
                    const Triangle& rkTAdj = rkTSet[j];
                    pkVI = kVisitedMap.find(rkTAdj);
                    assert(pkVI != kVisitedMap.end());
                    if (pkVI->second == false)
                    {
                        // This adjacent triangle not yet visited.
                        kStack.push(rkTAdj);
                        pkVI->second = true;
                        iTSize--;
                    }
                }
            }
        }

        // Store the connectivity information for this component.
        set<Triangle> kTSet;
        pkComponent->GetTriangles(kTSet);
        delete pkComponent;

        rkIndex.push_back(iIndex);
        set<Triangle>::iterator pkTIter;
        for (pkTIter = kTSet.begin(); pkTIter != kTSet.end(); pkTIter++)
        {
            assert(iIndex+3 <= iIQuantity);
            const Triangle& rkT = *pkTIter;
            raiConnect[iIndex++] = rkT.V[0];
            raiConnect[iIndex++] = rkT.V[1];
            raiConnect[iIndex++] = rkT.V[2];
        }

    }
    
    rkIndex.push_back(iIQuantity);
}
//----------------------------------------------------------------------------
void VETMesh::RemoveComponent (int& riIQuantity, int* aiConnect)
{
    // Do a depth-first search of the mesh to find connected components.  The
    // input array is assumed to be large enough to hold the component (see
    // the comments in WmlTriangleMesh.h for RemoveComponent).
    riIQuantity = 0;

    int iTSize = (int)m_kTMap.size();
    if (iTSize == 0)
    {
        return;
    }

    // Find the connected component containing the first triangle in the mesh.
    // A set is used instead of a stack to avoid having a large-memory
    // 'visited' map.
    set<Triangle> kVisited;
    kVisited.insert(m_kTMap.begin()->first);

    // Traverse the connected component.
    while (!kVisited.empty())
    {
        // Start at the current triangle.
        Triangle kT = *kVisited.begin();

        // Add adjacent triangles to the set for recursive processing.
        for (int i = 0; i < 3; i++)
        {
            // Get an edge of the current triangle.
            Edge kE(kT.V[i],kT.V[(i+1)%3]);
            MECIter pkE = m_kEMap.find(kE);
            assert(pkE != m_kEMap.end());

            // Visit each adjacent triangle.
            const TSmallUnorderedSet<Triangle>& rkTSet = pkE->second.TSet;
            for (int j = 0; j < rkTSet.GetQuantity(); j++)
            {
                Triangle kTAdj = rkTSet[j];
                if (kTAdj != kT)
                {
                    kVisited.insert(kTAdj);
                }
            }
        }

        // Add triangle to connectivity array.
        aiConnect[riIQuantity++] = kT.V[0];
        aiConnect[riIQuantity++] = kT.V[1];
        aiConnect[riIQuantity++] = kT.V[2];

        // Remove the current triangle (visited, no longer needed).
        kVisited.erase(kT);
        RemoveTriangle(kT);
    }
}
//----------------------------------------------------------------------------
bool VETMesh::GetConsistentComponents (vector<VETMesh*>& rkComponents)
{
    if (!IsManifold())
    {
        return false;
    }

    // Do a depth-first search of the mesh to find connected components.
    int iTSize = (int)m_kTMap.size();
    if (iTSize == 0)
    {
        return true;
    }

    // for marking visited triangles during the traversal
    map<Triangle,bool> kVisitedMap;
    MTCIter pkT;
    for (pkT = m_kTMap.begin(); pkT != m_kTMap.end(); pkT++)
    {
        kVisitedMap.insert(make_pair(pkT->first,false));
    }

    while (iTSize > 0)
    {
        // Find an unvisited triangle in the mesh.  Any triangle pushed onto
        // the stack is considered to have a consistent ordering.
        stack<Triangle> kStack;
        map<Triangle,bool>::iterator pkVI = kVisitedMap.begin();
        while (pkVI != kVisitedMap.end())
        {
            if (pkVI->second == false)
            {
                kStack.push(pkVI->first);
                pkVI->second = true;
                iTSize--;
                break;
            }
            pkVI++;
        }

        // Traverse the connected component of the starting triangle.
        VETMesh* pkComponent = Create();
        while (!kStack.empty())
        {
            // Start at the current triangle.
            Triangle kT = kStack.top();
            kStack.pop();
            pkComponent->InsertTriangle(kT);

            for (int i = 0; i < 3; i++)
            {
                // Get an edge of the current triangle.
                int iV0 = kT.V[i], iV1 = kT.V[(i+1)%3], iV2;
                Edge kE(iV0,iV1);
                MECIter pkE = m_kEMap.find(kE);

                int iSize = pkE->second.TSet.GetQuantity();
                assert(iSize == 1 || iSize == 2);  // mesh is manifold
                const Triangle* pkTAdj = &pkE->second.TSet[0];
                if (iSize == 2)
                {
                    // Get the adjacent triangle to the current one.
                    if (*pkTAdj == kT)
                    {
                        pkTAdj = &pkE->second.TSet[1];
                    }

                    pkVI = kVisitedMap.find(*pkTAdj);
                    assert(pkVI != kVisitedMap.end());
                    if (pkVI->second == false)
                    {
                        // Adjacent triangle not yet visited.
                        if ((pkTAdj->V[0]==iV0 && pkTAdj->V[1]==iV1)
                        ||  (pkTAdj->V[1]==iV0 && pkTAdj->V[2]==iV1)
                        ||  (pkTAdj->V[2]==iV0 && pkTAdj->V[0]==iV1))
                        {
                            // Adjacent triangle must be reordered.
                            iV0 = pkTAdj->V[0];
                            iV1 = pkTAdj->V[1];
                            iV2 = pkTAdj->V[2];
                            kVisitedMap.erase(*pkTAdj);
                            RemoveTriangle(iV0,iV1,iV2);
                            InsertTriangle(iV1,iV0,iV2);
                            kVisitedMap.insert(make_pair(Triangle(iV1,iV0,
                                iV2),false));

                            // Refresh the iterators since maps changed.
                            pkE = m_kEMap.find(kE);
                            pkTAdj = &pkE->second.TSet[0];
                            if (*pkTAdj == kT)
                            {
                                pkTAdj = &pkE->second.TSet[1];
                            }
                            pkVI = kVisitedMap.find(*pkTAdj);
                            assert(pkVI != kVisitedMap.end());
                        }

                        kStack.push(*pkTAdj);
                        pkVI->second = true;
                        iTSize--;
                    }
                }
            }
        }
        rkComponents.push_back(pkComponent);
    }

    return true;
}
//----------------------------------------------------------------------------
VETMesh* VETMesh::GetReversedOrderMesh () const
{
    VETMesh* pkReversed = Create();

    for (MTCIter pkT = m_kTMap.begin(); pkT != m_kTMap.end(); pkT++)
    {
        pkReversed->InsertTriangle(pkT->first.V[0],pkT->first.V[2],
            pkT->first.V[1]);
    }

    return pkReversed;
}
//----------------------------------------------------------------------------
void VETMesh::GetStatistics (int& riVQuantity, int& riEQuantity,
    int& riTQuantity, float& rfAverageEdgesPerVertex,
    float& rfAverageTrianglesPerVertex, float& rfAverageTrianglesPerEdge,
    int& riMaximumEdgesPerVertex, int& riMaximumTrianglesPerVertex,
    int& riMaximumTrianglesPerEdge)
{
    riVQuantity = (int)m_kVMap.size();
    riEQuantity = (int)m_kEMap.size();
    riTQuantity = (int)m_kTMap.size();

    int iESumForV = 0;
    int iTSumForV = 0;
    riMaximumEdgesPerVertex = 0;
    riMaximumTrianglesPerVertex = 0;

    int iESize, iTSize;
    
    for (MVCIter pkV = m_kVMap.begin(); pkV != m_kVMap.end(); pkV++)
    {
        iESize = pkV->second.ESet.GetQuantity();
        iTSize = pkV->second.TSet.GetQuantity();
        iESumForV += iESize;
        iTSumForV += iTSize;
        if (iESize > riMaximumEdgesPerVertex)
        {
            riMaximumEdgesPerVertex = iESize;
        }
        if (iTSize > riMaximumTrianglesPerVertex)
        {
            riMaximumTrianglesPerVertex = iTSize;
        }
    }
    
    int iTSumForE = 0;
    riMaximumTrianglesPerEdge = 0;
    for (MECIter pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
    {
        iTSize = pkE->second.TSet.GetQuantity();
        iTSumForE += iTSize;
        if (iTSize > riMaximumTrianglesPerEdge)
        {
            riMaximumTrianglesPerEdge = iTSize;
        }
    }
    
    rfAverageEdgesPerVertex = ((float)iESumForV)/riVQuantity;
    rfAverageTrianglesPerVertex = ((float)iTSumForV)/riVQuantity;
    rfAverageTrianglesPerEdge = ((float)iTSumForE)/riEQuantity;
}
//----------------------------------------------------------------------------
