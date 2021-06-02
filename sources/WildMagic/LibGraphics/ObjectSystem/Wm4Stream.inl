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
inline int Stream::GetOrderedQuantity () const
{
    return (int)m_kOrdered.size();
}
//----------------------------------------------------------------------------
inline Object* Stream::GetOrderedObject (int i) const
{
    assert(0 <= i && i < (int)m_kOrdered.size());
    if (i < 0 || i >= (int)m_kOrdered.size())
    {
        return 0;
    }

    return m_kOrdered[i];
}
//----------------------------------------------------------------------------
inline int Stream::GetBufferSize () const
{
    return m_iBufferSize;
}
//----------------------------------------------------------------------------
inline int Stream::GetBufferNext () const
{
    return m_iBufferNext;
}
//----------------------------------------------------------------------------
inline StreamVersion Stream::GetVersion () const
{
    return m_kVersion;
}
//----------------------------------------------------------------------------
