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

#include "Portals.h"

WM4_WINDOW_APPLICATION(Portals);

//----------------------------------------------------------------------------
Portals::Portals ()
    :
    WindowApplication3("Portals",0,0,640,480,ColorRGBA(0.9f,0.9f,0.9f,1.0f))
{
    System::InsertDirectory("Data");
}
//----------------------------------------------------------------------------
bool Portals::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,1.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);

    kCDir = Vector3f(0.25708461f,0.96640458f,0.0f);
    kCRight = kCDir.Cross(kCUp);

    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // set up scene
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    ConvexRegionManager* pkCRM = CreateBspTree();
    pkCRM->AttachOutside(CreateOutside());
    m_spkScene->AttachChild(pkCRM);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

#if 1
    InitializeCameraMotion(0.001f,0.001f);
#else
    // Enable this to see the problem with eyepoint outside the environment,
    // but the near plane inside.  The initial configuration has the eyepoint
    // outside, but the near plane partially inside the environment.  Press
    // the UP-ARROW key a couple of times and see the inside pop into view.
    kCLoc = Vector3f(-0.74392152f,-5.0354514f,1.0f);
    kCDir = Vector3f(-0.65598810f,0.75463903f,0.0f);
    kCUp = Vector3f(0.0f,0.0f,1.0f);
    kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);
    m_fTrnSpeed = 0.0001f;
    InitializeCameraMotion(0.0001f,0.001f);
#endif

    return true;
}
//----------------------------------------------------------------------------
void Portals::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Portals::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Portals::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"Portals.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Portals::CreateCenterCube (const std::string& rkFloor,
    const std::string& rkCeiling, const std::string& rkWall,
    const std::string& rkPicture0, const std::string& rkPicture1,
    const std::string& rkPicture2, const std::string& rkPicture3,
    Node*& rpkCube, Portal**& rapkPortal)
{
    rpkCube = WM4_NEW Node;

    // walls
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,24);
    pkVBuffer->Position3( 0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3( 1) = Vector3f(+1.0f,-1.0f,0.0f);
    pkVBuffer->Position3( 2) = Vector3f(+1.0f,+1.0f,0.0f);
    pkVBuffer->Position3( 3) = Vector3f(-1.0f,+1.0f,0.0f);
    pkVBuffer->Position3( 4) = Vector3f(-1.0f,-1.0f,2.0f);
    pkVBuffer->Position3( 5) = Vector3f(+1.0f,-1.0f,2.0f);
    pkVBuffer->Position3( 6) = Vector3f(+1.0f,+1.0f,2.0f);
    pkVBuffer->Position3( 7) = Vector3f(-1.0f,+1.0f,2.0f);
    pkVBuffer->Position3( 8) = Vector3f(+0.5f,-1.0f,0.0f);
    pkVBuffer->Position3( 9) = Vector3f(+0.5f,-1.0f,1.5f);
    pkVBuffer->Position3(10) = Vector3f(-0.5f,-1.0f,1.5f);
    pkVBuffer->Position3(11) = Vector3f(-0.5f,-1.0f,0.0f);
    pkVBuffer->Position3(12) = Vector3f(-0.5f,+1.0f,0.0f);
    pkVBuffer->Position3(13) = Vector3f(-0.5f,+1.0f,1.5f);
    pkVBuffer->Position3(14) = Vector3f(+0.5f,+1.0f,1.5f);
    pkVBuffer->Position3(15) = Vector3f(+0.5f,+1.0f,0.0f);
    pkVBuffer->Position3(16) = Vector3f(+1.0f,+0.5f,0.0f);
    pkVBuffer->Position3(17) = Vector3f(+1.0f,+0.5f,1.5f);
    pkVBuffer->Position3(18) = Vector3f(+1.0f,-0.5f,1.5f);
    pkVBuffer->Position3(19) = Vector3f(+1.0f,-0.5f,0.0f);
    pkVBuffer->Position3(20) = Vector3f(-1.0f,-0.5f,0.0f);
    pkVBuffer->Position3(21) = Vector3f(-1.0f,-0.5f,1.5f);
    pkVBuffer->Position3(22) = Vector3f(-1.0f,+0.5f,1.5f);
    pkVBuffer->Position3(23) = Vector3f(-1.0f,+0.5f,0.0f);

    pkVBuffer->TCoord2(0, 0) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 1) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 2) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 3) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 4) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 5) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 6) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 7) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 8) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0, 9) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,10) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,11) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,12) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0,13) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,14) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,15) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,16) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,17) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,18) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,19) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0,20) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,21) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,22) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,23) = Vector2f(0.75f,0.00f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(72);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] =  1;  aiIndex[ 1] =  8;  aiIndex[ 2] =  9;
    aiIndex[ 3] =  1;  aiIndex[ 4] =  9;  aiIndex[ 5] =  5;
    aiIndex[ 6] =  5;  aiIndex[ 7] =  9;  aiIndex[ 8] =  4;
    aiIndex[ 9] =  9;  aiIndex[10] = 10;  aiIndex[11] =  4;
    aiIndex[12] = 10;  aiIndex[13] =  0;  aiIndex[14] =  4;
    aiIndex[15] = 11;  aiIndex[16] =  0;  aiIndex[17] = 10;
    aiIndex[18] =  3;  aiIndex[19] = 12;  aiIndex[20] = 13;
    aiIndex[21] =  3;  aiIndex[22] = 13;  aiIndex[23] =  7;
    aiIndex[24] =  7;  aiIndex[25] = 13;  aiIndex[26] =  6;
    aiIndex[27] = 13;  aiIndex[28] = 14;  aiIndex[29] =  6;
    aiIndex[30] = 14;  aiIndex[31] =  2;  aiIndex[32] =  6;
    aiIndex[33] = 15;  aiIndex[34] =  2;  aiIndex[35] = 14;
    aiIndex[36] =  2;  aiIndex[37] = 16;  aiIndex[38] = 17;
    aiIndex[39] =  2;  aiIndex[40] = 17;  aiIndex[41] =  6;
    aiIndex[42] =  6;  aiIndex[43] = 17;  aiIndex[44] =  5;
    aiIndex[45] = 17;  aiIndex[46] = 18;  aiIndex[47] =  5;
    aiIndex[48] = 18;  aiIndex[49] =  1;  aiIndex[50] =  5;
    aiIndex[51] = 19;  aiIndex[52] =  1;  aiIndex[53] = 18;
    aiIndex[54] =  0;  aiIndex[55] = 20;  aiIndex[56] = 21;
    aiIndex[57] =  0;  aiIndex[58] = 21;  aiIndex[59] =  4;
    aiIndex[60] =  4;  aiIndex[61] = 21;  aiIndex[62] =  7;
    aiIndex[63] = 21;  aiIndex[64] = 22;  aiIndex[65] =  7;
    aiIndex[66] = 22;  aiIndex[67] =  3;  aiIndex[68] =  7;
    aiIndex[69] = 23;  aiIndex[70] =  3;  aiIndex[71] = 22;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkWall));
    rpkCube->AttachChild(pkMesh);

    // portals
    rapkPortal = WM4_NEW Portal*[4];

    Vector3f* akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(11);
    akModelVertex[1] = pkVBuffer->Position3(10);
    akModelVertex[2] = pkVBuffer->Position3( 9);
    akModelVertex[3] = pkVBuffer->Position3( 8);
    Plane3f kModelPlane(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[0] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(15);
    akModelVertex[1] = pkVBuffer->Position3(14);
    akModelVertex[2] = pkVBuffer->Position3(13);
    akModelVertex[3] = pkVBuffer->Position3(12);
    kModelPlane = Plane3f(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[1] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(19);
    akModelVertex[1] = pkVBuffer->Position3(18);
    akModelVertex[2] = pkVBuffer->Position3(17);
    akModelVertex[3] = pkVBuffer->Position3(16);
    kModelPlane = Plane3f(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[2] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(23);
    akModelVertex[1] = pkVBuffer->Position3(22);
    akModelVertex[2] = pkVBuffer->Position3(21);
    akModelVertex[3] = pkVBuffer->Position3(20);
    kModelPlane = Plane3f(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[3] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    // floor
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,0.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkFloor));
    rpkCube->AttachChild(pkMesh);

    // ceiling
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,2.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,2.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 2;  aiIndex[2] = 1;
    aiIndex[3] = 0;  aiIndex[4] = 3;  aiIndex[5] = 2;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkCeiling));
    rpkCube->AttachChild(pkMesh);

    // object 0
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-0.875f,-0.625f,0.75f);
    pkVBuffer->Position3(1) = Vector3f(-0.875f,-0.625f,1.10f);
    pkVBuffer->Position3(2) = Vector3f(-0.625f,-0.875f,1.10f);
    pkVBuffer->Position3(3) = Vector3f(-0.625f,-0.875f,0.75f);

    pkVBuffer->TCoord2(0,0) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,0.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkPicture0));
    rpkCube->AttachChild(pkMesh);

    // object 1
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(+0.875f,+0.625f,0.75f);
    pkVBuffer->Position3(1) = Vector3f(+0.875f,+0.625f,1.10f);
    pkVBuffer->Position3(2) = Vector3f(+0.625f,+0.875f,1.10f);
    pkVBuffer->Position3(3) = Vector3f(+0.625f,+0.875f,0.75f);

    pkVBuffer->TCoord2(0,0) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,0.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkPicture1));
    rpkCube->AttachChild(pkMesh);

    // object 2
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-0.625f,+0.875f,0.75f);
    pkVBuffer->Position3(1) = Vector3f(-0.625f,+0.875f,1.10f);
    pkVBuffer->Position3(2) = Vector3f(-0.875f,+0.625f,1.10f);
    pkVBuffer->Position3(3) = Vector3f(-0.875f,+0.625f,0.75f);

    pkVBuffer->TCoord2(0,0) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,0.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkPicture2));
    rpkCube->AttachChild(pkMesh);

    // object 3
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(+0.625f,-0.875f,0.75f);
    pkVBuffer->Position3(1) = Vector3f(+0.625f,-0.875f,1.10f);
    pkVBuffer->Position3(2) = Vector3f(+0.875f,-0.625f,1.10f);
    pkVBuffer->Position3(3) = Vector3f(+0.875f,-0.625f,0.75f);

    pkVBuffer->TCoord2(0,0) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,0.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkPicture3));
    rpkCube->AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
