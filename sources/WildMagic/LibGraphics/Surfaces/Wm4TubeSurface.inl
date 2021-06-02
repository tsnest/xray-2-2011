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
inline Curve3f*& TubeSurface::Medial ()
{
    return m_pkMedial;
}
//----------------------------------------------------------------------------
inline const Curve3f* TubeSurface::GetMedial () const
{
    return m_pkMedial;
}
//----------------------------------------------------------------------------
inline TubeSurface::RadialFunction& TubeSurface::Radial ()
{
    return m_oRadial;
}
//----------------------------------------------------------------------------
inline TubeSurface::RadialFunction TubeSurface::GetRadial () const
{
    return m_oRadial;
}
//----------------------------------------------------------------------------
inline Vector3f& TubeSurface::UpVector ()
{
    return m_kUpVector;
}
//----------------------------------------------------------------------------
inline const Vector3f& TubeSurface::GetUpVector () const
{
    return m_kUpVector;
}
//----------------------------------------------------------------------------
inline int TubeSurface::GetSliceSamples () const
{
    return m_iSliceSamples;
}
//----------------------------------------------------------------------------
inline int TubeSurface::Index (int iS, int iM)
{
    return iS + (m_iSliceSamples+1)*iM;
}
//----------------------------------------------------------------------------
