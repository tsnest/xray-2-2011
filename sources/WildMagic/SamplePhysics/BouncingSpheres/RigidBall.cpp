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
// Version: 4.0.1 (2007/06/16)

#include "RigidBall.h"
#include "Wm4StandardMesh.h"
#include "Wm4TextureEffect.h"

//----------------------------------------------------------------------------
RigidBall::RigidBall (float fRadius)
{
    m_fRadius = fRadius;
    Moved = false;

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    m_spkMesh = StandardMesh(kAttr).Sphere(16,16,m_fRadius);

    m_spkMesh->AttachEffect(WM4_NEW TextureEffect("BallTexture"));
}
//----------------------------------------------------------------------------
float RigidBall::GetRadius () const
{ 
    return m_fRadius; 
}
//----------------------------------------------------------------------------
TriMeshPtr& RigidBall::Mesh ()
{
    return m_spkMesh;
}
//----------------------------------------------------------------------------