void Portals::CreateAxisConnector (const std::string& rkFloor,
    const std::string& rkCeiling, const std::string& rkWall, Node*& rpkCube,
    Portal**& rapkPortal)
{
    rpkCube = WM4_NEW Node;

    // walls
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,16);
    pkVBuffer->Position3( 0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3( 1) = Vector3f(+1.0f,-1.0f,0.0f);
    pkVBuffer->Position3( 2) = Vector3f(+1.0f,+1.0f,0.0f);
    pkVBuffer->Position3( 3) = Vector3f(-1.0f,+1.0f,0.0f);
    pkVBuffer->Position3( 4) = Vector3f(-1.0f,-1.0f,2.0f);
    pkVBuffer->Position3( 5) = Vector3f(+1.0f,-1.0f,2.0f);
    pkVBuffer->Position3( 6) = Vector3f(+1.0f,+1.0f,2.0f);
    pkVBuffer->Position3( 7) = Vector3f(-1.0f,+1.0f,2.0f);
    pkVBuffer->Position3( 8) = Vector3f(+0.5f,-1.0f,0.0f);
    pkVBuffer->Position3( 9) = Vector3f(+0.5f,-1.0f,1.5f);
    pkVBuffer->Position3(10) = Vector3f(-0.5f,-1.0f,1.5f);
    pkVBuffer->Position3(11) = Vector3f(-0.5f,-1.0f,0.0f);
    pkVBuffer->Position3(12) = Vector3f(-0.5f,+1.0f,0.0f);
    pkVBuffer->Position3(13) = Vector3f(-0.5f,+1.0f,1.5f);
    pkVBuffer->Position3(14) = Vector3f(+0.5f,+1.0f,1.5f);
    pkVBuffer->Position3(15) = Vector3f(+0.5f,+1.0f,0.0f);

    pkVBuffer->TCoord2(0, 0) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 1) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 2) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 3) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 4) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 5) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 6) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 7) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 8) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0, 9) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,10) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,11) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,12) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0,13) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,14) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,15) = Vector2f(0.25f,0.00f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(48);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] =  1;  aiIndex[ 1] =  8;  aiIndex[ 2] =  9;
    aiIndex[ 3] =  1;  aiIndex[ 4] =  9;  aiIndex[ 5] =  5;
    aiIndex[ 6] =  5;  aiIndex[ 7] =  9;  aiIndex[ 8] =  4;
    aiIndex[ 9] =  9;  aiIndex[10] = 10;  aiIndex[11] =  4;
    aiIndex[12] = 10;  aiIndex[13] =  0;  aiIndex[14] =  4;
    aiIndex[15] = 11;  aiIndex[16] =  0;  aiIndex[17] = 10;
    aiIndex[18] =  3;  aiIndex[19] = 12;  aiIndex[20] = 13;
    aiIndex[21] =  3;  aiIndex[22] = 13;  aiIndex[23] =  7;
    aiIndex[24] =  7;  aiIndex[25] = 13;  aiIndex[26] =  6;
    aiIndex[27] = 13;  aiIndex[28] = 14;  aiIndex[29] =  6;
    aiIndex[30] = 14;  aiIndex[31] =  2;  aiIndex[32] =  6;
    aiIndex[33] = 15;  aiIndex[34] =  2;  aiIndex[35] = 14;
    aiIndex[36] =  0;  aiIndex[37] =  7;  aiIndex[38] =  4;
    aiIndex[39] =  0;  aiIndex[40] =  3;  aiIndex[41] =  7;
    aiIndex[42] =  2;  aiIndex[43] =  5;  aiIndex[44] =  6;
    aiIndex[45] =  2;  aiIndex[46] =  1;  aiIndex[47] =  5;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkWall));
    rpkCube->AttachChild(pkMesh);

    // portals
    rapkPortal = WM4_NEW Portal*[2];

    Vector3f* akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(11);
    akModelVertex[1] = pkVBuffer->Position3(10);
    akModelVertex[2] = pkVBuffer->Position3( 9);
    akModelVertex[3] = pkVBuffer->Position3( 8);
    Plane3f kModelPlane(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[0] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(15);
    akModelVertex[1] = pkVBuffer->Position3(14);
    akModelVertex[2] = pkVBuffer->Position3(13);
    akModelVertex[3] = pkVBuffer->Position3(12);
    kModelPlane = Plane3f(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[1] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    // floor
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,0.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkFloor));
    rpkCube->AttachChild(pkMesh);

    // ceiling
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,2.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,2.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 2;  aiIndex[2] = 1;
    aiIndex[3] = 0;  aiIndex[4] = 3;  aiIndex[5] = 2;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkCeiling));
    rpkCube->AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
