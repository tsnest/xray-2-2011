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

#ifndef WM4COLLISIONRECORD_H
#define WM4COLLISIONRECORD_H

#include "Wm4GraphicsLIB.h"
#include "Wm4BoundingVolumeTree.h"
#include "Wm4Intersector.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM CollisionRecord
{
public:
    typedef void (*Callback) (CollisionRecord& rkRecord0, int iT0,
        CollisionRecord& rkRecord1, int iT1,
        Intersector<float,Vector3f>* pkIntersector);

    // Construction and destruction.  CollisionRecord assumes responsibility
    // for deleting pkTree, so it should be dynamically allocated.
    CollisionRecord (TriMesh* pkMesh, BoundingVolumeTree* pkTree,
        Vector3f* pkVelocity, Callback oCallback, void* pvCallbackData);

    ~CollisionRecord ();

    // member access
    TriMesh* GetMesh ();
    Vector3f* GetVelocity ();
    void* GetCallbackData ();

    // intersection queries
    void TestIntersection (CollisionRecord& rkRecord);
    void FindIntersection (CollisionRecord& rkRecord);
    void TestIntersection (float fTMax, CollisionRecord& rkRecord);
    void FindIntersection (float fTMax ,CollisionRecord& rkRecord);

protected:
    TriMesh* m_pkMesh;
    BoundingVolumeTree* m_pkTree;
    Vector3f* m_pkVelocity;
    Callback m_oCallback;
    void* m_pvCallbackData;
};

#include "Wm4CollisionRecord.inl"

}

#endif
