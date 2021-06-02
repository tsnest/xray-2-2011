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
// Version: 4.0.1 (2006/09/25)

#include "MeshEnvelope.h"
#include "CreateEnvelope.h"

WM4_WINDOW_APPLICATION(MeshEnvelope);

//----------------------------------------------------------------------------
MeshEnvelope::MeshEnvelope ()
    :
    WindowApplication2("MeshEnvelope",0,0,X_SIZE,Y_SIZE,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_akVertex = 0;
    m_aiIndex = 0;
    m_akEVertex = 0;
}
//----------------------------------------------------------------------------
bool MeshEnvelope::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Load the 3D face mesh.
    const char* acPath = System::GetPath("FacePN.wmof",System::SM_READ);
    Stream kIStream;
    kIStream.Load(acPath);
    TriMeshPtr spkMesh = StaticCast<TriMesh>(kIStream.GetObjectAt(0));
    VertexBuffer* pkVBuffer = spkMesh->VBuffer;
    m_iVQuantity = pkVBuffer->GetVertexQuantity();

#if 1
    // Rotate the mesh just to see how the algorithm behaves.
    Matrix3f kRot(Vector3f::UNIT_X,Mathf::HALF_PI);
    for (int j = 0; j < m_iVQuantity; j++)
    {
        pkVBuffer->Position3(j) = kRot*pkVBuffer->Position3(j);
    }
#endif

    // Project the mesh onto the xy-plane.
    m_akVertex = WM4_NEW Vector2f[m_iVQuantity];
    float fX = pkVBuffer->Position3(0).X();
    float fY = pkVBuffer->Position3(0).Y();
    m_akVertex[0].X() = fX;
    m_akVertex[0].Y() = fY;
    float fXMin = fX, fXMax = fX;
    float fYMin = fY, fYMax = fY;
    int i;
    for (i = 1; i < m_iVQuantity; i++)
    {
        fX = pkVBuffer->Position3(i).X();
        fY = pkVBuffer->Position3(i).Y();
        m_akVertex[i][0] = fX;
        m_akVertex[i][1] = fY;

        if (fX < fXMin)
        {
            fXMin = fX;
        }
        else if (fX > fXMax)
        {
            fXMax = fX;
        }

        if (fY < fYMin)
        {
            fYMin = fY;
        }
        else if (fY > fYMax)
        {
            fYMax = fY;
        }
    }

    // Transform the mesh into window coordinates.
    float fXOffset = 0.125f*X_SIZE;
    float fYOffset = 0.125f*Y_SIZE;
    float fXMult = (0.75f*X_SIZE)/(fXMax - fXMin);
    float fYMult = (0.75f*Y_SIZE)/(fYMax - fYMin);
    for (i = 0; i < m_iVQuantity; i++)
    {
        m_akVertex[i].X() = fXOffset + fXMult*(m_akVertex[i].X() - fXMin);
        m_akVertex[i].Y() = fYOffset + fYMult*(m_akVertex[i].Y() - fYMin);
    }

    // Copy the mesh connectivity information.
    IndexBuffer* pkIBuffer = spkMesh->IBuffer;
    m_iIQuantity = pkIBuffer->GetIndexQuantity();
    m_aiIndex = WM4_NEW int[m_iIQuantity];
    size_t uiSize = m_iIQuantity*sizeof(int);
    System::Memcpy(m_aiIndex,uiSize,pkIBuffer->GetData(),uiSize);

    // Create the envelope of the mesh.
    CreateEnvelope(m_iVQuantity,m_akVertex,m_iIQuantity,m_aiIndex,
        m_iEQuantity,m_akEVertex);

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void MeshEnvelope::OnTerminate ()
{
    WM4_DELETE[] m_akVertex;
    WM4_DELETE[] m_aiIndex;
    WM4_DELETE[] m_akEVertex;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void MeshEnvelope::OnDisplay ()
{
    ClearScreen();

    // Draw the mesh triangles.
    int i, iV0, iV1, iV2, iX0, iY0, iX1, iY1, iX2, iY2;
    for (i = 0; i < m_iIQuantity; /**/)
    {
        iV0 = m_aiIndex[i++];
        iV1 = m_aiIndex[i++];
        iV2 = m_aiIndex[i++];
        iX0 = (int)m_akVertex[iV0].X();
        iY0 = (int)m_akVertex[iV0].Y();
        iX1 = (int)m_akVertex[iV1].X();
        iY1 = (int)m_akVertex[iV1].Y();
        iX2 = (int)m_akVertex[iV2].X();
        iY2 = (int)m_akVertex[iV2].Y();
        DrawLine(iX0,iY0,iX1,iY1,Color(0,0,0));
        DrawLine(iX1,iY1,iX2,iY2,Color(0,0,0));
        DrawLine(iX2,iY2,iX0,iY0,Color(0,0,0));
    }

    // Draw the mesh envelope.
    iX0 = (int)m_akEVertex[0].X();
    iY0 = (int)m_akEVertex[0].Y();
    iX1 = iX0;
    iY1 = iY0;
    for (i = 1; i < m_iEQuantity; i++)
    {
        iX2 = (int)m_akEVertex[i].X();
        iY2 = (int)m_akEVertex[i].Y();
        DrawLine(iX1,iY1,iX2,iY2,Color(255,0,0));
        iX1 = iX2;
        iY1 = iY2;
    }
    DrawLine(iX1,iY1,iX0,iY0,Color(255,0,0));

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
