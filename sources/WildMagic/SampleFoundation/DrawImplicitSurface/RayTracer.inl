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
inline Vector3f& RayTracer::Location ()
{
    return m_kLocation;
}
//----------------------------------------------------------------------------
inline Vector3f& RayTracer::Direction ()
{
    return m_kDirection;
}
//----------------------------------------------------------------------------
inline Vector3f& RayTracer::Up ()
{
    return m_kUp;
}
//----------------------------------------------------------------------------
inline Vector3f& RayTracer::Right ()
{
    return m_kRight;
}
//----------------------------------------------------------------------------
inline float& RayTracer::Near ()
{
    return m_fNear;
}
//----------------------------------------------------------------------------
inline float& RayTracer::Far ()
{
    return m_fFar;
}
//----------------------------------------------------------------------------
inline float& RayTracer::HalfWidth ()
{
    return m_fHalfWidth;
}
//----------------------------------------------------------------------------
inline float& RayTracer::HalfHeight ()
{
    return m_fHalfHeight;
}
//----------------------------------------------------------------------------
inline int RayTracer::GetWidth () const
{
    return m_iWidth;
}
//----------------------------------------------------------------------------
inline int RayTracer::GetHeight () const
{
    return m_iHeight;
}
//----------------------------------------------------------------------------
inline const float* RayTracer::GetImage () const
{
    return m_afImage;
}
//----------------------------------------------------------------------------
