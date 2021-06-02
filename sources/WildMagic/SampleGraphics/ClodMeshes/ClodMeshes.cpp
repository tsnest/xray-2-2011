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

#include "ClodMeshes.h"

WM4_WINDOW_APPLICATION(ClodMeshes);

//----------------------------------------------------------------------------
ClodMeshes::ClodMeshes ()
    :
    WindowApplication3("ClodMeshes",0,0,640,480,
        ColorRGBA(0.9f,0.9f,0.9f,1.0f))
{
}
//----------------------------------------------------------------------------
bool ClodMeshes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // center-and-fit scene in the view frustum
    m_spkScene->UpdateGS();
    m_spkScene->GetChild(0)->Local.SetTranslate(
        -m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10000.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(1.0f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ClodMeshes::OnTerminate ()
{
#ifdef USE_CLOD_MESH
    m_aspkClod[0] = 0;
    m_aspkClod[1] = 0;
    m_spkActive = 0;
#endif
    m_spkScene = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ClodMeshes::OnIdle ()
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
#ifdef USE_CLOD_MESH
        DrawTriangleQuantity(128,GetHeight()-8,ColorRGBA::BLACK);
#endif
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ClodMeshes::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
#ifdef USE_CLOD_MESH
    case '0':
        m_spkActive = m_aspkClod[0];
        return true;
    case '1':
        m_spkActive = m_aspkClod[1];
        return true;

    case '-':  // decrease triangle quantity
    case '_':
        if (m_spkActive->TargetRecord() < m_spkActive->GetRecordQuantity()-1)
        {
            m_spkActive->TargetRecord()++;
        }
        m_spkActive->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;

    case '+':  // increase triangle quantity
    case '=':
        if (m_spkActive->TargetRecord() > 0)
        {
            m_spkActive->TargetRecord()--;
        }
        m_spkActive->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;

    case 'c':  // toggle between 1/4 and full resolution
    case 'C':
        if (m_spkActive->TargetRecord() != 0)
        {
            m_spkActive->TargetRecord() = 0;
        }
        else
        {
            m_spkActive->TargetRecord() =
                3*(m_spkActive->GetRecordQuantity()-1)/4;
        }
        m_spkActive->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
#endif

    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"ClodMeshes.wmof");
        return true;
     }

    return false;
}
//----------------------------------------------------------------------------
void ClodMeshes::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    Node* pkTrnNode = WM4_NEW Node;
    m_spkScene->AttachChild(pkTrnNode);
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Load the face model.
    const char* acPath = System::GetPath("FacePN.wmof",System::SM_READ);
    Stream kIStream;
    kIStream.Load(acPath);
    TriMeshPtr spkMesh = StaticCast<TriMesh>(kIStream.GetObjectAt(0));

    // Remove the normals and add texture coordinates.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVB0 = spkMesh->VBuffer;
    int iVQuantity = pkVB0->GetVertexQuantity();
    VertexBuffer* pkVB1 = WM4_NEW VertexBuffer(kAttr,iVQuantity);

    float fXMin = Mathf::MAX_REAL, fXMax = -Mathf::MAX_REAL;
    float fYMin = Mathf::MAX_REAL, fYMax = -Mathf::MAX_REAL;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB1->Position3(i) = pkVB0->Position3(i);

        float fX = pkVB0->Position3(i).X();
        float fY = pkVB0->Position3(i).Z();
        pkVB1->TCoord2(0,i).X() = fX;
        pkVB1->TCoord2(0,i).Y() = fY;
        if (fX < fXMin) { fXMin = fX; }
        if (fX > fXMax) { fXMax = fX; }
        if (fY < fYMin) { fYMin = fY; }
        if (fY > fYMax) { fYMax = fY; }
    }
    float fXMult = 1.0f/(fXMax - fXMin);
    float fYMult = 1.0f/(fYMax - fYMin);
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB1->TCoord2(0,i).X() = (pkVB1->TCoord2(0,i).X() - fXMin)*fXMult;
        pkVB1->TCoord2(0,i).Y() = (pkVB1->TCoord2(0,i).Y() - fYMin)*fYMult;
    }
    spkMesh->VBuffer = pkVB1;

    // Create a texture for the face.  Use the generated texture coordinates.
    TextureEffect* pkEffect = WM4_NEW TextureEffect("Magician");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::CLAMP_EDGE);
    pkTexture->SetWrapType(1,Texture::CLAMP_EDGE);

