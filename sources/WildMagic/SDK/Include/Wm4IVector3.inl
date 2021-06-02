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
inline IVector3::IVector3 ()
{
    // the vector is uninitialized
}
//----------------------------------------------------------------------------
inline IVector3::IVector3 (const IVector3& rkV)
{
    m_aiTuple[0] = rkV.m_aiTuple[0];
    m_aiTuple[1] = rkV.m_aiTuple[1];
    m_aiTuple[2] = rkV.m_aiTuple[2];
}
//----------------------------------------------------------------------------
inline IVector3::IVector3 (const TIVector<3>& rkV)
{
    m_aiTuple[0] = rkV[0];
    m_aiTuple[1] = rkV[1];
    m_aiTuple[2] = rkV[2];
}
//----------------------------------------------------------------------------
inline IVector3::IVector3 (const Integer64& riX, const Integer64& riY,
    const Integer64& riZ)
{
    m_aiTuple[0] = riX;
    m_aiTuple[1] = riY;
    m_aiTuple[2] = riZ;
}
//----------------------------------------------------------------------------
inline IVector3& IVector3::operator= (const IVector3& rkV)
{
    m_aiTuple[0] = rkV.m_aiTuple[0];
    m_aiTuple[1] = rkV.m_aiTuple[1];
    m_aiTuple[2] = rkV.m_aiTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
inline IVector3& IVector3::operator= (const TIVector<3>& rkV)
{
    m_aiTuple[0] = rkV[0];
    m_aiTuple[1] = rkV[1];
    m_aiTuple[2] = rkV[2];
    return *this;
}
//----------------------------------------------------------------------------
inline Integer64 IVector3::X () const
{
    return m_aiTuple[0];
}
//----------------------------------------------------------------------------
inline Integer64& IVector3::X ()
{
    return m_aiTuple[0];
}
//----------------------------------------------------------------------------
inline Integer64 IVector3::Y () const
{
    return m_aiTuple[1];
}
//----------------------------------------------------------------------------
inline Integer64& IVector3::Y ()
{
    return m_aiTuple[1];
}
//----------------------------------------------------------------------------
inline Integer64 IVector3::Z () const
{
    return m_aiTuple[2];
}
//----------------------------------------------------------------------------
inline Integer64& IVector3::Z ()
{
    return m_aiTuple[2];
}
//----------------------------------------------------------------------------
inline Integer64 IVector3::Dot (const IVector3& rkV) const
{
    return m_aiTuple[0]*rkV.m_aiTuple[0] + m_aiTuple[1]*rkV.m_aiTuple[1] +
        m_aiTuple[2]*rkV.m_aiTuple[2];
}
//----------------------------------------------------------------------------
inline IVector3 IVector3::Cross (const IVector3& rkV) const
{
    return IVector3(
        m_aiTuple[1]*rkV.m_aiTuple[2] - m_aiTuple[2]*rkV.m_aiTuple[1],
        m_aiTuple[2]*rkV.m_aiTuple[0] - m_aiTuple[0]*rkV.m_aiTuple[2],
        m_aiTuple[0]*rkV.m_aiTuple[1] - m_aiTuple[1]*rkV.m_aiTuple[0]);
}
//----------------------------------------------------------------------------
inline Integer64 IVector3::TripleScalar (const IVector3& rkU,
    const IVector3& rkV) const
{
    return Dot(rkU.Cross(rkV));
}
//----------------------------------------------------------------------------