void Portals::CreateEndCube (const std::string& rkFloor,
    const std::string& rkCeiling, const std::string& rkWall, Node*& rpkCube,
    Portal**& rapkPortal)
{
    rpkCube = WM4_NEW Node;

    // walls
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,20);
    pkVBuffer->Position3( 0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3( 1) = Vector3f(+1.0f,-1.0f,0.0f);
    pkVBuffer->Position3( 2) = Vector3f(+1.0f,+1.0f,0.0f);
    pkVBuffer->Position3( 3) = Vector3f(-1.0f,+1.0f,0.0f);
    pkVBuffer->Position3( 4) = Vector3f(-1.0f,-1.0f,2.0f);
    pkVBuffer->Position3( 5) = Vector3f(+1.0f,-1.0f,2.0f);
    pkVBuffer->Position3( 6) = Vector3f(+1.0f,+1.0f,2.0f);
    pkVBuffer->Position3( 7) = Vector3f(-1.0f,+1.0f,2.0f);
    pkVBuffer->Position3( 8) = Vector3f(+0.5f,-1.0f,0.0f);
    pkVBuffer->Position3( 9) = Vector3f(+0.5f,-1.0f,1.5f);
    pkVBuffer->Position3(10) = Vector3f(-0.5f,-1.0f,1.5f);
    pkVBuffer->Position3(11) = Vector3f(-0.5f,-1.0f,0.0f);
    pkVBuffer->Position3(12) = Vector3f(-0.5f,+1.0f,0.0f);
    pkVBuffer->Position3(13) = Vector3f(-0.5f,+1.0f,1.5f);
    pkVBuffer->Position3(14) = Vector3f(+0.5f,+1.0f,1.5f);
    pkVBuffer->Position3(15) = Vector3f(+0.5f,+1.0f,0.0f);
    pkVBuffer->Position3(16) = Vector3f(+1.0f,+0.5f,0.0f);
    pkVBuffer->Position3(17) = Vector3f(+1.0f,+0.5f,1.5f);
    pkVBuffer->Position3(18) = Vector3f(+1.0f,-0.5f,1.5f);
    pkVBuffer->Position3(19) = Vector3f(+1.0f,-0.5f,0.0f);

    pkVBuffer->TCoord2(0, 0) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 1) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 2) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 3) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 4) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 5) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 6) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 7) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 8) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0, 9) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,10) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,11) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,12) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0,13) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,14) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,15) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,16) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,17) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,18) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,19) = Vector2f(0.75f,0.00f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(60);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] =  1;  aiIndex[ 1] =  8;  aiIndex[ 2] =  9;
    aiIndex[ 3] =  1;  aiIndex[ 4] =  9;  aiIndex[ 5] =  5;
    aiIndex[ 6] =  5;  aiIndex[ 7] =  9;  aiIndex[ 8] =  4;
    aiIndex[ 9] =  9;  aiIndex[10] = 10;  aiIndex[11] =  4;
    aiIndex[12] = 10;  aiIndex[13] =  0;  aiIndex[14] =  4;
    aiIndex[15] = 11;  aiIndex[16] =  0;  aiIndex[17] = 10;
    aiIndex[18] =  3;  aiIndex[19] = 12;  aiIndex[20] = 13;
    aiIndex[21] =  3;  aiIndex[22] = 13;  aiIndex[23] =  7;
    aiIndex[24] =  7;  aiIndex[25] = 13;  aiIndex[26] =  6;
    aiIndex[27] = 13;  aiIndex[28] = 14;  aiIndex[29] =  6;
    aiIndex[30] = 14;  aiIndex[31] =  2;  aiIndex[32] =  6;
    aiIndex[33] = 15;  aiIndex[34] =  2;  aiIndex[35] = 14;
    aiIndex[36] =  2;  aiIndex[37] = 16;  aiIndex[38] = 17;
    aiIndex[39] =  2;  aiIndex[40] = 17;  aiIndex[41] =  6;
    aiIndex[42] =  6;  aiIndex[43] = 17;  aiIndex[44] =  5;
    aiIndex[45] = 17;  aiIndex[46] = 18;  aiIndex[47] =  5;
    aiIndex[48] = 18;  aiIndex[49] =  1;  aiIndex[50] =  5;
    aiIndex[51] = 19;  aiIndex[52] =  1;  aiIndex[53] = 18;
    aiIndex[54] =  0;  aiIndex[55] =  7;  aiIndex[56] =  4;
    aiIndex[57] =  0;  aiIndex[58] =  3;  aiIndex[59] =  7;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkWall));
    rpkCube->AttachChild(pkMesh);

    // portals
    rapkPortal = WM4_NEW Portal*[3];

    Vector3f* akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(11);
    akModelVertex[1] = pkVBuffer->Position3(10);
    akModelVertex[2] = pkVBuffer->Position3( 9);
    akModelVertex[3] = pkVBuffer->Position3( 8);
    Plane3f kModelPlane(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[0] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(15);
    akModelVertex[1] = pkVBuffer->Position3(14);
    akModelVertex[2] = pkVBuffer->Position3(13);
    akModelVertex[3] = pkVBuffer->Position3(12);
    kModelPlane = Plane3f(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[1] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(19);
    akModelVertex[1] = pkVBuffer->Position3(18);
    akModelVertex[2] = pkVBuffer->Position3(17);
    akModelVertex[3] = pkVBuffer->Position3(16);
    kModelPlane = Plane3f(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[2] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    // floor
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,0.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkFloor));
    rpkCube->AttachChild(pkMesh);

    // ceiling
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,2.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,2.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 2;  aiIndex[2] = 1;
    aiIndex[3] = 0;  aiIndex[4] = 3;  aiIndex[5] = 2;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkCeiling));
    rpkCube->AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