#ifdef USE_CLOD_MESH
    // Create the collapse records to be shared by two CLOD meshes.
    int iCQuantity = 0;
    CollapseRecord* akCRecord = 0;
    CreateClodMesh kCCM(spkMesh->VBuffer,spkMesh->IBuffer,iCQuantity,
        akCRecord);
    CollapseRecordArray* pkRecords = WM4_NEW CollapseRecordArray(iCQuantity,
        akCRecord);

    m_aspkClod[0] = WM4_NEW ClodMesh(spkMesh->VBuffer,spkMesh->IBuffer,
        pkRecords);
    m_aspkClod[0]->Local = spkMesh->Local;
    m_aspkClod[0]->Local.SetTranslate(spkMesh->Local.GetTranslate() -
        150.0f*Vector3f::UNIT_X);
    m_aspkClod[0]->AttachEffect(pkEffect);
    pkTrnNode->AttachChild(m_aspkClod[0]);

    m_aspkClod[1] = WM4_NEW ClodMesh(spkMesh->VBuffer,spkMesh->IBuffer,
        pkRecords);
    m_aspkClod[1]->Local = spkMesh->Local;
    m_aspkClod[1]->Local.SetTranslate(spkMesh->Local.GetTranslate() +
        150.0f*Vector3f::UNIT_X - 100.0f*Vector3f::UNIT_Y);
    m_aspkClod[1]->AttachEffect(pkEffect);
    pkTrnNode->AttachChild(m_aspkClod[1]);

    m_spkActive = m_aspkClod[0];
#else
    TriMesh* pkFace = WM4_NEW TriMesh(spkMesh->VBuffer,spkMesh->IBuffer);
    pkFace->Local = spkMesh->Local;
    pkFace->Local.SetTranslate(spkMesh->Local.GetTranslate() -
        150.0f*Vector3f::UNIT_X);
    pkFace->AttachEffect(pkEffect);
    pkTrnNode->AttachChild(pkFace);

    pkFace = WM4_NEW TriMesh(spkMesh->VBuffer,spkMesh->IBuffer);
    pkFace->Local = spkMesh->Local;
    pkFace->Local.SetTranslate(spkMesh->Local.GetTranslate() +
        150.0f*Vector3f::UNIT_X - 100.0f*Vector3f::UNIT_Y);
    pkFace->AttachEffect(pkEffect);
    pkTrnNode->AttachChild(pkFace);
#endif
}
//----------------------------------------------------------------------------
#ifdef USE_CLOD_MESH
void ClodMeshes::DrawTriangleQuantity (int iX, int iY,
    const ColorRGBA& rkColor)
{
    const size_t uiSize = 256;
    char acMessage[uiSize];
    System::Sprintf(acMessage,uiSize,"triangles0: %4d   triangles1: %4d",
        m_aspkClod[0]->GetTriangleQuantity(),
        m_aspkClod[1]->GetTriangleQuantity());
    m_pkRenderer->Draw(iX,iY,rkColor,acMessage);
}
//----------------------------------------------------------------------------
void ClodMeshes::MoveForward ()
{
    WindowApplication3::MoveForward();
    UpdateClods();
}
//----------------------------------------------------------------------------
void ClodMeshes::MoveBackward ()
{
    WindowApplication3::MoveBackward();
    UpdateClods();
}
//----------------------------------------------------------------------------
void ClodMeshes::UpdateClods ()
{
    // Adjust the triangle quantities for the clod meshes.  The distance along
    // the camera direction controls the triangle quantities.  A nonlinear
    // drop-off is used.
    for (int i = 0; i < 2; i++)
    {
        Vector3f kDiff = m_aspkClod[i]->WorldBound->GetCenter() -
            m_spkCamera->GetLocation();
        float fDepth = kDiff.Dot(m_spkCamera->GetDVector());
        if (fDepth <= m_spkCamera->GetDMin())
        {
            m_aspkClod[i]->TargetRecord() = 0;
        }
        else if (fDepth >= m_spkCamera->GetDMax())
        {
            m_aspkClod[i]->TargetRecord() =
                m_aspkClod[i]->GetRecordQuantity() - 1;
        }
        else
        {
            float fN = m_spkCamera->GetDMin();
            float fF = m_spkCamera->GetDMax();
            float fRatio = Mathf::Pow((fDepth - fN)/(fF - fN),0.5f);
            m_aspkClod[i]->TargetRecord() = 
                (int)((m_aspkClod[i]->GetRecordQuantity() - 1)*fRatio);
        }
    }
}
#endif
//----------------------------------------------------------------------------
