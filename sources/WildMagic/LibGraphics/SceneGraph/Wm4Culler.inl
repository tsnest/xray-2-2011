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

//----------------------------------------------------------------------------
inline void Culler::SetCamera (const Camera* pkCamera)
{
    m_pkCamera = pkCamera;
}
//----------------------------------------------------------------------------
inline const Camera* Culler::GetCamera () const
{
    return m_pkCamera;
}
//----------------------------------------------------------------------------
inline const float* Culler::GetFrustum () const
{
    return m_afFrustum;
}
//----------------------------------------------------------------------------
inline VisibleSet& Culler::GetVisibleSet ()
{
    return m_kVisible;
}
//----------------------------------------------------------------------------
inline void Culler::Insert (Spatial* pkObject, Effect* pkGlobalEffect)
{
    m_kVisible.Insert(pkObject,pkGlobalEffect);
}
//----------------------------------------------------------------------------
inline int Culler::GetPlaneQuantity () const
{
    return m_iPlaneQuantity;
}
//----------------------------------------------------------------------------
inline const Plane3f* Culler::GetPlanes () const
{
    return m_akPlane;
}
//----------------------------------------------------------------------------
inline void Culler::SetPlaneState (unsigned int uiPlaneState)
{
    m_uiPlaneState = uiPlaneState;
}
//----------------------------------------------------------------------------
inline unsigned int Culler::GetPlaneState () const
{
    return m_uiPlaneState;
}
//----------------------------------------------------------------------------
inline void Culler::PushPlane (const Plane3f& rkPlane)
{
    if (m_iPlaneQuantity < VS_MAX_PLANE_QUANTITY)
    {
        // The number of user-defined planes is limited.
        m_akPlane[m_iPlaneQuantity++] = rkPlane;
    }
}
//----------------------------------------------------------------------------
inline void Culler::PopPlane ()
{
    if (m_iPlaneQuantity > Camera::VF_QUANTITY)
    {
        // Frustum planes may not be removed from the stack.
        m_iPlaneQuantity--;
    }
}
//----------------------------------------------------------------------------
