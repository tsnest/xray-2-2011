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

#ifndef WM4RENDERERCONSTANT_H
#define WM4RENDERERCONSTANT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Main.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM RendererConstant
{
    WM4_DECLARE_INITIALIZE;
    WM4_DECLARE_TERMINATE;

public:
    enum Type
    {
        W_MATRIX,                          // 4x4 model-to-world matrix
        V_MATRIX,                          // 4x4 world-to-view matrix
        P_MATRIX,                          // 4x4 view-to-clip matrix
        WV_MATRIX,                         // 4x4 model-to-view matrix
        VP_MATRIX,                         // 4x4 world-to-clip matrix
        WVP_MATRIX,                        // 4x4 model-to-clip matrix
        W_MATRIX_TRANSPOSE,                // 4x4 trans model-to-world
        V_MATRIX_TRANSPOSE,                // 4x4 trans world-to-view
        P_MATRIX_TRANSPOSE,                // 4x4 trans view-to-clip
        WV_MATRIX_TRANSPOSE,               // 4x4 trans model-to-view
        VP_MATRIX_TRANSPOSE,               // 4x4 trans world-to-clip
        WVP_MATRIX_TRANSPOSE,              // 4x4 trans model-to-clip
        W_MATRIX_INVERSE,                  // 4x4 inv model-to-world
        V_MATRIX_INVERSE,                  // 4x4 inv world-to-view
        P_MATRIX_INVERSE,                  // 4x4 inv view-to-clip
        WV_MATRIX_INVERSE,                 // 4x4 inv model-to-view
        VP_MATRIX_INVERSE,                 // 4x4 inv world-to-clip
        WVP_MATRIX_INVERSE,                // 4x4 inv model-to-clip
        W_MATRIX_INVERSE_TRANSPOSE,        // 4x4 inv-trans model-to-world
        V_MATRIX_INVERSE_TRANSPOSE,        // 4x4 inv-trans world-to-view
        P_MATRIX_INVERSE_TRANSPOSE,        // 4x4 inv-trans view-to-clip
        WV_MATRIX_INVERSE_TRANSPOSE,       // 4x4 inv-trans model-to-view
        VP_MATRIX_INVERSE_TRANSPOSE,       // 4x4 inv-trans world-to-clip
        WVP_MATRIX_INVERSE_TRANSPOSE,      // 4x4 inv-trans model-to-clip

        MATERIAL_EMISSIVE,                 // (r,g,b)
        MATERIAL_AMBIENT,                  // (r,g,b)
        MATERIAL_DIFFUSE,                  // (r,g,b;alpha)
        MATERIAL_SPECULAR,                 // (r,g,b;shininess)

        CAMERA_MODEL_POSITION,             // (x,y,z,1)
        CAMERA_MODEL_DIRECTION,            // (x,y,z,0)
        CAMERA_MODEL_UP,                   // (x,y,z,0)
        CAMERA_MODEL_RIGHT,                // (x,y,z,0)

        CAMERA_WORLD_POSITION,             // (x,y,z,1)
        CAMERA_WORLD_DIRECTION,            // (x,y,z,0)
        CAMERA_WORLD_UP,                   // (x,y,z,0)
        CAMERA_WORLD_RIGHT,                // (x,y,z,0)

        PROJECTOR_MODEL_POSITION,          // (x,y,z,1)
        PROJECTOR_MODEL_DIRECTION,         // (x,y,z,0)
        PROJECTOR_MODEL_UP,                // (x,y,z,0)
        PROJECTOR_MODEL_RIGHT,             // (x,y,z,0)

        PROJECTOR_WORLD_POSITION,          // (x,y,z,1)
        PROJECTOR_WORLD_DIRECTION,         // (x,y,z,0)
        PROJECTOR_WORLD_UP,                // (x,y,z,0)
        PROJECTOR_WORLD_RIGHT,             // (x,y,z,0)

        PROJECTOR_MATRIX,                  // 4x4 world-to-clip matrix

        LIGHT0_MODEL_POSITION,             // (x,y,z,1)
        LIGHT0_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT0_WORLD_POSITION,             // (x,y,z,1)
        LIGHT0_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT0_AMBIENT,                    // (r,g,b,a)
        LIGHT0_DIFFUSE,                    // (r,g,b,a)
        LIGHT0_SPECULAR,                   // (r,g,b,a)
        LIGHT0_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT0_ATTENUATION,                // (const,lin,quad,intensity)

        LIGHT1_MODEL_POSITION,             // (x,y,z,1)
        LIGHT1_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT1_WORLD_POSITION,             // (x,y,z,1)
        LIGHT1_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT1_AMBIENT,                    // (r,g,b,a)
        LIGHT1_DIFFUSE,                    // (r,g,b,a)
        LIGHT1_SPECULAR,                   // (r,g,b,a)
        LIGHT1_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT1_ATTENUATION,                // (const,lin,quad,intensity)

        LIGHT2_MODEL_POSITION,             // (x,y,z,1)
        LIGHT2_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT2_WORLD_POSITION,             // (x,y,z,1)
        LIGHT2_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT2_AMBIENT,                    // (r,g,b,a)
        LIGHT2_DIFFUSE,                    // (r,g,b,a)
        LIGHT2_SPECULAR,                   // (r,g,b,a)
        LIGHT2_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT2_ATTENUATION,                // (const,lin,quad,intensity)

        LIGHT3_MODEL_POSITION,             // (x,y,z,1)
        LIGHT3_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT3_WORLD_POSITION,             // (x,y,z,1)
        LIGHT3_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT3_AMBIENT,                    // (r,g,b,a)
        LIGHT3_DIFFUSE,                    // (r,g,b,a)
        LIGHT3_SPECULAR,                   // (r,g,b,a)
        LIGHT3_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT3_ATTENUATION,                // (const,lin,quad,intensity)

        LIGHT4_MODEL_POSITION,             // (x,y,z,1)
        LIGHT4_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT4_WORLD_POSITION,             // (x,y,z,1)
        LIGHT4_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT4_AMBIENT,                    // (r,g,b,a)
        LIGHT4_DIFFUSE,                    // (r,g,b,a)
        LIGHT4_SPECULAR,                   // (r,g,b,a)
        LIGHT4_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT4_ATTENUATION,                // (const,lin,quad,intensity)

        LIGHT5_MODEL_POSITION,             // (x,y,z,1)
        LIGHT5_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT5_WORLD_POSITION,             // (x,y,z,1)
        LIGHT5_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT5_AMBIENT,                    // (r,g,b,a)
        LIGHT5_DIFFUSE,                    // (r,g,b,a)
        LIGHT5_SPECULAR,                   // (r,g,b,a)
        LIGHT5_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT5_ATTENUATION,                // (const,lin,quad,intensity)

        LIGHT6_MODEL_POSITION,             // (x,y,z,1)
        LIGHT6_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT6_WORLD_POSITION,             // (x,y,z,1)
        LIGHT6_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT6_AMBIENT,                    // (r,g,b,a)
        LIGHT6_DIFFUSE,                    // (r,g,b,a)
        LIGHT6_SPECULAR,                   // (r,g,b,a)
        LIGHT6_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT6_ATTENUATION,                // (const,lin,quad,intensity)

        LIGHT7_MODEL_POSITION,             // (x,y,z,1)
        LIGHT7_MODEL_DIRECTION,            // (x,y,z,0)
        LIGHT7_WORLD_POSITION,             // (x,y,z,1)
        LIGHT7_WORLD_DIRECTION,            // (x,y,z,0)
        LIGHT7_AMBIENT,                    // (r,g,b,a)
        LIGHT7_DIFFUSE,                    // (r,g,b,a)
        LIGHT7_SPECULAR,                   // (r,g,b,a)
        LIGHT7_SPOTCUTOFF,                 // (angle,cos,sin,exponent)
        LIGHT7_ATTENUATION,                // (const,lin,quad,intensity)

        MAX_TYPES
    };

    // Construction and destruction.  The base register must be nonnegative.
    // The register quantity is between 1 and 4.  Each register represents
    // four floating-point values.
    RendererConstant (Type eType, int iBaseRegister, int iRegisterQuantity);
    ~RendererConstant ();

    // Member access.  The renderer will use these to set the registers with
    // the appropriate values.
    Type GetType () const;
    int GetBaseRegister () const;
    int GetRegisterQuantity () const;
    float* GetData () const;

    // Mappings between enums and strings.
    static const std::string& GetName (Type eType);
    static Type GetType (const std::string& rkName);

private:
    Type m_eType;
    int m_iBaseRegister;
    int m_iRegisterQuantity;  // maximum of 4
    float m_afData[16];       // maximum storage, avoid dynamic allocation

    // For a lookup of the renderer constant string from its type.
    static std::string ms_kStringMap[MAX_TYPES+1];

    // For a lookup of the renderer constant type from its string name.
    static std::map<std::string,Type>* ms_pkTypeMap;
};

WM4_REGISTER_INITIALIZE(RendererConstant);
WM4_REGISTER_TERMINATE(RendererConstant);
#include "Wm4RendererConstant.inl"

}

#endif
