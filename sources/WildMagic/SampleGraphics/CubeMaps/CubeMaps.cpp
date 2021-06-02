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

#include "CubeMaps.h"

WM4_WINDOW_APPLICATION(CubeMaps);

//----------------------------------------------------------------------------
CubeMaps::CubeMaps ()
    :
    WindowApplication3("CubeMaps",0,0,512,512,ColorRGBA::WHITE)
{
    System::InsertDirectory("Data");

    m_afAmplitude = 0;
    m_afFrequency = 0;
}
//----------------------------------------------------------------------------
bool CubeMaps::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,m_iWidth/(float)m_iHeight,0.01f,10.0f);
    Vector3f kCLoc = Vector3f(0.0f,0.0f,0.85f);
    Vector3f kCDir = -Vector3f::UNIT_Z;
    Vector3f kCUp = Vector3f::UNIT_Y;
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkSphere);
    return true;
}
//----------------------------------------------------------------------------
void CubeMaps::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkCull = 0;
    m_spkSphere = 0;
    m_spkCubeMap = 0;

    WM4_DELETE[] m_afAmplitude;
    WM4_DELETE[] m_afFrequency;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void CubeMaps::OnIdle ()
{
    MeasureTime();

    MorphSphere();

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);

        if (m_spkCubeMap->CanDynamicallyUpdate())
        {
            // Cull the sphere object because it is the object which reflects
            // the environment.
            m_spkSphere->Culling = Spatial::CULL_ALWAYS;

            // You can take a snapshot of the environment from any camera
            // position and camera orientation.  In this sample application,
            // the environment is always rendered from the center of the cube
            // object and using the axes of that cube for the orientation.
            m_spkCubeMap->UpdateFaces(m_pkRenderer,m_spkScene,
                Vector3f::ZERO,Vector3f::UNIT_Z,Vector3f::UNIT_Y,
                -Vector3f::UNIT_X);

            // Restore the sphere object's culling state.
            m_spkSphere->Culling = Spatial::CULL_DYNAMIC;
        }
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool CubeMaps::OnKeyDown (unsigned char ucKey, int iX, int iY)
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

    case 'c':
    case 'C':
        m_spkCull->Enabled = !m_spkCull->Enabled;
        return true;

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"CubeMaps.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void CubeMaps::CreateScene ()
{
    // create scene
    m_spkScene = WM4_NEW Node;
    Node* pkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(pkTrnNode);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Create the walls of the cube room.  Each of the six texture images is
    // RGB 64-by-64.
    Node* pkRoom = WM4_NEW Node;
    pkTrnNode->AttachChild(pkRoom);

    // Index buffer shared by the room walls.
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 3;
    aiIndex[3] = 0;  aiIndex[4] = 3;  aiIndex[5] = 2;

    Attributes kWallAttr;
    kWallAttr.SetPositionChannels(3);
    kWallAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer;
    TriMesh* pkWall;

    // +x wall
    pkVBuffer = WM4_NEW VertexBuffer(kWallAttr,4);
    pkVBuffer->Position3(0) = Vector3f(+1.0f,-1.0f,-1.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,+1.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,-1.0f);
    pkVBuffer->Position3(3) = Vector3f(+1.0f,+1.0f,+1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(1.0f,1.0f);
    pkWall = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkWall->AttachEffect(WM4_NEW TextureEffect("fxp"));
    pkRoom->AttachChild(pkWall);

    // -x wall
    pkVBuffer = WM4_NEW VertexBuffer(kWallAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,+1.0f);
    pkVBuffer->Position3(1) = Vector3f(-1.0f,-1.0f,-1.0f);
    pkVBuffer->Position3(2) = Vector3f(-1.0f,+1.0f,+1.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,-1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(1.0f,1.0f);
    pkWall = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkWall->AttachEffect(WM4_NEW TextureEffect("fxm"));
    pkRoom->AttachChild(pkWall);

    // +y wall
    pkVBuffer = WM4_NEW VertexBuffer(kWallAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,+1.0f,-1.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,+1.0f,-1.0f);
    pkVBuffer->Position3(2) = Vector3f(-1.0f,+1.0f,+1.0f);
    pkVBuffer->Position3(3) = Vector3f(+1.0f,+1.0f,+1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(1.0f,1.0f);
    pkWall = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkWall->AttachEffect(WM4_NEW TextureEffect("fyp"));
    pkRoom->AttachChild(pkWall);

    // -y wall
    pkVBuffer = WM4_NEW VertexBuffer(kWallAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,+1.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,+1.0f);
    pkVBuffer->Position3(2) = Vector3f(-1.0f,-1.0f,-1.0f);
    pkVBuffer->Position3(3) = Vector3f(+1.0f,-1.0f,-1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(1.0f,1.0f);
    pkWall = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkWall->AttachEffect(WM4_NEW TextureEffect("fym"));
    pkRoom->AttachChild(pkWall);

    // +z wall
    pkVBuffer = WM4_NEW VertexBuffer(kWallAttr,4);
    pkVBuffer->Position3(0) = Vector3f(+1.0f,-1.0f,+1.0f);
    pkVBuffer->Position3(1) = Vector3f(-1.0f,-1.0f,+1.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,+1.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,+1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(1.0f,1.0f);
    pkWall = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkWall->AttachEffect(WM4_NEW TextureEffect("fzp"));
    pkRoom->AttachChild(pkWall);

    // -z wall
    pkVBuffer = WM4_NEW VertexBuffer(kWallAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,-1.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,-1.0f);
    pkVBuffer->Position3(2) = Vector3f(-1.0f,+1.0f,-1.0f);
    pkVBuffer->Position3(3) = Vector3f(+1.0f,+1.0f,-1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(1.0f,1.0f);
    pkWall = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkWall->AttachEffect(WM4_NEW TextureEffect("fzm"));
    pkRoom->AttachChild(pkWall);

    // A sphere to reflect the environment via a cube map.  The colors will
    // be used to modulate the cube map texture.
    Attributes kSphereAttr;
    kSphereAttr.SetPositionChannels(3);
    kSphereAttr.SetNormalChannels(3);
    kSphereAttr.SetColorChannels(0,3);
    m_fRadius = 0.125f;
    m_spkSphere = StandardMesh(kSphereAttr).Sphere(64,64,m_fRadius);
    pkRoom->AttachChild(m_spkSphere);

    // Generate random vertex colors for the sphere.  The StandardMesh class
    // produces a sphere with duplicated vertices along a longitude line.
    // This allows texture coordinates to be assigned in a manner that treats
    // the sphere as if it were a rectangle mesh.  For vertex colors, we want
    // the duplicated vertices to have the same color, so a hash table is used
    // to look up vertex colors for the duplicates.
    //
    // Also generate amplitudes and frequencies for morphing the sphere.  The
    // hash table must be used again to guarantee duplicated vertices have
    // the same data.
    pkVBuffer = m_spkSphere->VBuffer;
    int iVQuantity = pkVBuffer->GetVertexQuantity();
    m_afAmplitude = WM4_NEW float[iVQuantity];
    m_afFrequency = WM4_NEW float[iVQuantity];
    std::map<Vector3f,Vector4f> kMap;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        Vector3f& rkVertex = pkVBuffer->Position3(i);
        ColorRGB& rkColor = pkVBuffer->Color3(0,i);
        std::map<Vector3f,Vector4f>::iterator pkIter = kMap.find(rkVertex);
        if (pkIter != kMap.end())
        {
            rkColor.R() = 0.0f;
            rkColor.G() = pkIter->second[0];
            rkColor.B() = pkIter->second[1];
            m_afAmplitude[i] = pkIter->second[2];
            m_afFrequency[i] = pkIter->second[3];
        }
        else
        {
            rkColor.R() = 0.0f;
            rkColor.G() = Mathf::IntervalRandom(0.5f,0.75f);
            rkColor.B() = Mathf::IntervalRandom(0.75f,1.0f);
            m_afAmplitude[i] = Mathf::IntervalRandom(0.001f,0.002f);
            m_afFrequency[i] = Mathf::IntervalRandom(1.0f,5.0f);

            Vector4f kData;
            kData[0] = rkColor.G();
            kData[1] = rkColor.B();
            kData[2] = m_afAmplitude[i];
            kData[3] = m_afFrequency[i];
            kMap.insert(std::make_pair(rkVertex,kData));
        }
    }

    // Create the cube map and attach it to the sphere.
    ImagePtr spkXpImage = Image::Load("fxp");
    ImagePtr spkXmImage = Image::Load("fxm");
    ImagePtr spkYpImage = Image::Load("fyp");
    ImagePtr spkYmImage = Image::Load("fym");
    ImagePtr spkZpImage = Image::Load("fzp");
    ImagePtr spkZmImage = Image::Load("fzm");
    float fReflectivity = 0.5f;
    m_spkCubeMap = WM4_NEW CubeMapEffect(spkXpImage,spkXmImage,spkYpImage,
        spkYmImage,spkZpImage,spkZmImage,fReflectivity,true);

    m_spkSphere->AttachEffect(m_spkCubeMap);

    // Allow culling to be disabled on the sphere so when you move inside
    // the sphere, you can see the previously hidden facets and verify that
    // the cube image for those facets is correctly oriented.
    m_spkCull = WM4_NEW CullState;
    m_spkSphere->AttachGlobalState(m_spkCull);
}
//----------------------------------------------------------------------------
void CubeMaps::MorphSphere ()
{
    return;
    float fTime = (float)System::GetTime();

    VertexBuffer* pkVBuffer = m_spkSphere->VBuffer;
    int iVQuantity = pkVBuffer->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        float fDelta = m_fRadius + m_afAmplitude[i] *
            Mathf::Sin(m_afFrequency[i]*fTime);
        pkVBuffer->Position3(i).Normalize();
        pkVBuffer->Position3(i) *= fDelta;
    }
}
//----------------------------------------------------------------------------
