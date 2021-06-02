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
// Version: 4.0.4 (2007/09/23)

#ifndef WM4SOFTRENDERER_H
#define WM4SOFTRENDERER_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4SoftShaderUtility.h"
#include "Wm4SoftUtilityMCR.h"
#include "Wm4Renderer.h"
#include "Wm4Geometry.h"
#include "Wm4DynamicTexture.h"
#include "Wm4SoftArray.h"
#include "Wm4VertexShader.h"
#include "Wm4PixelShader.h"
#include "Wm4SoftSampler1.h"
#include "Wm4SoftSampler2.h"
#include "Wm4SoftSampler3.h"
#include "Wm4SoftSamplerCube.h"
#include "Wm4SoftSamplerProj.h"
#include "Wm4SoftFrameBuffer.h"

namespace Wm4
{

class VBufferID;
class IBufferID;

class WM4_RENDERER_ITEM SoftRenderer : public Renderer
{
    WM4_DECLARE_INITIALIZE;
    WM4_DECLARE_TERMINATE;

public:
    // Run-time type information.
    virtual int GetType () const;

    // Renderer-specific information for loading shader programs.
    virtual const char* GetExtension () const;
    virtual char GetCommentCharacter () const;

    // Abstract class.  The derived classes must implement DisplayBackBuffer.
    virtual ~SoftRenderer ();

    // full window buffer operations
    virtual void ClearBackBuffer () = 0;
    virtual void ClearZBuffer ();
    virtual void ClearStencilBuffer ();
    virtual void ClearBuffers ();

    // clear the buffer in the specified subwindow
    virtual void ClearBackBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearZBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearBuffers (int iXPos, int iYPos, int iWidth,
        int iHeight);

    // Include additional clip planes.  The input plane must be in model
    // coordinates.  It is transformed internally to camera coordinates to
    // support clipping in clip space.
    virtual void EnableUserClipPlane (int i, const Plane3f& rkPlane);
    virtual void DisableUserClipPlane (int i);

    // point size and line width (TO DO:  Not yet implemented)
    virtual void SetPointSize (float fSize);
    virtual void SetLineWidth (float fWidth);
    virtual void SetLineStipple (int iRepeat, unsigned short usPattern);

    // Dynamic reading and writing of textures.  TO DO:  These are disabled
    // for now.  The software renderer maps its Image data to ColorRGBA
    // data.  In WM5, preserve the original image format.  On the operations
    // lock/write/unlock, convert to ColorRGBA.
    virtual void* Lock (DynamicTexture*, unsigned int)
    {
        assert(false);
        return 0;
    }
    virtual void Unlock (DynamicTexture*)
    {
        assert(false);
    }
    virtual void* Lock (int, DynamicTexture*, unsigned int)
    {
        assert(false);
        return 0;
    }
    virtual void Unlock (int, DynamicTexture*)
    {
        assert(false);
    }

protected:
    // This class is abstract.  Derived classes are platform specific and
    // optionally may provide the chunks of memory to be used for
    // rasterization.  If the derived class wants to allocate the memory,
    // set bAllocateBackBuffer to 'false'; otherwise, SoftRenderer will
    // allocate the back buffer.
    SoftRenderer (FrameBuffer::FormatType eFormat,
        FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
        FrameBuffer::BufferingType eBuffering,
        FrameBuffer::MultisamplingType eMultisampling, int iWidth,
        int iHeight, bool bCreateColorBuffer);

    // support for clearing the back buffer
    virtual void ClearSubBackBuffer (int iXMin, int iYMin, int iXMax,
        int iYMax) = 0;

    // Set both m_auiBBuffer and m_akBBuffer values, adhering to the current
    // color masks m_bAllowRed, m_bAllowGreen, m_bAllowBlue, and
    // m_bAllowAlpha.  Regarding m_auiBBuffer, the color channels in [0,1] are
    // mapped to an unsigned int that contains four channels, each in [0,255].
    // The order of red, green, blue, and alpha depends on the platform.
    virtual void SetColorBuffer (int iX, int iY, int iIndex,
        const ColorRGBA& rkColor) = 0;

    // global render state management
    virtual void SetCullState (CullState* pkState);
    virtual void SetPolygonOffsetState (PolygonOffsetState* pkState);

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

