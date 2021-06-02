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
// Version: 4.0.17 (2009/01/23)

#include "Wm4GraphicsPCH.h"
#include "Wm4Renderer.h"
#include "Wm4Camera.h"
#include "Wm4Effect.h"
#include "Wm4Geometry.h"
#include "Wm4Light.h"
#include "Wm4Node.h"
#include "Wm4ShaderEffect.h"
using namespace Wm4;

int Renderer::ms_iMaxLights = 0;
int Renderer::ms_iMaxColors = 0;
int Renderer::ms_iMaxTCoords = 0;
int Renderer::ms_iMaxVShaderImages = 0;
int Renderer::ms_iMaxPShaderImages = 0;
int Renderer::ms_iMaxStencilIndices = 0;
int Renderer::ms_iMaxUserClipPlanes = 0;
float Renderer::ms_fMaxAnisotropy = 1.0f;

Renderer::SetConstantFunction Renderer::ms_aoSCFunction[SC_QUANTITY] =
{
    &Renderer::SetConstantWMatrix,
    &Renderer::SetConstantVMatrix,
    &Renderer::SetConstantPMatrix,
    &Renderer::SetConstantWVMatrix,
    &Renderer::SetConstantVPMatrix,
    &Renderer::SetConstantWVPMatrix,

    &Renderer::SetConstantMaterialEmissive,
    &Renderer::SetConstantMaterialAmbient,
    &Renderer::SetConstantMaterialDiffuse,
    &Renderer::SetConstantMaterialSpecular,

    &Renderer::SetConstantCameraModelPosition,
    &Renderer::SetConstantCameraModelDirection,
    &Renderer::SetConstantCameraModelUp,
    &Renderer::SetConstantCameraModelRight,
    &Renderer::SetConstantCameraWorldPosition,
    &Renderer::SetConstantCameraWorldDirection,
    &Renderer::SetConstantCameraWorldUp,
    &Renderer::SetConstantCameraWorldRight,

    &Renderer::SetConstantProjectorModelPosition,
    &Renderer::SetConstantProjectorModelDirection,
    &Renderer::SetConstantProjectorModelUp,
    &Renderer::SetConstantProjectorModelRight,
    &Renderer::SetConstantProjectorWorldPosition,
    &Renderer::SetConstantProjectorWorldDirection,
    &Renderer::SetConstantProjectorWorldUp,
    &Renderer::SetConstantProjectorWorldRight,
    &Renderer::SetConstantProjectorMatrix,

    &Renderer::SetConstantLightModelPosition,
    &Renderer::SetConstantLightModelDirection,
    &Renderer::SetConstantLightWorldPosition,
    &Renderer::SetConstantLightWorldDirection,
    &Renderer::SetConstantLightAmbient,
    &Renderer::SetConstantLightDiffuse,
    &Renderer::SetConstantLightSpecular,
    &Renderer::SetConstantLightSpotCutoff,
    &Renderer::SetConstantLightAttenuation
};

