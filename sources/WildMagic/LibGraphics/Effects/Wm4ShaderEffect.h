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
// Version: 4.0.2 (2007/09/24)

#ifndef WM4SHADEREFFECT_H
#define WM4SHADEREFFECT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Effect.h"
#include "Wm4VertexShader.h"
#include "Wm4PixelShader.h"

namespace Wm4
{

class Renderer;

class WM4_GRAPHICS_ITEM ShaderEffect : public Effect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    ShaderEffect (int iPassQuantity);
    virtual ~ShaderEffect ();

    // The shader effect is a manager of the vertex and pixel shaders.  It
    // reimplements the user-relevant interfaces for the managed objects as a
    // convenience to avoid long expressions involving pointer dereferencing.

    // The number of vertex/pixel shader pairs.  The Set* call reallocates the
    // vertex shader, pixel shader, and alpha state arrays.
    void SetPassQuantity (int iPassQuantity);
    int GetPassQuantity () const;

    // Blending modes for multipass effects.  Mode i specifies how the pixel
    // colors from pass i-1 and pass i are blended.  For a single effect
    // attached to a Geometry object, blending mode 0 is irrelevant in that
    // the source mode is equivalent to SBF_ONE and the destination mode is
    // equivalent to SDF_ZERO; that is, the frame buffer values are replaced
    // with the pixel values from the shader.  If multiple effects are
    // attached to a Geometry object, blending mode 0 specifies how the
    // current effect is blended with the frame buffer.
    AlphaState* GetBlending (int iPass);

    // Set any global state needed by the pass and restore it later.  The
    // base class enables and disables blending as needed for multipass
    // and multieffect drawing.
    virtual void SetGlobalState (int iPass, Renderer* pkRenderer,
        bool bPrimaryEffect);
    virtual void RestoreGlobalState (int iPass, Renderer* pkRenderer,
        bool bPrimaryEffect);

    // Allow derived classes to set the data sources in the UserConstant
    // objects that correspond to shader programs.
    void ConnectVShaderConstant (int iPass, const std::string& rkName,
        float* pfSource);
    void ConnectPShaderConstant (int iPass, const std::string& rkName,
        float* pfSource);
    void ConnectVShaderConstants (int iPass);
    void ConnectPShaderConstants (int iPass);

    // Vertex shader interfaces.
    bool SetVShader (int iPass, VertexShader* pkVShader);
    VertexShader* GetVShader (int iPass);
    VertexProgram* GetVProgram (int iPass);
    const std::string& GetVShaderName (int iPass) const;
    int GetVConstantQuantity (int iPass) const;
    UserConstant* GetVConstant (int iPass, int i);
    UserConstant* GetVConstant (int iPass, const std::string& rkName);
    int GetVTextureQuantity (int iPass) const;
    Texture* GetVTexture (int iPass, int i);
    Texture* GetVTexture (int iPass, const std::string& rkName);
    bool SetVTexture (int iPass, int i, Texture* pkTexture);
    bool SetVTexture (int iPass, int i, const std::string& rkName);

    // Pixel shader interfaces.
    bool SetPShader (int iPass, PixelShader* pkPShader);
    PixelShader* GetPShader (int iPass);
    PixelProgram* GetPProgram (int iPass);
    const std::string& GetPShaderName (int iPass) const;
    int GetPConstantQuantity (int iPass) const;
    UserConstant* GetPConstant (int iPass, int i);
    UserConstant* GetPConstant (int iPass, const std::string& rkName);
    int GetPTextureQuantity (int iPass) const;
    Texture* GetPTexture (int iPass, int i);
    Texture* GetPTexture (int iPass, const std::string& rkName);
    bool SetPTexture (int iPass, int i, Texture* pkTexture);
    bool SetPTexture (int iPass, int i, const std::string& rkName);

    // The functions are called by Renderer::LoadResources and
    // Renderer::ReleaseResources for Geometry and Effect objects.  The
    // resources are transferred from system memory to video memory.
    virtual void LoadResources (Renderer* pkRenderer, Geometry* pkGeometry);
    virtual void ReleaseResources (Renderer* pkRenderer,
        Geometry* pkGeometry);

protected:
    // Streaming support.
    ShaderEffect ();

    void SetDefaultAlphaState ();

    // When SetVShader and SetPShader are called, we must verify that the
    // outputs of the vertex program are compatible with the inputs of the
    // pixel program.  This function does so.  The first time you set the
    // shaders, there are no side effects to consider.  However, in the
    // unlikely event you want to change *both* shader programs later, call
    // SetVShader and SetPShader *first* with null inputs, and then call
    // them again with the new shaders.
    bool AreCompatible (const VertexProgram* pkVProgram,
        const PixelProgram* pkPProgram) const;

    int m_iPassQuantity;
    std::vector<VertexShaderPtr> m_kVShader;
    std::vector<PixelShaderPtr> m_kPShader;
    std::vector<AlphaStatePtr> m_kAlphaState;

    typedef std::pair<UserConstant*,float*> Connection;
    std::vector<std::vector<Connection> > m_kVConnect;
    std::vector<std::vector<Connection> > m_kPConnect;
};

WM4_REGISTER_STREAM(ShaderEffect);
typedef Pointer<ShaderEffect> ShaderEffectPtr;

#include "Wm4ShaderEffect.inl"

}

#endif
