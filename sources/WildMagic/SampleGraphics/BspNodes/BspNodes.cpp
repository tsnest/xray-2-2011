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

#include "BspNodes.h"

WM4_WINDOW_APPLICATION(BspNodes);

//----------------------------------------------------------------------------
BspNodes::BspNodes ()
    :
    WindowApplication3("BspNodes",0,0,640,480,ColorRGBA(0.9f,0.9f,0.9f,1.0f))
{
}
//----------------------------------------------------------------------------
bool BspNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCLoc(0.0f,-1.0f,0.1f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
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
void BspNodes::OnTerminate ()
{
    m_spkScene = 0;
    m_spkBsp = 0;
    m_spkWireframe = 0;
    m_spkRCull = 0;
    m_spkRWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BspNodes::OnIdle ()
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
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BspNodes::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"BspNodes.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void BspNodes::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Disable z-buffering.
    ZBufferState* pkZS = WM4_NEW ZBufferState;
    pkZS->Enabled = false;
    pkZS->Writable = false;
    m_spkScene->AttachGlobalState(pkZS);

    // Create the ground.  It covers a square with vertices (1,1,0), (1,-1,0),
    // (-1,1,0), and (-1,-1,0).  Multiply the texture coordinates by a factor
    // to enhance the wrap-around.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    TriMesh* pkGround = kSM.Rectangle(2,2,16.0f,16.0f);
    for (int i = 0; i < pkGround->VBuffer->GetVertexQuantity(); i++)
    {
        pkGround->VBuffer->TCoord2(0,i) *= 128.0f;
    }

    ShaderEffect* pkEffect = WM4_NEW TextureEffect("Horizontal");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetFilterType(Texture::LINEAR_LINEAR);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkGround->AttachEffect(pkEffect);
    pkGround->AttachEffect(pkEffect);
    m_spkScene->AttachChild(pkGround);

    // Partition the region above the ground into 5 convex pieces.  Each
    // plane is perpendicular to the ground (not required generally).  A
    // rectangle mesh representing the plane is attached to the plane as the
    // coplanar child.  Rectangles will be attached to each binary separating
    // plane, just for visualization purposes.  Disable back-face culling so
    // the rectangles are always visible.  Enable wireframe to see through the
    // rectangles.
    m_spkRCull = WM4_NEW CullState;
    m_spkRCull->Enabled = false;
    m_spkRWireframe = WM4_NEW WireframeState;
    m_spkRWireframe->Enabled = true;

    Vector2f kV0(-1.0f,1.0f);
    Vector2f kV1(1.0f,-1.0f);
    Vector2f kV2(-0.25f,0.25f);
    Vector2f kV3(-1.0f,-1.0f);
    Vector2f kV4(0.0f,0.0f);
    Vector2f kV5(1.0f,0.5f);
    Vector2f kV6(-0.75f,-7.0f/12.0f);
    Vector2f kV7(-0.75f,0.75f);
    Vector2f kV8(1.0f,1.0f);

    BspNode* pkBsp0 = CreateNode(kV0,kV1,ColorRGB(1.0f,0.0f,0.0f));
    BspNode* pkBsp1 = CreateNode(kV2,kV3,ColorRGB(0.0f,0.5f,0.0f));
    BspNode* pkBsp2 = CreateNode(kV4,kV5,ColorRGB(0.0f,0.0f,1.0f));
    BspNode* pkBsp3 = CreateNode(kV6,kV7,ColorRGB(0.0f,0.0f,0.0f));

    m_spkBsp = pkBsp0;
    pkBsp0->AttachPositiveChild(pkBsp1);
    pkBsp0->AttachNegativeChild(pkBsp2);
    pkBsp1->AttachPositiveChild(pkBsp3);

    // Attach an object in each convex region.  Since at least one object
    // is nonconvex, z-buffering must be enabled for that object.  However,
    // only z-writes need to occur for the convex objects.
    float fHeight = 0.1f;
    Vector2f kCenter;
    TriMesh* pkMesh;

    // (R0) Create a torus mesh.  The torus is not convex, so z-buffering is
    // required to obtain correct drawing.
    pkMesh = kSM.Torus(16,16,1.0f,0.25f);
    pkMesh->AttachGlobalState(WM4_NEW ZBufferState);
    pkEffect = WM4_NEW TextureEffect("Flower");
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV2+kV6+kV7)/3.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp3->AttachPositiveChild(pkMesh);

    // The remaining objects are convex, so z-buffering is not required for
    // them within the BSP tree because of the correct object sorting that
    // tree does.
    ZBufferState* pkZBuffer = WM4_NEW ZBufferState;
    pkZBuffer->Enabled = false;
    pkZBuffer->Writable = false;

    // (R1) create a sphere mesh
    pkMesh = kSM.Sphere(32,16,1.0f);
    pkMesh->AttachGlobalState(pkZBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV0+kV3+kV6+kV7)/4.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp3->AttachNegativeChild(pkMesh);

    // (R2) create a tetrahedron
    pkMesh = kSM.Tetrahedron();
    pkMesh->AttachGlobalState(pkZBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV1+kV2+kV3)/3.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp1->AttachNegativeChild(pkMesh);

    // (R3) create a hexahedron
    pkMesh = kSM.Hexahedron();
    pkMesh->AttachGlobalState(pkZBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV1+kV4+kV5)/3.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp2->AttachPositiveChild(pkMesh);

    // (R4) create an octahedron
    pkMesh = kSM.Octahedron();
    pkMesh->AttachGlobalState(pkZBuffer);
    pkMesh->AttachEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV0+kV4+kV5+kV8)/4.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp2->AttachNegativeChild(pkMesh);

    m_spkScene->AttachChild(m_spkBsp);
}
//----------------------------------------------------------------------------
BspNode* BspNodes::CreateNode (const Vector2f& rkV0, const Vector2f& rkV1,
    const ColorRGB& rkColor)
{
    Vector2f kDir = rkV1 - rkV0;
    Vector3f kNormal(kDir.Y(),-kDir.X(),0.0f);
    kNormal.Normalize();
    float fConstant = kNormal.Dot(Vector3f(rkV0.X(),rkV0.Y(),0.0f));
    float fXExtent = 0.5f*kDir.Length();
    float fYExtent = 0.125f;
    Vector3f kTrn(0.5f*(rkV0.X()+rkV1.X()),0.5f*(rkV0.Y()+rkV1.Y()),
        fYExtent+1e-03f);
    Matrix3f kRot =
        Matrix3f(Vector3f::UNIT_Z,Mathf::ATan2(kDir.Y(),kDir.X())) *
        Matrix3f(Vector3f::UNIT_X,Mathf::HALF_PI);

    BspNode* pkBsp = WM4_NEW BspNode(Plane3f(kNormal,fConstant));

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    StandardMesh kSM(kAttr);
    TriMesh* pkRect = kSM.Rectangle(2,2,fXExtent,fYExtent);
    pkRect->Local.SetTranslate(kTrn);
    pkRect->Local.SetRotate(kRot);
    for (int i = 0; i < 4; i++)
    {
        pkRect->VBuffer->Color3(0,i) = rkColor;
    }

    pkRect->AttachEffect(WM4_NEW VertexColor3Effect);

    pkRect->AttachGlobalState(m_spkRCull);
    pkRect->AttachGlobalState(m_spkRWireframe);

    pkBsp->AttachCoplanarChild(pkRect);

    return pkBsp;
}
//----------------------------------------------------------------------------
