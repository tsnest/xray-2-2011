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

#include "Polylines.h"

WM4_WINDOW_APPLICATION(Polylines);

//----------------------------------------------------------------------------
Polylines::Polylines ()
    :
    WindowApplication3("Polylines",0,0,640,480,
        ColorRGBA(0.75f,0.75f,0.75f,1.0f))
{
}
//----------------------------------------------------------------------------
bool Polylines::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(4.0f,0.0f,0.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

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
void Polylines::OnTerminate ()
{
    m_spkScene = 0;
    m_spkPolyline = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Polylines::OnIdle ()
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
bool Polylines::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'c':
    case 'C':
        m_spkPolyline->SetClosed(!m_spkPolyline->GetClosed());
        return true;
    case 'k':
    case 'K':
        m_spkPolyline->SetContiguous(!m_spkPolyline->GetContiguous());
        break;
    case '+':
    case '=':
    {
        int iVQuantity = m_spkPolyline->VBuffer->GetVertexQuantity();
        int iAQuantity = m_spkPolyline->GetActiveQuantity();
        if (iAQuantity < iVQuantity-2)
        {
            m_spkPolyline->SetActiveQuantity(iAQuantity+2);
        }
        return true;
    }
    case '-':
    case '_':
    {
        int iAQuantity = m_spkPolyline->GetActiveQuantity();
        if (iAQuantity >= 4)
        {
            m_spkPolyline->SetActiveQuantity(iAQuantity-2);
        }
        return true;
    }
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"Polylines.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Polylines::CreateScene ()
{
    m_spkScene = WM4_NEW Node;

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    int iVQuantity = 128;
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    for (int i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i) = Vector3f(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(),Mathf::SymmetricRandom());
        pkVBuffer->Color3(0,i) = ColorRGB(Mathf::UnitRandom(),
            Mathf::UnitRandom(),Mathf::UnitRandom());
    }

    bool bClosed = false;
    bool bContiguous = true;
    m_spkPolyline = WM4_NEW Polyline(pkVBuffer,bClosed,bContiguous);
    m_spkPolyline->AttachEffect(WM4_NEW VertexColor3Effect);

    m_spkScene->AttachChild(m_spkPolyline);
}
//----------------------------------------------------------------------------