void Portals::CreateDiagonalConnector (const std::string& rkFloor,
    const std::string& rkCeiling, const std::string& rkWall, Node*& rpkCube,
    Portal**& rapkPortal)
{
    rpkCube = WM4_NEW Node;

    // walls
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,16);
    pkVBuffer->Position3( 0) = Vector3f(3.0f,1.0f,0.0f);
    pkVBuffer->Position3( 1) = Vector3f(5.0f,1.0f,0.0f);
    pkVBuffer->Position3( 2) = Vector3f(1.0f,5.0f,0.0f);
    pkVBuffer->Position3( 3) = Vector3f(1.0f,3.0f,0.0f);
    pkVBuffer->Position3( 4) = Vector3f(3.0f,1.0f,2.0f);
    pkVBuffer->Position3( 5) = Vector3f(5.0f,1.0f,2.0f);
    pkVBuffer->Position3( 6) = Vector3f(1.0f,5.0f,2.0f);
    pkVBuffer->Position3( 7) = Vector3f(1.0f,3.0f,2.0f);
    pkVBuffer->Position3( 8) = Vector3f(4.5f,1.0f,0.0f);
    pkVBuffer->Position3( 9) = Vector3f(4.5f,1.0f,1.5f);
    pkVBuffer->Position3(10) = Vector3f(3.5f,1.0f,1.5f);
    pkVBuffer->Position3(11) = Vector3f(3.5f,1.0f,0.0f);
    pkVBuffer->Position3(12) = Vector3f(1.0f,3.5f,0.0f);
    pkVBuffer->Position3(13) = Vector3f(1.0f,3.5f,1.5f);
    pkVBuffer->Position3(14) = Vector3f(1.0f,4.5f,1.5f);
    pkVBuffer->Position3(15) = Vector3f(1.0f,4.5f,0.0f);

    pkVBuffer->TCoord2(0, 0) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 1) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 2) = Vector2f(0.00f,0.00f);
    pkVBuffer->TCoord2(0, 3) = Vector2f(1.00f,0.00f);
    pkVBuffer->TCoord2(0, 4) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 5) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 6) = Vector2f(0.00f,1.00f);
    pkVBuffer->TCoord2(0, 7) = Vector2f(1.00f,1.00f);
    pkVBuffer->TCoord2(0, 8) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0, 9) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,10) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,11) = Vector2f(0.25f,0.00f);
    pkVBuffer->TCoord2(0,12) = Vector2f(0.75f,0.00f);
    pkVBuffer->TCoord2(0,13) = Vector2f(0.75f,0.75f);
    pkVBuffer->TCoord2(0,14) = Vector2f(0.25f,0.75f);
    pkVBuffer->TCoord2(0,15) = Vector2f(0.25f,0.00f);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(48);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] =  1;  aiIndex[ 1] =  8;  aiIndex[ 2] =  9;
    aiIndex[ 3] =  1;  aiIndex[ 4] =  9;  aiIndex[ 5] =  5;
    aiIndex[ 6] =  5;  aiIndex[ 7] =  9;  aiIndex[ 8] =  4;
    aiIndex[ 9] =  9;  aiIndex[10] = 10;  aiIndex[11] =  4;
    aiIndex[12] = 10;  aiIndex[13] =  0;  aiIndex[14] =  4;
    aiIndex[15] = 11;  aiIndex[16] =  0;  aiIndex[17] = 10;
    aiIndex[18] =  3;  aiIndex[19] = 12;  aiIndex[20] = 13;
    aiIndex[21] =  3;  aiIndex[22] = 13;  aiIndex[23] =  7;
    aiIndex[24] =  7;  aiIndex[25] = 13;  aiIndex[26] =  6;
    aiIndex[27] = 13;  aiIndex[28] = 14;  aiIndex[29] =  6;
    aiIndex[30] = 14;  aiIndex[31] =  2;  aiIndex[32] =  6;
    aiIndex[33] = 15;  aiIndex[34] =  2;  aiIndex[35] = 14;
    aiIndex[36] =  0;  aiIndex[37] =  7;  aiIndex[38] =  4;
    aiIndex[39] =  0;  aiIndex[40] =  3;  aiIndex[41] =  7;
    aiIndex[42] =  2;  aiIndex[43] =  5;  aiIndex[44] =  6;
    aiIndex[45] =  2;  aiIndex[46] =  1;  aiIndex[47] =  5;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkWall));
    rpkCube->AttachChild(pkMesh);

    // portals
    rapkPortal = WM4_NEW Portal*[2];

    Vector3f* akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(11);
    akModelVertex[1] = pkVBuffer->Position3(10);
    akModelVertex[2] = pkVBuffer->Position3( 9);
    akModelVertex[3] = pkVBuffer->Position3( 8);
    Plane3f kModelPlane(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[0] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    akModelVertex = WM4_NEW Vector3f[4];
    akModelVertex[0] = pkVBuffer->Position3(15);
    akModelVertex[1] = pkVBuffer->Position3(14);
    akModelVertex[2] = pkVBuffer->Position3(13);
    akModelVertex[3] = pkVBuffer->Position3(12);
    kModelPlane = Plane3f(akModelVertex[0],akModelVertex[1],akModelVertex[2]);
    rapkPortal[1] = WM4_NEW Portal(4,akModelVertex,kModelPlane,0,true);

    // floor
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(3.0f,1.0f,0.0f);
    pkVBuffer->Position3(1) = Vector3f(5.0f,1.0f,0.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,5.0f,0.0f);
    pkVBuffer->Position3(3) = Vector3f(1.0f,3.0f,0.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkFloor));
    rpkCube->AttachChild(pkMesh);

    // ceiling
    pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(3.0f,1.0f,2.0f);
    pkVBuffer->Position3(1) = Vector3f(5.0f,1.0f,2.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,5.0f,2.0f);
    pkVBuffer->Position3(3) = Vector3f(1.0f,3.0f,2.0f);

    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);

    pkIBuffer = WM4_NEW IndexBuffer(6);
    aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 2;  aiIndex[2] = 1;
    aiIndex[3] = 0;  aiIndex[4] = 3;  aiIndex[5] = 2;

    pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW TextureEffect(rkCeiling));
    rpkCube->AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
