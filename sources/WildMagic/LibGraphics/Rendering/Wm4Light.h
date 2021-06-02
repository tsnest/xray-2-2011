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

#ifndef WM4LIGHT_H
#define WM4LIGHT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4ColorRGB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Light : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    enum LightType
    {
        LT_AMBIENT,
        LT_DIRECTIONAL,
        LT_POINT,
        LT_SPOT,
        LT_QUANTITY
    };

    Light (LightType eType);
    virtual ~Light ();

    // If the Light class were to have no data, or just ambient color and
    // intensity, you could use a standard class hierarchy:
    //
    //   class Light
    //       [ambient, intensity]
    //   class AmbientLight : public Light
    //       [no additional data]
    //   class DirectionalLight : public Light
    //       [direction, diffuse, specular]
    //   class PointLight : public Light
    //       [position, diffuse, specular, attenuation]
    //   class SpotLight : public PointLight
    //       [cone axis, cone angle, spot exponent]
    //
    // The renderer holds onto lights via the base class Light.  The
    // consequences of a standard class hierarchy are that the renderer must
    // use dynamic casting to determine the type of a light in order to set
    // shader program constants in the Renderer::SetConstantLightFOOBAR calls.
    // This is an expense I wish to avoid.
    //
    // An alternative is to allow Light to store all the data in public scope,
    // but to derive the specific light classes using a protected Light base
    // class.  Thus, Renderer has access to all the data it needs without
    // having to dynamically cast and the derived-class objects have functions
    // to access only the data relevant to them.  Unfortunately, you run into
    // problems with access rights to Object items (such as increment and
    // decrement of reference counts for smart pointers).
    //
    // In the end, I chose to make the Light class a generic class that
    // stores everything needed by the various light types.

    LightType Type;     // default: LT_AMBIENT

    // The colors of the light.
    ColorRGB Ambient;   // default: ColorRGB(0,0,0)
    ColorRGB Diffuse;   // default: ColorRGB(0,0,0)
    ColorRGB Specular;  // default: ColorRGB(0,0,0)

    // Attenuation is typically specified as a modulator
    //   m = 1/(C + L*d + Q*d*d)
    // where C is the constant coefficient, L is the linear coefficient,
    // Q is the quadratic coefficient, and d is the distance from the light
    // position to the vertex position.  To allow for a linear adjustment of
    // intensity, my choice is to use instead
    //   m = I/(C + L*d + Q*d*d)
    // where I is an "intensity" factor.
    float Constant;     // default: 1
    float Linear;       // default: 0
    float Quadratic;    // default: 0
    float Intensity;    // default: 1

    // Parameters for spot lights.  The cone angle must be in radians and
    // should satisfy 0 < Angle <= pi.
    float Angle;        // default: pi
    float CosAngle;     // default: -1
    float SinAngle;     // default:  0
    float Exponent;     // default:  1

    // A helper function that lets you set Angle and have CosAngle and
    // SinAngle computed for you.
    void SetAngle (float fAngle);

    // Although the standard directional and spot lights need only a direction
    // vector, to allow for new types of derived-class lights that would use
    // a full coordinate frame, Light provides storage for such a frame.  The
    // light frame is always in world coordinates.
    //   default position  P = (0,0,0)
    //   default direction D = (0,0,-1)
    //   default up        U = (0,1,0)
    //   default right     R = (1,0,0)
    // The set {D,U,R} must be a right-handed orthonormal set.  That is, each
    // vector is unit length, the vectors are mutually perpendicular, and
    // R = Cross(D,U).
    Vector3f Position, DVector, UVector, RVector;

    // A helper function that lets you set the direction vector and computes
    // the up and right vectors automatically.
    void SetDirection (const Vector3f& rkDirection);

    // This is for debug mode to allow you to check if the coordinate frame
    // vectors form a right-handed orthonormal set.
    bool IsValidFrame () const;

protected:
    // Streaming support.
    Light ();

private:
    static const char* ms_aacType[LT_QUANTITY];
};

WM4_REGISTER_STREAM(Light);
typedef Pointer<Light> LightPtr;

}

#endif
