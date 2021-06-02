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
// Version: 4.0.1 (2007/08/11)

#ifndef WM4MULTITEXTUREEFFECT_H
#define WM4MULTITEXTUREEFFECT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4ShaderEffect.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM MultitextureEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    MultitextureEffect (int iTextureQuantity);
    virtual ~MultitextureEffect ();

    // Selection of the textures to be used by the effect.  The first call
    // should be SetTextureQuantity for the desired number of textures.  For
    // each texture, specify its image with SetImageName.  Texture 0 is used
    // as is (replace mode).  Texture i is blended with texture i-1
    // (for i > 0) according to the modes specified by alpha state i to
    // produce the current colors.  After setting all the texture names and
    // all the blending modes, call Configure() to activate the correct shader
    // program for the current set of textures.
    void SetTextureQuantity (int iTextureQuantity);
    int GetTextureQuantity () const;
    void SetTextureName (int i, const std::string& rkTextureName);
    const std::string& GetTextureName (int i) const;
    void Configure ();

protected:
    // streaming support
    MultitextureEffect ();

    int m_iTextureQuantity;
    std::string* m_akTextureName;
};

WM4_REGISTER_STREAM(MultitextureEffect);
typedef Pointer<MultitextureEffect> MultitextureEffectPtr;

}

#endif
