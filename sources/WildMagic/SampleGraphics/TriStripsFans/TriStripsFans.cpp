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
// Version: 4.0.4 (2007/06/16)

#include "TriStripsFans.h"

WM4_WINDOW_APPLICATION(TriStripsFans);

//----------------------------------------------------------------------------
TriStripsFans::TriStripsFans ()
    :
    WindowApplication3("TriStripsFans",0,0,512,512,ColorRGBA::WHITE)
{
    m_pkActive = 0;
}
//----------------------------------------------------------------------------
bool TriStripsFans::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,1.0f,0.1f,100.0f);
    Vector3f kCLoc = Vector3f(0.0f,0.0f,2.0f);
    Vector3f kCDir = -Vector3f::UNIT_Z;
    Vector3f kCUp = Vector3f::UNIT_Y;
    Vector3f kCRight = Vector3f::UNIT_X;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TriStripsFans::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkStrip = 0;
    m_spkStripMesh = 0;
    m_spkFan = 0;
    m_spkFanMesh = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TriStripsFans::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    if (MoveObject())
    {
        m_spkScene->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        if (m_pkActive == m_spkStrip)
        {
            if (m_spkStrip->Culling == Spatial::CULL_ALWAYS)
            {
                m_pkRenderer->Draw(128,GetHeight()-8,ColorRGBA::BLACK,
                    "strip as mesh");
            }
            else
            {
                m_pkRenderer->Draw(128,GetHeight()-8,ColorRGBA::BLACK,
                    "strip");
            }
        }
        else
        {
            if (m_spkFan->Culling == Spatial::CULL_ALWAYS)
            {
                m_pkRenderer->Draw(128,GetHeight()-8,ColorRGBA::BLACK,
                    "fan as mesh");
            }
            else
            {
                m_pkRenderer->Draw(128,GetHeight()-8,ColorRGBA::BLACK,
                    "fan");
            }
        }
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool TriStripsFans::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 'a':
    case 'A':
        if (m_pkActive == m_spkStrip)
        {
            m_pkActive = m_spkFan;
            m_spkStrip->Culling = Spatial::CULL_ALWAYS;
            m_spkStripMesh->Culling = Spatial::CULL_ALWAYS;
            m_spkFan->Culling = Spatial::CULL_DYNAMIC;
            m_spkFanMesh->Culling = Spatial::CULL_ALWAYS;
            m_kCuller.ComputeVisibleSet(m_spkScene);
        }
        else
        {
            m_pkActive = m_spkStrip;
            m_spkStrip->Culling = Spatial::CULL_DYNAMIC;
            m_spkStripMesh->Culling = Spatial::CULL_ALWAYS;
            m_spkFan->Culling = Spatial::CULL_ALWAYS;
            m_spkFanMesh->Culling = Spatial::CULL_ALWAYS;
            m_kCuller.ComputeVisibleSet(m_spkScene);
        }
        return true;

    case 'c':
    case 'C':
        if (m_pkActive == m_spkStrip)
        {
            if (m_spkStrip->Culling == Spatial::CULL_ALWAYS)
            {
                m_spkStrip->Culling = Spatial::CULL_DYNAMIC;
                m_spkStripMesh->Culling = Spatial::CULL_ALWAYS;
            }
            else
            {
                m_spkStrip->Culling = Spatial::CULL_ALWAYS;
                m_spkStripMesh->Culling = Spatial::CULL_DYNAMIC;
            }
            m_spkFan->Culling = Spatial::CULL_ALWAYS;
            m_spkFanMesh->Culling = Spatial::CULL_ALWAYS;
            m_kCuller.ComputeVisibleSet(m_spkScene);
        }
        else
        {
            if (m_spkFan->Culling == Spatial::CULL_ALWAYS)
            {
                m_spkFan->Culling = Spatial::CULL_DYNAMIC;
                m_spkFanMesh->Culling = Spatial::CULL_ALWAYS;
            }
            else
            {
                m_spkFan->Culling = Spatial::CULL_ALWAYS;
                m_spkFanMesh->Culling = Spatial::CULL_DYNAMIC;
            }
            m_spkStrip->Culling = Spatial::CULL_ALWAYS;
            m_spkStripMesh->Culling = Spatial::CULL_ALWAYS;
            m_kCuller.ComputeVisibleSet(m_spkScene);
        }
        return true;

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"TriStripsFans.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TriStripsFans::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB::WHITE;
    pkLight->Diffuse = ColorRGB::WHITE;
    pkLight->Specular = ColorRGB::BLACK;
    pkLight->DVector = -Vector3f::UNIT_Z;
    m_spkScene->AttachLight(pkLight);

    // The texture and the lighting are multiplied together.
    ShaderEffect* pkEffect = WM4_NEW TextureEffect("Magician");
    AlphaState* pkAState = pkEffect->GetBlending(0);
    pkAState->SrcBlend = AlphaState::SBF_DST_COLOR;
    pkAState->DstBlend = AlphaState::DBF_ZERO;

    // Create the vertices and texture coordinates for the cube tristrip
    // and the trimesh corresponding to it.
    int iSideDivisions = 498;
    int iSideVQuantity = 2*(2+iSideDivisions);
    int iVQuantity = 6*iSideVQuantity;
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    float fScale = 2.0f/3.0f;
    int i, iSide;
    for (iSide = 0; iSide < 6; iSide++)
    {
        // Create initial corner vertices for this side of the cube.
        Vector3f akCornerVertex[4];
        Vector2f akCornerUV[4];
        int iAxis = iSide >> 1;
        float fB2 = ((iSide & 1) ? +1.0f : -1.0f);
        for (i = 0; i < 4; i++)
        {
            float fB0 = ((i & 1) ? +1.0f : -1.0f);
            float fB1 = ((i & 2) ? +1.0f : -1.0f);

            switch (iAxis)
            {
            case 0:
                akCornerVertex[i] = Vector3f(fB2,-fB0,-fB1*fB2)*fScale;
                break;
            case 1:
                akCornerVertex[i] = Vector3f(fB1*fB2,fB2,fB0)*fScale;
                break;
            case 2:
                akCornerVertex[i] = Vector3f(fB1*fB2,-fB0,fB2)*fScale;
                break;
            }
            akCornerUV[i][0] = (float)(1 - ((i >> 1) & 1));
            akCornerUV[i][1] = (float)(1 - (i & 1));
        }

        // Interpolate between the corner vertices.
        int iOffset = iSide*iSideVQuantity;
        for (i = 0; i < iSideVQuantity; i++)
        {
            int iParity = (i & 1);
            float fInterpolation = (float)(i >> 1)/(float)(1+iSideDivisions);
            assert(i+iOffset < iVQuantity);
            pkVBuffer->Position3(i+iOffset) = akCornerVertex[iParity] *
                (1.0f - fInterpolation) + akCornerVertex[2+iParity] *
                fInterpolation;
            pkVBuffer->TCoord2(0,i+iOffset) = akCornerUV[iParity] *
                (1.0f-fInterpolation) + akCornerUV[2+iParity]*fInterpolation;
        }
    }

    // Generate tristrip indices for the cube (T = 6*(iSideVQuantity-2)).
    int iIQuantity = 6*iSideVQuantity + 10;
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* piIndex = pkIBuffer->GetData();
    for (iSide = 0; iSide < 6; iSide++)
    {
        // Tristrip indices for the side.
        for (i = 0; i < iSideVQuantity; i++)
        {
            *piIndex++ = iSide*iSideVQuantity + i;
        }

        // Join the next side by replication using the last vertex index from
        // the current side and the first vertex index from the next side.
        if (iSide < 5)
        {
            *piIndex++ = (iSide + 1)*iSideVQuantity - 1;
            *piIndex++ = (iSide + 1)*iSideVQuantity;
        }
    }

    m_spkStrip = WM4_NEW TriStrip(pkVBuffer,pkIBuffer);
    m_spkStrip->AttachEffect(pkEffect);
    m_spkStrip->Culling = Spatial::CULL_DYNAMIC;
    m_spkScene->AttachChild(m_spkStrip);

    // Construct the TriMesh object corresponding to the TriStrip object.
    // The vertex buffer from the TriStrip object should not be shared,
    // because the vertex normals are computed differently between TriMeshs
    // and TriStrips.
    VertexBuffer* pkMVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    for (i = 0; i < iVQuantity; i++)
    {
        pkMVBuffer->Position3(i) = pkVBuffer->Position3(i);
        pkMVBuffer->TCoord2(0,i) = pkVBuffer->TCoord2(0,i);
    }

    // Convert tristrip indices to trimesh format.  First, count the number
    // of degenerate triangles.  Second, create the trimesh indices.
    int iTQuantity = 0;
    iIQuantity = pkIBuffer->GetIndexQuantity();
    for (i = 0; i < iIQuantity - 2; i++)
    {
        int iV0, iV1, iV2;
        if (m_spkStrip->GetTriangle(i,iV0,iV1,iV2))
        {
            iTQuantity++;
        }
    }
    assert(iTQuantity > 0);
    int iMIQuantity = 3*iTQuantity;
    IndexBuffer* pkMIBuffer = WM4_NEW IndexBuffer(iMIQuantity);
    int* aiMIndex = pkMIBuffer->GetData();
    int* piMIndex = aiMIndex;
    for (i = 0; i < iIQuantity - 2; i++)
    {
        int iV0, iV1, iV2;
        if (m_spkStrip->GetTriangle(i,iV0,iV1,iV2))
        {
            *piMIndex++ = iV0;
            *piMIndex++ = iV1;
            *piMIndex++ = iV2;
        }
    }

    m_spkStripMesh = WM4_NEW TriMesh(pkMVBuffer,pkMIBuffer);
    m_spkStripMesh->AttachEffect(pkEffect);
    m_spkStripMesh->Culling = Spatial::CULL_ALWAYS;
    m_spkScene->AttachChild(m_spkStripMesh);

    // Create the vertices and texture coordinates for the quarter-circle
    // tristrip and the trimesh corresponding to it.
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity+1);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    for (i = 1; i <= iVQuantity; i++)
    {
        float fAngle = Mathf::HALF_PI*(i-1.0f)/(iVQuantity-1.0f);
        float fCos = Mathf::Cos(fAngle);
        float fSin = Mathf::Sin(fAngle);
        pkVBuffer->Position3(i) = Vector3f(2.0f*fCos-1.0f,2.0f*fSin-1.0f,
            0.0f);
        pkVBuffer->TCoord2(0,i) = Vector2f(fCos,fSin);
    }

    m_spkFan = WM4_NEW TriFan(pkVBuffer);
    m_spkFan->AttachEffect(pkEffect);
    m_spkFan->Culling = Spatial::CULL_ALWAYS;
    m_spkScene->AttachChild(m_spkFan);

    // Construct the TriMesh object corresponding to the TriStrip object.
    iTQuantity = iVQuantity - 1;
    pkIBuffer = WM4_NEW IndexBuffer(3*iTQuantity);
    piIndex = pkIBuffer->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        *piIndex++ = 0;
        *piIndex++ = i+1;
        *piIndex++ = i+2;
    }

    m_spkFanMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkFanMesh->AttachEffect(pkEffect);
    m_spkFanMesh->Culling = Spatial::CULL_ALWAYS;
    m_spkScene->AttachChild(m_spkFanMesh);

    m_pkActive = m_spkStrip;
}
//----------------------------------------------------------------------------
