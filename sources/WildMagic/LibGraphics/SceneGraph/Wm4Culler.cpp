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
// Version: 4.0.0 (2006/06/28)

#include "Wm4GraphicsPCH.h"
#include "Wm4Culler.h"
using namespace Wm4;

//----------------------------------------------------------------------------
Culler::Culler (int iMaxQuantity, int iGrowBy, const Camera* pkCamera)
    :
    m_kVisible(iMaxQuantity,iGrowBy)
{
    m_pkCamera = pkCamera;
    m_iPlaneQuantity = 6;

    // The data members m_afFrustum, m_akPlane, and m_uiPlaneState are
    // uninitialized.  They are initialized in the GetVisibleSet call.
}
//----------------------------------------------------------------------------
Culler::~Culler ()
{
}
//----------------------------------------------------------------------------
void Culler::SetFrustum (const float* afFrustum)
{
    assert(m_pkCamera);
    if (!m_pkCamera)
    {
        return;
    }

    // copy the frustum values
    size_t uiSize = Camera::VF_QUANTITY*sizeof(float);
    System::Memcpy(m_afFrustum,uiSize,afFrustum,uiSize);
    float fDMin2 = m_afFrustum[Camera::VF_DMIN]*m_afFrustum[Camera::VF_DMIN];
    float fRMin2 = m_afFrustum[Camera::VF_RMIN]*m_afFrustum[Camera::VF_RMIN];
    float fRMax2 = m_afFrustum[Camera::VF_RMAX]*m_afFrustum[Camera::VF_RMAX];
    float fUMin2 = m_afFrustum[Camera::VF_UMIN]*m_afFrustum[Camera::VF_UMIN];
    float fUMax2 = m_afFrustum[Camera::VF_UMAX]*m_afFrustum[Camera::VF_UMAX];

    // get the camera coordinate frame
    Vector3f kLoc = m_pkCamera->GetLocation();
    Vector3f kDVec = m_pkCamera->GetDVector();
    Vector3f kUVec = m_pkCamera->GetUVector();
    Vector3f kRVec = m_pkCamera->GetRVector();
    float fDdE = kDVec.Dot(kLoc);

    // update the near plane
    m_akPlane[Camera::VF_DMIN].Normal = kDVec;
    m_akPlane[Camera::VF_DMIN].Constant =
        fDdE + m_afFrustum[Camera::VF_DMIN];

    // update the far plane
    m_akPlane[Camera::VF_DMAX].Normal = -kDVec;
    m_akPlane[Camera::VF_DMAX].Constant =
        -(fDdE + m_afFrustum[Camera::VF_DMAX]);

    // update the bottom plane
    float fInvLength = 1.0f/Mathf::Sqrt(fDMin2 + fUMin2);
    float fC0 = -m_afFrustum[Camera::VF_UMIN]*fInvLength;  // D component
    float fC1 = +m_afFrustum[Camera::VF_DMIN]*fInvLength;  // U component
    m_akPlane[Camera::VF_UMIN].Normal = fC0*kDVec + fC1*kUVec;
    m_akPlane[Camera::VF_UMIN].Constant =
        kLoc.Dot(m_akPlane[Camera::VF_UMIN].Normal);

    // update the top plane
    fInvLength = 1.0f/Mathf::Sqrt(fDMin2 + fUMax2);
    fC0 = +m_afFrustum[Camera::VF_UMAX]*fInvLength;  // D component
    fC1 = -m_afFrustum[Camera::VF_DMIN]*fInvLength;  // U component
    m_akPlane[Camera::VF_UMAX].Normal = fC0*kDVec + fC1*kUVec;
    m_akPlane[Camera::VF_UMAX].Constant =
        kLoc.Dot(m_akPlane[Camera::VF_UMAX].Normal);

    // update the left plane
    fInvLength = 1.0f/Mathf::Sqrt(fDMin2 + fRMin2);
    fC0 = -m_afFrustum[Camera::VF_RMIN]*fInvLength;  // D component
    fC1 = +m_afFrustum[Camera::VF_DMIN]*fInvLength;  // R component
    m_akPlane[Camera::VF_RMIN].Normal = fC0*kDVec + fC1*kRVec;
    m_akPlane[Camera::VF_RMIN].Constant =
        kLoc.Dot(m_akPlane[Camera::VF_RMIN].Normal);

    // update the right plane
    fInvLength = 1.0f/Mathf::Sqrt(fDMin2 + fRMax2);
    fC0 = +m_afFrustum[Camera::VF_RMAX]*fInvLength;  // D component
    fC1 = -m_afFrustum[Camera::VF_DMIN]*fInvLength;  // R component
    m_akPlane[Camera::VF_RMAX].Normal = fC0*kDVec + fC1*kRVec;
    m_akPlane[Camera::VF_RMAX].Constant =
        kLoc.Dot(m_akPlane[Camera::VF_RMAX].Normal);

    // all planes are active initially
    m_uiPlaneState = (unsigned int)~0;
}
//----------------------------------------------------------------------------
bool Culler::IsVisible (const BoundingVolume* pkBound)
{
    // Start with the last pushed plane, which is potentially the most
    // restrictive plane.
    int iP = m_iPlaneQuantity - 1;
    unsigned int uiMask = 1 << iP;

    for (int i = 0; i < m_iPlaneQuantity; i++, iP--, uiMask >>= 1)
    {
        if (m_uiPlaneState & uiMask)
        {
            int iSide = pkBound->WhichSide(m_akPlane[iP]);

            if (iSide < 0)
            {
                // Object is on negative side.  Cull it.
                return false;
            }

            if (iSide > 0)
            {
                // Object is on positive side of plane.  There is no need to
                // compare subobjects against this plane, so mark it as
                // inactive.
                m_uiPlaneState &= ~uiMask;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Culler::IsVisible (int iVertexQuantity, const Vector3f* akVertex,
    bool bIgnoreNearPlane)
{
    // The Boolean variable bIgnoreNearPlane should be set to 'true' when
    // the test polygon is a portal.  This avoids the situation when the
    // portal is in the view pyramid (eye+left/right/top/bottom), but is
    // between the eye and near plane.  In such a situation you do not want
    // the portal system to cull the portal.  This situation typically occurs
    // when the camera moves through the portal from current region to
    // adjacent region.

    // Start with last pushed plane, which is potentially the most
    // restrictive plane.
    int iP = m_iPlaneQuantity - 1;
    for (int i = 0; i < m_iPlaneQuantity; i++, iP--)
    {
        Plane3f& rkPlane = m_akPlane[iP];
        if (bIgnoreNearPlane && iP == Camera::VF_DMIN)
        {
            continue;
        }

        int iV;
        for (iV = 0; iV < iVertexQuantity; iV++)
        {
            int iSide = rkPlane.WhichSide(akVertex[iV]);
            if (iSide >= 0)
            {
                // polygon is not totally outside this plane
                break;
            }
        }

        if (iV == iVertexQuantity)
        {
            // polygon is totally outside this plane
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
int Culler::WhichSide (const Plane3f& rkPlane) const
{
    // The plane is N*(X-C) = 0 where the * indicates dot product.  The signed
    // distance from the camera location E to the plane is N*(E-C).
    float fNdEmC = rkPlane.DistanceTo(m_pkCamera->GetLocation());

    float fNdD = rkPlane.Normal.Dot(m_pkCamera->GetDVector());
    float fNdU = rkPlane.Normal.Dot(m_pkCamera->GetUVector());
    float fNdR = rkPlane.Normal.Dot(m_pkCamera->GetRVector());
    float fFdN = m_afFrustum[Camera::VF_DMAX]/m_afFrustum[Camera::VF_DMIN];

    int iPositive = 0, iNegative = 0;
    float fSgnDist;

    // check near-plane vertices
    float fPDMin = m_afFrustum[Camera::VF_DMIN]*fNdD;
    float fNUMin = m_afFrustum[Camera::VF_UMIN]*fNdU;
    float fNUMax = m_afFrustum[Camera::VF_UMAX]*fNdU;
    float fNRMin = m_afFrustum[Camera::VF_RMIN]*fNdR;
    float fNRMax = m_afFrustum[Camera::VF_RMAX]*fNdR;

    // V = E + dmin*D + umin*U + rmin*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umin*(N*U) + rmin*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMin + fNRMin;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    // V = E + dmin*D + umin*U + rmax*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umin*(N*U) + rmax*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMin + fNRMax;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    // V = E + dmin*D + umax*U + rmin*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umax*(N*U) + rmin*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMax + fNRMin;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    // V = E + dmin*D + umax*U + rmax*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umax*(N*U) + rmax*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMax + fNRMax;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    // check far-plane vertices (s = dmax/dmin)
    float fPDMax = m_afFrustum[Camera::VF_DMAX]*fNdD;
    float fFUMin = fFdN*fNUMin;
    float fFUMax = fFdN*fNUMax;
    float fFRMin = fFdN*fNRMin;
    float fFRMax = fFdN*fNRMax;

    // V = E + dmax*D + umin*U + rmin*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umin*(N*U) + s*rmin*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMin + fFRMin;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    // V = E + dmax*D + umin*U + rmax*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umin*(N*U) + s*rmax*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMin + fFRMax;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    // V = E + dmax*D + umax*U + rmin*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umax*(N*U) + s*rmin*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMax + fFRMin;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    // V = E + dmax*D + umax*U + rmax*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umax*(N*U) + s*rmax*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMax + fFRMax;
    if (fSgnDist > 0.0f)
    {
        iPositive++;
    }
    else if (fSgnDist < 0.0f)
    {
        iNegative++;
    }

    if (iPositive > 0)
    {
        if (iNegative > 0)
        {
            // frustum straddles the plane
            return 0;
        }

        // frustum is fully on the positive side
        return +1;
    }

    // frustum is fully on the negative side
    return -1;
}
//----------------------------------------------------------------------------
void Culler::ComputeVisibleSet (Spatial* pkScene)
{
    assert(m_pkCamera && pkScene);
    if (m_pkCamera && pkScene)
    {
        SetFrustum(m_pkCamera->GetFrustum());
        m_kVisible.Clear();
        pkScene->OnGetVisibleSet(*this,false);
    }
}
//----------------------------------------------------------------------------
