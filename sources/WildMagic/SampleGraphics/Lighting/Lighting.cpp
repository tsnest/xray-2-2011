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
// Version: 4.0.4 (2007/09/24)

#include "Lighting.h"

WM4_WINDOW_APPLICATION(Lighting);

//----------------------------------------------------------------------------
Lighting::Lighting ()
    :
    WindowApplication3("Lighting",0,0,512,512,
        ColorRGBA(0.0f,0.25f,0.75f,1.0f))
{
    m_iAQuantity = 0;
    m_iDQuantity = 0;
    m_iPQuantity = 0;
    m_iSQuantity = 0;
    m_iActiveLight = -1;
    m_iLightQuantity = 0;
    for (int i = 0; i < 8; i++)
    {
        m_acCaption[i] = '.';
    }
    m_acCaption[8] = 0;
}
//----------------------------------------------------------------------------
bool Lighting::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,1.0f,0.1f,100.0f);
    Vector3f kCLoc = Vector3f(8.0f,0.0f,4.0f);
    Vector3f kCDir = -kCLoc;  // lookat origin
    kCDir.Normalize();
    Vector3f kCUp(kCDir.Z(),0,-kCDir.X());
    Vector3f kCRight = Vector3f::UNIT_Y;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);

    return true;
}
//----------------------------------------------------------------------------
void Lighting::OnTerminate ()
{
    m_spkScene = 0;
    m_spkPlane = 0;
    m_spkSphere = 0;
    m_spkWireframe = 0;

    for (int i = 0; i < 8; i++)
    {
        m_aspkALight[i] = 0;
        m_aspkDLight[i] = 0;
        m_aspkPLight[i] = 0;
        m_aspkSLight[i] = 0;
    }

    m_spkDefaultEffect = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Lighting::OnIdle ()
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
        m_pkRenderer->Draw(8,16,ColorRGBA::WHITE,m_acCaption);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Lighting::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    if (ucKey == 'w' || ucKey == 'W')
    {
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    }

    switch (ucKey)
    {
    case 'A':  // remove an ambient light
        if (m_iAQuantity > 0)
        {
            m_iAQuantity--;
            m_iLightQuantity--;
            UpdateEffects();
        }
        return true;
    case 'a':  // add an ambient light
        if (m_iLightQuantity < 8)
        {
            m_iAQuantity++;
            m_iLightQuantity++;
            UpdateEffects();
        }
        return true;
    case 'D':  // remove a directional light
        if (m_iDQuantity > 0)
        {
            m_iDQuantity--;
            m_iLightQuantity--;
            UpdateEffects();
        }
        return true;
    case 'd':  // add a directional light
        if (m_iLightQuantity < 8)
        {
            m_iDQuantity++;
            m_iLightQuantity++;
            UpdateEffects();
        }
        return true;
    case 'P':  // remove a point light
        if (m_iPQuantity > 0)
        {
            m_iPQuantity--;
            m_iLightQuantity--;
            UpdateEffects();
        }
        return true;
    case 'p':  // add a point light
        if (m_iLightQuantity < 8)
        {
            m_iPQuantity++;
            m_iLightQuantity++;
            UpdateEffects();
        }
        return true;
    case 'S':  // remove a spot light
        if (m_iSQuantity > 0)
        {
            m_iSQuantity--;
            m_iLightQuantity--;
            UpdateEffects();
        }
        return true;
    case 's':  // add a spot light
        if (m_iLightQuantity < 8)
        {
            m_iSQuantity++;
            m_iLightQuantity++;
            UpdateEffects();
        }
        return true;
    case 'v':
    case 'V':
        TestStreaming(m_spkScene,128,128,640,480,"Lighting.wmof");
        return true;
    }

    if (m_iLightQuantity == 0)
    {
        return false;
    }

    if ('0' <= ucKey && ucKey <= '0' + m_iLightQuantity - 1)
    {
        m_iActiveLight = (int)ucKey - (int)'0';
        return true;
    }

    if (m_iActiveLight < 0 || m_iActiveLight >= m_iLightQuantity)
    {
        return false;
    }

    Light* pkLight = m_spkScene->GetLight(m_iActiveLight);

    switch (ucKey)
    {
    case 'i':
        pkLight->Intensity -= 0.125f;
        if (pkLight->Intensity < 0.0f)
        {
            pkLight->Intensity = 0.0f;
        }
        return true;
    case 'I':
        pkLight->Intensity += 0.125f;
        return true;
    case 'c':
        pkLight->Angle -= 0.1f;
        if (pkLight->Angle < 0.0f)
        {
            pkLight->Angle = 0.0f;
        }
        pkLight->SetAngle(pkLight->Angle);
        return true;
    case 'C':
        pkLight->Angle += 0.1f;
        if (pkLight->Angle > Mathf::HALF_PI)
        {
            pkLight->Angle = Mathf::HALF_PI;
        }
        pkLight->SetAngle(pkLight->Angle);
        return true;
    case 'e':
        pkLight->Exponent *= 0.5f;
        return true;
    case 'E':
        pkLight->Exponent *= 2.0f;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Lighting::CreateScene ()
{
    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    CreateLights();
    CreatePlane();
    CreateSphere();

    m_spkDefaultEffect = WM4_NEW DefaultShaderEffect;
    m_spkPlane->AttachEffect(m_spkDefaultEffect);
    m_spkSphere->AttachEffect(m_spkDefaultEffect);
}
//----------------------------------------------------------------------------
void Lighting::CreateLights ()
{
    int i;
    for (i = 0; i < 8; i++)
    {
        m_aspkALight[i] = WM4_NEW Light(Light::LT_AMBIENT);
        m_aspkDLight[i] = WM4_NEW Light(Light::LT_DIRECTIONAL);
        m_aspkPLight[i] = WM4_NEW Light(Light::LT_POINT);
        m_aspkSLight[i] = WM4_NEW Light(Light::LT_SPOT);
    }

    // ambient lights
    float fValue = 0.75f;
    m_aspkALight[0]->Ambient = ColorRGB(fValue,fValue,fValue);
    m_aspkALight[1]->Ambient = ColorRGB(fValue,0.0f,  0.0f);
    m_aspkALight[2]->Ambient = ColorRGB(0.0f,  fValue,0.0f);
    m_aspkALight[3]->Ambient = ColorRGB(0.0f,  0.0f,  fValue);
    m_aspkALight[4]->Ambient = ColorRGB(0.0f,  fValue,fValue);
    m_aspkALight[5]->Ambient = ColorRGB(fValue,0.0f,  fValue);
    m_aspkALight[6]->Ambient = ColorRGB(fValue,fValue,0.0f);
    m_aspkALight[7]->Ambient = ColorRGB(fValue,fValue,fValue);

    // directional lights
    m_aspkDLight[0]->Ambient = ColorRGB(fValue,fValue,fValue);
    m_aspkDLight[1]->Ambient = ColorRGB(fValue,0.0f,  0.0f);
    m_aspkDLight[2]->Ambient = ColorRGB(0.0f,  fValue,0.0f);
    m_aspkDLight[3]->Ambient = ColorRGB(0.0f,  0.0f,  fValue);
    m_aspkDLight[4]->Ambient = ColorRGB(0.0f,  fValue,fValue);
    m_aspkDLight[5]->Ambient = ColorRGB(fValue,0.0f,  fValue);
    m_aspkDLight[6]->Ambient = ColorRGB(fValue,fValue,0.0f);
    m_aspkDLight[7]->Ambient = ColorRGB(fValue,fValue,fValue);
    fValue = -Mathf::Sqrt(1.0f/3.0f);
    m_aspkDLight[0]->DVector = Vector3f(+fValue,+fValue,+fValue);
    m_aspkDLight[1]->DVector = Vector3f(+fValue,+fValue,-fValue);
    m_aspkDLight[2]->DVector = Vector3f(+fValue,-fValue,+fValue);
    m_aspkDLight[3]->DVector = Vector3f(+fValue,-fValue,-fValue);
    m_aspkDLight[4]->DVector = Vector3f(-fValue,+fValue,+fValue);
    m_aspkDLight[5]->DVector = Vector3f(-fValue,+fValue,-fValue);
    m_aspkDLight[6]->DVector = Vector3f(-fValue,-fValue,+fValue);
    m_aspkDLight[7]->DVector = Vector3f(-fValue,-fValue,-fValue);
    for (i = 0; i < 8; i++)
    {
        m_aspkDLight[i]->Diffuse = ColorRGB::WHITE;
        m_aspkDLight[i]->Specular = ColorRGB::WHITE;
    }

    // point lights
    fValue = 0.1f;
    m_aspkPLight[0]->Ambient = ColorRGB(fValue,fValue,fValue);
    m_aspkPLight[1]->Ambient = ColorRGB(fValue,0.0f,  0.0f);
    m_aspkPLight[2]->Ambient = ColorRGB(0.0f,  fValue,0.0f);
    m_aspkPLight[3]->Ambient = ColorRGB(0.0f,  0.0f,  fValue);
    m_aspkPLight[4]->Ambient = ColorRGB(0.0f,  fValue,fValue);
    m_aspkPLight[5]->Ambient = ColorRGB(fValue,0.0f,  fValue);
    m_aspkPLight[6]->Ambient = ColorRGB(fValue,fValue,0.0f);
    m_aspkPLight[7]->Ambient = ColorRGB(fValue,fValue,fValue);
    fValue = 4.0f;
    m_aspkPLight[0]->Position = Vector3f(+fValue,+fValue,+fValue);
    m_aspkPLight[1]->Position = Vector3f(+fValue,+fValue,-fValue);
    m_aspkPLight[2]->Position = Vector3f(+fValue,-fValue,+fValue);
    m_aspkPLight[3]->Position = Vector3f(+fValue,-fValue,-fValue);
    m_aspkPLight[4]->Position = Vector3f(-fValue,+fValue,+fValue);
    m_aspkPLight[5]->Position = Vector3f(-fValue,+fValue,-fValue);
    m_aspkPLight[6]->Position = Vector3f(-fValue,-fValue,+fValue);
    m_aspkPLight[7]->Position = Vector3f(-fValue,-fValue,-fValue);
    for (i = 0; i < 8; i++)
    {
        m_aspkPLight[i]->Diffuse = ColorRGB::WHITE;
        m_aspkPLight[i]->Specular = ColorRGB::WHITE;
    }

    // spot lights
    fValue = 0.1f;
    m_aspkSLight[0]->Ambient = ColorRGB(fValue,fValue,fValue);
    m_aspkSLight[1]->Ambient = ColorRGB(fValue,0.0f,  0.0f);
    m_aspkSLight[2]->Ambient = ColorRGB(0.0f,  fValue,0.0f);
    m_aspkSLight[3]->Ambient = ColorRGB(0.0f,  0.0f,  fValue);
    m_aspkSLight[4]->Ambient = ColorRGB(0.0f,  fValue,fValue);
    m_aspkSLight[5]->Ambient = ColorRGB(fValue,0.0f,  fValue);
    m_aspkSLight[6]->Ambient = ColorRGB(fValue,fValue,0.0f);
    m_aspkSLight[7]->Ambient = ColorRGB(fValue,fValue,fValue);
    fValue = 4.0f;
    m_aspkSLight[0]->Position = Vector3f(+fValue,+fValue,+fValue);
    m_aspkSLight[1]->Position = Vector3f(+fValue,+fValue,-fValue);
    m_aspkSLight[2]->Position = Vector3f(+fValue,-fValue,+fValue);
    m_aspkSLight[3]->Position = Vector3f(+fValue,-fValue,-fValue);
    m_aspkSLight[4]->Position = Vector3f(-fValue,+fValue,+fValue);
    m_aspkSLight[5]->Position = Vector3f(-fValue,+fValue,-fValue);
    m_aspkSLight[6]->Position = Vector3f(-fValue,-fValue,+fValue);
    m_aspkSLight[7]->Position = Vector3f(-fValue,-fValue,-fValue);
    fValue = -Mathf::Sqrt(1.0f/3.0f);
    m_aspkSLight[0]->DVector = Vector3f(+fValue,+fValue,+fValue);
    m_aspkSLight[1]->DVector = Vector3f(+fValue,+fValue,-fValue);
    m_aspkSLight[2]->DVector = Vector3f(+fValue,-fValue,+fValue);
    m_aspkSLight[3]->DVector = Vector3f(+fValue,-fValue,-fValue);
    m_aspkSLight[4]->DVector = Vector3f(-fValue,+fValue,+fValue);
    m_aspkSLight[5]->DVector = Vector3f(-fValue,+fValue,-fValue);
    m_aspkSLight[6]->DVector = Vector3f(-fValue,-fValue,+fValue);
    m_aspkSLight[7]->DVector = Vector3f(-fValue,-fValue,-fValue);
    for (i = 0; i < 8; i++)
    {
        m_aspkSLight[i]->Diffuse = ColorRGB::WHITE;
        m_aspkSLight[i]->Specular = ColorRGB::WHITE;
        m_aspkSLight[i]->Exponent = 1.0f;
        m_aspkSLight[i]->SetAngle(0.125f*Mathf::PI);
    }
}
//----------------------------------------------------------------------------
void Lighting::CreatePlane ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    StandardMesh kSM(kAttr);
    m_spkPlane = kSM.Rectangle(128,128,16.0f,16.0f);

    // polished copper
    MaterialState* pkMaterial = WM4_NEW MaterialState;
    pkMaterial->Emissive = ColorRGB::BLACK;
    pkMaterial->Ambient = ColorRGB(0.2295f,0.08825f,0.0275f);
    pkMaterial->Diffuse = ColorRGB(0.5508f,0.2118f,0.066f);
    pkMaterial->Specular = ColorRGB(0.580594f,0.223257f,0.0695701f);
    pkMaterial->Shininess = 51.2f;
    m_spkPlane->AttachGlobalState(pkMaterial);

    m_spkScene->AttachChild(m_spkPlane);
}
//----------------------------------------------------------------------------
void Lighting::CreateSphere ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetNormalChannels(3);
    StandardMesh kSM(kAttr);
    m_spkSphere = kSM.Sphere(64,64,1.0f);
    m_spkSphere->Local.SetTranslate(Vector3f(0.0f,0.0f,1.0f));

    // polished gold
    MaterialState* pkMaterial = WM4_NEW MaterialState;
    pkMaterial->Emissive = ColorRGB::BLACK;
    pkMaterial->Ambient = ColorRGB(0.24725f,0.2245f,0.0645f);
    pkMaterial->Diffuse = ColorRGB(0.34615f,0.3143f,0.0903f);
    pkMaterial->Specular = ColorRGB(0.797357f,0.723991f,0.208006f);
    pkMaterial->Shininess = 83.2f;
    m_spkSphere->AttachGlobalState(pkMaterial);

    m_spkScene->AttachChild(m_spkSphere);
}
//----------------------------------------------------------------------------
void Lighting::UpdateEffects ()
{
    int i;
    for (i = 0; i < 8; i++)
    {
        m_acCaption[i] = '.';
    }

    // Release all the resources and detach the effects and lights.
    m_spkPlane->VBuffer->Release();
    m_spkSphere->VBuffer->Release();
    m_spkPlane->DetachAllEffects();
    m_spkSphere->DetachAllEffects();
    m_spkScene->DetachAllLights();
    m_spkScene->UpdateRS();

    if (m_iLightQuantity > 0)
    {
        char* pcCaption = m_acCaption;
        for (i = 0; i < m_iAQuantity; i++)
        {
            m_spkScene->AttachLight(m_aspkALight[i]);
            *pcCaption++ = 'a';
        }
        for (i = 0; i < m_iDQuantity; i++)
        {
            m_spkScene->AttachLight(m_aspkDLight[i]);
            *pcCaption++ = 'd';
        }
        for (i = 0; i < m_iPQuantity; i++)
        {
            m_spkScene->AttachLight(m_aspkPLight[i]);
            *pcCaption++ = 'p';
        }
        for (i = 0; i < m_iSQuantity; i++)
        {
            m_spkScene->AttachLight(m_aspkSLight[i]);
            *pcCaption++ = 's';
        }
    }
    else
    {
        m_spkPlane->AttachEffect(m_spkDefaultEffect);
        m_spkSphere->AttachEffect(m_spkDefaultEffect);
    }

    m_spkScene->UpdateRS();
    m_iActiveLight = -1;
}
//----------------------------------------------------------------------------