//----------------------------------------------------------------------------
Renderer::Renderer (FrameBuffer::FormatType eFormat,
    FrameBuffer::DepthType eDepth, FrameBuffer::StencilType eStencil,
    FrameBuffer::BufferingType eBuffering,
    FrameBuffer::MultisamplingType eMultisampling, int iWidth, int iHeight)
    :
    m_eFormat(eFormat),
    m_eDepth(eDepth),
    m_eStencil(eStencil),
    m_eBuffering(eBuffering),
    m_eMultisampling(eMultisampling),
    m_iWidth(iWidth),
    m_iHeight(iHeight),
    m_kClearColor(ColorRGBA::WHITE),
    m_fClearDepth(1.0f),
    m_uiClearStencil(0),
    m_kWorldMatrix(Matrix4f::IDENTITY),
    m_kSaveWorldMatrix(Matrix4f::IDENTITY),
    m_kViewMatrix(Matrix4f::IDENTITY),
    m_kSaveViewMatrix(Matrix4f::IDENTITY),
    m_kProjectionMatrix(Matrix4f::IDENTITY),
    m_kSaveProjectionMatrix(Matrix4f::IDENTITY),
    m_kPostProjectionMatrix(Matrix4f::IDENTITY),
    m_bPostProjectionIsIdentity(true)
{
    // Derived classes should set the maximum values, then allocate the
    // m_aspkLight array accordingly.
    m_iNumActiveSamplers = 0;
    m_iCurrentSampler = 0;
    m_apkActiveSamplers = 0;
    m_aspkLight = 0;

    m_pkCamera = 0;
    m_pkProjector = 0;
    m_pkGeometry = 0;
    m_bReverseCullFace = false;
    m_bFullscreen = false;
    m_iFontID = 0;

    m_pkLockedTexture = 0;
    m_iLockedFace = 0;
}
//----------------------------------------------------------------------------
Renderer::~Renderer ()
{
    WM4_DELETE[] m_apkActiveSamplers;
    WM4_DELETE[] m_aspkLight;
    SetCamera(0);
}
//----------------------------------------------------------------------------
void Renderer::Activate ()
{
    OnViewportChange();
    OnFrustumChange();
    if (m_pkCamera)
    {
        OnFrameChange ();
    }
}
//----------------------------------------------------------------------------
void Renderer::SetCamera (Camera* pkCamera)
{
    if (m_pkCamera)
    {
        m_pkCamera->m_pkRenderer = 0;
    }

    if (pkCamera)
    {
        pkCamera->m_pkRenderer = this;
    }

    m_pkCamera = pkCamera;

    if (m_pkCamera)
    {
        OnFrustumChange();
        OnViewportChange();
        OnFrameChange();
    }
}
//----------------------------------------------------------------------------
void Renderer::OnFrameChange ()
{
    Vector3f kEye = m_pkCamera->GetLocation();
    Vector3f kRVector = m_pkCamera->GetRVector();
    Vector3f kUVector = m_pkCamera->GetUVector();
    Vector3f kDVector = m_pkCamera->GetDVector();

    m_kViewMatrix[0][0] = kRVector[0];
    m_kViewMatrix[0][1] = kUVector[0];
    m_kViewMatrix[0][2] = kDVector[0];
    m_kViewMatrix[0][3] = 0.0f;
    m_kViewMatrix[1][0] = kRVector[1];
    m_kViewMatrix[1][1] = kUVector[1];
    m_kViewMatrix[1][2] = kDVector[1];
    m_kViewMatrix[1][3] = 0.0f;
    m_kViewMatrix[2][0] = kRVector[2];
    m_kViewMatrix[2][1] = kUVector[2];
    m_kViewMatrix[2][2] = kDVector[2];
    m_kViewMatrix[2][3] = 0.0f;
    m_kViewMatrix[3][0] = -kRVector.Dot(kEye);
    m_kViewMatrix[3][1] = -kUVector.Dot(kEye);
    m_kViewMatrix[3][2] = -kDVector.Dot(kEye);
    m_kViewMatrix[3][3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::OnFrustumChange ()
{
    if (!m_pkCamera)
    {
        return;
    }

    float fRMin, fRMax, fUMin, fUMax, fDMin, fDMax;
    m_pkCamera->GetFrustum(fRMin,fRMax,fUMin,fUMax,fDMin,fDMax);

    float fInvRDiff = 1.0f/(fRMax - fRMin);
    float fInvUDiff = 1.0f/(fUMax - fUMin);
    float fInvDDiff = 1.0f/(fDMax - fDMin);

    if (m_pkCamera->Perspective)
    {
        if (GetType() == OPENGL)
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[-1,1].
            m_kProjectionMatrix[0][0] = 2.0f*fDMin*fInvRDiff;
            m_kProjectionMatrix[0][1] = 0.0f;
            m_kProjectionMatrix[0][2] = 0.0f;
            m_kProjectionMatrix[0][3] = 0.0f;
            m_kProjectionMatrix[1][0] = 0.0f;
            m_kProjectionMatrix[1][1] = 2.0f*fDMin*fInvUDiff;
            m_kProjectionMatrix[1][2] = 0.0f;
            m_kProjectionMatrix[1][3] = 0.0f;
            m_kProjectionMatrix[2][0] = -(fRMin + fRMax)*fInvRDiff;
            m_kProjectionMatrix[2][1] = -(fUMin + fUMax)*fInvUDiff;
            m_kProjectionMatrix[2][2] = (fDMin + fDMax)*fInvDDiff;
            m_kProjectionMatrix[2][3] = 1.0f;
            m_kProjectionMatrix[3][0] = 0.0f;
            m_kProjectionMatrix[3][1] = 0.0f;
            m_kProjectionMatrix[3][2] = -2.0f*fDMax*fDMin*fInvDDiff;
            m_kProjectionMatrix[3][3] = 0.0f;
        }
        else
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[0,1].
            m_kProjectionMatrix[0][0] = 2.0f*fDMin*fInvRDiff;
            m_kProjectionMatrix[0][1] = 0.0f;
            m_kProjectionMatrix[0][2] = 0.0f;
            m_kProjectionMatrix[0][3] = 0.0f;
            m_kProjectionMatrix[1][0] = 0.0f;
            m_kProjectionMatrix[1][1] = 2.0f*fDMin*fInvUDiff;
            m_kProjectionMatrix[1][2] = 0.0f;
            m_kProjectionMatrix[1][3] = 0.0f;
            m_kProjectionMatrix[2][0] = -(fRMin + fRMax)*fInvRDiff;
            m_kProjectionMatrix[2][1] = -(fUMin + fUMax)*fInvUDiff;
            m_kProjectionMatrix[2][2] = fDMax*fInvDDiff;
            m_kProjectionMatrix[2][3] = 1.0f;
            m_kProjectionMatrix[3][0] = 0.0f;
            m_kProjectionMatrix[3][1] = 0.0f;
            m_kProjectionMatrix[3][2] = -fDMax*fDMin*fInvDDiff;
            m_kProjectionMatrix[3][3] = 0.0f;
        }
    }
    else
    {
        if (GetType() == OPENGL)
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[-1,1].
            m_kProjectionMatrix[0][0] = 2.0f*fInvRDiff;
            m_kProjectionMatrix[0][1] = 0.0f;
            m_kProjectionMatrix[0][2] = 0.0f;
            m_kProjectionMatrix[0][3] = 0.0f;
            m_kProjectionMatrix[1][0] = 0.0f;
            m_kProjectionMatrix[1][1] = 2.0f*fInvUDiff;
            m_kProjectionMatrix[1][2] = 0.0f;
            m_kProjectionMatrix[1][3] = 0.0f;
            m_kProjectionMatrix[2][0] = 0.0f;
            m_kProjectionMatrix[2][1] = 0.0f;
            m_kProjectionMatrix[2][2] = 2.0f*fInvDDiff;
            m_kProjectionMatrix[2][3] = 0.0f;
            m_kProjectionMatrix[3][0] = -(fRMin + fRMax)*fInvRDiff;
            m_kProjectionMatrix[3][1] = -(fUMin + fUMax)*fInvUDiff;
            m_kProjectionMatrix[3][2] = -(fDMin + fDMax)*fInvDDiff;
            m_kProjectionMatrix[3][3] = 1.0f;
        }
        else
        {
            // Map (x,y,z) into [-1,1]x[-1,1]x[0,1].
            m_kProjectionMatrix[0][0] = 2.0f*fInvRDiff;
            m_kProjectionMatrix[0][1] = 0.0f;
            m_kProjectionMatrix[0][2] = 0.0f;
            m_kProjectionMatrix[0][3] = 0.0f;
            m_kProjectionMatrix[1][0] = 0.0f;
            m_kProjectionMatrix[1][1] = 2.0f*fInvUDiff;
            m_kProjectionMatrix[1][2] = 0.0f;
            m_kProjectionMatrix[1][3] = 0.0f;
            m_kProjectionMatrix[2][0] = 0.0f;
            m_kProjectionMatrix[2][1] = 0.0f;
            m_kProjectionMatrix[2][2] = fInvDDiff;
            m_kProjectionMatrix[2][3] = 0.0f;
            m_kProjectionMatrix[3][0] = -(fRMin + fRMax)*fInvRDiff;
            m_kProjectionMatrix[3][1] = -(fUMin + fUMax)*fInvUDiff;
            m_kProjectionMatrix[3][2] = -fDMin*fInvDDiff;
            m_kProjectionMatrix[3][3] = 1.0f;
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::Resize (int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    OnViewportChange();
}
//----------------------------------------------------------------------------
void Renderer::SetGlobalState (GlobalStatePtr aspkState[])
{
    GlobalState* pkState = aspkState[GlobalState::ALPHA];
    if (pkState)
    {
        SetAlphaState((AlphaState*)pkState);
    }

    pkState = aspkState[GlobalState::CULL];
    if (pkState)
    {
        SetCullState((CullState*)pkState);
    }

    pkState = aspkState[GlobalState::MATERIAL];
    if (pkState)
    {
        SetMaterialState((MaterialState*)pkState);
    }

    pkState = aspkState[GlobalState::POLYGONOFFSET];
    if (pkState)
    {
        SetPolygonOffsetState((PolygonOffsetState*)pkState);
    }

    pkState = aspkState[GlobalState::STENCIL];
    if (pkState)
    {
        SetStencilState((StencilState*)pkState);
    }

    pkState = aspkState[GlobalState::WIREFRAME];
    if (pkState)
    {
        SetWireframeState((WireframeState*)pkState);
    }

    pkState = aspkState[GlobalState::ZBUFFER];
    if (pkState)
    {
        SetZBufferState((ZBufferState*)pkState);
    }
}
//----------------------------------------------------------------------------
void Renderer::RestoreGlobalState (GlobalStatePtr aspkState[])
{
    GlobalState* pkState;
    
    if (aspkState[GlobalState::ALPHA])
    {
        pkState = GlobalState::Default[GlobalState::ALPHA];
        SetAlphaState((AlphaState*)pkState);
    }

    if (aspkState[GlobalState::CULL])
    {
        pkState = GlobalState::Default[GlobalState::CULL];
        SetCullState((CullState*)pkState);
    }

    if (aspkState[GlobalState::MATERIAL])
    {
        pkState = GlobalState::Default[GlobalState::MATERIAL];
        SetMaterialState((MaterialState*)pkState);
    }

    if (aspkState[GlobalState::POLYGONOFFSET])
    {
        pkState = GlobalState::Default[GlobalState::POLYGONOFFSET];
        SetPolygonOffsetState((PolygonOffsetState*)pkState);
    }

    if (aspkState[GlobalState::STENCIL])
    {
        pkState = GlobalState::Default[GlobalState::STENCIL];
        SetStencilState((StencilState*)pkState);
    }

    if (aspkState[GlobalState::WIREFRAME])
    {
        pkState = GlobalState::Default[GlobalState::WIREFRAME];
        SetWireframeState((WireframeState*)pkState);
    }

    if (aspkState[GlobalState::ZBUFFER])
    {
        pkState = GlobalState::Default[GlobalState::ZBUFFER];
        SetZBufferState((ZBufferState*)pkState);
    }
}
//----------------------------------------------------------------------------
void Renderer::SetPostWorldTransformation (const Matrix4f& rkMatrix)
{
    m_kSaveViewMatrix = m_kViewMatrix;
    m_kViewMatrix = rkMatrix.TransposeTimes(m_kSaveViewMatrix);
}
//----------------------------------------------------------------------------
void Renderer::RestorePostWorldTransformation ()
{
    m_kViewMatrix = m_kSaveViewMatrix;
}
//----------------------------------------------------------------------------
void Renderer::SetPostProjectionTransformation (const Matrix4f& rkMatrix,
    bool bReverseFaces)
{
    m_kPostProjectionMatrix = rkMatrix;
    m_bPostProjectionIsIdentity = false;
    m_bReverseCullFace = bReverseFaces;
}
//----------------------------------------------------------------------------
void Renderer::RestorePostProjectionTransformation ()
{
    m_kPostProjectionMatrix = Matrix4f::IDENTITY;
    m_bPostProjectionIsIdentity = true;
    m_bReverseCullFace = false;
}
//----------------------------------------------------------------------------
void Renderer::SetWorldTransformation ()
{
    m_kWorldMatrix = m_pkGeometry->HWorld;
}
//----------------------------------------------------------------------------
void Renderer::RestoreWorldTransformation ()
{
    // Stub for derived classes.
}
//----------------------------------------------------------------------------
void Renderer::SetColorMask (bool bAllowRed, bool bAllowGreen,
    bool bAllowBlue, bool bAllowAlpha)
{
    m_bAllowRed = bAllowRed;
    m_bAllowGreen = bAllowGreen;
    m_bAllowBlue = bAllowBlue;
    m_bAllowAlpha = bAllowAlpha;
}
//----------------------------------------------------------------------------
void Renderer::GetColorMask (bool& rbAllowRed, bool& rbAllowGreen,
    bool& rbAllowBlue, bool& rbAllowAlpha)
{
    rbAllowRed = m_bAllowRed;
    rbAllowGreen = m_bAllowGreen;
    rbAllowBlue = m_bAllowBlue;
    rbAllowAlpha = m_bAllowAlpha;
}
//----------------------------------------------------------------------------
void Renderer::ComputeViewportAABB (const VertexBuffer* pkVBuffer,
    const Transformation& rkTransform, float& rfXMin, float& rfXMax,
    float& rfYMin, float& rfYMax)
{
    assert(m_pkCamera && pkVBuffer->GetAttributes().HasPosition());

    Matrix4f kWorldMatrix;
    rkTransform.GetHomogeneous(kWorldMatrix);
    Matrix4f kWVPMatrix = kWorldMatrix * m_kViewMatrix * m_kProjectionMatrix;
    if (!m_bPostProjectionIsIdentity)
    {
        kWVPMatrix = kWVPMatrix*m_kPostProjectionMatrix;
    }

    // Compute the bounding rectangle in normalized display coordinates.
    rfXMin = Mathf::MAX_REAL;
    rfXMax = -Mathf::MAX_REAL;
    float fYMin = Mathf::MAX_REAL;
    float fYMax = -Mathf::MAX_REAL;
    const int iVQuantity = pkVBuffer->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        Vector3f kPos = pkVBuffer->Position3(i);
        Vector4f kHPos = Vector4f(kPos[0],kPos[1],kPos[2],1.0f)*kWVPMatrix;
        float fInvW = 1.0f/kHPos[3];
        float fXNdc = kHPos[0]*fInvW;
        float fYNdc = -kHPos[1]*fInvW;  // Reflect y in window coordinates.
        if (fXNdc < rfXMin)
        {
            rfXMin = fXNdc;
        }
        if (fXNdc > rfXMax)
        {
            rfXMax = fXNdc;
        }
        if (fYNdc < fYMin)
        {
            fYMin = fYNdc;
        }
        if (fYNdc > fYMax)
        {
            fYMax = fYNdc;
        }
    }

    // Map the rectangle to window space.
    float fLeft, fRight, fTop, fBottom;
    m_pkCamera->GetViewport(fLeft,fRight,fTop,fBottom);
    float fRpL = fRight + fLeft;
    float fRmL = fRight - fLeft;
    float fTmB = fTop - fBottom;
    float f2mBmT = 2.0f - fBottom - fTop;
    float fHalfW = 0.5f*fRmL*m_iWidth;
    float fHalfH = 0.5f*fTmB*m_iHeight;

    rfXMin = fHalfW*(fRpL + fRmL*rfXMin);
    rfXMax = fHalfW*(fRpL + fRmL*rfXMax);

    // fBmT < 0, so ymin and ymax ordering needs to be handled correctly.
    rfYMax = fHalfH*(f2mBmT - fTmB*fYMin);
    rfYMin = fHalfH*(f2mBmT - fTmB*fYMax);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// set renderer constants required by shader programs
//----------------------------------------------------------------------------
void Renderer::GetTransform (const Matrix4f& rkMat, int iOperation,
    float* afData)
{
    const size_t uiSize = 16*sizeof(float);

    if (iOperation == 0)
    {
        // matrix
        System::Memcpy(afData,uiSize,(const float*)rkMat,uiSize);
    }
    else if (iOperation == 1)
    {
        // transpose of matrix
        Matrix4f kMT = rkMat.Transpose();
        System::Memcpy(afData,uiSize,(const float*)kMT,uiSize);
    }
    else if (iOperation == 2)
    {
        // inverse of matrix
        Matrix4f kMI = rkMat.Inverse();
        System::Memcpy(afData,uiSize,(const float*)kMI,uiSize);
    }
    else
    {
        // inverse-transpose of matrix
        Matrix4f kMIT = rkMat.Inverse().Transpose();
        System::Memcpy(afData,uiSize,(const float*)kMIT,uiSize);
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantWMatrix (int iOperation, float* afData)
{
    GetTransform(m_kWorldMatrix,iOperation,afData);
}
//----------------------------------------------------------------------------
void Renderer::SetConstantVMatrix (int iOperation, float* afData)
{
    GetTransform(m_kViewMatrix,iOperation,afData);
}
//----------------------------------------------------------------------------
void Renderer::SetConstantPMatrix (int iOperation, float* afData)
{
    if (m_bPostProjectionIsIdentity)
    {
        GetTransform(m_kProjectionMatrix,iOperation,afData);
    }
    else
    {
        Matrix4f kMatrix = m_kProjectionMatrix*m_kPostProjectionMatrix;
        GetTransform(kMatrix,iOperation,afData);
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantWVMatrix (int iOperation, float* afData)
{
    Matrix4f kWV = m_kWorldMatrix*m_kViewMatrix;
    GetTransform(kWV,iOperation,afData);
}
//----------------------------------------------------------------------------
void Renderer::SetConstantVPMatrix (int iOperation, float* afData)
{
    Matrix4f kVP = m_kViewMatrix*m_kProjectionMatrix;
    if (!m_bPostProjectionIsIdentity)
    {
        kVP = kVP*m_kPostProjectionMatrix;
    }
    GetTransform(kVP,iOperation,afData);
}
//----------------------------------------------------------------------------
void Renderer::SetConstantWVPMatrix (int iOperation, float* afData)
{
    Matrix4f kWVP = m_kWorldMatrix*m_kViewMatrix*m_kProjectionMatrix;
    if (!m_bPostProjectionIsIdentity)
    {
        kWVP = kWVP*m_kPostProjectionMatrix;
    }
    GetTransform(kWVP,iOperation,afData);
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorMatrix (int, float* afData)
{
    assert(m_pkProjector);

    // Set up the view matrix for the projector.
    const Vector3f& rkEye = m_pkProjector->GetLocation();
    const Vector3f& rkRVector = m_pkProjector->GetRVector();
    const Vector3f& rkUVector = m_pkProjector->GetUVector();
    const Vector3f& rkDVector = m_pkProjector->GetDVector();
    float fRdE = rkRVector.Dot(rkEye);
    float fUdE = rkUVector.Dot(rkEye);
    float fDdE = rkDVector.Dot(rkEye);
    Matrix4f kProjVMatrix(
        rkRVector[0], rkUVector[0], rkDVector[0], 0.0f,
        rkRVector[1], rkUVector[1], rkDVector[1], 0.0f,
        rkRVector[2], rkUVector[2], rkDVector[2], 0.0f,
        -fRdE,        -fUdE,        -fDdE,        1.0f);

    // Set up the projection matrix for the projector.
    float fRMin, fRMax, fUMin, fUMax, fDMin, fDMax;
    m_pkProjector->GetFrustum(fRMin,fRMax,fUMin,fUMax,fDMin,fDMax);
    float fInvRDiff = 1.0f/(fRMax - fRMin);
    float fInvUDiff = 1.0f/(fUMax - fUMin);
    float fInvDDiff = 1.0f/(fDMax - fDMin);
    float fRTerm0 = fDMin*fInvRDiff;
    float fUTerm0 = fDMin*fInvUDiff;
    float fDTerm0 = fDMin*fInvDDiff;
    float fRTerm1 = -(fRMin+fRMax)*fInvRDiff;
    float fUTerm1 = -(fUMin+fUMax)*fInvUDiff;
    float fDTerm1 = fDMax*fInvDDiff;
    Matrix4f kProjPMatrix(
        2.0f*fRTerm0, 0.0f,         0.0f,           0.0f,
        0.0f,         2.0f*fUTerm0, 0.0f,           0.0f,
        fRTerm1,      fUTerm1,      fDTerm1,        1.0f,
        0.0f,         0.0f,         -fDMax*fDTerm0, 0.0f);

    // Set up the bias and scale matrix for the projector.
    Matrix4f kProjBSMatrix(
        0.5f,0.0f,0.0f,0.0f,
        0.0f,0.5f,0.0f,0.0f,
        0.0f,0.0f,1.0f,0.0f,
        0.5f,0.5f,0.0f,1.0f);

    // The projector matrix.
    Matrix4f kProjectorMatrix =
        m_kWorldMatrix * kProjVMatrix * kProjPMatrix * kProjBSMatrix;

    GetTransform(kProjectorMatrix,0,afData);
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialEmissive (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_aspkState[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Emissive.R();
    afData[1] = pkMaterial->Emissive.G();
    afData[2] = pkMaterial->Emissive.B();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialAmbient (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_aspkState[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Ambient.R();
    afData[1] = pkMaterial->Ambient.G();
    afData[2] = pkMaterial->Ambient.B();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialDiffuse (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_aspkState[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Diffuse.R();
    afData[1] = pkMaterial->Diffuse.G();
    afData[2] = pkMaterial->Diffuse.B();
    afData[3] = pkMaterial->Alpha;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialSpecular (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_aspkState[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Specular.R();
    afData[1] = pkMaterial->Specular.G();
    afData[2] = pkMaterial->Specular.B();
    afData[3] = pkMaterial->Shininess;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraModelPosition (int, float* afData)
{
    Vector3f kMLocation = m_pkGeometry->World.ApplyInverse(
        m_pkCamera->GetLocation());

    afData[0] = kMLocation.X();
    afData[1] = kMLocation.Y();
    afData[2] = kMLocation.Z();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraModelDirection (int, float* afData)
{
    Vector3f kMDVector = m_pkGeometry->World.InvertVector(
        m_pkCamera->GetDVector());

    afData[0] = kMDVector.X();
    afData[1] = kMDVector.Y();
    afData[2] = kMDVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraModelUp (int, float* afData)
{
    Vector3f kMUVector = m_pkGeometry->World.InvertVector(
        m_pkCamera->GetUVector());

    afData[0] = kMUVector.X();
    afData[1] = kMUVector.Y();
    afData[2] = kMUVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraModelRight (int, float* afData)
{
    Vector3f kMRVector = m_pkGeometry->World.InvertVector(
        m_pkCamera->GetRVector());

    afData[0] = kMRVector.X();
    afData[1] = kMRVector.Y();
    afData[2] = kMRVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraWorldPosition (int, float* afData)
{
    Vector3f kWLocation = m_pkCamera->GetLocation();

    afData[0] = kWLocation.X();
    afData[1] = kWLocation.Y();
    afData[2] = kWLocation.Z();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraWorldDirection (int, float* afData)
{
    Vector3f kWDVector = m_pkCamera->GetDVector();

    afData[0] = kWDVector.X();
    afData[1] = kWDVector.Y();
    afData[2] = kWDVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraWorldUp (int, float* afData)
{
    Vector3f kWUVector = m_pkCamera->GetUVector();

    afData[0] = kWUVector.X();
    afData[1] = kWUVector.Y();
    afData[2] = kWUVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraWorldRight (int, float* afData)
{
    Vector3f kWRVector = m_pkCamera->GetRVector();

    afData[0] = kWRVector.X();
    afData[1] = kWRVector.Y();
    afData[2] = kWRVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorModelPosition (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kMLocation = m_pkGeometry->World.ApplyInverse(
        m_pkProjector->GetLocation());

    afData[0] = kMLocation.X();
    afData[1] = kMLocation.Y();
    afData[2] = kMLocation.Z();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorModelDirection (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kMDVector = m_pkGeometry->World.InvertVector(
        m_pkProjector->GetDVector());

    afData[0] = kMDVector.X();
    afData[1] = kMDVector.Y();
    afData[2] = kMDVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorModelUp (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kMUVector = m_pkGeometry->World.InvertVector(
        m_pkProjector->GetUVector());

    afData[0] = kMUVector.X();
    afData[1] = kMUVector.Y();
    afData[2] = kMUVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorModelRight (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kMRVector = m_pkGeometry->World.InvertVector(
        m_pkProjector->GetRVector());

    afData[0] = kMRVector.X();
    afData[1] = kMRVector.Y();
    afData[2] = kMRVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorWorldPosition (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kWLocation = m_pkProjector->GetLocation();

    afData[0] = kWLocation.X();
    afData[1] = kWLocation.Y();
    afData[2] = kWLocation.Z();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorWorldDirection (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kWDVector = m_pkProjector->GetDVector();

    afData[0] = kWDVector.X();
    afData[1] = kWDVector.Y();
    afData[2] = kWDVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorWorldUp (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kWUVector = m_pkProjector->GetUVector();

    afData[0] = kWUVector.X();
    afData[1] = kWUVector.Y();
    afData[2] = kWUVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantProjectorWorldRight (int, float* afData)
{
    assert(m_pkProjector);

    Vector3f kWRVector = m_pkProjector->GetRVector();

    afData[0] = kWRVector.X();
    afData[1] = kWRVector.Y();
    afData[2] = kWRVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightModelPosition (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    if (pkLight)
    {
        Vector3f kMPosition = m_pkGeometry->World.ApplyInverse(
            pkLight->Position);

        afData[0] = kMPosition[0];
        afData[1] = kMPosition[1];
        afData[2] = kMPosition[2];
        afData[3] = 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightModelDirection (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    if (pkLight)
    {
        Vector3f kMDVector = m_pkGeometry->World.InvertVector(
            pkLight->DVector);

        // If the model-to-world transformation has scaling, the direction
        // vector must be normalized.
        kMDVector.Normalize();

        afData[0] = kMDVector[0];
        afData[1] = kMDVector[1];
        afData[2] = kMDVector[2];
        afData[3] = 0.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightWorldPosition (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    if (pkLight)
    {
        afData[0] = pkLight->Position[0];
        afData[1] = pkLight->Position[1];
        afData[2] = pkLight->Position[2];
        afData[3] = 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightWorldDirection (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    if (pkLight)
    {
        // If the light's direction was set via Light::SetDirection(), then
        // DVector should be unit length.  If the light is managed by a
        // LightNode, then LightNode::UpdateWorldData() guarantees that
        // DVector is unit length.  However, to ensure we have a unit-length
        // vector, normalize anyway.
        Vector3f kWDVector = pkLight->DVector;
        kWDVector.Normalize();

        afData[0] = kWDVector[0];
        afData[1] = kWDVector[1];
        afData[2] = kWDVector[2];
        afData[3] = 0.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightAmbient (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    assert(pkLight);
    afData[0] = pkLight->Ambient.R();
    afData[1] = pkLight->Ambient.G();
    afData[2] = pkLight->Ambient.B();
    afData[3] = 1.0f;
 }
//----------------------------------------------------------------------------
void Renderer::SetConstantLightDiffuse (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    if (pkLight)
    {
        afData[0] = pkLight->Diffuse[0];
        afData[1] = pkLight->Diffuse[1];
        afData[2] = pkLight->Diffuse[2];
        afData[3] = 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightSpecular (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    assert(pkLight);
    afData[0] = pkLight->Specular.R();
    afData[1] = pkLight->Specular.G();
    afData[2] = pkLight->Specular.B();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightSpotCutoff (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    if (pkLight)
    {
        afData[0] = pkLight->Angle;
        afData[1] = pkLight->CosAngle;
        afData[2] = pkLight->SinAngle;
        afData[3] = pkLight->Exponent;
    }
    else
    {
        afData[0] = Mathf::PI;
        afData[1] = -1.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightAttenuation (int iLight, float* afData)
{
    Light* pkLight = GetLight(iLight);
    if (pkLight)
    {
        afData[0] = pkLight->Constant;
        afData[1] = pkLight->Linear;
        afData[2] = pkLight->Quadratic;
        afData[3] = pkLight->Intensity;
    }
    else
    {
        afData[0] = 1.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetRendererConstant (RendererConstant::Type eRCType,
    float* afData)
{
    int iRCType = (int)eRCType;
    int iFunction;

    if (iRCType <= (int)RendererConstant::WVP_MATRIX_INVERSE_TRANSPOSE)
    {
        // The constant involves matrices.  These occur in blocks of 6:
        // matrix, transpose, inverse, inverse-transpose.
        int iOperation = iRCType / 6;
        iFunction = iRCType % 6;
        (this->*ms_aoSCFunction[iFunction])(iOperation,afData);
    }
    else if (iRCType <= (int)RendererConstant::PROJECTOR_MATRIX)
    {
        // The constant is for material, camera, or projector data.  The
        // first parameter is irrelevant, so just set it to zero.  The "6"
        // in iFunction is the index into ms_aoSCFunction for the function
        // SetConstantMaterialEmissive.  The remainder of iFunction is a
        // relative offset to locate the material, fog, camera, or projector
        // SetConstant* functions.  The maximum iFunction value is 26.
        iFunction = 6 + iRCType - (int)RendererConstant::MATERIAL_EMISSIVE;
        (this->*ms_aoSCFunction[iFunction])(0,afData);
    }
    else
    {
        // The constant involves lights.  These occur in blocks of 9:  model
        // position, model direction, world position, world direction,
        // ambient, diffuse, specular, spotcutoff, shininess.  The 27 in
        // iFunction is the index into ms_aoSCFunction for the function
        // SetConstantLightModelPosition.  The remainder of iFunction is a
        // relative offset to locate the light SetConstant* functions.  The
        // maximum iFunction value is 35.
        int iDiff = iRCType - (int)RendererConstant::LIGHT0_MODEL_POSITION;
        int iLight = iDiff / 9;
        int iAttribute = iDiff % 9;
        iFunction = 27 + iAttribute;
        (this->*ms_aoSCFunction[iFunction])(iLight,afData);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// load and release resources
//----------------------------------------------------------------------------
void Renderer::LoadAllResources (Spatial* pkScene)
{
    Geometry* pkGeometry = DynamicCast<Geometry>(pkScene);
    if (pkGeometry)
    {
        LoadResources(pkGeometry);
    }

    Node* pkNode = DynamicCast<Node>(pkScene);
    if (pkNode)

    {
        for (int i = 0; i < pkNode->GetQuantity(); i++)
        {
            Spatial* pkChild = pkNode->GetChild(i);
            if (pkChild)
            {
                LoadAllResources(pkChild);
            }
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleaseAllResources (Spatial* pkScene)
{
    Geometry* pkGeometry = DynamicCast<Geometry>(pkScene);
    if (pkGeometry)
    {
        ReleaseResources(pkGeometry);
    }

    Node* pkNode = DynamicCast<Node>(pkScene);
    if (pkNode)
    {
        for (int i = 0; i < pkNode->GetQuantity(); i++)
        {
            Spatial* pkChild = pkNode->GetChild(i);
            if (pkChild)
            {
                ReleaseAllResources(pkChild);
            }
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::LoadResources (Geometry* pkGeometry)
{
    assert(pkGeometry);
    assert(pkGeometry->VBuffer);
    assert(pkGeometry->IBuffer);

    // Load the index buffer into video memory.
    LoadIBuffer(pkGeometry->IBuffer);

    // Load the vertex buffer(s) and effects resources into video memory.
    const int iEffectQuantity = pkGeometry->GetEffectQuantity();
    for (int i = 0; i < iEffectQuantity; i++)
    {
        Effect* pkEffect = pkGeometry->GetEffect(i);
        pkEffect->LoadResources(this,pkGeometry);
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleaseResources (Geometry* pkGeometry)
{
    assert(pkGeometry);
    assert(pkGeometry->VBuffer);
    assert(pkGeometry->IBuffer);

    // Release the index buffer from video memory.
    ReleaseIBuffer(pkGeometry->IBuffer);

    // Release the vertex buffer(s) from video memory.
    while (pkGeometry->VBuffer->GetInfoQuantity(this) > 0)
    {
        ReleaseVBuffer(pkGeometry->VBuffer);
    }

    // Release the effects resources from video memory.
    const int iEffectQuantity = pkGeometry->GetEffectQuantity();
    for (int i = 0; i < iEffectQuantity; i++)
    {
        Effect* pkEffect = pkGeometry->GetEffect(i);
        pkEffect->ReleaseResources(this,pkGeometry);
    }
}
//----------------------------------------------------------------------------
void Renderer::LoadResources (Effect* pkEffect)
{
    assert(pkEffect);
    pkEffect->LoadResources(this,0);
}
//----------------------------------------------------------------------------
void Renderer::ReleaseResources (Effect* pkEffect)
{
    assert(pkEffect);
    pkEffect->ReleaseResources(this,0);
}
//----------------------------------------------------------------------------
void Renderer::LoadVProgram (VertexProgram* pkVProgram)
{
    if (!pkVProgram)
    {
        return;
    }

    ResourceIdentifier* pkID = pkVProgram->GetIdentifier(this);
    if (!pkID)
    {
        OnLoadVProgram(pkID,pkVProgram);
        pkVProgram->OnLoad(this,&Renderer::ReleaseVProgram,pkID);
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleaseVProgram (Bindable* pkVProgram)
{
    if (!pkVProgram)
    {
        return;
    }

    ResourceIdentifier* pkID = pkVProgram->GetIdentifier(this);
    if (pkID)
    {
        OnReleaseVProgram(pkID);
        pkVProgram->OnRelease(this,pkID);
    }
}
//----------------------------------------------------------------------------
void Renderer::LoadPProgram (PixelProgram* pkPProgram)
{
    if (!pkPProgram)
    {
        return;
    }

    ResourceIdentifier* pkID = pkPProgram->GetIdentifier(this);
    if (!pkID)
    {
        OnLoadPProgram(pkID,pkPProgram);
        pkPProgram->OnLoad(this,&Renderer::ReleasePProgram,pkID);
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleasePProgram (Bindable* pkPProgram)
{
    if (!pkPProgram)
    {
        return;
    }

    ResourceIdentifier* pkID = pkPProgram->GetIdentifier(this);
    if (pkID)
    {
        OnReleasePProgram(pkID);
        pkPProgram->OnRelease(this,pkID);
    }
}
//----------------------------------------------------------------------------
void Renderer::LoadTexture (Texture* pkTexture)
{
    if (!pkTexture)
    {
        return;
    }

    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    if (!pkID)
    {
        OnLoadTexture(pkID,pkTexture);
        pkTexture->OnLoad(this,&Renderer::ReleaseTexture,pkID);
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleaseTexture (Bindable* pkTexture)
{
    if (!pkTexture)
    {
        return;
    }

    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    if (pkID)
    {
        OnReleaseTexture(pkID);
        pkTexture->OnRelease(this,pkID);
    }
}
//----------------------------------------------------------------------------
void Renderer::LoadVBuffer (const Attributes& rkIAttr,
    const Attributes& rkOAttr, VertexBuffer* pkVBuffer)
{
    if (!pkVBuffer)
    {
        return;
    }

    // Search for a matching vertex buffer that was used during previous
    // passes.
    ResourceIdentifier* pkID = 0;
    for (int i = 0; i < pkVBuffer->GetInfoQuantity(); i++)
    {
        pkID = pkVBuffer->GetIdentifier(i,this);
        if (pkID)
        {
            if (rkIAttr == *(Attributes*)pkID)
            {
                // Found a matching vertex buffer in video memory.
                return;
            }
        }
    }

    // The vertex buffer is encountered the first time.
    OnLoadVBuffer(pkID,rkIAttr,rkOAttr,pkVBuffer);
    pkVBuffer->OnLoad(this,&Renderer::ReleaseVBuffer,pkID);
}
//----------------------------------------------------------------------------
void Renderer::ReleaseVBuffer (Bindable* pkVBuffer)
{
    if (!pkVBuffer)
    {
        return;
    }

    for (int i = 0; i < pkVBuffer->GetInfoQuantity(); i++)
    {
        ResourceIdentifier* pkID = pkVBuffer->GetIdentifier(i,this);
        if (pkID)
        {
            OnReleaseVBuffer(pkID);
            pkVBuffer->OnRelease(this,pkID);
            return;
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::LoadIBuffer (IndexBuffer* pkIBuffer)
{
    if (!pkIBuffer)
    {
        return;
    }

    ResourceIdentifier* pkID = pkIBuffer->GetIdentifier(this);
    if (!pkID)
    {
        OnLoadIBuffer(pkID,pkIBuffer);
        pkIBuffer->OnLoad(this,&Renderer::ReleaseIBuffer,pkID);
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleaseIBuffer (Bindable* pkIBuffer)
{
    if (!pkIBuffer)
    {
        return;
    }

    ResourceIdentifier* pkID = pkIBuffer->GetIdentifier(this);
    if (pkID)
    {
        OnReleaseIBuffer(pkID);
        pkIBuffer->OnRelease(this,pkID);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// enable and disable resources
//----------------------------------------------------------------------------
void Renderer::EnableVProgram (VertexProgram* pkVProgram)
{
    assert(pkVProgram);
    LoadVProgram(pkVProgram);
    ResourceIdentifier* pkID = pkVProgram->GetIdentifier(this);
    assert(pkID);

    OnEnableVProgram(pkID);

    // Process the sampler information.
    int i;
    for (i = 0; i < pkVProgram->GetSamplerInformationQuantity(); i++)
    {
        assert(m_iNumActiveSamplers < ms_iMaxPShaderImages);
        m_apkActiveSamplers[m_iNumActiveSamplers++] =
            pkVProgram->GetSamplerInformation(i);
    }

    // Process the renderer constants.
    for (i = 0; i < pkVProgram->GetRendererConstantQuantity(); i++)
    {
        RendererConstant* pkRC = pkVProgram->GetRendererConstant(i);
        assert(pkRC);
        SetRendererConstant(pkRC->GetType(),pkRC->GetData());
        SetVProgramConstant(CT_RENDERER,pkRC->GetBaseRegister(),
            pkRC->GetRegisterQuantity(),pkRC->GetData());
    }

    // Process the numerical constants.
    for (i = 0; i < pkVProgram->GetNumericalConstantQuantity(); i++)
    {
        NumericalConstant* pkNC = pkVProgram->GetNumericalConstant(i);
        assert(pkNC);
        SetVProgramConstant(CT_NUMERICAL,pkNC->GetRegister(),1,
            pkNC->GetData());
    }

    // Process the user-defined constants.
    for (i = 0; i < pkVProgram->GetUserConstantQuantity(); i++)
    {
        UserConstant* pkUC = pkVProgram->GetUserConstant(i);
        assert(pkUC);
        SetVProgramConstant(CT_USER,pkUC->GetBaseRegister(),
            pkUC->GetRegisterQuantity(),pkUC->GetData());
    }
}
//----------------------------------------------------------------------------
void Renderer::DisableVProgram (VertexProgram* pkVProgram)
{
    assert(pkVProgram);
    ResourceIdentifier* pkID = pkVProgram->GetIdentifier(this);
    assert(pkID);
    OnDisableVProgram(pkID);
}
//----------------------------------------------------------------------------
void Renderer::EnablePProgram (PixelProgram* pkPProgram)
{
    assert(pkPProgram);
    LoadPProgram(pkPProgram);
    ResourceIdentifier* pkID = pkPProgram->GetIdentifier(this);
    assert(pkID);

    OnEnablePProgram(pkID);

    // Process the sampler information.
    int i;
    for (i = 0; i < pkPProgram->GetSamplerInformationQuantity(); i++)
    {
        assert(m_iNumActiveSamplers < ms_iMaxPShaderImages);
        m_apkActiveSamplers[m_iNumActiveSamplers++] =
            pkPProgram->GetSamplerInformation(i);
    }

    // Process the renderer constants.
    for (i = 0; i < pkPProgram->GetRendererConstantQuantity(); i++)
    {
        RendererConstant* pkRC = pkPProgram->GetRendererConstant(i);
        assert(pkRC);
        SetRendererConstant(pkRC->GetType(),pkRC->GetData());
        SetPProgramConstant(CT_RENDERER,pkRC->GetBaseRegister(),
            pkRC->GetRegisterQuantity(),pkRC->GetData());
    }

    // Process the numerical constants.
    for (i = 0; i < pkPProgram->GetNumericalConstantQuantity(); i++)
    {
        NumericalConstant* pkNC = pkPProgram->GetNumericalConstant(i);
        assert(pkNC);
        SetPProgramConstant(CT_NUMERICAL,pkNC->GetRegister(),1,
            pkNC->GetData());
    }

    // Process the user-defined constants.
    for (i = 0; i < pkPProgram->GetUserConstantQuantity(); i++)
    {
        UserConstant* pkUC = pkPProgram->GetUserConstant(i);
        assert(pkUC);
        SetPProgramConstant(CT_USER,pkUC->GetBaseRegister(),
            pkUC->GetRegisterQuantity(),pkUC->GetData());
    }
}
//----------------------------------------------------------------------------
void Renderer::DisablePProgram (PixelProgram* pkPProgram)
{
    assert(pkPProgram);
    ResourceIdentifier* pkID = pkPProgram->GetIdentifier(this);
    assert(pkID);
    OnDisablePProgram(pkID);
}
//----------------------------------------------------------------------------
void Renderer::EnableTexture (Texture* pkTexture)
{
    assert(pkTexture);
    LoadTexture(pkTexture);
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    OnEnableTexture(pkID);
}
//----------------------------------------------------------------------------
void Renderer::DisableTexture (Texture* pkTexture)
{
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    OnDisableTexture(pkID);
}
//----------------------------------------------------------------------------
ResourceIdentifier* Renderer::EnableVBuffer (const Attributes& rkIAttr,
    const Attributes& rkOAttr)
{
    VertexBuffer* pkVBuffer = m_pkGeometry->VBuffer;
    LoadVBuffer(rkIAttr,rkOAttr,pkVBuffer);

    ResourceIdentifier* pkID = 0;
    for (int i = 0; i < pkVBuffer->GetInfoQuantity(); i++)
    {
        pkID = pkVBuffer->GetIdentifier(i,this);
        if (pkID)
        {
            if (rkIAttr == *(Attributes*)pkID)
            {
                // Found a matching vertex buffer in video memory.
                break;
            }
        }
    }
    assert(pkID);

    OnEnableVBuffer(pkID);
    return pkID;
}
//----------------------------------------------------------------------------
void Renderer::DisableVBuffer (ResourceIdentifier* pkID)
{
    OnDisableVBuffer(pkID);
}
//----------------------------------------------------------------------------
void Renderer::EnableIBuffer ()
{
    IndexBuffer* pkIBuffer = m_pkGeometry->IBuffer;
    LoadIBuffer(pkIBuffer);
    ResourceIdentifier* pkID = pkIBuffer->GetIdentifier(this);
    assert(pkID);
    OnEnableIBuffer(pkID);
}
//----------------------------------------------------------------------------
void Renderer::DisableIBuffer ()
{
    IndexBuffer* pkIBuffer = m_pkGeometry->IBuffer;
    ResourceIdentifier* pkID = pkIBuffer->GetIdentifier(this);
    assert(pkID);
    OnDisableIBuffer(pkID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// drawing functions
//----------------------------------------------------------------------------
void Renderer::DrawScene (VisibleSet& rkVisibleSet)
{
    // NOTE:  The stack of 2-tuples is limited to having 64 elements.  This
    // should be plenty, because the chances of having 64 global effects
    // in the same path is small (that is a *lot* of effects to apply in
    // one frame).  If it needs to be larger for your applications, increase
    // the maximum size.
    const int iMaxTuples = 64;    // maximum number of stack elements
    int aaiStack[iMaxTuples][2];  // elements are (startIndex,finalIndex)
    int iTop = -1;                // stack is initially empty

    const int iVisibleQuantity = rkVisibleSet.GetQuantity();
    VisibleObject* akVisible = rkVisibleSet.GetVisible();
    for (int i = 0; i < iVisibleQuantity; i++)
    {
        if (akVisible[i].Object)
        {
            if (akVisible[i].GlobalEffect)
            {
                // Begin the scope of a global effect.
                iTop++;
                assert(iTop < iMaxTuples);
                aaiStack[iTop][0] = i;
                aaiStack[iTop][1] = i;
            }
            else
            {
                // Found a leaf Geometry object.
                if (iTop == -1)
                {
                    Draw((Geometry*)akVisible[i].Object);
                }
                else
                {
                    aaiStack[iTop][1]++;
                }
            }
        }
        else
        {
            // End the scope of a global effect.
            assert(!akVisible[i].GlobalEffect);
            int jMin = aaiStack[iTop][0];
            int jMax = aaiStack[iTop][1];

            akVisible[jMin].GlobalEffect->Draw(this,akVisible[jMin].Object,
                jMin+1,jMax,akVisible);

            if (--iTop >= 0)
            {
                aaiStack[iTop][1] = jMax + 1;
            }
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::ApplyEffect (ShaderEffect* pkEffect, bool& rbPrimaryEffect)
{
    const int iPassQuantity = pkEffect->GetPassQuantity();
    for (int iPass = 0; iPass < iPassQuantity; iPass++)
    {
        // The global state must be set before enabling programs because the
        // programs set sampler state for samplers about to be enabled.
        pkEffect->SetGlobalState(iPass,this,rbPrimaryEffect);

        // Keep track of the current number of active samplers.
        m_iNumActiveSamplers = 0;

        // Derived classes can set the data sources of the UserConstant
        // objects in the shader programs.  Set the sources before
        // EnableVProgram and EnablePProgram, because those functions
        // pass the data sources to the renderers for reading the
        // shader constants.
        pkEffect->ConnectVShaderConstants(iPass);
        pkEffect->ConnectPShaderConstants(iPass);

        // Enable the vertex program.  This indirectly calls LoadVProgram,
        // which copies the SamplerInformation pointers into
        // m_apkActiveSamplers and increments m_iNumActiveSamplers
        // accordingly.
        VertexProgram* pkVProgram = pkEffect->GetVProgram(iPass);
        EnableVProgram(pkVProgram);

        // Enable the pixel program.  This indirectly calls LoadVProgram,
        // which copies the SamplerInformation pointers into
        // m_apkActiveSamplers and increments m_iNumActiveSamplers
        // accordingly.
        PixelProgram* pkPProgram = pkEffect->GetPProgram(iPass);
        EnablePProgram(pkPProgram);

        // Keep track of the current sampler to be used in enabling the
        // textures.
        m_iCurrentSampler = 0;

        // Enable the textures used by the vertex program.  The derived-class
        // renderers use m_iCurrentSampler as the index into
        // m_apkActiveSamplers
        const int iVTQuantity = pkEffect->GetVTextureQuantity(iPass);
        int iTexture;
        for (iTexture = 0; iTexture < iVTQuantity; iTexture++)
        {
            EnableTexture(pkEffect->GetVTexture(iPass,iTexture));
            m_iCurrentSampler++;
        }

        // Enable the textures used by the pixel program.
        const int iPTQuantity = pkEffect->GetPTextureQuantity(iPass);
        for (iTexture = 0; iTexture < iPTQuantity; iTexture++)
        {
            EnableTexture(pkEffect->GetPTexture(iPass,iTexture));
            m_iCurrentSampler++;
        }

        // Create or find a compatible vertex buffer for the vertex program
        // and enable it.
        const Attributes& rkIAttr = pkVProgram->GetInputAttributes();
        const Attributes& rkOAttr = pkVProgram->GetOutputAttributes();
        ResourceIdentifier* pkID = EnableVBuffer(rkIAttr,rkOAttr);

        DrawElements();

        // Disable the vertex buffer for the vertex program.
        DisableVBuffer(pkID);

        // Keep track of the current sampler to be used in disabling the
        // textures.
        m_iCurrentSampler = 0;

        // Disable the textures used by the vertex program.
        for (iTexture = 0; iTexture < iVTQuantity; iTexture++)
        {
            DisableTexture(pkEffect->GetVTexture(iPass,iTexture));
        }

        // Disable the textures used by the pixel program.
        for (iTexture = 0; iTexture < iPTQuantity; iTexture++)
        {
            DisableTexture(pkEffect->GetPTexture(iPass,iTexture));
        }

        // Disable the pixel program.
        DisablePProgram(pkPProgram);

        // Disable the vertex program.
        DisableVProgram(pkVProgram);

        // Restore the global state that was active before this pass.
        pkEffect->RestoreGlobalState(iPass,this,rbPrimaryEffect);
    }

    rbPrimaryEffect = false;
}
//----------------------------------------------------------------------------
void Renderer::Draw (Geometry* pkGeometry)
{
    m_pkGeometry = pkGeometry;

    SetGlobalState(m_pkGeometry->States);
    SetWorldTransformation();

    // Enable the index buffer.  The connectivity information is the same
    // across all effects and all passes per effect.
    EnableIBuffer();

    // Lighting (if any) is applied first.  Effects are applied second.
    bool bPrimaryEffect = true;
    const int iMin = m_pkGeometry->GetStartEffect();
    const int iMax = m_pkGeometry->GetEffectQuantity();
    for (int i = iMin; i < iMax; i++)
    {
        // The effect should be a ShaderEffect-derived object, but it is
        // possible that an incorrectly designed Effect-derived class
        // attaches itself to the Geometry object in its overloaded
        // Effect::Draw function before calling Renderer::Draw(Geometry*).
        ShaderEffect* pkEffect =
            DynamicCast<ShaderEffect>(m_pkGeometry->GetEffect(i));
        if (pkEffect)
        {
            ApplyEffect(pkEffect,bPrimaryEffect);
        }
        else
        {
            // Enable this line of code to trap problems with incorrectly
            // designed Effect-derived classes.
            //assert(false);
        }
    }

    // Disable the index buffer.
    DisableIBuffer();

    RestoreWorldTransformation();
    RestoreGlobalState(m_pkGeometry->States);

    m_pkGeometry = 0;
}
//----------------------------------------------------------------------------
