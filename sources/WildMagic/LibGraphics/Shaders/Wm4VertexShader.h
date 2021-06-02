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

#ifndef WM4VERTEXSHADER_H
#define WM4VERTEXSHADER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Shader.h"
#include "Wm4VertexProgram.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM VertexShader : public Shader
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.
    VertexShader (const std::string& rkShaderName);
    virtual ~VertexShader ();

    // Access to the shader's program.
    VertexProgram* GetProgram () const;

protected:
    // Streaming support.
    VertexShader ();
};

WM4_REGISTER_STREAM(VertexShader);
typedef Pointer<VertexShader> VertexShaderPtr;

}

#endif
