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

#ifndef RIGIDTETRA_H
#define RIGIDTETRA_H

#include "Wm4RigidBody.h"
#include "Wm4TriMesh.h"
using namespace Wm4;

class RigidTetra : public RigidBodyf
{
public:
    RigidTetra (float fSize, float fMass, const Vector3f& rkPos,
        const Vector3f& rkLinMom, const Vector3f& rkAngMom);

    TriMeshPtr Mesh () const;
    float GetRadius () const;
    void GetVertices (Vector3f* akVertex) const;

    bool Moved;

private:
    TriMeshPtr m_spkMesh;
    float m_fRadius;
};

#endif
