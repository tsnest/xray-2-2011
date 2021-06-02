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

#include "RigidTetra.h"
#include "Wm4VertexColor3Effect.h"

//----------------------------------------------------------------------------
RigidTetra::RigidTetra (float fSize, float fMass, const Vector3f& rkPos,
    const Vector3f& rkLinMom, const Vector3f& rkAngMom)
{
    Moved = false;

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = -(fSize/3.0f)*Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(+fSize,0.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(0.0f,+fSize,0.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,0.0f,+fSize);
    pkVBuffer->Color3(0,0) = ColorRGB(1.0f,1.0f,1.0f);
    pkVBuffer->Color3(0,1) = ColorRGB(1.0f,0.0f,0.0f);
    pkVBuffer->Color3(0,2) = ColorRGB(0.0f,1.0f,0.0f);
    pkVBuffer->Color3(0,3) = ColorRGB(0.0f,0.0f,1.0f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(12);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] = 0; aiIndex[ 1] = 2; aiIndex[ 2] = 1;
    aiIndex[ 3] = 0; aiIndex[ 4] = 3; aiIndex[ 5] = 2;
    aiIndex[ 6] = 0; aiIndex[ 7] = 1; aiIndex[ 8] = 3;
    aiIndex[ 9] = 1; aiIndex[10] = 2; aiIndex[11] = 3;

    m_spkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

    // inertia tensor
    Matrix3f kInertia;
    int i, j;
    for (i = 0; i < 3; i++)
    {
        kInertia[i][i] = 0.0f;
        for (j = 0; j < 3; j++)
        {
            if (i != j)
            {
                kInertia[i][j] = 0.0f;
                for (int k = 0; k < 4; k++)
                {
                    kInertia[i][i] +=
                        0.25f*fMass*m_spkMesh->VBuffer->Position3(k)[j]*
                        m_spkMesh->VBuffer->Position3(k)[j];
                    kInertia[i][j] -=
                        0.25f*fMass*m_spkMesh->VBuffer->Position3(k)[i]*
                        m_spkMesh->VBuffer->Position3(k)[j];
                }
            }
        }
    }

    // compute the radius of a sphere bounding the tetrahedron
    Vector3f kCentroid = (fSize/6.0f)*Vector3f(1.0f,1.0f,1.0f);
    m_fRadius = 0.0f;
    for (j = 0; j < 4; j++)
    {
        m_spkMesh->VBuffer->Position3(j) -= kCentroid;
        float fTemp = (m_spkMesh->VBuffer->Position3(j)-kCentroid).Length();
        if (fTemp > m_fRadius)
        {
            m_fRadius = fTemp;
        }
    }

    SetMass(fMass);
    SetBodyInertia(kInertia);
    SetPosition(rkPos);
    SetQOrientation(Quaternionf::IDENTITY);
    SetLinearMomentum(rkLinMom);
    SetAngularMomentum(rkAngMom);
}
//----------------------------------------------------------------------------
TriMeshPtr RigidTetra::Mesh () const
{
    return m_spkMesh;
}
//----------------------------------------------------------------------------
float RigidTetra::GetRadius () const
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
void RigidTetra::GetVertices (Vector3f* akVertex) const
{
    // Do not move the boundaries.  The hard-coded numbers here depend on
    // those of the floor/walls in the application.
    Vector3f kPos;
    if (0.0f < m_kPos.X() && m_kPos.X() < 20.0f)
    {
        kPos = m_kPos;
    }
    else
    {
        kPos = Vector3f::ZERO;
    }

    // move the tetra vertices
    for (int i = 0; i < 4; i++)
    {
        akVertex[i] = m_kROrient*m_spkMesh->VBuffer->Position3(i) + kPos;
    }
}
//----------------------------------------------------------------------------
