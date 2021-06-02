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
// Version: 4.0.3 (2007/09/23)

#ifndef WM4DX9RENDERER_H
#define WM4DX9RENDERER_H

#include "Wm4Dx9RendererLIB.h"
#include "Wm4Dx9Utility.h"
#include "Wm4Dx9FrameBuffer.h"
#include "Wm4Renderer.h"
#include "Wm4Geometry.h"
#include "Wm4DynamicTexture.h"

namespace Wm4
{

class WM4_RENDERER_ITEM Dx9Renderer : public Renderer
{
public:
    // Run-time type information.
    virtual int GetType () const { return Renderer::DIRECTX; }

    // Renderer-specific information for loading shader programs.
    virtual const char* GetExtension () const { return "dx9"; }
    virtual char GetCommentCharacter () const { return '/'; }

    // construction
    Dx9Renderer (HWND hWnd, FrameBuffer::FormatType eFormat,
        FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
        FrameBuffer::BufferingType eBuffering,
        FrameBuffer::MultisamplingType eMultisampling, int iWidth,
        int iHeight);

    virtual ~Dx9Renderer();

    virtual void ToggleFullscreen ();
    virtual bool BeginScene ();
    virtual void EndScene ();

    // full window buffer operations
    virtual void ClearBackBuffer ();
    virtual void ClearZBuffer ();
    virtual void ClearStencilBuffer ();
    virtual void ClearBuffers ();
    virtual void DisplayBackBuffer ();

    // clear the buffer in the specified subwindow
    virtual void ClearBackBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearZBuffer (int iXPos, int iYPos, int iWidth, int iHeight);
    virtual void ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearBuffers (int iXPos, int iYPos, int iWidth, int iHeight);

    // specialized drawing
    virtual bool SelectFont (int iFontID);
    int LoadFont (const char* acFace, int iSize, bool bBold, bool bItalic);
    void UnloadFont (int iFontID);
    virtual void Draw (int iX, int iY, const ColorRGBA& rkColor,
        const char* acText);
    virtual void Draw (const unsigned char* aucBuffer);

    // point size and line width
    virtual void SetPointSize (float fSize);
    virtual void SetLineWidth (float fWidth);
    virtual void SetLineStipple (int iRepeat, unsigned short usPattern);

    // Enable or disable which color channels will be written to the color
    // buffer.
    virtual void SetColorMask (bool bAllowRed, bool bAllowGreen,
        bool bAllowBlue, bool bAllowAlpha);

    // Include additional clip planes.  The input plane must be in model
    // coordinates.  It is transformed internally to camera coordinates to
    // support clipping in clip space.
    virtual void EnableUserClipPlane (int i, const Plane3f& rkPlane);
    virtual void DisableUserClipPlane (int i);

    // Dynamic reading and writing of textures.
    virtual void* Lock (DynamicTexture* pkTexture, unsigned int eAccess);
    virtual void Unlock (DynamicTexture* pkTexture);
    virtual void* Lock (int iFace, DynamicTexture* pkTexture,
        unsigned int eAccess);
    virtual void Unlock (int iFace, DynamicTexture* pkTexture);

protected:
    // device management
    void OnDeviceLost ();
    void ResetDevice ();

    // camera updates
    virtual void OnViewportChange ();
    virtual void OnDepthRangeChange ();

    // global render state management
    virtual void SetAlphaState (AlphaState* pkState);
    virtual void SetCullState (CullState* pkState);
    virtual void SetPolygonOffsetState (PolygonOffsetState* pkState);
    virtual void SetStencilState (StencilState* pkState);
    virtual void SetWireframeState (WireframeState* pkState);
    virtual void SetZBufferState (ZBufferState* pkState);

    // The entry point to drawing a geometry object.
    virtual void DrawElements ();

    // Resource loading and releasing (to/from video memory).
    virtual void OnLoadVProgram (ResourceIdentifier*& rpkID,
        VertexProgram* pkVProgram);
    virtual void OnReleaseVProgram (ResourceIdentifier* pkID);
    virtual void OnLoadPProgram (ResourceIdentifier*& rpkID,
        PixelProgram* pkPProgram);
    virtual void OnReleasePProgram (ResourceIdentifier* pkID);
    virtual void OnLoadTexture (ResourceIdentifier*& rpkID,
        Texture* pkTexture);
    virtual void OnReleaseTexture (ResourceIdentifier* pkID);
    virtual void OnLoadVBuffer (ResourceIdentifier*& rkpID,
        const Attributes& rkIAttr, const Attributes& rkOAttr,
        VertexBuffer* pkVBuffer);
    virtual void OnReleaseVBuffer (ResourceIdentifier* pkID);
    virtual void OnLoadIBuffer (ResourceIdentifier*& rpkID,
        IndexBuffer* pkIBuffer);
    virtual void OnReleaseIBuffer (ResourceIdentifier* pkID);

    // Resource enabling and disabling.
    virtual void SetVProgramConstant (int eCType, int iBaseRegister,
        int iRegisterQuantity, float* afData);
    virtual void SetPProgramConstant (int eCType, int iBaseRegister,
        int iRegisterQuantity, float* afData);
    virtual void OnEnableVProgram (ResourceIdentifier* pkID);
    virtual void OnDisableVProgram (ResourceIdentifier* pkID);
    virtual void OnEnablePProgram (ResourceIdentifier* pkID);
    virtual void OnDisablePProgram (ResourceIdentifier* pkID);
    virtual void OnEnableTexture (ResourceIdentifier* pkID);
    virtual void OnDisableTexture (ResourceIdentifier* pkID);
    virtual void OnEnableVBuffer (ResourceIdentifier* pkID);
    virtual void OnDisableVBuffer (ResourceIdentifier* pkID);
    virtual void OnEnableIBuffer (ResourceIdentifier* pkID);
    virtual void OnDisableIBuffer (ResourceIdentifier* pkID);

// member variables
protected:
    friend class Dx9FrameBuffer;

    LPDIRECT3D9 m_pqMain;
    LPDIRECT3DDEVICE9 m_pqDevice;
    D3DPRESENT_PARAMETERS m_kPresent;
    RECT m_rcWindow;
    bool m_bDeviceLost;
    bool m_bCursorVisible;
    bool m_bBeginSceneActive;
    LPDIRECT3DSURFACE9 m_pkLockedFace;

    // font handling
    std::vector<LPD3DXFONT> m_kFontArray;

    // for error checking and exception handling
    static HRESULT ms_hResult;

    static D3DPRIMITIVETYPE ms_aeObjectType[];
    static DWORD ms_adwAlphaSrcBlend[];
    static DWORD ms_adwAlphaDstBlend[];
    static DWORD ms_adwAlphaTest[];
    static DWORD ms_adwCullType[];
    static DWORD ms_adwStencilCompare[];
    static DWORD ms_adwStencilOperation[];
    static DWORD ms_adwZBufferCompare[];
    static DWORD ms_adwTexMinFilter[];
    static DWORD ms_adwTexMipFilter[];
    static DWORD ms_adwTexWrapMode[];
    static D3DFORMAT ms_aeImageFormat[];
};

}

#endif
