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
inline int RevolutionSurface::GetCurveSamples () const
{
    return m_iCurveSamples;
}
//----------------------------------------------------------------------------
inline int RevolutionSurface::GetRadialSamples () const
{
    return m_iRadialSamples;
}
//----------------------------------------------------------------------------
inline void RevolutionSurface::SetCurve (Curve2f* pkCurve)
{
    m_pkCurve = pkCurve;
}
//----------------------------------------------------------------------------
inline const Curve2f* RevolutionSurface::GetCurve () const
{
    return m_pkCurve;
}
//----------------------------------------------------------------------------
inline RevolutionSurface::TopologyType RevolutionSurface::GetTopology () const
{
    return m_eTopology;
}
//----------------------------------------------------------------------------
inline bool& RevolutionSurface::SampleByArcLength ()
{
    return m_bSampleByArcLength;
}
//----------------------------------------------------------------------------
inline bool RevolutionSurface::GetSampleByArcLength () const
{
    return m_bSampleByArcLength;
}
//----------------------------------------------------------------------------