    // camera access and support
    virtual void OnViewportChange ();
    virtual void OnDepthRangeChange ();

    // Selection of the framebuffer to render to.  The input is null for
    // rendering to the main framebuffer, but nonnull for an offscreen
    // framebuffer.
    friend class SoftFrameBuffer;
    void SetRenderTarget (SoftFrameBuffer* pkFrameBuffer);

// internal use
public:
    // Shader program registration.
    typedef void (*VProgram)(const float*, const float*, float*);
    typedef ColorRGBA (*PProgram)(const float*, SoftSampler**, const float*);
    static void RegisterVProgram (const char* acName, VProgram oVProgram);
    static void RegisterPProgram (const char* acName, PProgram oPProgram);

// helpers
private:
    bool Scissor (int iXPos, int iYPos, int iWidth, int iHeight, int& riXMin,
        int& riYMin, int& riXMax, int& riYMax) const;

    void ClearSubZBuffer (int iXMin, int iYMin, int iXMax, int iYMax);
    void ClearSubStencilBuffer (int iXMin, int iYMin, int iXMax, int iYMax);
    void ClearSubBuffers (int iXMin, int iYMin, int iXMax, int iYMax);

    // VERTEX SHADER SYSTEM.  The vertex register system uses four floats per
    // register.
    enum
    {
        // p, n, c0, c1, t0 through t7, all with max 4 channels
        MAX_CHANNELS = 48
    };
    float* GetOutputVertex (int i);
    void ApplyVertexShader ();

    // CLIPPING SYSTEM.  Clip a triangle against the view frustum using
    // clip-space coordinates.  The input is an array of the three triangle
    // vertices.  The output is the intersection of the triangle and the
    // (solid) view frustum.  This intersection is guaranteed to be a convex
    // polygon.  The input integer arrays must have enough elements to support
    // the number of extra vertices generated by the standard 6 frustum
    // planes and up to 6 user-defined clip planes (32 certainly suffices).
    enum { SOFT_MAX_CLIP_INDICES = 32 };

    int ClipInterpolate (int aiIndex[SOFT_MAX_CLIP_INDICES],
        float afTest[SOFT_MAX_CLIP_INDICES], int iPrev, int iCurr);

    void ClipPolygonAgainstPlane (int& riQuantity,
        int aiIndex[SOFT_MAX_CLIP_INDICES], int iFirstPosIndex,
        float afTest[SOFT_MAX_CLIP_INDICES],
        int aiEdge[SOFT_MAX_CLIP_INDICES]);

    void ClipPolygon (const Vector4f& rkPlane, int& riQuantity,
        int aiIndex[SOFT_MAX_CLIP_INDICES],
        int aiEdge[SOFT_MAX_CLIP_INDICES]);

    void ClipPolygon (int& riQuantity,
        int aiIndex[SOFT_MAX_CLIP_INDICES],
        int aiEdge[SOFT_MAX_CLIP_INDICES]);

    void ClipSegment (const Vector4f& rkPlane, int& riQuantity, int aiEnd[2]);
    void ClipSegment (int& riQuantity, int aiEnd[2]);

    // PROJECTION TO WINDOW COORDINATES.  Mapping of clip-space coordinates to
    // window-space coordinates.  The inputs are clip-space points (x,y,z,w).
    // For a full-sized viewport and a full depth range, the outputs are
    //   xWindow = 0.5*(width-1)*(x/w + 1)
    //   yWindow = 0.5*(height-1)*(1 - y/w)
    //   depth = z/w
    //   inverseW = 1/w
    // If the viewport is a subrectangle of the screen delimited by relative
    // port measurements pL, pR, pB, and pT with 0 <= pL < pR <= 1 and
    // 0 <= pB < pT <= 1, and if the depth range is set to [pN,pF], then
    //  the outputs are
    //   xWindow = 0.5*(W-1)*[(pR + pL) + (pR - pL)*(x/w)]
    //   yWindow = 0.5*(H-1)*[(2 - pT - pB) + (pT - pB)*(y/w)]
    //   depth = (pF-pN)*(z/w) + pN
    //   inverseW = 1/w
    void ClipToWindow (const float* afVertex, float& rfXWindow,
        float& rfYWindow, float& rfDepth, float& rfInverseW);

