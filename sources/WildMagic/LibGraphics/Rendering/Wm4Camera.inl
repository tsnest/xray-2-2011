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
inline Vector3f Camera::GetLocation () const
{
    return m_kLocation;
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetDVector () const
{
    return m_kDVector;
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetUVector () const
{
    return m_kUVector;
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetRVector () const
{
    return m_kRVector;
}
//----------------------------------------------------------------------------
inline const float* Camera::GetFrustum () const
{
    return m_afFrustum;
}
//----------------------------------------------------------------------------
inline float Camera::GetDMin () const
{
    return m_afFrustum[VF_DMIN];
}
//----------------------------------------------------------------------------
inline float Camera::GetDMax () const
{
    return m_afFrustum[VF_DMAX];
}
//----------------------------------------------------------------------------
inline float Camera::GetUMin () const
{
    return m_afFrustum[VF_UMIN];
}
//----------------------------------------------------------------------------
inline float Camera::GetUMax () const
{
    return m_afFrustum[VF_UMAX];
}
//----------------------------------------------------------------------------
inline float Camera::GetRMin () const
{
    return m_afFrustum[VF_RMIN];
}
//----------------------------------------------------------------------------
inline float Camera::GetRMax () const
{
    return m_afFrustum[VF_RMAX];
}
//----------------------------------------------------------------------------
