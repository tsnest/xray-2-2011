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
inline Image::FormatMode Image::GetFormat () const
{
    return m_eFormat;
}
//----------------------------------------------------------------------------
inline const std::string& Image::GetFormatName () const
{
    return ms_akFormatName[m_eFormat];
}
//----------------------------------------------------------------------------
inline std::string Image::GetFormatName (int eFormat)
{
    return ms_akFormatName[eFormat];
}
//----------------------------------------------------------------------------
inline bool Image::IsDepthImage () const
{
    return m_eFormat == IT_DEPTH16
        || m_eFormat == IT_DEPTH24
        || m_eFormat == IT_DEPTH32;
}
//----------------------------------------------------------------------------
inline bool Image::IsCubeImage () const
{
    return m_eFormat == IT_CUBE_RGB888 || m_eFormat == IT_CUBE_RGBA8888;
}
//----------------------------------------------------------------------------
inline int Image::GetBytesPerPixel () const
{
    return ms_aiBytesPerPixel[m_eFormat];
}
//----------------------------------------------------------------------------
inline int Image::GetBytesPerPixel (int eFormat)
{
    return ms_aiBytesPerPixel[eFormat];
}
//----------------------------------------------------------------------------
inline int Image::GetDimension () const
{
    return m_iDimension;
}
//----------------------------------------------------------------------------
inline int Image::GetBound (int i) const
{
    assert(0 <= i && i < 3);
    return m_aiBound[i];
}
//----------------------------------------------------------------------------
inline int Image::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline unsigned char* Image::GetData () const
{
    return m_aucData;
}
//----------------------------------------------------------------------------
inline unsigned char* Image::operator() (int i)
{
    return m_aucData + i*ms_aiBytesPerPixel[m_eFormat];
}
//----------------------------------------------------------------------------
