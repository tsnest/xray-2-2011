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
// Version: 4.0.7 (2007/09/23)

#include "Wm4Dx9RendererPCH.h"
#include "Wm4Dx9Renderer.h"
#include "Wm4Camera.h"
using namespace Wm4;

HRESULT Dx9Renderer::ms_hResult = 0;

//----------------------------------------------------------------------------
Dx9Renderer::Dx9Renderer (HWND hWnd, FrameBuffer::FormatType eFormat,
    FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
    FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iWidth, int iHeight)
    :
    Renderer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,iWidth,iHeight)
{
    m_pqMain = Direct3DCreate9(D3D_SDK_VERSION);
    assert(m_pqMain);

    m_kPresent.BackBufferWidth = iWidth;
    m_kPresent.BackBufferHeight = iHeight;
    m_kPresent.BackBufferFormat = D3DFMT_A8R8G8B8;
    m_kPresent.BackBufferCount = 1;
    m_kPresent.hDeviceWindow = hWnd;
    m_kPresent.Windowed = true;
    m_kPresent.Flags = 0;
    m_kPresent.FullScreen_RefreshRateInHz = 0;
    m_kPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Request depth and stencil buffers.  The parameters are not independent
    // for DirectX.  TO DO.  For now, just grab a 24-bit depth buffer and an
    // 8-bit stencil buffer.
    m_kPresent.EnableAutoDepthStencil = TRUE;
    m_kPresent.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Request multisampling.
    m_kPresent.SwapEffect = D3DSWAPEFFECT_FLIP; 
    m_kPresent.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_kPresent.MultiSampleQuality = 0;
    if (m_eMultisampling == FrameBuffer::MT_SAMPLING_4)
    {
        if (SUCCEEDED(m_pqMain->CheckDeviceMultiSampleType(
            D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_A8R8G8B8,FALSE,
            D3DMULTISAMPLE_4_SAMPLES,NULL)))
        {
            m_kPresent.SwapEffect = D3DSWAPEFFECT_DISCARD; 
            m_kPresent.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
        }
    }
    else if (m_eMultisampling == FrameBuffer::MT_SAMPLING_2)
    {
        if (SUCCEEDED(m_pqMain->CheckDeviceMultiSampleType(
            D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_A8R8G8B8,FALSE,
            D3DMULTISAMPLE_2_SAMPLES,NULL)))
        {
            m_kPresent.SwapEffect = D3DSWAPEFFECT_DISCARD; 
            m_kPresent.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
        }
    }

    ms_hResult = m_pqMain->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
        &m_kPresent,&m_pqDevice);
    assert(SUCCEEDED(ms_hResult));
    
    // Create the default font.
    m_iFontID = LoadFont("Arial",18,false,false);
    assert(m_iFontID == 0);

    // Query the device for its capabilities.
    D3DCAPS9 kDeviceCaps;
    ms_hResult = m_pqMain->GetDeviceCaps(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        &kDeviceCaps);
    assert(SUCCEEDED(ms_hResult));

    ms_iMaxColors = 2;
    ms_iMaxTCoords = kDeviceCaps.MaxSimultaneousTextures;
    ms_iMaxVShaderImages = ms_iMaxTCoords;
    ms_iMaxPShaderImages = ms_iMaxTCoords;
    if (ms_iMaxPShaderImages > 0)
    {
        m_apkActiveSamplers =
            WM4_NEW SamplerInformation*[ms_iMaxPShaderImages];

        memset(m_apkActiveSamplers,0,ms_iMaxPShaderImages*
            sizeof(SamplerInformation*));
    }

    ms_iMaxUserClipPlanes = (int)kDeviceCaps.MaxUserClipPlanes;

    ms_iMaxLights = (int)kDeviceCaps.MaxActiveLights;
    assert(ms_iMaxLights > 0);
    m_aspkLight = WM4_NEW ObjectPtr[ms_iMaxLights];

    if (kDeviceCaps.StencilCaps & (D3DSTENCILOP_KEEP | D3DSTENCILOP_REPLACE))
    {
        ms_iMaxStencilIndices = 8;
    }

    ms_fMaxAnisotropy = (float)kDeviceCaps.MaxAnisotropy;

    // Initialize global render state to default settings.
    SetGlobalState(GlobalState::Default);

    // Turn off lighting (DX defaults to lighting on).  TO DO:  Do we need
    // this for the shader-based engine?
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
    assert(SUCCEEDED(ms_hResult));

    m_bCursorVisible = true;
    m_bDeviceLost = false;
    m_bBeginSceneActive = false;
    m_pkLockedFace = 0;
}
//----------------------------------------------------------------------------
Dx9Renderer::~Dx9Renderer ()
{
    // release all fonts
    for (int i = 0; i < (int)m_kFontArray.size(); i++)
    {
        UnloadFont(i);
    }

    // clean up cursor
    if (!m_bCursorVisible)
    {
        ShowCursor(true);
    }
}
//----------------------------------------------------------------------------
void Dx9Renderer::ToggleFullscreen ()
{
    Renderer::ToggleFullscreen();
    m_kPresent.Windowed = !m_kPresent.Windowed;

    if (m_kPresent.Windowed)
    {
        m_kPresent.FullScreen_RefreshRateInHz = 0;
    }
    else
    {
        GetWindowRect(m_kPresent.hDeviceWindow,&m_rcWindow);
        m_kPresent.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    }

    OnDeviceLost();
    ResetDevice();  

    if (m_kPresent.Windowed)
    {
        SetWindowPos(m_kPresent.hDeviceWindow, 
            NULL, 
            m_rcWindow.left,
            m_rcWindow.top, 
            m_rcWindow.right - m_rcWindow.left,
            m_rcWindow.bottom - m_rcWindow.top,
            SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW);
    }
}
//----------------------------------------------------------------------------
bool Dx9Renderer::BeginScene ()
{
    ms_hResult = m_pqDevice->TestCooperativeLevel();
    
    switch (ms_hResult)
    {
    case D3DERR_DEVICELOST:
        if (!m_bDeviceLost)
        {
            m_bDeviceLost = true;
            OnDeviceLost();
        }
        return false;
    case D3DERR_DEVICENOTRESET:
        ResetDevice();
        m_bDeviceLost = false;
        break;
    }

    ms_hResult = m_pqDevice->BeginScene();
    assert(SUCCEEDED(ms_hResult));

    m_bBeginSceneActive = true;
    return true;
}
//----------------------------------------------------------------------------
void Dx9Renderer::EndScene ()
{
    ms_hResult = m_pqDevice->EndScene();
    assert(SUCCEEDED(ms_hResult));
    m_bBeginSceneActive = false;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnDeviceLost ()
{
    m_kFontArray[0]->OnLostDevice();
}
//----------------------------------------------------------------------------
void Dx9Renderer::ResetDevice ()
{
    ms_hResult = m_pqDevice->Reset(&m_kPresent);
    assert(SUCCEEDED(ms_hResult));
    m_kFontArray[0]->OnResetDevice();

    OnFrustumChange();
    OnViewportChange();
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearBackBuffer ()
{
    DWORD dwClearColor = D3DCOLOR_COLORVALUE(m_kClearColor.R(),
        m_kClearColor.G(),m_kClearColor.B(),m_kClearColor.A());

    ms_hResult = m_pqDevice->Clear(0,0,D3DCLEAR_TARGET,dwClearColor,1.0f,0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearZBuffer ()
{
    ms_hResult = m_pqDevice->Clear(0,0,D3DCLEAR_ZBUFFER,0,m_fClearDepth,0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearStencilBuffer ()
{
    ms_hResult = m_pqDevice->Clear(0,0,D3DCLEAR_STENCIL,0,1.0f,
        (DWORD)m_uiClearStencil);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearBuffers ()
{
    DWORD dwClearColor = D3DCOLOR_COLORVALUE(m_kClearColor.R(),
        m_kClearColor.G(),m_kClearColor.B(),m_kClearColor.A());

    ms_hResult = m_pqDevice->Clear(0,0,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        dwClearColor,m_fClearDepth,(DWORD)m_uiClearStencil);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearBackBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    DWORD dwClearColor = D3DCOLOR_COLORVALUE(m_kClearColor.R(),
        m_kClearColor.G(),m_kClearColor.B(),m_kClearColor.A());

    ms_hResult = m_pqDevice->Clear(1,&kRect,D3DCLEAR_TARGET,dwClearColor,
        1.0f,0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearZBuffer (int iXPos, int iYPos, int iWidth, int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    ms_hResult = m_pqDevice->Clear(1,&kRect,D3DCLEAR_ZBUFFER,0,
        m_fClearDepth,0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    ms_hResult = m_pqDevice->Clear(1,&kRect,D3DCLEAR_STENCIL,0,1.0f,
        (DWORD)m_uiClearStencil);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::ClearBuffers (int iXPos, int iYPos, int iWidth, int iHeight)
{
    D3DRECT kRect;
    kRect.x1 = (long)iXPos;
    kRect.y1 = (long)iYPos;
    kRect.x2 = (long)(iXPos + iWidth - 1);
    kRect.y2 = (long)(iYPos + iHeight - 1);

    DWORD dwClearColor = D3DCOLOR_COLORVALUE(m_kClearColor.R(),
        m_kClearColor.G(),m_kClearColor.B(),m_kClearColor.A());

    ms_hResult = m_pqDevice->Clear(1,&kRect,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        dwClearColor,m_fClearDepth,(DWORD)m_uiClearStencil);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::DisplayBackBuffer ()
{
    ms_hResult = m_pqDevice->Present(0,0,0,0);
    if (ms_hResult != D3DERR_DEVICELOST)
    {
        assert(SUCCEEDED(ms_hResult));
    }
} 
//----------------------------------------------------------------------------
void Dx9Renderer::SetColorMask (bool bAllowRed, bool bAllowGreen,
    bool bAllowBlue, bool bAllowAlpha)
{
    Renderer::SetColorMask(bAllowRed,bAllowGreen,bAllowBlue,bAllowAlpha);

    DWORD dwMask = 0;

    if (bAllowRed)
    {
        dwMask |= D3DCOLORWRITEENABLE_RED;
    }

    if (bAllowGreen)
    {
        dwMask |= D3DCOLORWRITEENABLE_GREEN;
    }

    if (bAllowBlue)
    {
        dwMask |= D3DCOLORWRITEENABLE_BLUE;
    }

    if (bAllowAlpha)
    {
        dwMask |= D3DCOLORWRITEENABLE_ALPHA;
    }

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORWRITEENABLE,dwMask);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::EnableUserClipPlane (int i, const Plane3f& rkPlane)
{
    Vector4f kMPlane(rkPlane.Normal[0],rkPlane.Normal[1],rkPlane.Normal[2],
        -rkPlane.Constant);
    Matrix4f kWorldViewMatrix = m_kWorldMatrix*m_kViewMatrix;
    Vector4f kCPlane = kWorldViewMatrix.Inverse()*kMPlane;

    DWORD dwClipPlanesEnabled;
    ms_hResult = m_pqDevice->GetRenderState(D3DRS_CLIPPLANEENABLE,
        &dwClipPlanesEnabled);
    assert(SUCCEEDED(ms_hResult));

    ms_hResult = m_pqDevice->SetClipPlane((DWORD)i,(float*)kCPlane);
    assert(SUCCEEDED(ms_hResult));

    dwClipPlanesEnabled |= (1 << i);
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,
        dwClipPlanesEnabled);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::DisableUserClipPlane (int i)
{
    DWORD dwClipPlanesEnabled;
    ms_hResult = m_pqDevice->GetRenderState(D3DRS_CLIPPLANEENABLE,
        &dwClipPlanesEnabled);
    assert(SUCCEEDED(ms_hResult));

    dwClipPlanesEnabled &= ~(1 << i);
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,
        dwClipPlanesEnabled);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::SetPointSize (float fSize)
{
    Renderer::SetPointSize(fSize);

    ms_hResult = m_pqDevice->SetRenderState(D3DRS_POINTSIZE,
        *((DWORD*)&m_fPointSize));
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::SetLineWidth (float fWidth)
{
    Renderer::SetLineWidth(fWidth);

    // TO DO.  This is not currently supported.  The ID3DXLine is what needs
    // to be used and requires a lot of setup.  Maybe another day...
}
//----------------------------------------------------------------------------
void Dx9Renderer::SetLineStipple (int iRepeat, unsigned short usPattern)
{
    Renderer::SetLineStipple(iRepeat,usPattern);

    // TO DO.  This is not currently supported.  The ID3DXLine is what needs
    // to be used and requires a lot of setup.  Maybe another day...
}
//----------------------------------------------------------------------------
