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
// Version: 4.0.0 (2006/09/21)

//----------------------------------------------------------------------------
inline int PdeFilter::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline float PdeFilter::GetBorderValue () const
{
    return m_fBorderValue;
}
//----------------------------------------------------------------------------
inline PdeFilter::ScaleType PdeFilter::GetScaleType () const
{
    return m_eScaleType;
}
//----------------------------------------------------------------------------
inline void PdeFilter::SetTimeStep (float fTimeStep)
{
    m_fTimeStep = fTimeStep;
}
//----------------------------------------------------------------------------
inline float PdeFilter::GetTimeStep () const
{
    return m_fTimeStep;
}
//----------------------------------------------------------------------------
