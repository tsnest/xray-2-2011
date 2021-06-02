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
// Version: 4.0.3 (2007/08/11)

//----------------------------------------------------------------------------
inline void Texture::SetImage (Image* pkImage)
{
    m_spkImage = pkImage;
}
//----------------------------------------------------------------------------
inline Image* Texture::GetImage ()
{
    return m_spkImage;
}
//----------------------------------------------------------------------------
inline const Image* Texture::GetImage () const
{
    return m_spkImage;
}
//----------------------------------------------------------------------------
inline void Texture::SetFilterType (FilterType eFType)
{
    m_eFType = eFType;
}
//----------------------------------------------------------------------------
inline Texture::FilterType Texture::GetFilterType () const
{
    return m_eFType;
}
//----------------------------------------------------------------------------
inline void Texture::SetWrapType (int i, WrapType eWType)
{
    assert(0 <= i && i < 3);
    m_aeWType[i] = eWType;
}
//----------------------------------------------------------------------------
inline Texture::WrapType Texture::GetWrapType (int i) const
{
    assert(0 <= i && i < 3);
    return m_aeWType[i];
}
//----------------------------------------------------------------------------
inline void Texture::SetBorderColor (const ColorRGBA& rkBorderColor)
{
    m_kBorderColor = rkBorderColor;
}
//----------------------------------------------------------------------------
inline ColorRGBA Texture::GetBorderColor () const
{
    return m_kBorderColor;
}
//----------------------------------------------------------------------------
inline void Texture::SetAnisotropyValue (float fAnisotropy)
{
    m_fAnisotropy = fAnisotropy;
}
//----------------------------------------------------------------------------
inline float Texture::GetAnisotropyValue () const
{
    return m_fAnisotropy;
}
//----------------------------------------------------------------------------
inline bool Texture::IsOffscreenTexture () const
{
    return m_bOffscreenTexture;
}
//----------------------------------------------------------------------------
inline void Texture::SetOffscreenTexture (bool bOffscreenTexture)
{
    m_bOffscreenTexture = bOffscreenTexture;
}
//----------------------------------------------------------------------------