    // Project the clip-space point to window values (x,y), but with no
    // clamping to the window width or height.  The depth is not computed by
    // this function.
    void ClipToWindowXY (const float* afVertex, int& riX, int& riY);

    // RASTERIZATION SYSTEM.  Edge buffering is used for the rasterization.
    // The active pixel shader is called for each visited pixel.
    void PerspectiveInterpolate (const float* afAttr0, const float* afAttr1,
        int i0, int i1, float (*aafInterp)[MAX_CHANNELS]) const;

    bool ComputeEdgeBuffers (const int aiX[3], const int aiY[3],
        const float* aafAttr[3], int& riYMin, int& riYMax);

    void TwoBuffers (int i0, int i1, int j0, int j1, const int aiX[3],
        const int aiY[3], const float* aafAttr[3], int& riYMin,
        int& riYMax);

    void ThreeBuffers (int i0, int i1, int i2, const int aiX[3],
        const int aiY[3], const float* aafAttr[3], int* aiXExt0,
        float (*aafXAttr0)[MAX_CHANNELS], int* aiXExt1,
        float (*aafXAttr1)[MAX_CHANNELS], int& riYMin,
        int& riYMax);

    void ComputeEdgeBuffer (int iX0, int iY0, const float* afAttr0, int iX1,
        int iY1, const float* afAttr1, int* aiXBuffer,
        float (*aafAttrBuffer)[MAX_CHANNELS]);

    // Support for polygon offset (depth biasing).
    float ComputePolygonOffset (int* aiX, int* aiY, float* afDepth);

    void RasterizePoint (int iV0);
    void RasterizeEdge (int iV0, int iV1);
    void RasterizeTriangle (int iV0, int iV1, int iV2);

    // PIXEL SHADER SYSTEM.  The pixel register system uses four floats per
    // register.
    void ApplyPixelShader (int iX, int iY, float fDepth, float fInverseW,
        const float* afInPixel);
    bool ApplyStencilCompare (int iIndex);
    void ApplyStencilOperation (int iIndex,
        StencilState::OperationType eType);
    bool ApplyZBufferCompare (int iIndex, unsigned int uiDepth);
    ColorRGBA GetBlendedColor (const ColorRGBA& rkSrcColor,
        const ColorRGBA& rkDstColor);

    // PRIMITIVE DRAWING SYSTEM.  This is the entry point into drawing the
    // geometric primitives.  The validation call checks if vertex or pixel
    // shaders.  If they do, the input of the pixel shader must match the
    // output of the vertex shader.  If these do not match, the default
    // shaders are used.
    virtual void DrawElements ();
    void DrawPolypoint ();
    void DrawPolylineDisjoint ();
    void DrawPolylineContiguous ();
    void DrawTriMesh ();
    void DrawTriStrip ();
    void DrawTriFan ();
    void DrawTriangle (int iV0, int iV1, int iV2);

// data members
protected:
    // The main framebuffer.
    SoftFrameBuffer m_kMain;

    // The active framebuffer.  The values m_iWidth and m_iHeight are part of
    // this context, but are members of the base class Renderer.
    int m_iQuantity;
    unsigned int* m_auiColorBuffer;
    ColorRGBA* m_akColorBuffer;
    unsigned int m_uiMaxDepthValue;
    unsigned int* m_auiDepthBuffer;
    float* m_afDepthBuffer;
    unsigned int m_uiMaxStencilValue;
    unsigned int* m_auiStencilBuffer;

    // The color mask for writing to the color buffer.  The value is specific
    // to the platform, so is set by SoftRenderer-derived classes in
    // SetColorMask.
    unsigned int m_uiColorMask;

    // Parameters for mapping from clip space to screen space, including any
    // adjustment for a viewport not the full window.  If (x/w,y/w) are the
    // normalized display values in [-1,1]^2, then the screen space values are
    //   x_scr = xcoeff0 + xcoeff1*(x/w)
    //   y_scr = ycoeff0 + ycoeff1*(y/w)
    // The coefficients are
    //   xcoeff0 = 0.5*(width-1)*(portR+portL) + 0.5
    //   xcoeff1 = 0.5*((width-1)*(portR-portL)
    //   ycoeff0 = 0.5*((height-1)*(portT+portB) + 0.5
    //   ycoeff1 = -0.5*((height-1)*(portT-portB)
    // The actual computed values for xcoeff0 and ycoeff0 have an extra 0.5
    // added to them for round-to-nearest integer in the transformation.  The
    // negative term for ycoeff1 converts from right-handed viewport
    // coordinates to the left-handed screen coordinates.
    float m_fXCoeff0, m_fXCoeff1, m_fYCoeff0, m_fYCoeff1;
    float m_fMinDepth, m_fMaxDepth;  // default: min = 0, max = 1
    float m_fRangeDepth;  // = (max-min)

