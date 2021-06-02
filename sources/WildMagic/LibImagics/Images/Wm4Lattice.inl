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
// Version: 4.0.1 (2006/09/21)

//----------------------------------------------------------------------------
inline int Lattice::GetDimensions () const
{
    return m_iDimensions;
}
//----------------------------------------------------------------------------
inline const int* Lattice::GetBounds () const
{
    return m_aiBound;
}
//----------------------------------------------------------------------------
inline int Lattice::GetBound (int i) const
{
    assert(i < m_iDimensions);
    return m_aiBound[i];
}
//----------------------------------------------------------------------------
inline int Lattice::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline const int* Lattice::GetOffsets () const
{
    return m_aiOffset;
}
//----------------------------------------------------------------------------
inline int Lattice::GetOffset (int i) const
{
    assert(i < m_iDimensions);
    return m_aiOffset[i];
}
//----------------------------------------------------------------------------
