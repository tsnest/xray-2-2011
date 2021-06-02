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
// Version: 4.0.3 (2009/01/23)

#include "MorphControllers.h"
#include <fstream>
using namespace std;

WM4_WINDOW_APPLICATION(MorphControllers);

//----------------------------------------------------------------------------
MorphControllers::MorphControllers ()
    :
    WindowApplication3("MorphControllers",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
    System::InsertDirectory("Data");

    m_dBaseTime = -1.0;
    m_dCurrTime = -1.0;
    m_iXPick = -1;
    m_iYPick = -1;
    System::Sprintf(m_acPickMessage,PICK_MESSAGE_SIZE,"no selection");
    m_bPickPending = false;
}
//----------------------------------------------------------------------------
bool MorphControllers::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,0.0f,0.0f);
    Vector3f kCDir(1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // The face is force culled, so no point in computing the visible set
    // at this time.
    m_kCuller.SetCamera(m_spkCamera);

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void MorphControllers::OnTerminate ()
{
    m_kPicker.Records.clear();
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkFace = 0;
    m_spkIBuffer = 0;
    m_spkMaterial = 0;
    m_spkLight = 0;
    for (int i = 0; i < TARGET_QUANTITY; i++)
    {
        m_aspkTarget[i] = 0;
    }

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void MorphControllers::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    MoveObject();

    // update the morph animation
    if (m_dBaseTime == -1.0)
    {
        m_dBaseTime = System::GetTime();
        m_dCurrTime = m_dBaseTime;
    }
    else
    {
        m_dCurrTime = System::GetTime();
    }
    m_spkScene->UpdateGS(m_dCurrTime - m_dBaseTime);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        Vector3f kPos, kDir;  // the pick ray
        bool bPickingAttempted = false;

        int iWidth = m_pkRenderer->GetWidth();
        int iHeight = m_pkRenderer->GetHeight();
        int iLPort, iRPort, iTPort, iBPort;

        // Draw the 5 targets in the upper half of the screen.
        for (int i = 0; i < TARGET_QUANTITY; i++)
        {
            //float fLPort = 0.2f*i, fRPort = 0.2f*(i+1);
            //m_spkCamera->SetViewport(fLPort,fRPort,1.0f,0.8f);
            iLPort = (int)(0.2f*i*iWidth);
            iRPort = (int)(0.2f*(i+1)*iWidth);
            iTPort = iHeight;
            iBPort = (int)(0.8f*iHeight);
            m_spkCamera->SetViewport(iLPort,iRPort,iTPort,iBPort,iWidth,
                iHeight);
            m_aspkTarget[i]->Culling = Spatial::CULL_DYNAMIC;
            m_kCuller.ComputeVisibleSet(m_spkScene);
            m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
            m_aspkTarget[i]->Culling = Spatial::CULL_ALWAYS;

            if (m_bPickPending)
            {
                if (m_spkCamera->GetPickRay(m_iXPick,m_iYPick,GetWidth(),
                    GetHeight(),kPos,kDir))
                {
                    m_bPickPending = false;
                    bPickingAttempted = true;
                    m_kPicker.Execute(m_spkScene,kPos,kDir,0.0f,
                        Mathf::MAX_REAL);
                    if (m_kPicker.Records.size() > 0)
                    {
                        System::Sprintf(m_acPickMessage,PICK_MESSAGE_SIZE,
                            "target %d",i);
                    }
                }
            }
        }

        // Draw the animated face in the lower half of the screen.
        //m_spkCamera->SetViewport(0.1f,0.9f,0.8f,0.0f);
        iLPort = (int)(0.1f*iWidth);
        iRPort = (int)(0.9f*iWidth);
        iTPort = (int)(0.8f*iHeight);
        iBPort = 0;
        m_spkCamera->SetViewport(iLPort,iRPort,iTPort,iBPort,iWidth,
            iHeight);
        m_spkFace->Culling = Spatial::CULL_DYNAMIC;
        m_kCuller.ComputeVisibleSet(m_spkScene);
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        m_spkFace->Culling = Spatial::CULL_ALWAYS;

        if (m_bPickPending)
        {
            if (m_spkCamera->GetPickRay(m_iXPick,m_iYPick,GetWidth(),
                GetHeight(),kPos,kDir))
            {
                m_bPickPending = false;
                bPickingAttempted = true;
                m_kPicker.Execute(m_spkScene,kPos,kDir,0.0f,Mathf::MAX_REAL);
                if (m_kPicker.Records.size() > 0)
                {
                    System::Sprintf(m_acPickMessage,PICK_MESSAGE_SIZE,
                        "morphed");
                }
            }
        }

        if (bPickingAttempted && m_kPicker.Records.size() == 0)
        {
            System::Sprintf(m_acPickMessage,PICK_MESSAGE_SIZE,
                "no selection");
        }

        //m_spkCamera->SetViewport(0.0f,1.0f,1.0f,0.0f);
        m_spkCamera->SetViewport(0,iWidth,iHeight,0,iWidth,iHeight);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->Draw(320,GetHeight()-8,ColorRGBA::WHITE,
            m_acPickMessage);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool MorphControllers::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    static int iFace = -1;

    switch (ucKey)
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case ' ':
        iFace++;
        if (iFace == 0)
        {
            m_spkFace->Culling = Spatial::CULL_ALWAYS;
            m_aspkTarget[0]->Culling = Spatial::CULL_DYNAMIC;
        }
        else if (iFace == TARGET_QUANTITY)
        {
            m_aspkTarget[iFace-1]->Culling = Spatial::CULL_ALWAYS;
            m_spkFace->Culling = Spatial::CULL_DYNAMIC;
            iFace = -1;
        }
        else
        {
            m_aspkTarget[iFace-1]->Culling = Spatial::CULL_ALWAYS;
            m_aspkTarget[iFace]->Culling = Spatial::CULL_DYNAMIC;
        }
        return true;

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"MorphControllers.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool MorphControllers::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if (iState != MOUSE_DOWN || iButton != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    m_iXPick = iX;
    m_iYPick = iY;
    m_bPickPending = true;
    return true;
}
//----------------------------------------------------------------------------
void MorphControllers::CreateScene ()
{
    // Set up scene.  The transformations are selected using prior knowledge
    // of the input data sets.
    m_spkScene = WM4_NEW Node;
    m_spkScene->Local.SetRotate(Matrix3f(Vector3f::UNIT_Z,Vector3f::UNIT_X,
        Vector3f::UNIT_Y,true));
    m_spkScene->Local.SetTranslate(Vector3f(250.0f,0.0f,0.0f));

    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // create a material
    m_spkMaterial = WM4_NEW MaterialState;
    m_spkMaterial->Emissive = ColorRGB(0.0f,0.0f,0.0f);
    m_spkMaterial->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    m_spkMaterial->Diffuse = ColorRGB(0.99607f,0.83920f,0.67059f);
    m_spkMaterial->Specular = ColorRGB(0.8f,0.8f,0.8f);
    m_spkMaterial->Alpha = 1.0f;
    m_spkMaterial->Shininess = 0.0f;

    // create a directional light
    m_spkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    m_spkLight->Ambient = ColorRGB(0.25f,0.25f,0.25f);
    m_spkLight->Diffuse = ColorRGB::WHITE;
    m_spkLight->Specular = ColorRGB::BLACK;
    m_spkLight->DVector = Vector3f::UNIT_X;

    LoadTargets();
    CreateFace();
    CreateController();

    // Attach the face to the scene.  Also attach the targets so we can
    // display them above the animated face.
    m_spkFace->Culling = Spatial::CULL_ALWAYS;
    m_spkScene->AttachChild(m_spkFace);
    for (int i = 0; i < TARGET_QUANTITY; i++)
    {
        m_aspkTarget[i]->Culling = Spatial::CULL_ALWAYS;
        m_spkScene->AttachChild(m_aspkTarget[i]);
    }
}
//----------------------------------------------------------------------------
void MorphControllers::LoadTargets ()
{
    // Create triangle meshes for the targets just for the purposes of this
    // application.  The targets will be displayed directly above the animated
    // face.

    // load triangle indices
    const char* acPath = System::GetPath("FaceIndices.raw",System::SM_READ);
    FILE* pkIFile = System::Fopen(acPath,"rb");
    assert(pkIFile);

    int i, iTQuantity;
    System::Read4le(pkIFile,1,&iTQuantity);

    m_spkIBuffer = WM4_NEW IndexBuffer(3*iTQuantity);
    int* piIndex = m_spkIBuffer->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        System::Read4le(pkIFile,3,piIndex);
        piIndex += 3;
    }

    System::Fclose(pkIFile);

    // Load the targets.  Create triangle meshes for each of the targets just
    // for the purposes of this application.  Normally target0 and the
    // differences between the other targets and target0 are stored only in
    // the controller.
    for (int j = 0; j < TARGET_QUANTITY; j++)
    {
        const size_t uiSize = 64;
        char acFilename[uiSize];
        System::Sprintf(acFilename,uiSize,"FaceTarget%d.raw",j);
        acPath = System::GetPath(acFilename,System::SM_READ);
        pkIFile = System::Fopen(acPath,"rb");
        assert(pkIFile);

        int iVQuantity;
        System::Read4le(pkIFile,1,&iVQuantity);

        Attributes kAttr;
        kAttr.SetPositionChannels(3);
        kAttr.SetNormalChannels(3);
        VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
        for (int i = 0; i < iVQuantity; i++)
        {
            System::Read4le(pkIFile,3,pkVBuffer->PositionTuple(i));
        }

        System::Fclose(pkIFile);

        m_aspkTarget[j] = WM4_NEW TriMesh(pkVBuffer,m_spkIBuffer);
        m_aspkTarget[j]->GenerateNormals();
        m_aspkTarget[j]->AttachGlobalState(m_spkMaterial);
        m_aspkTarget[j]->AttachLight(m_spkLight);
        m_aspkTarget[j]->SetName(acFilename);
    }
}
//----------------------------------------------------------------------------
void MorphControllers::CreateFace ()
{
    // This is the triangle mesh that will be controlled.  Make a copy of the
    // vertices, own your normals, but share the connectivity, material
    // effect, and lighting effect.

    m_spkFace = WM4_NEW TriMesh(
        WM4_NEW VertexBuffer(m_aspkTarget[0]->VBuffer),
        m_aspkTarget[0]->IBuffer);
    m_spkFace->GenerateNormals();
    m_spkFace->AttachGlobalState(m_spkMaterial);

#if 1
    m_spkFace->AttachLight(m_spkLight);
    m_spkFace->SetName("original");
#else
    // For regenerating the face WMOF whenever the engine streaming changes.
    Stream kStream;
    kStream.Insert(m_spkFace);
    kStream.Save("FacePN.wmof");
#endif
}
//----------------------------------------------------------------------------
void MorphControllers::CreateController ()
{
    // Create the controller.  The parameters set here are known information
    // about the model.
    int iVQuantity = m_spkFace->VBuffer->GetVertexQuantity();
    MorphController* pkCtrl = WM4_NEW MorphController(iVQuantity,
        TARGET_QUANTITY,KEY_QUANTITY);
    pkCtrl->Repeat = Controller::RT_WRAP;
    pkCtrl->MinTime = 0.0;
    pkCtrl->MaxTime = 16000.0;
    pkCtrl->Phase = 0.0;
    pkCtrl->Frequency = 4800.0;

    // create the targets (target 0 is base, others are difference from base)
    Vector3f* akBase = WM4_NEW Vector3f[iVQuantity];
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        akBase[i] = m_spkFace->VBuffer->Position3(i);
    }
    Vector3fArray* pkBase = WM4_NEW Vector3fArray(iVQuantity,akBase);
    pkCtrl->SetVertices(0,pkBase);
    int j;
    for (j = 1; j < TARGET_QUANTITY; j++)
    {
        Vector3f* akDelta = WM4_NEW Vector3f[iVQuantity];
        for (i = 0; i < iVQuantity; i++)
        {
            akDelta[i] = m_aspkTarget[j]->VBuffer->Position3(i) - akBase[i];
        }
        Vector3fArray* pkDelta = WM4_NEW Vector3fArray(iVQuantity,akDelta);
        pkCtrl->SetVertices(j,pkDelta);
    }

    // load the keys
    float* afTimes = WM4_NEW float[KEY_QUANTITY];
    FloatArrayPtr spkTimes = WM4_NEW FloatArray(KEY_QUANTITY,afTimes);
    pkCtrl->SetTimes(spkTimes);
    for (j = 0; j < KEY_QUANTITY; j++)
    {
        const size_t uiSize = 64;
        char acFilename[uiSize];
        System::Sprintf(acFilename,uiSize,"FaceKey%d.raw",j);
        const char* acPath = System::GetPath(acFilename,System::SM_READ);
        FILE* pkIFile = System::Fopen(acPath,"rb");
        assert(pkIFile);

        int iTargetQuantity;
        System::Read4le(pkIFile,1,&iTargetQuantity);

        float* afWeight = WM4_NEW float[iTargetQuantity-1];
        FloatArrayPtr spkWeight = WM4_NEW FloatArray(iTargetQuantity-1,
            afWeight);
        pkCtrl->SetWeights(j,spkWeight);
        System::Read4le(pkIFile,1,&afTimes[j]);

        float fW0;
        System::Read4le(pkIFile,1,&fW0);  // discard, not needed
        System::Read4le(pkIFile,iTargetQuantity-1,afWeight);

        System::Fclose(pkIFile);
    }

    m_spkFace->AttachController(pkCtrl);
}
//----------------------------------------------------------------------------
