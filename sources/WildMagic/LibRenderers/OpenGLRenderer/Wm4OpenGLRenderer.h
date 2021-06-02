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
// Version: 4.0.5 (2007/09/23)

#ifndef WM4OPENGLRENDERER_H
#define WM4OPENGLRENDERER_H

#include "Wm4OpenGLRendererLIB.h"
#include "GTglExtensions.h"
#include "Wm4OpenGLFrameBuffer.h"
#include "Wm4Renderer.h"
#include "Wm4Geometry.h"
#include "Wm4DynamicTexture.h"

namespace Wm4
{

class BitmapFont;

class WM4_RENDERER_ITEM OpenGLRenderer : public Renderer
{
public:
    // Run-time type information.
    virtual int GetType () const { return Renderer::OPENGL; }

    // Renderer-specific information for loading shader programs.
    virtual const char* GetExtension () const { return "ogl"; }
    virtual char GetCommentCharacter () const { return '#'; }

    // Abstract base class.  The InitializeState call is made by derived
    // class constructors after an OpenGL context is created.
    virtual ~OpenGLRenderer();
    void InitializeState ();

    // Full window buffer operations.
    virtual void ClearBackBuffer ();
    virtual void ClearZBuffer ();
    virtual void ClearStencilBuffer ();
    virtual void ClearBuffers ();
    virtual void DisplayBackBuffer () = 0;

    // Clear the buffer in the specified subwindow.
    virtual void ClearBackBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearZBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearBuffers (int iXPos, int iYPos, int iWidth,
        int iHeight);

    // Specialized drawing functions.
    virtual bool SelectFont (int iFontID);
    virtual void UnloadFont (int iFontID);
    virtual void Draw (int iX, int iY, const ColorRGBA& rkColor,
        const char* acText);
    virtual void Draw (const unsigned char* aucBuffer);

    // point size, line width, and line stipple
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
    // construction
    OpenGLRenderer (FrameBuffer::FormatType eFormat,
        FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
        FrameBuffer::BufferingType eBuffering,
        FrameBuffer::MultisamplingType eMultisampling, int iWidth,
        int iHeight);

    // context handling
    virtual void SaveContext () = 0;
    virtual void RestoreContext () = 0;

    // camera updates
    friend class OpenGLFrameBuffer;
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

    // text drawing
    void DrawCharacter (const BitmapFont& rkFont, char cChar);

    // Display list base indices for fonts/characters.
    class DisplayListInfo
    {
    public:
        int Quantity;  // number of display lists, input to glGenLists
        int Start;     // start index, output from glGenLists
        int Base;      // base index for glListBase
    };
    std::vector<DisplayListInfo> m_kDLInfo;

    // Mappings from Wild Magic constants to OpenGL enumerations.
    static GLenum ms_aeObjectType[Geometry::GT_MAX_QUANTITY];
    static GLenum ms_aeAlphaSrcBlend[AlphaState::SBF_QUANTITY];
    static GLenum ms_aeAlphaDstBlend[AlphaState::DBF_QUANTITY];
    static GLenum ms_aeAlphaTest[AlphaState::TF_QUANTITY];
    static GLenum ms_aeFrontFace[CullState::FT_QUANTITY];
    static GLenum ms_aeCullFace[CullState::CT_QUANTITY];
    static GLenum ms_aeStencilCompare[StencilState::CF_QUANTITY];
    static GLenum ms_aeStencilOperation[StencilState::OT_QUANTITY];
    static GLenum ms_aeZBufferCompare[ZBufferState::CF_QUANTITY];
    static GLenum ms_aeTextureMipmap[Texture::MAX_FILTER_TYPES];
    static GLenum ms_aeWrapMode[Texture::MAX_WRAP_TYPES];
    static GLenum ms_aeImageComponents[Image::IT_QUANTITY];
    static GLenum ms_aeImageFormats[Image::IT_QUANTITY];
    static GLenum ms_aeImageTypes[Image::IT_QUANTITY];
    static GLenum ms_aeSamplerTypes[SamplerInformation::MAX_SAMPLER_TYPES];
    static GLenum ms_aeMapTypes[3];
};

}

#endif
