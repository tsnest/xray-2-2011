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
// Version: 4.0.3 (2007/09/23)

#ifndef WM4TEXTURE_H
#define WM4TEXTURE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Bindable.h"
#include "Wm4ColorRGBA.h"
#include "Wm4Image.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Texture : public Object, public Bindable
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    enum FilterType
    {
        NEAREST,          // nearest neighbor
        LINEAR,           // linear filtering
        NEAREST_NEAREST,  // nearest within image, nearest across images
        NEAREST_LINEAR,   // nearest within image, linear across images
        LINEAR_NEAREST,   // linear within image, nearest across images
        LINEAR_LINEAR,    // linear within image, linear across images
        MAX_FILTER_TYPES
    };

    enum WrapType
    {
        CLAMP,
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_BORDER,
        CLAMP_EDGE,
        MAX_WRAP_TYPES
    };

    // Construction and destruction.
    Texture (const char* acTextureName, Image* pkImage);
    virtual ~Texture ();

    // Access to texture images.
    virtual void SetImage (Image* pkImage);
    Image* GetImage ();
    const Image* GetImage () const;

    // Access to filter modes.  The default is LINEAR.
    virtual void SetFilterType (FilterType eFType);
    FilterType GetFilterType () const;

    // Access to wrap modes.  The input i to SetWrapType and GetWrapType must
    // satisfy 0 <= i < GetDimension().  The defaults are CLAMP_TO_EDGE.
    void SetWrapType (int i, WrapType eWType);
    WrapType GetWrapType (int i) const;

    // Access to the border color used for sampling outside the texture image.
    // The default is ColorRGBA(0,0,0,1).
    void SetBorderColor (const ColorRGBA& rkBorderColor);
    ColorRGBA GetBorderColor () const;

    // Access to the anisotropy value for image filtering.
    void SetAnisotropyValue (float fAnisotropy);
    float GetAnisotropyValue () const;

    // Support for offscreen textures.
    bool IsOffscreenTexture () const;
    void SetOffscreenTexture (bool bOffscreenTexture);

    // Streaming support.  The sharing system is automatically invoked by
    // these calls.  THIS IS A TEMPORARY HACK TO SUPPORT THE CATALOG SYSTEM
    // UNTIL WILD MAGIC 5 (WM5) SHIPS.  The Texture class will subsume the
    // Image class in WM5.  Image load/save will be a separate system that
    // deals with typical image files (.bmp, .tga, .jpg, .dds, and so on).
    // For now the names of the Texture and the Image it contains should be
    // the same name.  The Load function uses the active Catalog<Image> to
    // fetch the Image from memory or, if it is not in memory, from disk.
    // The name used for fetching is rkTextureName.  If the Image exists,
    // then the Texture uses this Image and the texture state (filter type,
    // wrap type, and so on) is set to the default values.  If the Image does
    // not exist, the function returns null.
    static Texture* Load (const std::string &rkTextureName);

protected:
    // Streaming support.
    Texture ();

    ImagePtr m_spkImage;       // default = null (no image)
    FilterType m_eFType;       // default = LINEAR
    WrapType m_aeWType[3];     // default = CLAMP_EDGE
    ColorRGBA m_kBorderColor;  // default = BLACK
    float m_fAnisotropy;       // default = 1.0f

    // Support for offscreen textures.  The default value is 'false'.
    bool m_bOffscreenTexture;

private:
    static const char* ms_aacFilterType[MAX_FILTER_TYPES];
    static const char* ms_aacWrapType[MAX_WRAP_TYPES];
};

WM4_REGISTER_STREAM(Texture);
typedef Pointer<Texture> TexturePtr;
#include "Wm4Texture.inl"

}

#endif
