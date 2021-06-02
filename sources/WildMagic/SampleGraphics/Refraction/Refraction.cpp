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
// Version: 4.0.2 (2007/06/16)

#include "Refraction.h"

WM4_WINDOW_APPLICATION(Refraction);

//----------------------------------------------------------------------------
Refraction::Refraction ()
    :
    WindowApplication3("Refraction",0,0,480,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
    System::InsertDirectory("Data");
}
//----------------------------------------------------------------------------
bool Refraction::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkCamera->SetFrustum(-0.4125f,0.4125f,-0.4125f,0.4125f,1.0f,10000.0f);
    Vector3f kCDir(1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = Vector3f(-250.0f,0.0f,0.0f);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Refraction::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkEffect = 0;
    m_spkScreenCamera = 0;
    m_spkScreenPolygon = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Refraction::OnIdle ()
{
    MeasureTime();

    if (MoveObject())
    {
        m_spkScene->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->SetCamera(m_spkScreenCamera);
        m_pkRenderer->Draw(m_spkScreenPolygon);

        m_pkRenderer->SetCamera(m_spkCamera);
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());

        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Refraction::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case ' ':
        m_bReflection = !m_bReflection;
        if (m_bReflection)
        {
            m_spkEffect->SetFresnelConstant(0,0.1f);
            m_spkEffect->SetFresnelConstant(1,0.1f);
            m_spkEffect->SetFresnelConstant(2,0.2f);
        }
        else
        {
            m_spkEffect->SetFresnelConstant(0,0.0f);
            m_spkEffect->SetFresnelConstant(1,0.0f);
            m_spkEffect->SetFresnelConstant(2,0.0f);
        }
        return true;
    case '+':
    case '=':
        m_spkEffect->SetEtaRatio(m_spkEffect->GetEtaRatio() + 0.1f);
        return true;
    case '-':
    case '_':
        m_spkEffect->SetEtaRatio(m_spkEffect->GetEtaRatio() - 0.1f);
        return true;
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"Refraction.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Refraction::CreateScene ()
{
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    m_spkScreenCamera = WM4_NEW Camera;
    m_spkScreenCamera->Perspective = false;
    m_spkScreenCamera->SetFrustum(0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    m_spkScreenCamera->SetFrame(Vector3f::ZERO,Vector3f::UNIT_Z,
        Vector3f::UNIT_Y,Vector3f::UNIT_X);

    m_spkScene = WM4_NEW Node;
    m_spkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkTrnNode);

    // Load the face model.
    Stream kStream;
    const char* acPath = System::GetPath("FacePN.wmof",System::SM_READ);
    bool bLoaded = kStream.Load(acPath);
    assert(bLoaded);
    TriMesh* pkFace = DynamicCast<TriMesh>(kStream.GetObjectAt(0));
    assert(pkFace);
    m_spkTrnNode->AttachChild(pkFace);

    // The face is centered at the origin.  The front of the face is visible
    // to an observer positioned on the negative y-axis and looking at the
    // origin.  The face orientation is such that the bottom to top of the
    // face is in the positive z-direction.  The environment mapping of the
    // shader program, reflection.cg, is in the x-direction.  This requires
    // positioning the face so that its front is visible looking in the
    // positive x-direction.  The camera must be positioned and oriented
    // accordingly.
    Matrix3f kZRot(Vector3f::UNIT_Z,-Mathf::HALF_PI);
    pkFace->Local.SetRotate(kZRot);

    // Create a background screen polygon.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,0.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,1.0f,1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,1.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,0.0f);
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    m_spkScreenPolygon = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkScreenPolygon->AttachEffect(WM4_NEW TextureEffect("TilesDisks"));
    m_spkScreenPolygon->UpdateGS();

    // Create a refraction effect for the face.
    m_spkEffect = WM4_NEW RefractionEffect("TilesEnvironment",
        "TilesReflection");
    pkFace->AttachEffect(m_spkEffect);

    // A negative index is not really physically correct, but it makes the
    // object appear to refract what is underneath it.
    m_spkEffect->SetEtaRatio(-0.4f);
    m_spkEffect->SetFresnelConstant(0,0.1f);
    m_spkEffect->SetFresnelConstant(1,0.1f);
    m_spkEffect->SetFresnelConstant(2,0.2f);
    m_bReflection = true;
}
//----------------------------------------------------------------------------
