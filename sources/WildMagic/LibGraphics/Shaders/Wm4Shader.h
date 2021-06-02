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
// Version: 4.0.2 (2007/08/11)

#ifndef WM4SHADER_H
#define WM4SHADER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4Program.h"
#include "Wm4Texture.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Shader : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Abstract base class.
    virtual ~Shader ();

    // The name of the shader object.  The program object has a name that
    // contains the shader name as a substring, but adds additional text
    // as needed (the path to a shader on disk, the identifying information
    // for a procedurally generated shader).
    const std::string& GetShaderName () const;

    // Get the number of Texture objects used by the Shader.
    int GetTextureQuantity () const;

    // Returns 'true' when 0 <= i < GetTextureQuantity() and the Texture
    // object is not null.  The non-null requirement is based on your knowing
    // that the shader program indeed requires a texture that it will sample.
    bool SetTexture (int i, Texture* pkTexture);

    // Returns 'true' when 0 <= i < GetTextureQuantity() and a Texture object
    // by the specified name exists in the active Catalog<Texture>.
    bool SetTexture (int i, const std::string& rkName);

    // Returns a non-null Texture object when 0 <= i < GetTextureQuantity().
    Texture* GetTexture (int i);
    const Texture* GetTexture (int i) const;

    // Returns a non-null Texture when the specified name corresponds to a
    // Texture object stored by the Shader.
    Texture* GetTexture (const std::string& rkName);
    const Texture* GetTexture (const std::string& rkName) const;

protected:
    // Support for streaming.
    Shader ();

    // The constructor called by the derived classes VertexShader and
    // PixelShader.
    Shader (const std::string& rkShaderName);

    // The VertexShader and PixelShader constructors must access their
    // respective catalogs to load programs.  After a program is loaded,
    // the user constants must be hooked up to their data sources and
    // the number of textures the samplers use must be set.  This function
    // does this work.
    void OnLoadProgram ();

    // The shader name, which contributes to a uniquely identifying string
    // for a shader program.
    std::string m_kShaderName;

    // The shader program, which is dependent on graphics API.
    ProgramPtr m_spkProgram;

    // The user-defined data are specific to each shader object.  The Program
    // object knows only the name, which register to assign the value to, and
    // how many registers to use.  The storage provided here is for the
    // convenience of Shader-derived classes.  However, a derived class may
    // provide alternate storage by calling UserConstant::SetDataSource for
    // each user constant of interest.
    std::vector<float> m_kUserData;

    // The Texture objects used by the samplers.
    std::vector<TexturePtr> m_kTextures;

private:
    // TO DO:  These are retained to avoid having to re-export *.wmof files
    // that are streamversion 4.5 and earlier.  Remove these at a later date.
    std::vector<std::string> m_kImageNames;
};

WM4_REGISTER_STREAM(Shader);
typedef Pointer<Shader> ShaderPtr;
#include "Wm4Shader.inl"

}

#endif
