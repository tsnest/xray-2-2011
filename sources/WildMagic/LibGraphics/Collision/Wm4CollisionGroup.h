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

#ifndef WM4COLLISIONGROUP_H
#define WM4COLLISIONGROUP_H

#include "Wm4GraphicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

class CollisionRecord;

class WM4_GRAPHICS_ITEM CollisionGroup
{
public:
    CollisionGroup ();
    ~CollisionGroup ();

    // CollisionGroup assumes responsibility for deleting the collision
    // records, so the input records should be dynamically allocated.
    bool Add (CollisionRecord* pkRecord);
    bool Remove (CollisionRecord* pkRecord);

    // Intersection queries.  If two objects in the group collide, the
    // corresponding records process the information accordingly.

    // The objects are assumed to be stationary (velocities are ignored) and
    // all pairs of objects are compared.
    void TestIntersection ();
    void FindIntersection ();
    
    // The objects are assumed to be moving.  Objects are compared when at
    // least one of them has a velocity vector associated with it (that
    // vector is allowed to be the zero vector).
    void TestIntersection (float fTMax);
    void FindIntersection (float fTMax);

protected:
    std::vector<CollisionRecord*> m_kRecord;
};

}

#endif
