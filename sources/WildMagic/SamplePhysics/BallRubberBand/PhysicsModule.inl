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
inline double PhysicsModule::GetTime () const
{
    return m_dTime;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetDeltaTime () const
{
    return m_dDeltaTime;
}
//----------------------------------------------------------------------------
inline const Wm4::Vector2d& PhysicsModule::GetPosition () const
{
    return m_kPosition;
}
//----------------------------------------------------------------------------
inline const Wm4::Vector2d& PhysicsModule::GetVelocity () const
{
    return m_kVelocity;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetFrequency () const
{
    return m_dFrequency;
}
//----------------------------------------------------------------------------