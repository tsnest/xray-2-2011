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

#include "Wm4GraphicsPCH.h"
#include "Wm4CollisionRecord.h"
#include "Wm4IntrTriangle3Triangle3.h"
using namespace Wm4;

//----------------------------------------------------------------------------
CollisionRecord::CollisionRecord (TriMesh* pkMesh, BoundingVolumeTree* pkTree,
    Vector3f* pkVelocity, Callback oCallback, void* pvCallbackData)
{
    assert(pkMesh && pkTree);

    m_pkMesh = pkMesh;
    m_pkTree = pkTree;
    m_pkVelocity = pkVelocity;
    m_oCallback = oCallback;
    m_pvCallbackData = pvCallbackData;
}
//----------------------------------------------------------------------------
CollisionRecord::~CollisionRecord ()
{
    WM4_DELETE m_pkTree;
}
//----------------------------------------------------------------------------
void CollisionRecord::TestIntersection (CollisionRecord& rkRecord)
{
    // convenience variables
    BoundingVolumeTree* pkTree0 = m_pkTree;
    BoundingVolumeTree* pkTree1 = rkRecord.m_pkTree;
    const TriMesh* pkMesh0 = m_pkTree->GetMesh();
    const TriMesh* pkMesh1 = rkRecord.m_pkTree->GetMesh();

    pkTree0->UpdateWorldBound();
    pkTree1->UpdateWorldBound();

    const BoundingVolume* pkWorldBV0 = pkTree0->GetWorldBound();
    const BoundingVolume* pkWorldBV1 = pkTree1->GetWorldBound();
    if (pkWorldBV0->TestIntersection(pkWorldBV1))
    {
        BoundingVolumeTree* pkRoot;

        if (pkTree0->IsInteriorNode())
        {
            pkRoot = m_pkTree;

            // compare Tree0.L to Tree1
            m_pkTree = pkRoot->GetLChild();
            TestIntersection(rkRecord);

            // compare Tree0.R to Tree1
            m_pkTree = pkRoot->GetRChild();
            TestIntersection(rkRecord);

            m_pkTree = pkRoot;
        }
        else if (pkTree1->IsInteriorNode())
        {
            pkRoot = rkRecord.m_pkTree;

            // compare Tree0 to Tree1.L
            rkRecord.m_pkTree = pkRoot->GetLChild();
            TestIntersection(rkRecord);

            // compare Tree0 to Tree1.R
            rkRecord.m_pkTree = pkRoot->GetRChild();
            TestIntersection(rkRecord);

            rkRecord.m_pkTree = pkRoot;
        }
        else
        {
            // at a leaf in each tree
            int iMax0 = pkTree0->GetTriangleQuantity();
            for (int i0 = 0; i0 < iMax0; i0++)
            {
                int iT0 = pkTree0->GetTriangle(i0);

                // get world space triangle
                Triangle3f kTri0;
                pkMesh0->GetWorldTriangle(iT0,kTri0);

                int iMax1 = pkTree1->GetTriangleQuantity();
                for (int i1 = 0; i1 < iMax1; i1++)
                {
                    int iT1 = pkTree1->GetTriangle(i1);

                    // get world space triangle
                    Triangle3f kTri1;
                    pkMesh1->GetWorldTriangle(iT1,kTri1);

                    IntrTriangle3Triangle3<float> kIntersector(kTri0,kTri1);
                    if (kIntersector.Test())
                    {
                        if (m_oCallback)
                        {
                            m_oCallback(*this,iT0,rkRecord,iT1,&kIntersector);
                        }

                        if (rkRecord.m_oCallback)
                        {
                            rkRecord.m_oCallback(rkRecord,iT1,*this,iT0,
                                &kIntersector);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void CollisionRecord::FindIntersection (CollisionRecord& rkRecord)
{
    // convenience variables
    BoundingVolumeTree* pkTree0 = m_pkTree;
    BoundingVolumeTree* pkTree1 = rkRecord.m_pkTree;
    const TriMesh* pkMesh0 = m_pkTree->GetMesh();
    const TriMesh* pkMesh1 = rkRecord.m_pkTree->GetMesh();

    pkTree0->UpdateWorldBound();
    pkTree1->UpdateWorldBound();

    const BoundingVolume* pkWorldBV0 = pkTree0->GetWorldBound();
    const BoundingVolume* pkWorldBV1 = pkTree1->GetWorldBound();
    if (pkWorldBV0->TestIntersection(pkWorldBV1))
    {
        BoundingVolumeTree* pkRoot;

        if (pkTree0->IsInteriorNode())
        {
            pkRoot = m_pkTree;

            // compare Tree0.L to Tree1
            m_pkTree = pkRoot->GetLChild();
            FindIntersection(rkRecord);

            // compare Tree0.R to Tree1
            m_pkTree = pkRoot->GetRChild();
            FindIntersection(rkRecord);

            m_pkTree = pkRoot;
        }
        else if (pkTree1->IsInteriorNode())
        {
            pkRoot = rkRecord.m_pkTree;

            // compare Tree0 to Tree1.L
            rkRecord.m_pkTree = pkRoot->GetLChild();
            FindIntersection(rkRecord);

            // compare Tree0 to Tree1.R
            rkRecord.m_pkTree = pkRoot->GetRChild();
            FindIntersection(rkRecord);

            rkRecord.m_pkTree = pkRoot;
        }
        else
        {
            // at a leaf in each tree
            int iMax0 = pkTree0->GetTriangleQuantity();
            for (int i0 = 0; i0 < iMax0; i0++)
            {
                int iT0 = pkTree0->GetTriangle(i0);

                // get world space triangle
                Triangle3f kTri0;
                pkMesh0->GetWorldTriangle(iT0,kTri0);

                int iMax1 = pkTree1->GetTriangleQuantity();
                for (int i1 = 0; i1 < iMax1; i1++)
                {
                    int iT1 = pkTree1->GetTriangle(i1);

                    // get world space triangle
                    Triangle3f kTri1;
                    pkMesh1->GetWorldTriangle(iT1,kTri1);

                    IntrTriangle3Triangle3<float> kIntersector(kTri0,kTri1);
                    if (kIntersector.Find())
                    {
                        if (m_oCallback)
                        {
                            m_oCallback(*this,iT0,rkRecord,iT1,&kIntersector);
                        }

                        if (rkRecord.m_oCallback)
                        {
                            rkRecord.m_oCallback(rkRecord,iT1,*this,iT0,
                                &kIntersector);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void CollisionRecord::TestIntersection (float fTMax,
    CollisionRecord& rkRecord)
{
    // convenience variables
    BoundingVolumeTree* pkTree0 = m_pkTree;
    BoundingVolumeTree* pkTree1 = rkRecord.m_pkTree;
    const TriMesh* pkMesh0 = m_pkTree->GetMesh();
    const TriMesh* pkMesh1 = rkRecord.m_pkTree->GetMesh();
    const Vector3f& rkVelocity0 = (m_pkVelocity ? *m_pkVelocity :
        Vector3f::ZERO);
    const Vector3f& rkVelocity1 = (rkRecord.m_pkVelocity ?
        *rkRecord.m_pkVelocity : Vector3f::ZERO);

    pkTree0->UpdateWorldBound();
    pkTree1->UpdateWorldBound();

    const BoundingVolume* pkWorldBV0 = pkTree0->GetWorldBound();
    const BoundingVolume* pkWorldBV1 = pkTree1->GetWorldBound();

    // TO DO. This TestIntersection needs to be for moving bounding volumes.
    if (pkWorldBV0->TestIntersection(pkWorldBV1))
    {
        BoundingVolumeTree* pkRoot;

        if (pkTree0->IsInteriorNode())
        {
            pkRoot = m_pkTree;

            // compare Tree0.L to Tree1
            m_pkTree = pkRoot->GetLChild();
            TestIntersection(fTMax,rkRecord);

            // compare Tree0.R to Tree1
            m_pkTree = pkRoot->GetRChild();
            TestIntersection(fTMax,rkRecord);

            m_pkTree = pkRoot;
        }
        else if (pkTree1->IsInteriorNode())
        {
            pkRoot = rkRecord.m_pkTree;

            // compare Tree0 to Tree1.L
            rkRecord.m_pkTree = pkRoot->GetLChild();
            TestIntersection(fTMax,rkRecord);

            // compare Tree0 to Tree1.R
            rkRecord.m_pkTree = pkRoot->GetRChild();
            TestIntersection(fTMax,rkRecord);

            rkRecord.m_pkTree = pkRoot;
        }
        else
        {
            // at a leaf in each tree
            int iMax0 = pkTree0->GetTriangleQuantity();
            for (int i0 = 0; i0 < iMax0; i0++)
            {
                int iT0 = pkTree0->GetTriangle(i0);

                // get world space triangle
                Triangle3f kTri0;
                pkMesh0->GetWorldTriangle(iT0,kTri0);

                int iMax1 = pkTree1->GetTriangleQuantity();
                for (int i1 = 0; i1 < iMax1; i1++)
                {
                    int iT1 = pkTree1->GetTriangle(i1);

                    // get world space triangle
                    Triangle3f kTri1;
                    pkMesh1->GetWorldTriangle(iT1,kTri1);

                    IntrTriangle3Triangle3<float> kIntersector(kTri0,kTri1);
                    if (kIntersector.Test(fTMax,rkVelocity0,rkVelocity1))
                    {
                        if (m_oCallback)
                        {
                            m_oCallback(*this,iT0,rkRecord,iT1,&kIntersector);
                        }

                        if (rkRecord.m_oCallback)
                        {
                            rkRecord.m_oCallback(rkRecord,iT1,*this,iT0,
                                &kIntersector);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void CollisionRecord::FindIntersection (float fTMax,
    CollisionRecord& rkRecord)
{
    // convenience variables
    BoundingVolumeTree* pkTree0 = m_pkTree;
    BoundingVolumeTree* pkTree1 = rkRecord.m_pkTree;
    const TriMesh* pkMesh0 = m_pkTree->GetMesh();
    const TriMesh* pkMesh1 = rkRecord.m_pkTree->GetMesh();
    const Vector3f& rkVelocity0 = (m_pkVelocity ? *m_pkVelocity :
        Vector3f::ZERO);
    const Vector3f& rkVelocity1 = (rkRecord.m_pkVelocity ?
        *rkRecord.m_pkVelocity : Vector3f::ZERO);

    pkTree0->UpdateWorldBound();
    pkTree1->UpdateWorldBound();

    const BoundingVolume* pkWorldBV0 = pkTree0->GetWorldBound();
    const BoundingVolume* pkWorldBV1 = pkTree1->GetWorldBound();

    // TO DO. This TestIntersection needs to be for moving bounding volumes.
    if (pkWorldBV0->TestIntersection(pkWorldBV1))
    {
        BoundingVolumeTree* pkRoot;

        if (pkTree0->IsInteriorNode())
        {
            pkRoot = m_pkTree;

            // compare Tree0.L to Tree1
            m_pkTree = pkRoot->GetLChild();
            FindIntersection(fTMax,rkRecord);

            // compare Tree0.R to Tree1
            m_pkTree = pkRoot->GetRChild();
            FindIntersection(fTMax,rkRecord);

            m_pkTree = pkRoot;
        }
        else if ( pkTree1->IsInteriorNode() )
        {
            pkRoot = rkRecord.m_pkTree;

            // compare Tree0 to Tree1.L
            rkRecord.m_pkTree = pkRoot->GetLChild();
            FindIntersection(fTMax,rkRecord);

            // compare Tree0 to Tree1.R
            rkRecord.m_pkTree = pkRoot->GetRChild();
            FindIntersection(fTMax,rkRecord);

            rkRecord.m_pkTree = pkRoot;
        }
        else
        {
            // at a leaf in each tree
            int iMax0 = pkTree0->GetTriangleQuantity();
            for (int i0 = 0; i0 < iMax0; i0++)
            {
                int iT0 = pkTree0->GetTriangle(i0);

                // get world space triangle
                Triangle3f kTri0;
                pkMesh0->GetWorldTriangle(iT0,kTri0);

                int iMax1 = pkTree1->GetTriangleQuantity();
                for (int i1 = 0; i1 < iMax1; i1++)
                {
                    int iT1 = pkTree1->GetTriangle(i1);

                    // get world space triangle
                    Triangle3f kTri1;
                    pkMesh1->GetWorldTriangle(iT1,kTri1);

                    IntrTriangle3Triangle3<float> kIntersector(kTri0,kTri1);
                    if (kIntersector.Find(fTMax,rkVelocity0,rkVelocity1))
                    {
                        if (m_oCallback)
                        {
                            m_oCallback(*this,iT0,rkRecord,iT1,&kIntersector);
                        }

                        if (rkRecord.m_oCallback)
                        {
                            rkRecord.m_oCallback(rkRecord,iT1,*this,iT0,
                                &kIntersector);
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
