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
inline ColorRGBA::operator const float* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline ColorRGBA::operator float* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
inline float ColorRGBA::operator[] (int i) const
{
    assert(0 <= i && i <= 3);
    if (i < 0)
    {
        i = 0;
    }
    else if ( i > 3 )
    {
        i = 3;
    }

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::operator[] (int i)
{
    assert(0 <= i && i <= 3);
    if (i < 0)
    {
        i = 0;
    }
    else if ( i > 3 )
    {
        i = 3;
    }

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::R () const
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::R ()
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::G () const
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::G ()
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::B () const
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::B ()
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
inline float ColorRGBA::A () const
{
    return m_afTuple[3];
}
//----------------------------------------------------------------------------
inline float& ColorRGBA::A ()
{
    return m_afTuple[3];
}
//----------------------------------------------------------------------------
