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
// Version: 4.0.10 (2009/01/23)

#ifndef WM4RENDERER_H
#define WM4RENDERER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4FrameBuffer.h"
#include "Wm4AlphaState.h"
#include "Wm4CullState.h"
#include "Wm4MaterialState.h"
#include "Wm4PolygonOffsetState.h"
#include "Wm4StencilState.h"
#include "Wm4WireframeState.h"
#include "Wm4ZBufferState.h"
#include "Wm4ColorRGB.h"
#include "Wm4ColorRGBA.h"
#include "Wm4Matrix4.h"
#include "Wm4RendererConstant.h"
#include "Wm4SamplerInformation.h"

namespace Wm4
{

class Attributes;
class Bindable;
class Camera;
class DynamicTexture;
class Effect;
class Geometry;
class IndexBuffer;
class Light;
class PixelProgram;
class ResourceIdentifier;
class ShaderEffect;
class Spatial;
class Texture;
class VertexBuffer;
class VertexProgram;
class VisibleObject;
class VisibleSet;

class WM4_GRAPHICS_ITEM Renderer
{
public:
    // Abstract API for renderers.  Each graphics API must implement this
    // layer.
    virtual ~Renderer ();

    // Run-time type information.
    enum
    {
        OPENGL,
        DIRECTX,
        SOFTWARE,
        MAX_RENDERER_TYPES
    };
    virtual int GetType () const = 0;

    // Make this renderer context the active one.
    virtual void Activate ();
    virtual bool IsActive () const;

    // Renderer-specific information for loading shader programs.
    virtual const char* GetExtension () const = 0;
    virtual char GetCommentCharacter () const = 0;
    static int GetMaxLights ();
    static int GetMaxColors ();
    static int GetMaxTCoords ();
    static int GetMaxVShaderImages ();
    static int GetMaxPShaderImages ();
    static int GetMaxStencilIndices ();
    static int GetMaxUserClipPlanes ();
    static float GetMaxAnisotropy ();

    // Access to the camera.
    void SetCamera (Camera* pkCamera);
    Camera* GetCamera () const;

    // Access to the geometry object that is to be drawn.
    void SetGeometry (Geometry* pkGeometry);
    Geometry* GetGeometry () const;

    // Frame buffer parameters.
    FrameBuffer::FormatType GetFormatType () const;
    FrameBuffer::DepthType GetDepthType () const;
    FrameBuffer::StencilType GetStencilType () const;
    FrameBuffer::BufferingType GetBufferingType () const;
    FrameBuffer::MultisamplingType GetMultisamplingType () const;

    // Window parameters.
    int GetWidth () const;
    int GetHeight () const;
    virtual void Resize (int iWidth, int iHeight);
    virtual void ToggleFullscreen ();

    // Access to the color value used for clearing the back buffer.
    void SetClearColor (const ColorRGBA& rkClearColor);
    const ColorRGBA& GetClearColor () const;

    // Access to the depth value used for clearing the depth buffer.
    void SetClearDepth (float fClearDepth);
    float GetClearDepth () const;

    // Access to the stencil value used for clearing the stencil buffer.
    void SetClearStencil (unsigned int uiClearValue);
    unsigned int GetClearStencil () const;

    // Support for predraw and postdraw semantics.
    virtual bool BeginScene ();
    virtual void EndScene ();

    // Support for full-sized window buffer operations.  The values used for
    // clearing are specified by SetBackgroundColor, SetClearDepth, and
    // SetClearStencil.
    virtual void ClearBackBuffer () = 0;
    virtual void ClearZBuffer () = 0;
    virtual void ClearStencilBuffer () = 0;
    virtual void ClearBuffers () = 0;
    virtual void DisplayBackBuffer () = 0;

