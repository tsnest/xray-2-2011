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

#include "SkinnedBiped.h"

WM4_WINDOW_APPLICATION(SkinnedBiped);

//----------------------------------------------------------------------------
SkinnedBiped::SkinnedBiped ()
    :
    WindowApplication3("SkinnedBiped",0,0,640,480,
        ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
    System::InsertDirectory("Data");

    m_dAnimTime = 0.0;
    m_dAnimTimeDelta = 0.01;
}
//----------------------------------------------------------------------------
bool SkinnedBiped::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // camera and view frustum
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(80.0f,0.0f,23.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // The biped has materials assigned to its triangle meshes, so they need
    // lighting.
    Light* pkLight = WM4_NEW Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGB(0.5f,0.5f,0.5f);
    pkLight->Diffuse = pkLight->Ambient;
    pkLight->Specular = ColorRGB::BLACK;
    pkLight->DVector = kCDir;
    m_spkScene->AttachLight(pkLight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void SkinnedBiped::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SkinnedBiped::OnIdle ()
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
bool SkinnedBiped::OnKeyDown (unsigned char ucKey, int iX, int iY)
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

    case 'g':
        m_dAnimTime += m_dAnimTimeDelta;
        m_spkScene->UpdateGS(m_dAnimTime);
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;
    case 'G':
        m_dAnimTime = 0.0;
        m_spkScene->UpdateGS(m_dAnimTime);
        m_kCuller.ComputeVisibleSet(m_spkScene);
        return true;

    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"SkinnedBiped.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SkinnedBiped::CreateScene ()
{
    Node* pkBiped = GetNode("Biped");
    Node* pkPelvis = GetNode("Pelvis");
    Node* pkSpine = GetNode("Spine");
    Node* pkSpine1 = GetNode("Spine1");
    Node* pkSpine2 = GetNode("Spine2");
    Node* pkSpine3 = GetNode("Spine3");
    Node* pkNeck = GetNode("Neck");
    Node* pkHead = GetNode("Head");
    Node* pkLClavicle = GetNode("L_Clavicle");
    Node* pkLUpperArm = GetNode("L_UpperArm");
    Node* pkLForeArm = GetNode("L_Forearm");
    Node* pkLHand = GetNode("L_Hand");
    Node* pkRClavicle = GetNode("R_Clavicle");
    Node* pkRUpperArm = GetNode("R_UpperArm");
    Node* pkRForeArm = GetNode("R_Forearm");
    Node* pkRHand = GetNode("R_Hand");
    Node* pkLThigh = GetNode("L_Thigh");
    Node* pkLCalf = GetNode("L_Calf");
    Node* pkLFoot = GetNode("L_Foot");
    Node* pkLToe = GetNode("L_Toe");
    Node* pkRThigh = GetNode("R_Thigh");
    Node* pkRCalf = GetNode("R_Calf");
    Node* pkRFoot = GetNode("R_Foot");
    Node* pkRToe = GetNode("R_Toe");

    pkBiped->AttachChild(pkPelvis);
        pkPelvis->AttachChild(pkSpine);
            pkSpine->AttachChild(pkSpine1);
                pkSpine1->AttachChild(pkSpine2);
                    pkSpine2->AttachChild(pkSpine3);
                        pkSpine3->AttachChild(pkNeck);
                            pkNeck->AttachChild(pkHead);
                                // pkHead->AttachChild(pkHair);
                            pkNeck->AttachChild(pkLClavicle);
                                pkLClavicle->AttachChild(pkLUpperArm);
                                    pkLUpperArm->AttachChild(pkLForeArm);
                                        pkLForeArm->AttachChild(pkLHand);
                                    // pkLUpperArm->AttachChild(pkLArm);
                            pkNeck->AttachChild(pkRClavicle);
                                pkRClavicle->AttachChild(pkRUpperArm);
                                    pkRUpperArm->AttachChild(pkRForeArm);
                                        pkRForeArm->AttachChild(pkRHand);
                                    // pkRUpperArm->AttachChild(pkRArm);
                        // pkSpine3->AttachChild(pkFace);
        pkPelvis->AttachChild(pkLThigh);
            pkLThigh->AttachChild(pkLCalf);
                pkLCalf->AttachChild(pkLFoot);
                    pkLFoot->AttachChild(pkLToe);
                // pkLCalf->AttachChild(pkLShoe);
            // pkLThigh->AttachChild(pkLLeg);
            // pkLThigh->AttachChild(pkLAnkle);
        pkPelvis->AttachChild(pkRThigh);
            pkRThigh->AttachChild(pkRCalf);
                pkRCalf->AttachChild(pkRFoot);
                    pkRFoot->AttachChild(pkRToe);
                // pkRCalf->AttachChild(pkRShoe);
            // pkRThigh->AttachChild(pkRLeg);
            // pkRThigh->AttachChild(pkRAnkle);
        // pkPelvis->AttachChild(pkShirt);
        // pkPelvis->AttachChild(pkPants);

    TriMesh* pkHair = GetMesh("Hair",pkBiped);
    TriMesh* pkLArm = GetMesh("L_Arm",pkBiped);
    TriMesh* pkRArm = GetMesh("R_Arm",pkBiped);
    TriMesh* pkFace = GetMesh("Face",pkBiped);
    TriMesh* pkLShoe = GetMesh("L_Shoe",pkBiped);
    TriMesh* pkLLeg = GetMesh("L_Leg",pkBiped);
    TriMesh* pkLAnkle = GetMesh("L_Ankle",pkBiped);
    TriMesh* pkRShoe = GetMesh("R_Shoe",pkBiped);
    TriMesh* pkRLeg = GetMesh("R_Leg",pkBiped);
    TriMesh* pkRAnkle = GetMesh("R_Ankle",pkBiped);
    TriMesh* pkShirt = GetMesh("Shirt",pkBiped);
    TriMesh* pkPants = GetMesh("Pants",pkBiped);

    pkHead->AttachChild(pkHair);
    pkLUpperArm->AttachChild(pkLArm);
    pkRUpperArm->AttachChild(pkRArm);
    pkSpine3->AttachChild(pkFace);
    pkLCalf->AttachChild(pkLShoe);
    pkLThigh->AttachChild(pkLLeg);
    pkLThigh->AttachChild(pkLAnkle);
    pkRCalf->AttachChild(pkRShoe);
    pkRThigh->AttachChild(pkRLeg);
    pkRThigh->AttachChild(pkRAnkle);
    pkPelvis->AttachChild(pkShirt);
    pkPelvis->AttachChild(pkPants);

    m_spkScene = WM4_NEW Node;
    m_spkScene->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        0.25f*Mathf::PI));
    m_spkScene->AttachChild(pkBiped);
    m_spkScene->UpdateGS();

#if 0
    // For regenerating the biped WMOF whenever engine streaming changes.
    Stream kStream;
    kStream.Insert(m_spkScene);
    kStream.Save("SkinnedBipedPN.wmof");
#endif
}
//----------------------------------------------------------------------------
Node* SkinnedBiped::GetNode (const char* acName)
{
    Node* pkNode = WM4_NEW Node;
    pkNode->SetName(acName);

    // load the keyframe controller
    const size_t uiSize = 256;
    char acFilename[uiSize];
    System::Sprintf(acFilename,uiSize,"%s.keyf.raw",acName);
    const char* acPath = System::GetPath(acFilename,System::SM_READ);
    FILE* pkIFile = System::Fopen(acPath,"rb");
    assert(pkIFile);

    KeyframeController* pkCtrl = WM4_NEW KeyframeController;

    int iRepeat;
    float fMinTime, fMaxTime, fPhase, fFrequency;
    System::Read4le(pkIFile,1,&iRepeat);
    System::Read4le(pkIFile,1,&fMinTime);
    System::Read4le(pkIFile,1,&fMaxTime);
    System::Read4le(pkIFile,1,&fPhase);
    System::Read4le(pkIFile,1,&fFrequency);

    pkCtrl->Repeat = (Controller::RepeatType)iRepeat;
    pkCtrl->MinTime = fMinTime;
    pkCtrl->MaxTime = fMaxTime;
    pkCtrl->Phase = fPhase;
    pkCtrl->Frequency = fFrequency;

    int iTQ, iRQ, iSQ;
    System::Read4le(pkIFile,1,&iTQ);
    System::Read4le(pkIFile,1,&iRQ);
    System::Read4le(pkIFile,1,&iSQ);

    if (iTQ > 0)
    {
        float* afTTime = WM4_NEW float[iTQ];
        Vector3f* akTData = WM4_NEW Vector3f[iTQ];
        System::Read4le(pkIFile,iTQ,afTTime);
        System::Read4le(pkIFile,3*iTQ,(float*)akTData);
        pkCtrl->TranslationTimes = WM4_NEW FloatArray(iTQ,afTTime);
        pkCtrl->TranslationData = WM4_NEW Vector3fArray(iTQ,akTData);
    }
    else
    {
        Vector3f kTrn;
        System::Read4le(pkIFile,3,(float*)kTrn);
        pkNode->Local.SetTranslate(kTrn);
    }

    if (iRQ > 0)
    {
        float* afRTime = WM4_NEW float[iRQ];
        Quaternionf* akRData = WM4_NEW Quaternionf[iRQ];

        System::Read4le(pkIFile,iRQ,afRTime);
        System::Read4le(pkIFile,4*iRQ,(float*)akRData);
        pkCtrl->RotationTimes = WM4_NEW FloatArray(iRQ,afRTime);
        pkCtrl->RotationData = WM4_NEW QuaternionfArray(iRQ,akRData);
    }
    else
    {
        Matrix3f kRot;
        System::Read4le(pkIFile,9,(float*)kRot);
        pkNode->Local.SetRotate(kRot);
    }

    if (iSQ > 0)
    {
        float* afSTime = WM4_NEW float[iSQ];
        float* afSData = WM4_NEW float[iSQ];

        System::Read4le(pkIFile,iSQ,afSTime);
        System::Read4le(pkIFile,iSQ,afSData);
        pkCtrl->ScaleTimes = WM4_NEW FloatArray(iSQ,afSTime);
        pkCtrl->ScaleData = WM4_NEW FloatArray(iSQ,afSData);
    }
    else
    {
        float fScale;
        System::Read4le(pkIFile,1,&fScale);
        pkNode->Local.SetUniformScale(fScale);
    }

    System::Fclose(pkIFile);

    pkNode->AttachController(pkCtrl);
    return pkNode;
}
//----------------------------------------------------------------------------
TriMesh* SkinnedBiped::GetMesh (const char* acName, Node* pkBiped)
{
    // load the triangle indices and material
    const size_t uiSize = 256;
    char acFilename[uiSize];
    System::Sprintf(acFilename,uiSize,"%s.triangle.raw",acName);
    const char* acPath = System::GetPath(acFilename,System::SM_READ);
    FILE* pkIFile = System::Fopen(acPath,"rb");
    assert(pkIFile);

    int iTQ;
    System::Read4le(pkIFile,1,&iTQ);

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(3*iTQ);
    int* aiIndex = pkIBuffer->GetData();
    System::Read4le(pkIFile,3*iTQ,aiIndex);

    ColorRGB kEmissive, kAmbient, kDiffuse, kSpecular;
    System::Read4le(pkIFile,3,(float*)kEmissive);
    System::Read4le(pkIFile,3,(float*)kAmbient);
    System::Read4le(pkIFile,3,(float*)kDiffuse);
    System::Read4le(pkIFile,3,(float*)kSpecular);

    System::Fclose(pkIFile);

    MaterialState* pkMS = WM4_NEW MaterialState;
    pkMS->Emissive.R() = kEmissive.R();
    pkMS->Emissive.G() = kEmissive.G();
    pkMS->Emissive.B() = kEmissive.B();
    pkMS->Ambient.R() = kAmbient.R();
    pkMS->Ambient.G() = kAmbient.G();
    pkMS->Ambient.B() = kAmbient.B();
    pkMS->Diffuse.R() = kDiffuse.R();
    pkMS->Diffuse.G() = kDiffuse.G();
    pkMS->Diffuse.B() = kDiffuse.B();
    pkMS->Specular.R() = kSpecular.R();
    pkMS->Specular.G() = kSpecular.G();
    pkMS->Specular.B() = kSpecular.B();

    // load the skin controller
    System::Sprintf(acFilename,uiSize,"%s.skin.raw",acName);
    acPath = System::GetPath(acFilename,System::SM_READ);
    pkIFile = System::Fopen(acPath,"rb");
    assert(pkIFile);

    int iRepeat;
    float fMinTime, fMaxTime, fPhase, fFrequency;
    System::Read4le(pkIFile,1,&iRepeat);
    System::Read4le(pkIFile,1,&fMinTime);
    System::Read4le(pkIFile,1,&fMaxTime);
    System::Read4le(pkIFile,1,&fPhase);
    System::Read4le(pkIFile,1,&fFrequency);

    int iVertexQuantity, iBoneQuantity;
    System::Read4le(pkIFile,1,&iVertexQuantity);
    System::Read4le(pkIFile,1,&iBoneQuantity);

    Node** apkBones = WM4_NEW Node*[iBoneQuantity];
    int i;
    for (i = 0; i < iBoneQuantity; i++)
    {
        int iLength;
        System::Read4le(pkIFile,1,&iLength);

        char* acBoneName = WM4_NEW char[iLength+1];
        System::Read1(pkIFile,iLength,acBoneName);
        acBoneName[iLength] = 0;

        apkBones[i] = (Node*)pkBiped->GetObjectByName(acBoneName);
        assert(apkBones[i]);
        WM4_DELETE[] acBoneName;
    }

    float** aafWeight;
    Allocate<float>(iBoneQuantity,iVertexQuantity,aafWeight);
    Vector3f** aakOffset;
    Allocate<Vector3f>(iBoneQuantity,iVertexQuantity,aakOffset);

    for (int j = 0; j < iVertexQuantity; j++)
    {
        for (i = 0; i < iBoneQuantity; i++)
        {
            System::Read4le(pkIFile,1,&aafWeight[j][i]);
            System::Read4le(pkIFile,3,&aakOffset[j][i]);
        }
    }

    System::Fclose(pkIFile);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVertexQuantity);

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    pkMesh->GenerateNormals();

    SkinController* pkCtrl = WM4_NEW SkinController(iVertexQuantity,
        iBoneQuantity,apkBones,aafWeight,aakOffset);
    pkCtrl->Repeat = (Controller::RepeatType)iRepeat;
    pkCtrl->MinTime = fMinTime;
    pkCtrl->MaxTime = fMaxTime;
    pkCtrl->Phase = fPhase;
    pkCtrl->Frequency = fFrequency;

    pkMesh->SetName(acName);
    pkMesh->AttachController(pkCtrl);
    pkMesh->AttachGlobalState(pkMS);
    return pkMesh;
}
//----------------------------------------------------------------------------
