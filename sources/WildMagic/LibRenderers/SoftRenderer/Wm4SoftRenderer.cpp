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
// Version: 4.0.6 (2007/08/11)

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftRenderer.h"
#include "Wm4Camera.h"
using namespace Wm4;

WM4_IMPLEMENT_INITIALIZE(SoftRenderer);
WM4_IMPLEMENT_TERMINATE(SoftRenderer);

std::map<std::string,SoftRenderer::VProgram>*
SoftRenderer::ms_pkVPrograms = 0;

std::map<std::string,SoftRenderer::PProgram>*
SoftRenderer::ms_pkPPrograms = 0;

//----------------------------------------------------------------------------
void SoftRenderer::Initialize ()
{
    ms_pkVPrograms = WM4_NEW std::map<std::string,VProgram>;
    ms_pkPPrograms = WM4_NEW std::map<std::string,PProgram>;
}
//----------------------------------------------------------------------------
void SoftRenderer::Terminate ()
{
    ms_pkVPrograms->clear();
    ms_pkPPrograms->clear();
    WM4_DELETE ms_pkVPrograms;
    WM4_DELETE ms_pkPPrograms;
    ms_pkVPrograms = 0;
    ms_pkPPrograms = 0;
}
//----------------------------------------------------------------------------
void SoftRenderer::RegisterVProgram (const char* acName, VProgram oVProgram)
{
    ms_pkVPrograms->insert(std::make_pair(acName,oVProgram));
}
//----------------------------------------------------------------------------
void SoftRenderer::RegisterPProgram (const char* acName, PProgram oPProgram)
{
    ms_pkPPrograms->insert(std::make_pair(acName,oPProgram));
}
//----------------------------------------------------------------------------
SoftRenderer::SoftRenderer (FrameBuffer::FormatType eFormat,
    FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
    FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iWidth, int iHeight,
    bool bCreateColorBuffer)
    :
    Renderer(eFormat,eDepth,eStencil,eBuffering,eMultisampling,iWidth,
        iHeight),
    m_kMain(eFormat,eDepth,eStencil,eBuffering,eMultisampling,0,0,iWidth,
        iHeight,bCreateColorBuffer),
    m_kOutputVertex(1024),
    m_kVRegister(128),
    m_kPRegister(128),
    m_kPSampler(16)
{
    m_uiColorMask = (unsigned int)~0;

    // For use in mapping clip-space coordinates to screen-space coordinates.
    // Initially, portR = 1, portL = 0, portT = 1, and portL = 0.
    m_fXCoeff0 = 0.5f*m_iWidth;
    m_fXCoeff1 = m_fXCoeff0;
    m_fYCoeff0 = 0.5f*m_iHeight;
    m_fYCoeff1 = -m_fYCoeff0;
    m_fMinDepth = 0.0f;
    m_fMaxDepth = 1.0f;
    m_fRangeDepth = 1.0f;

    // Functions for drawing of primitives.
    m_aoDrawFunction[0] = &SoftRenderer::DrawPolypoint;
    m_aoDrawFunction[1] = &SoftRenderer::DrawPolylineDisjoint;
    m_aoDrawFunction[2] = &SoftRenderer::DrawPolylineContiguous;
    m_aoDrawFunction[3] = &SoftRenderer::DrawPolylineContiguous;
    m_aoDrawFunction[4] = &SoftRenderer::DrawTriMesh;
    m_aoDrawFunction[5] = &SoftRenderer::DrawTriStrip;
    m_aoDrawFunction[6] = &SoftRenderer::DrawTriFan;

    // Global state.
    m_iCullSignAdjust = 1;
    m_fPolygonOffsetScale = 0.0f;
    m_fPolygonOffsetBias = 0.0f;

    // Limitations on rendering resources.  The first three assigned variables
    // are members of the base class Renderer.
    ms_iMaxColors = SOFT_MAX_COLORS;
    ms_iMaxTCoords = SOFT_MAX_TEXTURES;
    ms_iMaxVShaderImages = SOFT_MAX_TEXTURES;
    ms_iMaxPShaderImages = SOFT_MAX_TEXTURES;
    m_apkActiveSamplers = WM4_NEW SamplerInformation*[ms_iMaxPShaderImages];
    memset(m_apkActiveSamplers,0,
        ms_iMaxPShaderImages*sizeof(SamplerInformation*));

    ms_iMaxLights = SOFT_MAX_LIGHTS;
    ms_iMaxStencilIndices = 255;  // 8-bit stencil buffer
    ms_iMaxUserClipPlanes = SOFT_MAX_USER_CLIP_PLANES;

    // This member belongs to the base class Renderer and is used to store
    // active lights for a primitive.
    m_aspkLight = WM4_NEW ObjectPtr[SOFT_MAX_LIGHTS];

    // The view frustum planes in homogeneous coordinates.
    //   near   : ( 0, 0, 1, 0)*(x,y,z,w) = 0  [z = 0]
    //   far    : ( 0, 0,-1, 1)*(x,y,z,w) = 0  [w - z = 0]
    //   left   : ( 1, 0, 0, 1)*(x,y,z,w) = 0  [w + x = 0]
    //   right  : (-1, 0, 0, 1)*(x,y,z,w) = 0  [w - x = 0]
    //   bottom : ( 0, 1, 0, 1)*(x,y,z,w) = 0  [w + y = 0]
    //   top    : ( 0,-1, 0, 1)*(x,y,z,w) = 0  [w - y = 0]
    // The interior of the frustum is z >= 0, w-z >= 0, w+x >= 0, w-x >= 0,
    // w+y >= 0, and w-y >= 0.
    m_akFrustumClipPlane[0] = Vector4f( 0.0f, 0.0f, 1.0f, 0.0f);
    m_akFrustumClipPlane[1] = Vector4f( 0.0f, 0.0f,-1.0f, 1.0f);
    m_akFrustumClipPlane[2] = Vector4f( 1.0f, 0.0f, 0.0f, 1.0f);
    m_akFrustumClipPlane[3] = Vector4f(-1.0f, 0.0f, 0.0f, 1.0f);
    m_akFrustumClipPlane[4] = Vector4f( 0.0f, 1.0f, 0.0f, 1.0f);
    m_akFrustumClipPlane[5] = Vector4f( 0.0f,-1.0f, 0.0f, 1.0f);
    for (int i = 0; i < SOFT_MAX_USER_CLIP_PLANES; i++)
    {
        m_abUserClipPlaneActive[i] = false;
    }
    m_bSomeUserClipPlaneActive = false;

    // Data for vertex and pixel shaders.
    m_iNumInputChannels = 0;
    m_iNumOutputChannels = 0;
    m_iNextVertexIndex = 0;
    m_pkVBResource = 0;
    m_pkIBResource = 0;
    m_oVProgram = 0;
    m_oPProgram = 0;

    // Edge buffers for scan line rasterizing.
    m_aiXMin = WM4_NEW int[m_iHeight+1];
    m_aiXMax = WM4_NEW int[m_iHeight+1];
    m_aafXMinAttr = WM4_NEW float[m_iHeight+1][MAX_CHANNELS];
    m_aafXMaxAttr = WM4_NEW float[m_iHeight+1][MAX_CHANNELS];
    int iScanQuantity = (m_iWidth >= m_iHeight ? m_iWidth+1 : m_iHeight+1);
    m_aafScanAttr = WM4_NEW float[iScanQuantity][MAX_CHANNELS];

    // initialize global render state to default settings
    SetGlobalState(GlobalState::Default);
}
//----------------------------------------------------------------------------
SoftRenderer::~SoftRenderer ()
{
    WM4_DELETE[] m_aiXMin;
    WM4_DELETE[] m_aiXMax;
    WM4_DELETE[] m_aafXMinAttr;
    WM4_DELETE[] m_aafXMaxAttr;
    WM4_DELETE[] m_aafScanAttr;
}
//----------------------------------------------------------------------------
void SoftRenderer::SetRenderTarget (SoftFrameBuffer* pkFrameBuffer)
{
    if (pkFrameBuffer)
    {
        m_iWidth = pkFrameBuffer->GetWidth();
        m_iHeight = pkFrameBuffer->GetHeight();
        m_iQuantity = pkFrameBuffer->GetQuantity();
        m_auiColorBuffer = pkFrameBuffer->GetColorBufferUI();
        m_akColorBuffer = pkFrameBuffer->GetColorBufferRGBA();
        m_uiMaxDepthValue = pkFrameBuffer->GetMaxDepthValue();
        m_auiDepthBuffer = pkFrameBuffer->GetDepthBufferUI();
        m_afDepthBuffer = pkFrameBuffer->GetDepthBufferF();
        m_uiMaxStencilValue = pkFrameBuffer->GetMaxStencilValue();
        m_auiStencilBuffer = pkFrameBuffer->GetStencilBuffer();
    }
    else
    {
        m_iWidth = m_kMain.GetWidth();
        m_iHeight = m_kMain.GetHeight();
        m_iQuantity = m_kMain.GetQuantity();
        m_auiColorBuffer = m_kMain.GetColorBufferUI();
        m_akColorBuffer = m_kMain.GetColorBufferRGBA();
        m_uiMaxDepthValue = m_kMain.GetMaxDepthValue();
        m_auiDepthBuffer = m_kMain.GetDepthBufferUI();
        m_afDepthBuffer = m_kMain.GetDepthBufferF();
        m_uiMaxStencilValue = m_kMain.GetMaxStencilValue();
        m_auiStencilBuffer = m_kMain.GetStencilBuffer();
    }

    if (m_pkCamera)
    {
        OnFrameChange();
        OnFrustumChange();
        OnViewportChange();
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearZBuffer ()
{
    if (m_auiDepthBuffer)
    {
        unsigned int uiClearDepth = (unsigned int)(~0);
        if (m_fClearDepth < 1.0f)
        {
            uiClearDepth = (unsigned int)(m_fClearDepth*uiClearDepth);
        }

        for (int i = 0; i < m_iQuantity; i++)
        {
            m_auiDepthBuffer[i] = uiClearDepth;
            m_afDepthBuffer[i] = m_fClearDepth;
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearStencilBuffer ()
{
    if (m_auiStencilBuffer)
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            m_auiStencilBuffer[i] = m_uiClearStencil;
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearBuffers ()
{
    ClearBackBuffer();
    ClearZBuffer();
    ClearStencilBuffer();
}
//----------------------------------------------------------------------------
bool SoftRenderer::Scissor (int iXPos, int iYPos, int iWidth, int iHeight,
    int& riXMin, int& riYMin, int& riXMax, int& riYMax) const
{
    riXMin = iXPos;
    riYMin = iYPos;
    riXMax = iXPos + iWidth;
    riYMax = iYPos + iHeight;
    return 0 <= riXMin && riXMin <= riXMax && riXMax <= m_iWidth
        && 0 <= riYMin && riYMin <= riYMax && riYMax <= m_iHeight;
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearSubZBuffer (int iXMin, int iYMin, int iXMax,
    int iYMax)
{
    if (m_auiDepthBuffer)
    {
        unsigned int uiClearDepth = (unsigned int)(~0);
        if (m_fClearDepth < 1.0f)
        {
            uiClearDepth = (unsigned int)(m_fClearDepth*uiClearDepth);
        }

        int iBase = iXMin + m_iWidth*iYMin;
        for (int iY = iYMin; iY < iYMax; iY++)
        {
            unsigned int* auiDepth = &m_auiDepthBuffer[iBase];
            float* afDepth = &m_afDepthBuffer[iBase];
            for (int iX = iXMin, i = 0; iX < iXMax; iX++, i++)
            {
                auiDepth[i] = uiClearDepth;
                afDepth[i] = m_fClearDepth;
            }
            iBase += m_iWidth;
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearSubStencilBuffer (int iXMin, int iYMin, int iXMax,
    int iYMax)
{
    if (m_auiStencilBuffer)
    {
        int iBase = iXMin + m_iWidth*iYMin;
        for (int iY = iYMin; iY < iYMax; iY++)
        {
            unsigned int* auiStencil = &m_auiStencilBuffer[iBase];
            for (int iX = iXMin, i = 0; iX < iXMax; iX++, i++)
            {
                auiStencil[i] = m_uiClearStencil;
            }
            iBase += m_iWidth;
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearSubBuffers (int iXMin, int iYMin, int iXMax,
    int iYMax)
{
    ClearSubBackBuffer(iXMin,iYMin,iXMax,iYMax);
    ClearSubZBuffer(iXMin,iYMin,iXMax,iYMax);
    ClearSubStencilBuffer(iXMin,iYMin,iXMax,iYMax);
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearBackBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    int iXMin, iYMin, iXMax, iYMax;
    if (Scissor(iXPos,iYPos,iWidth,iHeight,iXMin,iYMin,iXMax,iYMax))
    {
        ClearSubBackBuffer(iXMin,iYMin,iXMax,iYMax);
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearZBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    int iXMin, iYMin, iXMax, iYMax;
    if (Scissor(iXPos,iYPos,iWidth,iHeight,iXMin,iYMin,iXMax,iYMax))
    {
        ClearSubZBuffer(iXMin,iYMin,iXMax,iYMax);
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    int iXMin, iYMin, iXMax, iYMax;
    if (Scissor(iXPos,iYPos,iWidth,iHeight,iXMin,iYMin,iXMax,iYMax))
    {
        ClearSubStencilBuffer(iXMin,iYMin,iXMax,iYMax);
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClearBuffers (int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    int iXMin, iYMin, iXMax, iYMax;
    if (Scissor(iXPos,iYPos,iWidth,iHeight,iXMin,iYMin,iXMax,iYMax))
    {
        ClearSubBackBuffer(iXMin,iYMin,iXMax,iYMax);
        ClearSubZBuffer(iXMin,iYMin,iXMax,iYMax);
        ClearSubStencilBuffer(iXMin,iYMin,iXMax,iYMax);
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::SetCullState (CullState* pkState)
{
    Renderer::SetCullState(pkState);

    // Determine the sign adjustment for the triangle visibility test.  The
    // visibility measure is computed assuming (1) counterclockwise triangles
    // are visible to the eye point and (2) back-facing triangles should be
    // culled.
    if (pkState->Enabled)
    {
        if (pkState->FrontFace == CullState::FT_CCW)
        {
            if (pkState->CullFace == CullState::CT_BACK)
            {
                m_iCullSignAdjust = 1;
            }
            else
            {
                m_iCullSignAdjust = -1;
            }
        }
        else
        {
            if (pkState->CullFace == CullState::CT_BACK)
            {
                m_iCullSignAdjust = -1;
            }
            else
            {
                m_iCullSignAdjust = 1;
            }
        }

        if (m_bReverseCullFace)
        {
            m_iCullSignAdjust = -m_iCullSignAdjust;
        }
    }
    else
    {
        m_iCullSignAdjust = 0;
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::EnableUserClipPlane (int i, const Plane3f& rkPlane)
{
    assert(0 <= i && i < SOFT_MAX_USER_CLIP_PLANES);
    m_abUserClipPlaneActive[i] = true;
    m_bSomeUserClipPlaneActive = true;

    // Map the model plane to camera coordinates.  If the model plane is
    //   0 = [N^t | -d]*[X_m | 1]^t
    // where superscript t denotes transpose, N is the model plane normal, d
    // is the model plane constant, and X_m is the model space position, then
    // the camera plane is
    //   0 = [N^t | -d]*A^{-1}*A*[X_m | 1] = ([N^t | -d]*A^{-1})*[X_e | 1]
    // where A is the model-view matrix, X_e is the camera coordinate for the
    // model space position, and A^{-1} is the inverse of A.  The 4-tuple
    // plane value is [N^t | -d]*A^{-1}, which is what the last line of code
    // here computes (Wild Magic uses vector*matrix to transpose(M)*vector.
    Vector4f kMPlane(rkPlane.Normal.X(),rkPlane.Normal.Y(),rkPlane.Normal.Z(),
        -rkPlane.Constant);
    Matrix4f kWorldViewMatrix = m_kWorldMatrix*m_kViewMatrix;
    m_akUserClipPlane[i] = kWorldViewMatrix.Inverse()*kMPlane;
}
//----------------------------------------------------------------------------
void SoftRenderer::DisableUserClipPlane (int i)
{
    assert(0 <= i && i < SOFT_MAX_USER_CLIP_PLANES);
    m_abUserClipPlaneActive[i] = false;

    m_bSomeUserClipPlaneActive = false;
    for (int j = 0; j < SOFT_MAX_USER_CLIP_PLANES; j++)
    {
        if (m_abUserClipPlaneActive[j])
        {
            m_bSomeUserClipPlaneActive = true;
            break;
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::SetPointSize (float fSize)
{
    Renderer::SetPointSize(fSize);

    // TO DO.  This is not currently supported.
}
//----------------------------------------------------------------------------
void SoftRenderer::SetLineWidth (float fWidth)
{
    Renderer::SetLineWidth(fWidth);

    // TO DO.  This is not currently supported.
}
//----------------------------------------------------------------------------
void SoftRenderer::SetLineStipple (int iRepeat, unsigned short usPattern)
{
    Renderer::SetLineStipple(iRepeat,usPattern);

    // TO DO.  This is not currently supported.
}
//----------------------------------------------------------------------------
