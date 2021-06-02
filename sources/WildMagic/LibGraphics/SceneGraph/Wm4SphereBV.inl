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
inline int SphereBV::GetBVType () const
{
    return BoundingVolume::BV_SPHERE;
}
//----------------------------------------------------------------------------
inline void SphereBV::SetCenter (const Vector3f& rkCenter)
{
    m_kSphere.Center = rkCenter;
}
//----------------------------------------------------------------------------
inline void SphereBV::SetRadius (float fRadius)
{
    m_kSphere.Radius = fRadius;
}
//----------------------------------------------------------------------------
inline Vector3f SphereBV::GetCenter () const
{
    return m_kSphere.Center;
}
//----------------------------------------------------------------------------
inline float SphereBV::GetRadius () const
{
    return m_kSphere.Radius;
}
//----------------------------------------------------------------------------
inline Sphere3f& SphereBV::Sphere ()
{
    return m_kSphere;
}
//----------------------------------------------------------------------------
inline const Sphere3f& SphereBV::GetSphere () const
{
    return m_kSphere;
}
//----------------------------------------------------------------------------
