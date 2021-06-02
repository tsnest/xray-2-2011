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
// Version: 4.0.1 (2006/11/24)

//----------------------------------------------------------------------------
inline int IndexBuffer::GetIndexQuantity () const
{
    return m_iIQuantity;
}
//----------------------------------------------------------------------------
inline int* IndexBuffer::GetData ()
{
    return m_aiIndex;
}
//----------------------------------------------------------------------------
inline const int* IndexBuffer::GetData () const
{
    return m_aiIndex;
}
//----------------------------------------------------------------------------
inline int IndexBuffer::operator[] (int i) const
{
    assert(0 <= i && i < m_iIQuantity);
    return m_aiIndex[i];
}
//----------------------------------------------------------------------------
inline int& IndexBuffer::operator[] (int i)
{
    assert(0 <= i && i < m_iIQuantity);
    return m_aiIndex[i];
}
//----------------------------------------------------------------------------
inline void IndexBuffer::SetIndexQuantity (int iIQuantity)
{
    m_iIQuantity = iIQuantity;
}
//----------------------------------------------------------------------------
inline void IndexBuffer::SetOffset (int iOffset)
{
    assert(iOffset >= 0);
    m_iOffset = iOffset;
}
//----------------------------------------------------------------------------
inline int IndexBuffer::GetOffset () const
{
    return m_iOffset;
}
//----------------------------------------------------------------------------
