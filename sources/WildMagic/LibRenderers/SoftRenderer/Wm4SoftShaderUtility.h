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

#ifndef WM4SOFTSHADERUTILITY_H
#define WM4SOFTSHADERUTILITY_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4ColorRGB.h"
#include "Wm4ColorRGBA.h"
#include "Wm4Matrix2.h"
#include "Wm4Matrix3.h"
#include "Wm4Matrix4.h"

namespace Wm4
{
// Map the components from [-1,1] to [0,1].
WM4_RENDERER_ITEM
float MapToUnit (float fValue);

WM4_RENDERER_ITEM
Vector2f MapToUnit (Vector2f kValue);

WM4_RENDERER_ITEM
Vector3f MapToUnit (Vector3f kValue);

WM4_RENDERER_ITEM
Vector4f MapToUnit (Vector4f kValue);

// Map the components from [0,1] to [-1,1].
WM4_RENDERER_ITEM
float MapFromUnit (float fValue);

WM4_RENDERER_ITEM
Vector2f MapFromUnit (Vector2f kValue);

WM4_RENDERER_ITEM
Vector3f MapFromUnit (Vector3f kValue);

WM4_RENDERER_ITEM
Vector4f MapFromUnit (Vector4f kValue);

// Clamp the components to [0,1].
WM4_RENDERER_ITEM
float Saturate (float fValue);

WM4_RENDERER_ITEM
ColorRGB Saturate (ColorRGB kValue);

WM4_RENDERER_ITEM
ColorRGBA Saturate (ColorRGBA kValue);

WM4_RENDERER_ITEM
Vector2f Saturate (Vector2f kValue);

WM4_RENDERER_ITEM
Vector3f Saturate (Vector3f kValue);

WM4_RENDERER_ITEM
Vector4f Saturate (Vector4f kValue);

// Linearly interpolate the components.  If V0, V1, and t are the inputs,
// the output is V = (1-t)*V0 + t*V1.
WM4_RENDERER_ITEM
float Lerp (float fV0, float fV1, float fT);

WM4_RENDERER_ITEM
ColorRGB Lerp (const ColorRGB& rkV0, const ColorRGB& rkV1, float fT);

WM4_RENDERER_ITEM
ColorRGBA Lerp (const ColorRGBA& rkV0, const ColorRGBA& rkV1, float fT);

WM4_RENDERER_ITEM
Vector2f Lerp (const Vector2f& rkV0, const Vector2f& rkV1, float fT);

WM4_RENDERER_ITEM
Vector3f Lerp (const Vector3f& rkV0, const Vector3f& rkV1, float fT);

WM4_RENDERER_ITEM
Vector4f Lerp (const Vector4f& rkV0, const Vector4f& rkV1, float fT);

// The incident direction is I, a nonzero vector.  The normal vector is N and
// must be unit length.  The reflection vector is
//    R = I - 2*Dot(I,N)*N
// which is a vector having the same length as I.
WM4_RENDERER_ITEM
Vector3f Reflect (const Vector3f& rkIncident, const Vector3f& rkNormal);

// The incident direction is I, a nonzero vector.  The normal vector is N and
// must be unit length.  The refraction vector is
//    R = e*I - sqrt(1 - e^2*(1-Dot(I,N)^2))*N
// where e is the ratio of the indices of refraction and when the argument of
// sqrt is positive.  If the argument of sqrt is nonpositive, the function
// returns the zero vector.
WM4_RENDERER_ITEM
Vector3f Refract (const Vector3f& rkIncident, const Vector3f& rkNormal,
    float fEtaRatio);

// Lighting utilities.
WM4_RENDERER_ITEM
void GetDirectionalLightFactors (const Vector3f& rkModelPosition,
    const Vector3f& rkModelNormal, const Vector3f& rkCameraPosition,
    const Vector3f& rkLightDirection, float fSpecularExponent,
    float& rfDiffuseFactor, float& rfSpecularFactor);

WM4_RENDERER_ITEM
void GetPointLightFactors (const Vector3f& rkModelPosition,
    const Vector3f& rkModelNormal, const Vector3f& rkCameraPosition,
    const Vector3f& rkLightPosition, float fSpecularExponent,
    float& rfDiffuseFactor, float& rfSpecularFactor);

WM4_RENDERER_ITEM
void GetSpotLightFactors (const Vector3f& rkModelPosition,
    const Vector3f& rkModelNormal, const Vector3f& rkCameraPosition,
    const Vector3f& rkLightPosition, float fSpecularExponent,
    const Vector3f& rkSpotAxis, float fSpotCosAngle, float fSpotExponent,
    float& rfDiffuseFactor, float& rfSpecularFactor, float& rfSpotFactor);

WM4_RENDERER_ITEM
float GetAttenuation (const Matrix4f& rkWMatrix, const Vector3f& rkModelPos,
    const Vector3f& rkLightPos, const float* afAttenuation);
}

#endif