    // Clear the buffer in the specified subwindow.  The values used for
    // clearing are specified by SetBackgroundColor, SetClearDepth, and
    // SetClearStencil.
    virtual void ClearBackBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;
    virtual void ClearZBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;
    virtual void ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;
    virtual void ClearBuffers (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;

    // The main entry point to drawing in the derived-class renderers.
    virtual void DrawElements () = 0;

    // Object drawing.
    void DrawScene (VisibleSet& rkVisibleSet);
    void Draw (Geometry* pkGeometry);

    // Text drawing.
    virtual int LoadFont (const char* acFace, int iSize, bool bBold,
        bool bItalic) = 0;
    virtual void UnloadFont (int iFontID) = 0;
    virtual bool SelectFont (int iFontID) = 0;
    virtual void Draw (int iX, int iY, const ColorRGBA& rkColor,
        const char* acText) = 0;

    // 2D drawing
    virtual void Draw (const unsigned char* aucBuffer) = 0;

    // Object drawing.
    void ApplyEffect (ShaderEffect* pkEffect, bool& rbPrimaryEffect);

    // Point size, line width, and line stipple.  These translate nicely to
    // OpenGL calls.  The point size is supported by Direct3D.  However, to
    // draw thicker lines or stippled lines, a separate interface in Direct3D
    // (ID3DXLine) must be used.  Until code is added to use this interface,
    // the Direct3D renderer ignores the SetLineWidth and SetLineStipple
    // function calls.  Line stippling is disabled when either of "repeat" or
    // "pattern" is zero.
    virtual void SetPointSize (float fSize);
    float GetPointSize () const;
    virtual void SetLineWidth (float fWidth);
    float GetLineWidth () const;
    virtual void SetLineStipple (int iRepeat, unsigned short usPattern);
    int GetLineStippleRepeat () const;
    unsigned short GetLineStipplePattern () const;

    // Global render state management.
    void SetGlobalState (GlobalStatePtr aspkState[]);
    void RestoreGlobalState (GlobalStatePtr aspkState[]);
    virtual void SetAlphaState (AlphaState* pkState);
    virtual void SetCullState (CullState* pkState);
    virtual void SetMaterialState (MaterialState* pkState);
    virtual void SetPolygonOffsetState (PolygonOffsetState* pkState);
    virtual void SetStencilState (StencilState* pkState);
    virtual void SetWireframeState (WireframeState* pkState);
    virtual void SetZBufferState (ZBufferState* pkState);
    AlphaState* GetAlphaState ();
    CullState* GetCullState ();
    MaterialState* GetMaterialState ();
    PolygonOffsetState* GetPolygonOffsetState ();
    StencilState* GetStencilState ();
    WireframeState* GetWireframeState ();
    ZBufferState* GetZBufferState ();
    void SetReverseCullFace (bool bReverseCullFace);
    bool GetReverseCullFace () const;

    // Function pointer types for binding and unbinding resources.
    typedef void (Renderer::*ReleaseFunction)(Bindable*);

    // Resource loading and releasing.
    void LoadAllResources (Spatial* pkScene);
    void ReleaseAllResources (Spatial* pkScene);
    void LoadResources (Geometry* pkGeometry);
    void ReleaseResources (Geometry* pkGeometry);
    void LoadResources (Effect* pkEffect);
    void ReleaseResources (Effect* pkEffect);
    void LoadVProgram (VertexProgram* pkVProgram);
    void ReleaseVProgram (Bindable* pkVProgram);
    void LoadPProgram (PixelProgram* pkPProgram);
    void ReleasePProgram (Bindable* pkPProgram);
    void LoadTexture (Texture* pkTexture);
    void ReleaseTexture (Bindable* pkTexture);
    void LoadVBuffer (const Attributes& rkIAttr, const Attributes& rkOAttr,
        VertexBuffer* pkVBuffer);
    void ReleaseVBuffer (Bindable* pkVBuffer);
    void LoadIBuffer (IndexBuffer* pkIBuffer);
    void ReleaseIBuffer (Bindable* pkIBuffer);

    // Support for locking and unlocking dynamic textures.
    enum
    {
        DT_READ,
        DT_WRITE,
        DT_READ_WRITE
    };

    // For 1D, 2D, and 3D textures.
    virtual void* Lock (DynamicTexture* pkTexture, unsigned int eAccess) = 0;
    virtual void Unlock (DynamicTexture* pkTexture) = 0;

    // For cubemaps.  The face index is
    //   0 : +x face
    //   1 : -x face
    //   2 : +y face
    //   3 : -y face
    //   4 : +z face
    //   5 : -z face
    virtual void* Lock (int iFace, DynamicTexture* pkTexture,
        unsigned int eAccess) = 0;
    virtual void Unlock (int iFace, DynamicTexture* pkTexture) = 0;

    // Resource enabling and disabling.
    enum  // ConstantType
    {
        CT_RENDERER,
        CT_NUMERICAL,
        CT_USER
    };
    virtual void SetVProgramConstant (int eCType, int iBaseRegister,
        int iRegisterQuantity, float* afData) = 0;
    virtual void SetPProgramConstant (int eCType, int iBaseRegister,
        int iRegisterQuantity, float* afData) = 0;

    void EnableVProgram (VertexProgram* pkVProgram);
    void DisableVProgram (VertexProgram* pkVProgram);
    void EnablePProgram (PixelProgram* pkPProgram);
    void DisablePProgram (PixelProgram* pkPProgram);
    void EnableTexture (Texture* pkTexture);
    void DisableTexture (Texture* pkTexture);
    ResourceIdentifier* EnableVBuffer (const Attributes& rkIAttr,
        const Attributes& rkOAttr);
    void DisableVBuffer (ResourceIdentifier* pkID);
    void EnableIBuffer ();
    void DisableIBuffer ();

    // For use by effects with lights.
    void SetLight (int i, Light* pkLight);
    Light* GetLight (int i);

    // For use by effects with projectors.
    void SetProjector (Camera* pkProjector);
    Camera* GetProjector ();

    // Enable or disable which color channels will be written to the color
    // buffer.
    virtual void SetColorMask (bool bAllowRed, bool bAllowGreen,
        bool bAllowBlue, bool bAllowAlpha);
    virtual void GetColorMask (bool& rbAllowRed, bool& rbAllowGreen,
        bool& rbAllowBlue, bool& rbAllowAlpha);

    // Include additional clip planes.  The input plane must be in model
    // coordinates.  It is transformed internally to camera coordinates to
    // support clipping in clip space.
    virtual void EnableUserClipPlane (int i, const Plane3f& rkPlane) = 0;
    virtual void DisableUserClipPlane (int i) = 0;

    // Support for model-to-world transformation management.
    void SetWorldTransformation ();
    void RestoreWorldTransformation ();

    // The input transformation is applied to world-space vertices before
    // the view matrix is applied.
    void SetPostWorldTransformation (const Matrix4f& rkMatrix);
    void RestorePostWorldTransformation ();

    // The input transformation is applied after the projection matrix.  If
    // the input matrix has negative determinant, it involves a reflection,
    // in which case you need to reverse the triangle ordering for culling.
    // It is necessary to attach to the about-to-be-drawn scene a CullState
    // object with the default settings.  Set bReverseFaces to 'true'.
    void SetPostProjectionTransformation (const Matrix4f& rkMatrix,
        bool bReverseFaces);

    // The post-projection matrix is restored to the identity matrix.
    void RestorePostProjectionTransformation ();

    // Access the current state of the matrices.
    const Matrix4f& GetWorldMatrix () const;
    const Matrix4f& GetViewMatrix () const;
    const Matrix4f& GetProjectionMatrix () const;

    // Compute the axis-aligned bounding viewport rectangle for a set of
    // vertices.  This uses the settings for the camera currently attached to
    // the renderer.
    void ComputeViewportAABB (const VertexBuffer* pkVBuffer,
        const Transformation& rkTransform, float& rfXMin, float& rfXMax,
        float& rfYMin, float& rfYMax);

protected:
    // The Renderer is an abstract base class.
    Renderer (FrameBuffer::FormatType eFormat, FrameBuffer::DepthType eDepth,
        FrameBuffer::StencilType eStencil,
        FrameBuffer::BufferingType eBuffering,
        FrameBuffer::MultisamplingType eMultisampling, int iWidth,
        int iHeight);

    // Support for camera access and transformation setting.
    friend class Camera;
    void OnFrameChange ();
    void OnFrustumChange ();
    virtual void OnViewportChange () = 0;
    virtual void OnDepthRangeChange () = 0;

    // Resource loading and releasing (to/from video memory).
    virtual void OnLoadVProgram (ResourceIdentifier*& rpkID,
        VertexProgram* pkVProgram) = 0;
    virtual void OnReleaseVProgram (ResourceIdentifier* pkID) = 0;
    virtual void OnLoadPProgram (ResourceIdentifier*& rpkID,
        PixelProgram* pkPProgram) = 0;
    virtual void OnReleasePProgram (ResourceIdentifier* pkID) = 0;
    virtual void OnLoadTexture (ResourceIdentifier*& rpkID,
        Texture* pkTexture) = 0;
    virtual void OnReleaseTexture (ResourceIdentifier* pkID) = 0;
    virtual void OnLoadVBuffer (ResourceIdentifier*& rkpID,
        const Attributes& rkIAttr, const Attributes& rkOAttr,
        VertexBuffer* pkVBuffer) = 0;
    virtual void OnReleaseVBuffer (ResourceIdentifier* pkID) = 0;
    virtual void OnLoadIBuffer (ResourceIdentifier*& rpkID,
        IndexBuffer* pkIBuffer) = 0;
    virtual void OnReleaseIBuffer (ResourceIdentifier* pkID) = 0;

    virtual void OnEnableVProgram (ResourceIdentifier* pkID) = 0;
    virtual void OnDisableVProgram (ResourceIdentifier* pkID) = 0;
    virtual void OnEnablePProgram (ResourceIdentifier* pkID) = 0;
    virtual void OnDisablePProgram (ResourceIdentifier* pkID) = 0;
    virtual void OnEnableTexture (ResourceIdentifier* pkID) = 0;
    virtual void OnDisableTexture (ResourceIdentifier* pkID) = 0;
    virtual void OnEnableVBuffer (ResourceIdentifier* pkID) = 0;
    virtual void OnDisableVBuffer (ResourceIdentifier* pkID) = 0;
    virtual void OnEnableIBuffer (ResourceIdentifier* pkID) = 0;
    virtual void OnDisableIBuffer (ResourceIdentifier* pkID) = 0;

    // The operations are
    //   0 = matrix
    //   1 = transpose of matrix
    //   2 = inverse of matrix
    //   3 = inverse-transpose of matrix
    void GetTransform (const Matrix4f& rkMat, int iOperation, float* afData);
    void SetConstantWMatrix (int iOperation, float* afData);
    void SetConstantVMatrix (int iOperation, float* afData);
    void SetConstantPMatrix (int iOperation, float* afData);
    void SetConstantWVMatrix (int iOperation, float* afData);
    void SetConstantVPMatrix (int iOperation, float* afData);
    void SetConstantWVPMatrix (int iOperation, float* afData);

    // These functions do not use the option parameter, but the parameter is
    // included to allow for a class-static array of function pointers to
    // handle all shader constants.
    void SetConstantMaterialEmissive (int, float* afData);
    void SetConstantMaterialAmbient (int, float* afData);
    void SetConstantMaterialDiffuse (int, float* afData);
    void SetConstantMaterialSpecular (int, float* afData);
    void SetConstantCameraModelPosition (int, float* afData);
    void SetConstantCameraModelDirection (int, float* afData);
    void SetConstantCameraModelUp (int, float* afData);
    void SetConstantCameraModelRight (int, float* afData);
    void SetConstantCameraWorldPosition (int, float* afData);
    void SetConstantCameraWorldDirection (int, float* afData);
    void SetConstantCameraWorldUp (int, float* afData);
    void SetConstantCameraWorldRight (int, float* afData);
    void SetConstantProjectorModelPosition (int, float* afData);
    void SetConstantProjectorModelDirection (int, float* afData);
    void SetConstantProjectorModelUp (int, float* afData);
    void SetConstantProjectorModelRight (int, float* afData);
    void SetConstantProjectorWorldPosition (int, float* afData);
    void SetConstantProjectorWorldDirection (int, float* afData);
    void SetConstantProjectorWorldUp (int, float* afData);
    void SetConstantProjectorWorldRight (int, float* afData);
    void SetConstantProjectorMatrix (int, float* afData);

    // These functions set the light state.  The index iLight is between 0
    // and 7 (eight lights are currently supported).
    void SetConstantLightModelPosition (int iLight, float* afData);
    void SetConstantLightModelDirection (int iLight, float* afData);
    void SetConstantLightWorldPosition (int iLight, float* afData);
    void SetConstantLightWorldDirection (int iLight, float* afData);
    void SetConstantLightAmbient (int iLight, float* afData);
    void SetConstantLightDiffuse (int iLight, float* afData);
    void SetConstantLightSpecular (int iLight, float* afData);
    void SetConstantLightSpotCutoff (int iLight, float* afData);
    void SetConstantLightAttenuation (int iLight, float* afData);

    enum { SC_QUANTITY = 36 };
    typedef void (Renderer::*SetConstantFunction)(int,float*);
    static SetConstantFunction ms_aoSCFunction[SC_QUANTITY];
    void SetRendererConstant (RendererConstant::Type eRCType, float* afData);

// data members
protected:
    // Resource limits.  The values are set by the Renderer-derived objects.
    static int ms_iMaxLights;
    static int ms_iMaxColors;
    static int ms_iMaxTCoords;
    static int ms_iMaxVShaderImages;
    static int ms_iMaxPShaderImages;
    static int ms_iMaxStencilIndices;
    static int ms_iMaxUserClipPlanes;
    static float ms_fMaxAnisotropy;

    // Active samplers in shader programs, at most m_iMaxVShaderImages of them
    // for vertex programs and at most m_iMaxPShaderImages of them for pixel
    // programs.
    int m_iNumActiveSamplers;
    int m_iCurrentSampler;
    SamplerInformation** m_apkActiveSamplers;

    // Parameters for the drawing window and frame buffer.
    FrameBuffer::FormatType m_eFormat;
    FrameBuffer::DepthType m_eDepth;
    FrameBuffer::StencilType m_eStencil;
    FrameBuffer::BufferingType m_eBuffering;
    FrameBuffer::MultisamplingType m_eMultisampling;
    int m_iWidth, m_iHeight;
    ColorRGBA m_kClearColor;
    float m_fClearDepth;
    unsigned int m_uiClearStencil;
    bool m_bAllowRed, m_bAllowGreen, m_bAllowBlue, m_bAllowAlpha;

    // The camera for establishing the view frustum.
    Camera* m_pkCamera;

    // Global render states.
    GlobalStatePtr m_aspkState[GlobalState::MAX_STATE_TYPE];

    // Light storage for lookup by the shader-constant-setting functions.
    // The Renderer-derived classes must allocate this array during
    // construction, creating m_iMaxLights elements.  The Renderer class
    // deallocates the array during destruction.
    ObjectPtr* m_aspkLight;

    // The projector for various effects such as projected textures and
    // shadow maps.
    Camera* m_pkProjector;

    // Current Geometry object for drawing.
    Geometry* m_pkGeometry;

    // Transformations used in the geometric pipeline.  These matrices are
    // stored to support 1x4 row vectors times 4x4 matrices.
    Matrix4f m_kWorldMatrix, m_kSaveWorldMatrix;
    Matrix4f m_kViewMatrix, m_kSaveViewMatrix;
    Matrix4f m_kProjectionMatrix, m_kSaveProjectionMatrix;

    // Post-projection matrix.  For example, you can use this to reflect the
    // rendering.
    Matrix4f m_kPostProjectionMatrix;
    bool m_bPostProjectionIsIdentity;

    // For locking/unlocking dynamic textures.
    DynamicTexture* m_pkLockedTexture;
    int m_iLockedFace;

    // Current font for text drawing.
    int m_iFontID;

    // Support for mirror effects (default 'false').
    bool m_bReverseCullFace;

    // Toggle for fullscreen/window mode.
    bool m_bFullscreen;

    // Data for point size, line width, and line stipple.
    float m_fPointSize;  // default = 1
    float m_fLineWidth;  // default = 1
    int m_iLineStippleRepeat;  // default = 0 (disabled)
    unsigned short m_usLineStipplePattern;  // default = 0 (disabled)
};

#include "Wm4Renderer.inl"

}

#endif
