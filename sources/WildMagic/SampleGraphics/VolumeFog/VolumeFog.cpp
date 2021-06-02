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

#include "VolumeFog.h"
#include "VolumeFogEffect.h"

WM4_WINDOW_APPLICATION(VolumeFog);

#define USE_FOG

//----------------------------------------------------------------------------
VolumeFog::VolumeFog ()
    :
    WindowApplication3("VolumeFog",0,0,640,480,
        ColorRGBA(0.0f,0.5f,0.75f,1.0f))
{
    System::InsertDirectory("Data");
}
//----------------------------------------------------------------------------
bool VolumeFog::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(60.0f,m_iWidth/(float)m_iHeight,0.01f,100.0f);
    Vector3f kCLoc(0.0f,-9.0f,1.5f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    kCDir.Normalize();
    kCUp.Normalize();
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.005f,0.002f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void VolumeFog::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkHeightField = 0;
    m_spkHeightImage = 0;
    m_spkBackPoly = 0;
    m_spkScreenCamera = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void VolumeFog::OnIdle ()
{
    MeasureTime();

    bool bFogNeedsUpdate = false;
    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
        bFogNeedsUpdate = true;
    }

    if (MoveObject())
    {
        m_spkScene->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
        bFogNeedsUpdate = true;
    }

    if (bFogNeedsUpdate)
    {
        UpdateFog();
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->SetCamera(m_spkScreenCamera);
        m_pkRenderer->Draw(m_spkBackPoly);
        m_pkRenderer->SetCamera(m_spkCamera);
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool VolumeFog::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        TestStreaming(m_spkScene,128,128,640,480,"VolumeFog.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void VolumeFog::CreateScene ()
{
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    m_spkScreenCamera = WM4_NEW Camera;
    m_spkScreenCamera->Perspective = false;
    m_spkScreenCamera->SetFrustum(0.0f,1.0f,0.0f,1.0f,0.0f,1.0f);
    m_spkScreenCamera->SetFrame(Vector3f::ZERO,Vector3f::UNIT_Z,
        Vector3f::UNIT_Y,Vector3f::UNIT_X);

    // The background is a textured screen polygon (z = 1).
    Attributes kBackAttr;
    kBackAttr.SetPositionChannels(3);
    kBackAttr.SetTCoordChannels(0,2);
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kBackAttr,4);
    pkVBuffer->Position3(0) = Vector3f(0.0f,0.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,0.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(3) = Vector3f(0.0f,1.0f,1.0f);
    pkVBuffer->TCoord2(0,0) = Vector2f(0.0f,0.0f);
    pkVBuffer->TCoord2(0,1) = Vector2f(1.0f,0.0f);
    pkVBuffer->TCoord2(0,2) = Vector2f(1.0f,1.0f);
    pkVBuffer->TCoord2(0,3) = Vector2f(0.0f,1.0f);
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    m_spkBackPoly = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkBackPoly->AttachEffect(WM4_NEW TextureEffect("Blueback"));
    m_spkBackPoly->UpdateGS();

    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    // Begin with a flat height field.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
#ifdef USE_FOG
    kAttr.SetColorChannels(0,4);
#endif
    kAttr.SetTCoordChannels(0,2);
    StandardMesh kSM(kAttr);
    m_spkHeightField = kSM.Rectangle(64,64,8.0f,8.0f);
    m_spkScene->AttachChild(m_spkHeightField);

    // Set the heights based on a precomputed height field.  Also create a
    // texture image to go with the height field.
    m_spkHeightImage = Image::Load("VolumeFogHeightField");
    unsigned char* pucData = m_spkHeightImage->GetData();
    VertexBuffer* pkVB = m_spkHeightField->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        unsigned char ucValue = *pucData;
        float fHeight = 3.0f*((float)ucValue)/255.0f +
            0.05f*Mathf::SymmetricRandom();

        *pucData++ = (unsigned char)Mathf::IntervalRandom(32.0f,64.0f);
        *pucData++ = 3*(128 - ucValue/2)/4;
        *pucData++ = 0;

        pkVB->Position3(i).Z() = fHeight;

#ifdef USE_FOG
        // The fog color is white.  The alpha channel is filled in by the
        // function UpdateFog().
        pkVB->Color4(0,i) = ColorRGBA(1.0f,1.0f,1.0f,0.0f);
#endif
    }

    UpdateFog();

#ifdef USE_FOG
    VolumeFogEffect* pkEffect =
        WM4_NEW VolumeFogEffect("VolumeFogHeightField");
#else
    TextureEffect* pkEffect = WM4_NEW TextureEffect("VolumeFogHeightField");
#endif
    m_spkHeightField->AttachEffect(pkEffect);
}
//----------------------------------------------------------------------------
void VolumeFog::UpdateFog ()
{
#ifdef USE_FOG
    // Compute the fog factors based on the intersection of a slab bounded
    // by z = zbot and z = ztop with rays emanating from the camera location
    // to each vertex in the height field.
    const float fZBot = 0.0f;  // slab bottom
    const float fZTop = 0.5f;  // slab top
    const float fFogConstant = 8.0f;  // fog = length/(length+fogconstant)
    float fTBot, fTTop, fInvDirZ, fLength;

    // Having the camera below the height field is not natural, so we just
    // leave the fog values the way they are.
    Vector3f kLoc = m_spkCamera->GetLocation();
    if (kLoc.Z() <= fZBot)
    {
        return;
    }

    VertexBuffer* pkVB = m_spkHeightField->VBuffer;
    int iVQuantity = pkVB->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        // The ray is E+t*D, where D = V-E with E the eye point and V the
        // vertex.  The ray reaches the vertex at t = 1.

        Vector3f kDir = pkVB->Position3(i) - kLoc;
        float fTVmE = kDir.Normalize();
        float fAlpha;

        if (kLoc.Z() >= fZTop)
        {
            if (kDir.Z() >= -Mathf::ZERO_TOLERANCE)
            {
                // The ray never intersects the slab, so no fog at vertex.
                fAlpha = 0.0f;
            }
            else
            {
                // The ray intersects the slab.  Compute the length of the
                // intersection and map it to a fog factor.
                fInvDirZ = 1.0f/kDir.Z();
                fTTop = (fZTop - kLoc.Z())*fInvDirZ;
                if (fTTop < fTVmE)
                {
                    // The eye is above the slab; the vertex is in or below.
                    fTBot = (fZBot - kLoc.Z())*fInvDirZ;
                    if (fTBot < fTVmE)
                    {
                        // The vertex is below the slab.  Clamp to bottom.
                        fLength = fTBot - fTTop;
                        fAlpha = fLength/(fLength + fFogConstant);
                    }
                    else
                    {
                        // The vertex is inside the slab.
                        fLength = fTVmE - fTTop;
                        fAlpha = fLength/(fLength + fFogConstant);
                    }
                }
                else
                {
                    // The eye and vertex are above the slab, so no fog.
                    fAlpha = 0.0f;
                }
            }
        }
        else  // kLoc.Z() in (z0,z1)
        {
            if (kDir.Z() >= Mathf::ZERO_TOLERANCE)
            {
                // The ray intersects the top of the slab.
                fInvDirZ = 1.0f/kDir.Z();
                fTTop = (fZTop - kLoc.Z())*fInvDirZ;
                if (fTTop < fTVmE)
                {
                    // The vertex is above the slab.
                    fAlpha = fTTop/(fTTop + fFogConstant);
                }
                else
                {
                    // The vertex is on or inside the slab.
                    fAlpha = fTVmE/(fTVmE + fFogConstant);
                }
            }
            else if (kDir.Z() <= -Mathf::ZERO_TOLERANCE)
            {
                // The ray intersects the bottom of the slab.
                fInvDirZ = 1.0f/kDir.Z();
                fTBot = (fZBot - kLoc.Z())*fInvDirZ;
                if (fTBot < fTVmE)
                {
                    // The vertex is below the slab.
                    fAlpha = fTBot/(fTBot + fFogConstant);
                }
                else
                {
                    // The vertex is on or inside the slab.
                    fAlpha = fTVmE/(fTVmE + fFogConstant);
                }
            }
            else
            {
                // The ray is parallel to the slab, so both eye and vertex
                // are inside the slab.
                fAlpha = fTVmE/(fTVmE + fFogConstant);
            }
        }

        pkVB->Color4(0,i).A() = fAlpha;
    }
#endif
}
//----------------------------------------------------------------------------