TriMesh* Portals::CreateOutside ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    int iVQuantity = 40;
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    pkVBuffer->Position3(0) = Vector3f(-1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(1) = Vector3f(+1.0f,-1.0f,2.0f);
    pkVBuffer->Position3(2) = Vector3f(+1.0f,+1.0f,2.0f);
    pkVBuffer->Position3(3) = Vector3f(-1.0f,+1.0f,2.0f);
    pkVBuffer->Position3(4) = Vector3f(-1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(5) = Vector3f(+1.0f,-1.0f,0.0f);
    pkVBuffer->Position3(6) = Vector3f(+1.0f,+1.0f,0.0f);
    pkVBuffer->Position3(7) = Vector3f(-1.0f,+1.0f,0.0f);

    Vector3f kTrn(4.0f,0.0f,0.0f);
    int i, j;
    for (i = 0, j = 8; i < 8; i++, j++)
    {
        pkVBuffer->Position3(j) = pkVBuffer->Position3(i) + kTrn;
    }

    kTrn = Vector3f(0.0f,4.0f,0.0f);
    for (i = 0, j = 16; i < 8; i++, j++)
    {
        pkVBuffer->Position3(j) = pkVBuffer->Position3(i) + kTrn;
    }

    kTrn = Vector3f(-4.0f,0.0f,0.0f);
    for (i = 0, j = 24; i < 8; i++, j++)
    {
        pkVBuffer->Position3(j) = pkVBuffer->Position3(i) + kTrn;
    }

    kTrn = Vector3f(0.0f,-4.0f,0.0f);
    for (i = 0, j = 32; i < 8; i++, j++)
    {
        pkVBuffer->Position3(j) = pkVBuffer->Position3(i) + kTrn;
    }

    for (i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Color3(0,i) = ColorRGB(Mathf::UnitRandom(),
            Mathf::UnitRandom(),Mathf::UnitRandom());
    }

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(276);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[ 0] =  0;  aiIndex[ 1] =  1;  aiIndex[ 2] =  2;
    aiIndex[ 3] =  0;  aiIndex[ 4] =  2;  aiIndex[ 5] =  3;
    aiIndex[ 6] =  1;  aiIndex[ 7] = 11;  aiIndex[ 8] =  2;
    aiIndex[ 9] =  1;  aiIndex[10] =  8;  aiIndex[11] = 11;
    aiIndex[12] =  8;  aiIndex[13] = 10;  aiIndex[14] = 11;
    aiIndex[15] =  8;  aiIndex[16] =  9;  aiIndex[17] = 10;
    aiIndex[18] = 25;  aiIndex[19] =  0;  aiIndex[20] =  3;
    aiIndex[21] = 25;  aiIndex[22] =  3;  aiIndex[23] = 26;
    aiIndex[24] = 24;  aiIndex[25] = 25;  aiIndex[26] = 26;
    aiIndex[27] = 24;  aiIndex[28] = 26;  aiIndex[29] = 27;
    aiIndex[30] =  3;  aiIndex[31] =  2;  aiIndex[32] = 17;
    aiIndex[33] =  3;  aiIndex[34] = 17;  aiIndex[35] = 16;
    aiIndex[36] = 16;  aiIndex[37] = 17;  aiIndex[38] = 18;
    aiIndex[39] = 16;  aiIndex[40] = 18;  aiIndex[41] = 19;
    aiIndex[42] = 35;  aiIndex[43] =  1;  aiIndex[44] =  0;
    aiIndex[45] = 35;  aiIndex[46] = 34;  aiIndex[47] =  1;
    aiIndex[48] = 32;  aiIndex[49] = 34;  aiIndex[50] = 35;
    aiIndex[51] = 32;  aiIndex[52] = 33;  aiIndex[53] = 34;
    aiIndex[54] = 27;  aiIndex[55] = 16;  aiIndex[56] = 19;
    aiIndex[57] = 27;  aiIndex[58] = 26;  aiIndex[59] = 16;
    aiIndex[60] = 17;  aiIndex[61] = 11;  aiIndex[62] = 18;
    aiIndex[63] = 18;  aiIndex[64] = 11;  aiIndex[65] = 10;
    aiIndex[66] = 24;  aiIndex[67] = 32;  aiIndex[68] = 25;
    aiIndex[69] = 25;  aiIndex[70] = 32;  aiIndex[71] = 35;
    aiIndex[72] = 34;  aiIndex[73] =  9;  aiIndex[74] =  8;
    aiIndex[75] = 34;  aiIndex[76] = 33;  aiIndex[77] =  9;

    for (i = 0, j = 78; i < 26; i++)
    {
        aiIndex[j++] = 4 + aiIndex[3*i];
        aiIndex[j++] = 4 + aiIndex[3*i+2];
        aiIndex[j++] = 4 + aiIndex[3*i+1];
    }

    aiIndex[156] =  6;  aiIndex[157] =  2;  aiIndex[158] = 11;
    aiIndex[159] =  6;  aiIndex[160] = 11;  aiIndex[161] = 15;
    aiIndex[162] = 21;  aiIndex[163] = 17;  aiIndex[164] =  2;
    aiIndex[165] = 21;  aiIndex[166] =  2;  aiIndex[167] =  6;
    aiIndex[168] = 15;  aiIndex[169] = 11;  aiIndex[170] = 17;
    aiIndex[171] = 15;  aiIndex[172] = 17;  aiIndex[173] = 21;
    aiIndex[174] = 12;  aiIndex[175] =  8;  aiIndex[176] =  1;
    aiIndex[177] = 12;  aiIndex[178] =  1;  aiIndex[179] =  5;
    aiIndex[180] =  5;  aiIndex[181] =  1;  aiIndex[182] = 34;
    aiIndex[183] =  5;  aiIndex[184] = 34;  aiIndex[185] = 38;
    aiIndex[186] = 38;  aiIndex[187] = 34;  aiIndex[188] =  8;
    aiIndex[189] = 38;  aiIndex[190] =  8;  aiIndex[191] = 12;
    aiIndex[192] =  4;  aiIndex[193] =  0;  aiIndex[194] = 25;
    aiIndex[195] =  4;  aiIndex[196] = 25;  aiIndex[197] = 29;
    aiIndex[198] = 39;  aiIndex[199] = 35;  aiIndex[200] =  0;
    aiIndex[201] = 39;  aiIndex[202] =  0;  aiIndex[203] =  4;
    aiIndex[204] = 29;  aiIndex[205] = 25;  aiIndex[206] = 35;
    aiIndex[207] = 29;  aiIndex[208] = 35;  aiIndex[209] = 39;
    aiIndex[210] =  7;  aiIndex[211] =  3;  aiIndex[212] = 16;
    aiIndex[213] =  7;  aiIndex[214] = 16;  aiIndex[215] = 20;
    aiIndex[216] = 30;  aiIndex[217] = 26;  aiIndex[218] =  3;
    aiIndex[219] = 30;  aiIndex[220] =  3;  aiIndex[221] =  7;
    aiIndex[222] = 20;  aiIndex[223] = 16;  aiIndex[224] = 26;
    aiIndex[225] = 20;  aiIndex[226] = 26;  aiIndex[227] = 30;
    aiIndex[228] = 14;  aiIndex[229] = 10;  aiIndex[230] =  9;
    aiIndex[231] = 14;  aiIndex[232] =  9;  aiIndex[233] = 13;
    aiIndex[234] = 22;  aiIndex[235] = 18;  aiIndex[236] = 10;
    aiIndex[237] = 22;  aiIndex[238] = 10;  aiIndex[239] = 14;
    aiIndex[240] = 23;  aiIndex[241] = 19;  aiIndex[242] = 18;
    aiIndex[243] = 23;  aiIndex[244] = 18;  aiIndex[245] = 22;
    aiIndex[246] = 31;  aiIndex[247] = 27;  aiIndex[248] = 19;
    aiIndex[249] = 31;  aiIndex[250] = 19;  aiIndex[251] = 23;
    aiIndex[252] = 28;  aiIndex[253] = 24;  aiIndex[254] = 27;
    aiIndex[255] = 28;  aiIndex[256] = 27;  aiIndex[257] = 31;
    aiIndex[258] = 36;  aiIndex[259] = 32;  aiIndex[260] = 24;
    aiIndex[261] = 36;  aiIndex[262] = 24;  aiIndex[263] = 28;
    aiIndex[264] = 37;  aiIndex[265] = 33;  aiIndex[266] = 32;
    aiIndex[267] = 37;  aiIndex[268] = 32;  aiIndex[269] = 36;
    aiIndex[270] = 13;  aiIndex[271] =  9;  aiIndex[272] = 33;
    aiIndex[273] = 13;  aiIndex[274] = 33;  aiIndex[275] = 37;

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->AttachEffect(WM4_NEW VertexColor3Effect);

    return pkMesh;
}
//----------------------------------------------------------------------------
ConvexRegionManager* Portals::CreateBspTree ()
{
    // construction of tree in BspTree.txt.
    ConvexRegionManager* pkC = WM4_NEW ConvexRegionManager;
    pkC->SetName("CRM");
    pkC->ModelPlane().Normal = Vector3f(0.0f,0.0f,-1.0f);
    pkC->ModelPlane().Constant = 0.0f;

    BspNode* pkR = WM4_NEW BspNode;
    pkR->SetName("R");
    pkR->ModelPlane().Normal = Vector3f(0.0f,0.0f,1.0f);
    pkR->ModelPlane().Constant = 2.0f;

    BspNode* pkRR = WM4_NEW BspNode;
    pkRR->SetName("RR");
    pkRR->ModelPlane().Normal = Vector3f(1.0f,0.0f,0.0f);
    pkRR->ModelPlane().Constant = 1.0f;

    BspNode* pkRRL = WM4_NEW BspNode;
    pkRRL->SetName("RRL");
    pkRRL->ModelPlane().Normal = Vector3f(0.0f,1.0f,0.0f);
    pkRRL->ModelPlane().Constant = 1.0f;

    BspNode* pkRRLL = WM4_NEW BspNode;
    pkRRLL->SetName("RRLL");
    pkRRLL->ModelPlane().Normal = Vector3f(1.0f,1.0f,0.0f);
    pkRRLL->ModelPlane().Constant = 4.0f;

    BspNode* pkRRLLL = WM4_NEW BspNode;
    pkRRLLL->SetName("RRLLL");
    pkRRLLL->ModelPlane().Normal = Vector3f(1.0f,1.0f,0.0f);
    pkRRLLL->ModelPlane().Constant = 6.0f;

    BspNode* pkRRLR = WM4_NEW BspNode;
    pkRRLR->SetName("RRLR");
    pkRRLR->ModelPlane().Normal = Vector3f(0.0f,-1.0f,0.0f);
    pkRRLR->ModelPlane().Constant = 1.0f;

    BspNode* pkRRLRL = WM4_NEW BspNode;
    pkRRLRL->SetName("RRLRL");
    pkRRLRL->ModelPlane().Normal = Vector3f(1.0f,-1.0f,0.0f);
    pkRRLRL->ModelPlane().Constant = 4.0f;

    BspNode* pkRRLRLL = WM4_NEW BspNode;
    pkRRLRLL->SetName("RRLRLL");
    pkRRLRLL->ModelPlane().Normal = Vector3f(1.0f,-1.0f,0.0f);
    pkRRLRLL->ModelPlane().Constant = 6.0f;

    BspNode* pkRRLRR = WM4_NEW BspNode;
    pkRRLRR->SetName("RRLRR");
    pkRRLRR->ModelPlane().Normal = Vector3f(1.0f,0.0f,0.0f);
    pkRRLRR->ModelPlane().Constant = 3.0f;

    BspNode* pkRRLRRL = WM4_NEW BspNode;
    pkRRLRRL->SetName("RRLRRL");
    pkRRLRRL->ModelPlane().Normal = Vector3f(1.0f,0.0f,0.0f);
    pkRRLRRL->ModelPlane().Constant = 5.0f;

    BspNode* pkRRR = WM4_NEW BspNode;
    pkRRR->SetName("RRR");
    pkRRR->ModelPlane().Normal = Vector3f(-1.0f,0.0f,0.0f);
    pkRRR->ModelPlane().Constant = 1.0f;

    BspNode* pkRRRL = WM4_NEW BspNode;
    pkRRRL->SetName("RRRL");
    pkRRRL->ModelPlane().Normal = Vector3f(0.0f,1.0f,0.0f);
    pkRRRL->ModelPlane().Constant = 1.0f;

    BspNode* pkRRRLL = WM4_NEW BspNode;
    pkRRRLL->SetName("RRRLL");
    pkRRRLL->ModelPlane().Normal = Vector3f(-1.0f,1.0f,0.0f);
    pkRRRLL->ModelPlane().Constant = 4.0f;

    BspNode* pkRRRLLL = WM4_NEW BspNode;
    pkRRRLLL->SetName("RRRLLL");
    pkRRRLLL->ModelPlane().Normal = Vector3f(-1.0f,1.0f,0.0f);
    pkRRRLLL->ModelPlane().Constant = 6.0f;

    BspNode* pkRRRLR = WM4_NEW BspNode;
    pkRRRLR->SetName("RRRLR");
    pkRRRLR->ModelPlane().Normal = Vector3f(0.0f,-1.0f,0.0f);
    pkRRRLR->ModelPlane().Constant = 1.0f;

    BspNode* pkRRRLRL = WM4_NEW BspNode;
    pkRRRLRL->SetName("RRRLRL");
    pkRRRLRL->ModelPlane().Normal = Vector3f(-1.0f,-1.0f,0.0f);
    pkRRRLRL->ModelPlane().Constant = 4.0f;

    BspNode* pkRRRLRLL = WM4_NEW BspNode;
    pkRRRLRLL->SetName("RRRLRLL");
    pkRRRLRLL->ModelPlane().Normal = Vector3f(-1.0f,-1.0f,0.0f);
    pkRRRLRLL->ModelPlane().Constant = 6.0f;

    BspNode* pkRRRLRR = WM4_NEW BspNode;
    pkRRRLRR->SetName("RRRLRR");
    pkRRRLRR->ModelPlane().Normal = Vector3f(-1.0f,0.0f,0.0f);
    pkRRRLRR->ModelPlane().Constant = 3.0f;

    BspNode* pkRRRLRRL = WM4_NEW BspNode;
    pkRRRLRRL->SetName("RRRLRRL");
    pkRRRLRRL->ModelPlane().Normal = Vector3f(-1.0f,0.0f,0.0f);
    pkRRRLRRL->ModelPlane().Constant = 5.0f;

    BspNode* pkRRRR = WM4_NEW BspNode;
    pkRRRR->SetName("RRRR");
    pkRRRR->ModelPlane().Normal = Vector3f(0.0f,1.0f,0.0f);
    pkRRRR->ModelPlane().Constant = 1.0f;

    BspNode* pkRRRRL = WM4_NEW BspNode;
    pkRRRRL->SetName("RRRRL");
    pkRRRRL->ModelPlane().Normal = Vector3f(0.0f,1.0f,0.0f);
    pkRRRRL->ModelPlane().Constant = 3.0f;

    BspNode* pkRRRRLL = WM4_NEW BspNode;
    pkRRRRLL->SetName("RRRRLL");
    pkRRRRLL->ModelPlane().Normal = Vector3f(0.0f,1.0f,0.0f);
    pkRRRRLL->ModelPlane().Constant = 5.0f;

    BspNode* pkRRRRR = WM4_NEW BspNode;
    pkRRRRR->SetName("RRRRR");
    pkRRRRR->ModelPlane().Normal = Vector3f(0.0f,-1.0f,0.0f);
    pkRRRRR->ModelPlane().Constant = 1.0f;

    BspNode* pkRRRRRL = WM4_NEW BspNode;
    pkRRRRRL->SetName("RRRRRL");
    pkRRRRRL->ModelPlane().Normal = Vector3f(0.0f,-1.0f,0.0f);
    pkRRRRRL->ModelPlane().Constant = 3.0f;

    BspNode* pkRRRRRLL = WM4_NEW BspNode;
    pkRRRRRLL->SetName("RRRRRLL");
    pkRRRRRLL->ModelPlane().Normal = Vector3f(0.0f,-1.0f,0.0f);
    pkRRRRRLL->ModelPlane().Constant = 5.0f;

    // create regions and portals
    Node* pkNode;
    Portal** apkPortal;

    CreateCenterCube("Floor","Ceiling","Wall1","Water","Agate","Bark","Sky",
        pkNode,apkPortal);
    ConvexRegion* pkC00 = WM4_NEW ConvexRegion(4,apkPortal);
    pkC00->SetName("C00");
    pkC00->AttachChild(pkNode);

    CreateAxisConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkC02 = WM4_NEW ConvexRegion(2,apkPortal);
    pkC02->SetName("C02");
    pkC02->AttachChild(pkNode);
    pkC02->Local.SetTranslate(Vector3f(0.0f,2.0f,0.0f));

    CreateAxisConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkC0m2 = WM4_NEW ConvexRegion(2,apkPortal);
    pkC0m2->SetName("C0m2");
    pkC0m2->AttachChild(pkNode);
    pkC0m2->Local.SetTranslate(Vector3f(0.0f,-2.0f,0.0f));

    CreateAxisConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkC20 = WM4_NEW ConvexRegion(2,apkPortal);
    pkC20->SetName("C20");
    pkC20->AttachChild(pkNode);
    pkC20->Local.SetTranslate(Vector3f(2.0f,0.0f,0.0f));
    pkC20->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        Mathf::HALF_PI));

    CreateAxisConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkCm20 = WM4_NEW ConvexRegion(2,apkPortal);
    pkCm20->SetName("Cm20");
    pkCm20->AttachChild(pkNode);
    pkCm20->Local.SetTranslate(Vector3f(-2.0f,0.0f,0.0f));
    pkCm20->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        Mathf::HALF_PI));

    CreateEndCube("Floor","Ceiling","Wall1",pkNode,apkPortal);
    ConvexRegion* pkC04 = WM4_NEW ConvexRegion(3,apkPortal);
    pkC04->SetName("C04");
    pkC04->AttachChild(pkNode);
    pkC04->Local.SetTranslate(Vector3f(0.0f,4.0f,0.0f));
    pkC04->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        -Mathf::HALF_PI));

    CreateEndCube("Floor","Ceiling","Wall1",pkNode,apkPortal);
    ConvexRegion* pkC0m4 = WM4_NEW ConvexRegion(3,apkPortal);
    pkC0m4->SetName("C0m4");
    pkC0m4->AttachChild(pkNode);
    pkC0m4->Local.SetTranslate(Vector3f(0.0f,-4.0f,0.0f));
    pkC0m4->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        Mathf::HALF_PI));

    CreateEndCube("Floor","Ceiling","Wall1",pkNode,apkPortal);
    ConvexRegion* pkC40 = WM4_NEW ConvexRegion(3,apkPortal);
    pkC40->SetName("C40");
    pkC40->AttachChild(pkNode);
    pkC40->Local.SetTranslate(Vector3f(4.0f,0.0f,0.0f));
    pkC40->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        Mathf::PI));

    CreateEndCube("Floor","Ceiling","Wall1",pkNode,apkPortal);
    ConvexRegion* pkCm40 = WM4_NEW ConvexRegion(3,apkPortal);
    pkCm40->SetName("Cm40");
    pkCm40->AttachChild(pkNode);
    pkCm40->Local.SetTranslate(Vector3f(-4.0f,0.0f,0.0f));

    CreateDiagonalConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkDPP = WM4_NEW ConvexRegion(2,apkPortal);
    pkDPP->SetName("DPP");
    pkDPP->AttachChild(pkNode);

    CreateDiagonalConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkDMP = WM4_NEW ConvexRegion(2,apkPortal);
    pkDMP->SetName("DMP");
    pkDMP->AttachChild(pkNode);
    pkDMP->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        Mathf::HALF_PI));

    CreateDiagonalConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkDMM = WM4_NEW ConvexRegion(2,apkPortal);
    pkDMM->SetName("DMM");
    pkDMM->AttachChild(pkNode);
    pkDMM->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        Mathf::PI));

    CreateDiagonalConnector("Floor","Ceiling","Wall2",pkNode,apkPortal);
    ConvexRegion* pkDPM = WM4_NEW ConvexRegion(2,apkPortal);
    pkDPM->SetName("DPM");
    pkDPM->AttachChild(pkNode);
    pkDPM->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        -Mathf::HALF_PI));

    // create adjacency graph
    pkC00->GetPortal(0)->AdjacentRegion() = pkC0m2;
    pkC00->GetPortal(1)->AdjacentRegion() = pkC02;
    pkC00->GetPortal(2)->AdjacentRegion() = pkC20;
    pkC00->GetPortal(3)->AdjacentRegion() = pkCm20;
    pkC02->GetPortal(0)->AdjacentRegion() = pkC00;
    pkC02->GetPortal(1)->AdjacentRegion() = pkC04;
    pkC0m2->GetPortal(0)->AdjacentRegion() = pkC0m4;
    pkC0m2->GetPortal(1)->AdjacentRegion() = pkC00;
    pkC20->GetPortal(0)->AdjacentRegion() = pkC40;
    pkC20->GetPortal(1)->AdjacentRegion() = pkC00;
    pkCm20->GetPortal(0)->AdjacentRegion() = pkC00;
    pkCm20->GetPortal(1)->AdjacentRegion() = pkCm40;
    pkC04->GetPortal(0)->AdjacentRegion() = pkDMP;
    pkC04->GetPortal(1)->AdjacentRegion() = pkDPP;
    pkC04->GetPortal(2)->AdjacentRegion() = pkC02;
    pkC0m4->GetPortal(0)->AdjacentRegion() = pkDPM;
    pkC0m4->GetPortal(1)->AdjacentRegion() = pkDMM;
    pkC0m4->GetPortal(2)->AdjacentRegion() = pkC0m2;
    pkC40->GetPortal(0)->AdjacentRegion() = pkDPP;
    pkC40->GetPortal(1)->AdjacentRegion() = pkDPM;
    pkC40->GetPortal(2)->AdjacentRegion() = pkC20;
    pkCm40->GetPortal(0)->AdjacentRegion() = pkDMM;
    pkCm40->GetPortal(1)->AdjacentRegion() = pkDMP;
    pkCm40->GetPortal(2)->AdjacentRegion() = pkCm20;
    pkDPP->GetPortal(0)->AdjacentRegion() = pkC40;
    pkDPP->GetPortal(1)->AdjacentRegion() = pkC04;
    pkDMP->GetPortal(0)->AdjacentRegion() = pkC04;
    pkDMP->GetPortal(1)->AdjacentRegion() = pkCm40;
    pkDMM->GetPortal(0)->AdjacentRegion() = pkCm40;
    pkDMM->GetPortal(1)->AdjacentRegion() = pkC0m4;
    pkDPM->GetPortal(0)->AdjacentRegion() = pkC0m4;
    pkDPM->GetPortal(1)->AdjacentRegion() = pkC40;

    // depth-first attachment
    pkC->AttachPositiveChild(0);
    pkC->AttachNegativeChild(pkR);
        pkR->AttachPositiveChild(0);
        pkR->AttachNegativeChild(pkRR);
            pkRR->AttachPositiveChild(pkRRL);
                pkRRL->AttachPositiveChild(pkRRLL);
                    pkRRLL->AttachPositiveChild(pkRRLLL);
                        pkRRLLL->AttachPositiveChild(0);
                        pkRRLLL->AttachNegativeChild(pkDPP);
                    pkRRLL->AttachNegativeChild(0);
                pkRRL->AttachNegativeChild(pkRRLR);
                    pkRRLR->AttachPositiveChild(pkRRLRL);
                        pkRRLRL->AttachPositiveChild(pkRRLRLL);
                            pkRRLRLL->AttachPositiveChild(0);
                            pkRRLRLL->AttachNegativeChild(pkDPM);
                        pkRRLRL->AttachNegativeChild(0);
                    pkRRLR->AttachNegativeChild(pkRRLRR);
                        pkRRLRR->AttachPositiveChild(pkRRLRRL);
                            pkRRLRRL->AttachPositiveChild(0);
                            pkRRLRRL->AttachNegativeChild(pkC40);
                        pkRRLRR->AttachNegativeChild(pkC20);
            pkRR->AttachNegativeChild(pkRRR);
                pkRRR->AttachPositiveChild(pkRRRL);
                    pkRRRL->AttachPositiveChild(pkRRRLL);
                        pkRRRLL->AttachPositiveChild(pkRRRLLL);
                            pkRRRLLL->AttachPositiveChild(0);
                            pkRRRLLL->AttachNegativeChild(pkDMP);
                        pkRRRLL->AttachNegativeChild(0);
                    pkRRRL->AttachNegativeChild(pkRRRLR);
                        pkRRRLR->AttachPositiveChild(pkRRRLRL);
                            pkRRRLRL->AttachPositiveChild(pkRRRLRLL);
                                pkRRRLRLL->AttachPositiveChild(0);
                                pkRRRLRLL->AttachNegativeChild(pkDMM);
                            pkRRRLRL->AttachNegativeChild(0);
                        pkRRRLR->AttachNegativeChild(pkRRRLRR);
                            pkRRRLRR->AttachPositiveChild(pkRRRLRRL);
                                pkRRRLRRL->AttachPositiveChild(0);
                                pkRRRLRRL->AttachNegativeChild(pkCm40);
                            pkRRRLRR->AttachNegativeChild(pkCm20);
                pkRRR->AttachNegativeChild(pkRRRR);
                    pkRRRR->AttachPositiveChild(pkRRRRL);
                        pkRRRRL->AttachPositiveChild(pkRRRRLL);
                            pkRRRRLL->AttachPositiveChild(0);
                            pkRRRRLL->AttachNegativeChild(pkC04);
                        pkRRRRL->AttachNegativeChild(pkC02);
                    pkRRRR->AttachNegativeChild(pkRRRRR);
                        pkRRRRR->AttachPositiveChild(pkRRRRRL);
                            pkRRRRRL->AttachPositiveChild(pkRRRRRLL);
                                pkRRRRRLL->AttachPositiveChild(0);
                                pkRRRRRLL->AttachNegativeChild(pkC0m4);
                            pkRRRRRL->AttachNegativeChild(pkC0m2);
                        pkRRRRR->AttachNegativeChild(pkC00);
    return pkC;
}
//----------------------------------------------------------------------------