    // Frustum clip planes (ordered by: near, far, left, right, bottom, top).
    // They are stored as (nx,ny,nz,-c).  If (x,y,z,w) is the clip space
    // point, the planes are
    //   near   : ( 0, 0, 1, 0)*(x,y,z,w) = 0  [z = 0]
    //   far    : ( 0, 0,-1, 1)*(x,y,z,w) = 0  [w - z = 0]
    //   left   : ( 1, 0, 0, 1)*(x,y,z,w) = 0  [w + x = 0]
    //   right  : (-1, 0, 0, 1)*(x,y,z,w) = 0  [w - x = 0]
    //   bottom : ( 0, 1, 0, 1)*(x,y,z,w) = 0  [w + y = 0]
    //   top    : ( 0,-1, 0, 1)*(x,y,z,w) = 0  [w - y = 0]
    // The inside of the frustum is defined by (a,b,c,d)*(x,y,z,w) >= 0 for
    // the six planes.
    Vector4f m_akFrustumClipPlane[6];

    // Additional clip planes to be specified by the user.  The user provides
    // planes in model coordinates, but the software renderer converts these
    // to camera coordinates for use by the clipping system.
    enum { SOFT_MAX_USER_CLIP_PLANES = 6 };
    Vector4f m_akUserClipPlane[SOFT_MAX_USER_CLIP_PLANES];
    bool m_abUserClipPlaneActive[SOFT_MAX_USER_CLIP_PLANES];
    bool m_bSomeUserClipPlaneActive;

    // For drawing various geometric primitives.
    typedef void (SoftRenderer::*DrawFunction)(void);
    DrawFunction m_aoDrawFunction[Geometry::GT_MAX_QUANTITY];

    // Global renderer states.
    int m_iCullSignAdjust;        // cull state
    float m_fPolygonOffsetScale;  // polygon-offset state
    float m_fPolygonOffsetBias;   // polygon-offset state

    // Resource limits.
    enum { SOFT_MAX_TEXTURES = 8 };
    enum { SOFT_MAX_LIGHTS = 8 };
    enum { SOFT_MAX_COLORS = 2 };

    // Texture samplers.  These are bound/unbound using the BindInfo
    // mechanism.
    std::set<SoftSampler*> m_kSamplers;

    // Data for vertex and pixel shaders.
    int m_iNumInputChannels;
    int m_iNumOutputChannels;
    int m_iNextVertexIndex;
    SoftArray<float> m_kOutputVertex;

    // Currently bound buffers.
    VBufferID* m_pkVBResource;
    IBufferID* m_pkIBResource;

    // Current vertex and pixel programs.
    VProgram m_oVProgram;
    PProgram m_oPProgram;
    Attributes m_kOAttr;

    // Vertex program and pixel program registers.
    SoftArray<float> m_kVRegister;
    SoftArray<float> m_kPRegister;

    // Pixel program samplers.
    std::vector<SoftSampler*> m_kPSampler;

    // Edge buffering for scan line setup.
    int* m_aiXMin;
    int* m_aiXMax;
    float (*m_aafXMinAttr)[MAX_CHANNELS];
    float (*m_aafXMaxAttr)[MAX_CHANNELS];
    float (*m_aafScanAttr)[MAX_CHANNELS];

    // Registered vertex programs.
    static std::map<std::string,VProgram>* ms_pkVPrograms;

    // Registered vertex programs.
    static std::map<std::string,PProgram>* ms_pkPPrograms;
};

WM4_REGISTER_INITIALIZE(SoftRenderer);
WM4_REGISTER_TERMINATE(SoftRenderer);
#include "Wm4SoftRenderer.inl"
#include "Wm4SoftShaderLibrary.h"

}

#endif
