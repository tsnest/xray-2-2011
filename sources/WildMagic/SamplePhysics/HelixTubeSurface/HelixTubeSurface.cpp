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

#include "HelixTubeSurface.h"

WM4_WINDOW_APPLICATION(HelixTubeSurface);

//----------------------------------------------------------------------------
HelixTubeSurface::HelixTubeSurface ()
    :
    WindowApplication3("HelixTubeSurface",0,0,640,480,
        ColorRGBA(0.9f,0.9f,0.9f,1.0f))
{
    m_fDeltaTime = 0.01f;
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.  The coordinate frame will be set by the MoveCamera
    // member function.
    m_spkCamera->SetFrustum(-0.0055f,0.0055f,-0.004125f,0.004125f,
        0.01f,10.0f);

    m_spkScene = WM4_NEW Node;
    m_spkWireframe = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframe);

    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    m_pkCurve = CreateCurve();
    Vector2f kUVMin(0.0f,0.0f), kUVMax(1.0f,32.0f);
    TubeSurface* pkTube = WM4_NEW TubeSurface(m_pkCurve,Radial,false,
        Vector3f::UNIT_Z,256,32,kAttr,false,true,&kUVMin,&kUVMax);

    TextureEffect* pkEffect = WM4_NEW TextureEffect("Grating");
    Texture* pkTexture = pkEffect->GetPTexture(0,0);
    pkTexture->SetWrapType(0,Texture::REPEAT);
    pkTexture->SetWrapType(1,Texture::REPEAT);
    pkTube->AttachEffect(pkEffect);

    m_spkScene->AttachChild(pkTube);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // The initial culling of scene.  The visible set is constructed in the
    // MoveCamera call.
    m_kCuller.SetCamera(m_spkCamera);

    MoveCamera();
    return true;
}
//----------------------------------------------------------------------------
void HelixTubeSurface::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void HelixTubeSurface::OnIdle ()
{
    MeasureTime();

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
bool HelixTubeSurface::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 'w':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case '+':
    case '=':
        m_fDeltaTime *= 2.0f;
        return true;
    case '-':
    case '_':
        m_fDeltaTime *= 0.5f;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::OnSpecialKeyDown (int iKey, int, int)
{
    if (iKey == KEY_UP_ARROW)
    {
        m_fCurveTime += m_fDeltaTime;
        if (m_fCurveTime > m_fMaxCurveTime)
        {
            m_fCurveTime -= m_fCurvePeriod;
        }
        MoveCamera();
        return true;
    }

    if (iKey == KEY_DOWN_ARROW)
    {
        m_fCurveTime -= m_fDeltaTime;
        if (m_fCurveTime < m_fMinCurveTime)
        {
            m_fCurveTime += m_fCurvePeriod;
        }
        MoveCamera();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::MoveCamera ()
{
    Vector3f kP = m_pkCurve->GetPosition(m_fCurveTime);
    Vector3f kT = m_pkCurve->GetTangent(m_fCurveTime);
    Vector3f kB = kT.UnitCross(Vector3f::UNIT_Z);
    Vector3f kN = kB.UnitCross(kT);
    m_spkCamera->SetFrame(kP,kT,kN,kB);
    m_kCuller.ComputeVisibleSet(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
MultipleCurve3f* HelixTubeSurface::CreateCurve ()
{
    // sample points on a looped helix (first and last point must match)
    const float fFourPi = 4.0f*Mathf::PI;
    int iSegments = 32;
    int iSegmentsP1 = iSegments + 1;
    float* afTime = WM4_NEW float[iSegmentsP1];
    Vector3f* akPoint = WM4_NEW Vector3f[iSegmentsP1];
    int i;
    for (i = 0; i <= iSegmentsP1/2; i++)
    {
        afTime[i] = i*fFourPi/iSegmentsP1;
        akPoint[i].X() = Mathf::Cos(afTime[i]);
        akPoint[i].Y() = Mathf::Sin(afTime[i]);
        akPoint[i].Z() = afTime[i];
    }

    for (i = iSegmentsP1/2 + 1; i < iSegments; i++)
    {
        afTime[i] = i*fFourPi/iSegments;
        akPoint[i].X() = 2.0f - Mathf::Cos(afTime[i]);
        akPoint[i].Y() = Mathf::Sin(afTime[i]);
        akPoint[i].Z() = fFourPi - afTime[i];
    }

    afTime[iSegments] = fFourPi;
    akPoint[iSegments] = akPoint[0];

    // save min and max times
    m_fMinCurveTime = 0.0f;
    m_fMaxCurveTime = fFourPi;
    m_fCurvePeriod = m_fMaxCurveTime - m_fMinCurveTime;
    m_fCurveTime = m_fMinCurveTime;

    // create a closed cubic curve containing the sample points
    NaturalSpline3f* pkCurve = WM4_NEW NaturalSpline3f(
        NaturalSpline3f::BT_CLOSED,iSegments,afTime,akPoint);

    return pkCurve;
}
//----------------------------------------------------------------------------
float HelixTubeSurface::Radial (float)
{
    return 0.0625f;
}
//----------------------------------------------------------------------------
