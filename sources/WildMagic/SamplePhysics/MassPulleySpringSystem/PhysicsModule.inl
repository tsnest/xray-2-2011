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
inline double PhysicsModule::GetInitialY1 () const
{
    return m_dY1;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCurrentY1 () const
{
    return m_dY1Curr;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCurrentY2 () const
{
    return m_dY2Curr;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCurrentY3 () const
{
    return m_dY3Curr;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetAngle () const
{
    return (m_dY1 - m_dY1Curr)/Radius;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCableFraction1 () const
{
    return (m_dY1Curr - m_dY3Curr)/WireLength;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCableFraction2 () const
{
    return (m_dY2Curr - m_dY3Curr)/WireLength;
}
//----------------------------------------------------------------------------
