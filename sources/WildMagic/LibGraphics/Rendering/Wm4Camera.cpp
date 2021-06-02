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
// Version: 4.0.3 (2009/01/23)

#include "Wm4GraphicsPCH.h"
#include "Wm4Camera.h"
#include "Wm4Renderer.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Camera,Object);
WM4_IMPLEMENT_STREAM(Camera);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Camera,Object);

//----------------------------------------------------------------------------
Camera::Camera ()
{
    m_pkRenderer = 0;

    SetFrustum(-0.5f,0.5f,-0.5f,0.5f,1.0f,2.0f);
    SetViewport(0,0,0,0,0,0);
    SetDepthRange(0.0f,1.0f);
    SetFrame(Vector3f::ZERO,-Vector3f::UNIT_Z,Vector3f::UNIT_Y,
        Vector3f::UNIT_X);

    Perspective = true;
}
//----------------------------------------------------------------------------
Camera::~Camera ()
{
}
//----------------------------------------------------------------------------
void Camera::SetFrame (const Vector3f& rkLocation, const Vector3f& rkDVector,
    const Vector3f& rkUVector, const Vector3f& rkRVector)
{
    m_kLocation = rkLocation;
    SetAxes(rkDVector,rkUVector,rkRVector);
}
//----------------------------------------------------------------------------
void Camera::SetLocation (const Vector3f& rkLocation)
{
    m_kLocation = rkLocation;

    if (m_pkRenderer)
    {
        m_pkRenderer->OnFrameChange();
    }
}
//----------------------------------------------------------------------------
void Camera::SetAxes (const Vector3f& rkDVector, const Vector3f& rkUVector,
    const Vector3f& rkRVector)
{
    m_kDVector = rkDVector;
    m_kUVector = rkUVector;
    m_kRVector = rkRVector;

    float fADet = Mathf::FAbs(m_kDVector.Dot(m_kUVector.Cross(m_kRVector)));
    if (Mathf::FAbs(1.0f-fADet) > 0.01f)
    {
        // The input vectors do not appear to form an orthonormal set.  Time
        // to renormalize.
        Vector3f::Orthonormalize(m_kDVector,m_kUVector,m_kRVector);
    }

    if (m_pkRenderer)
    {
        m_pkRenderer->OnFrameChange();
    }
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (float fRMin, float fRMax, float fUMin, float fUMax,
    float fDMin, float fDMax)
{
    m_afFrustum[VF_DMIN] = fDMin;
    m_afFrustum[VF_DMAX] = fDMax;
    m_afFrustum[VF_UMIN] = fUMin;
    m_afFrustum[VF_UMAX] = fUMax;
    m_afFrustum[VF_RMIN] = fRMin;
    m_afFrustum[VF_RMAX] = fRMax;

    if (m_pkRenderer)
    {
        m_pkRenderer->OnFrustumChange();
    }
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (const float* afFrustum)
{
    size_t uiSize = VF_QUANTITY*sizeof(float);
    System::Memcpy(m_afFrustum,uiSize,afFrustum,uiSize);

    if (m_pkRenderer)
    {
        m_pkRenderer->OnFrustumChange();
    }
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (float fUpFovDegrees, float fAspectRatio, float fDMin,
    float fDMax)
{
    float fHalfAngleRadians = 0.5f*fUpFovDegrees*Mathf::DEG_TO_RAD;
    m_afFrustum[VF_UMAX] = fDMin*Mathf::Tan(fHalfAngleRadians);
    m_afFrustum[VF_RMAX] = fAspectRatio*m_afFrustum[VF_UMAX];
    m_afFrustum[VF_UMIN] = -m_afFrustum[VF_UMAX];
    m_afFrustum[VF_RMIN] = -m_afFrustum[VF_RMAX];
    m_afFrustum[VF_DMIN] = fDMin;
    m_afFrustum[VF_DMAX] = fDMax;

    if (m_pkRenderer)
    {
        m_pkRenderer->OnFrustumChange();
    }
}
//----------------------------------------------------------------------------
void Camera::GetFrustum (float& rfRMin, float& rfRMax, float& rfUMin,
    float& rfUMax, float& rfDMin, float& rfDMax) const
{
    rfDMin = m_afFrustum[VF_DMIN];
    rfDMax = m_afFrustum[VF_DMAX];
    rfUMin = m_afFrustum[VF_UMIN];
    rfUMax = m_afFrustum[VF_UMAX];
    rfRMin = m_afFrustum[VF_RMIN];
    rfRMax = m_afFrustum[VF_RMAX];
}
//----------------------------------------------------------------------------
bool Camera::GetFrustum (float& rfUpFovDegrees, float& rfAspectRatio,
    float& rfDMin, float& rfDMax) const
{
    if (m_afFrustum[VF_RMIN] == -m_afFrustum[VF_RMAX]
    &&  m_afFrustum[VF_UMIN] == -m_afFrustum[VF_UMAX])
    {
        float fTmp = m_afFrustum[VF_UMAX]/m_afFrustum[VF_DMIN];
        rfUpFovDegrees = 2.0f*Mathf::ATan(fTmp)*Mathf::RAD_TO_DEG;
        rfAspectRatio = m_afFrustum[VF_RMAX]/m_afFrustum[VF_UMAX];
        rfDMin = m_afFrustum[VF_DMIN];
        rfDMax = m_afFrustum[VF_DMAX];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Camera::SetViewport (int iLeft, int iRight, int iTop, int iBottom,
    int iWidth, int iHeight)
{
    if (iWidth > 0 && iHeight > 0)
    {
        assert(0 <= iLeft && iLeft <= iWidth);
        assert(0 <= iRight && iRight <= iWidth);
        assert(0 <= iBottom && iBottom <= iHeight);
        assert(0 <= iTop && iTop <= iHeight);

        m_iWidth = iWidth;
        m_iHeight = iHeight;
        m_iPortL = iLeft;
        m_iPortR = iRight;
        m_iPortB = iBottom;
        m_iPortT = iTop;

        m_fPortL = (float)iLeft/(float)iWidth;
        m_fPortR = (float)iRight/(float)iWidth;
        m_fPortB = (float)iBottom/(float)iHeight;
        m_fPortT = (float)iTop/(float)iHeight;
    }
    else
    {
        m_iWidth = 0;
        m_iHeight = 0;
        m_iPortL = 0;
        m_iPortR = 0;
        m_iPortB = 0;
        m_iPortT = 0;

        m_fPortL = 0.0f;
        m_fPortR = 1.0f;
        m_fPortB = 0.0f;
        m_fPortT = 1.0f;
    }

    if (m_pkRenderer)
    {
        m_pkRenderer->OnViewportChange();
    }
}
//----------------------------------------------------------------------------
void Camera::GetViewport (int& riLeft, int& riRight, int& riTop,
    int& riBottom, int& riWidth, int& riHeight)
{
    riLeft = m_iPortL;
    riRight = m_iPortR;
    riTop = m_iPortT;
    riBottom = m_iPortB;
    riWidth = m_iWidth;
    riHeight = m_iHeight;
}
//----------------------------------------------------------------------------
void Camera::GetViewport (float& rfLeft, float& rfRight, float& rfTop,
    float& rfBottom)
{
    rfLeft = m_fPortL;
    rfRight = m_fPortR;
    rfTop = m_fPortT;
    rfBottom = m_fPortB;
}
//----------------------------------------------------------------------------
void Camera::SetDepthRange (float fNear, float fFar)
{
    m_fPortN = fNear;
    m_fPortF = fFar;

    if (m_pkRenderer)
    {
        m_pkRenderer->OnDepthRangeChange();
    }
}
//----------------------------------------------------------------------------
void Camera::GetDepthRange (float& rfNear, float& rfFar)
{
    rfNear = m_fPortN;
    rfFar = m_fPortF;
}
//----------------------------------------------------------------------------
bool Camera::GetPickRay (int iX, int iY, int iWidth, int iHeight,
    Vector3f& rkOrigin, Vector3f& rkDirection) const
{
    float fPortX = ((float)iX)/(float)(iWidth-1);
    if (fPortX < m_fPortL || fPortX > m_fPortR)
    {
        return false;
    }

    float fPortY = ((float)(iHeight-1-iY))/(float)(iHeight-1);
    if (fPortY < m_fPortB || fPortY > m_fPortT)
    {
        return false;
    }

    float fXWeight = (fPortX - m_fPortL)/(m_fPortR - m_fPortL);
    float fViewX = (1.0f-fXWeight)*m_afFrustum[VF_RMIN] +
        fXWeight*m_afFrustum[VF_RMAX];
    float fYWeight = (fPortY - m_fPortB)/(m_fPortT - m_fPortB);
    float fViewY = (1.0f-fYWeight)*m_afFrustum[VF_UMIN] +
        fYWeight*m_afFrustum[VF_UMAX];

    rkOrigin = m_kLocation;
    rkDirection = m_afFrustum[VF_DMIN]*m_kDVector + fViewX*m_kRVector +
        fViewY*m_kUVector;
    rkDirection.Normalize();
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Camera::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kLocation);
    rkStream.Read(m_kDVector);
    rkStream.Read(m_kUVector);
    rkStream.Read(m_kRVector);
    rkStream.Read(m_afFrustum[VF_DMIN]);
    rkStream.Read(m_afFrustum[VF_DMAX]);
    rkStream.Read(m_afFrustum[VF_UMIN]);
    rkStream.Read(m_afFrustum[VF_UMAX]);
    rkStream.Read(m_afFrustum[VF_RMIN]);
    rkStream.Read(m_afFrustum[VF_RMAX]);
    rkStream.Read(m_fPortL);
    rkStream.Read(m_fPortR);
    rkStream.Read(m_fPortT);
    rkStream.Read(m_fPortB);
    rkStream.Read(m_fPortN);
    rkStream.Read(m_fPortF);

    WM4_END_DEBUG_STREAM_LOAD(Camera);
}
//----------------------------------------------------------------------------
void Camera::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Camera::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void Camera::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    
    Object::Save(rkStream);

    // native data
    rkStream.Write(m_kLocation);
    rkStream.Write(m_kDVector);
    rkStream.Write(m_kUVector);
    rkStream.Write(m_kRVector);
    rkStream.Write(m_afFrustum[VF_DMIN]);
    rkStream.Write(m_afFrustum[VF_DMAX]);
    rkStream.Write(m_afFrustum[VF_UMIN]);
    rkStream.Write(m_afFrustum[VF_UMAX]);
    rkStream.Write(m_afFrustum[VF_RMIN]);
    rkStream.Write(m_afFrustum[VF_RMAX]);
    rkStream.Write(m_fPortL);
    rkStream.Write(m_fPortR);
    rkStream.Write(m_fPortT);
    rkStream.Write(m_fPortB);
    rkStream.Write(m_fPortN);
    rkStream.Write(m_fPortF);

    WM4_END_DEBUG_STREAM_SAVE(Camera);
}
//----------------------------------------------------------------------------
int Camera::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Object::GetDiskUsed(rkVersion) +
        sizeof(m_kLocation) +
        sizeof(m_kDVector) +
        sizeof(m_kUVector) +
        sizeof(m_kRVector) +
        sizeof(m_afFrustum[VF_DMIN]) +
        sizeof(m_afFrustum[VF_DMAX]) +
        sizeof(m_afFrustum[VF_UMIN]) +
        sizeof(m_afFrustum[VF_UMAX]) +
        sizeof(m_afFrustum[VF_RMIN]) +
        sizeof(m_afFrustum[VF_RMAX]) +
        sizeof(m_fPortL) +
        sizeof(m_fPortR) +
        sizeof(m_fPortT) +
        sizeof(m_fPortB) +
        sizeof(m_fPortN) +
        sizeof(m_fPortF);

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* Camera::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("location =",m_kLocation));
    pkTree->Append(Format("d vector =",m_kDVector));
    pkTree->Append(Format("u vector =",m_kUVector));
    pkTree->Append(Format("r vector =",m_kRVector));
    pkTree->Append(Format("dmin =",m_afFrustum[VF_DMIN]));
    pkTree->Append(Format("dmax =",m_afFrustum[VF_DMAX]));
    pkTree->Append(Format("umin =",m_afFrustum[VF_UMIN]));
    pkTree->Append(Format("umax =",m_afFrustum[VF_UMAX]));
    pkTree->Append(Format("rmin =",m_afFrustum[VF_RMIN]));
    pkTree->Append(Format("rmax =",m_afFrustum[VF_RMAX]));
    pkTree->Append(Format("port L =",m_fPortL));
    pkTree->Append(Format("port R =",m_fPortR));
    pkTree->Append(Format("port T =",m_fPortT));
    pkTree->Append(Format("port B =",m_fPortB));
    pkTree->Append(Format("port N =",m_fPortN));
    pkTree->Append(Format("port F =",m_fPortF));

    // children
    pkTree->Append(Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
