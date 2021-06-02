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
// Version: 4.0.1 (2008/11/01)

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftShaderUtility.h"

namespace Wm4
{
//----------------------------------------------------------------------------
float MapToUnit (float fValue)
{
    fValue = 0.5f*fValue + 0.5f;
    return fValue;
}
//----------------------------------------------------------------------------
Vector2f MapToUnit (Vector2f kValue)
{
    for (int i = 0; i < 2; i++)
    {
        kValue[i] = 0.5f*kValue[i] + 0.5f;
    }
    return kValue;
}
//----------------------------------------------------------------------------
Vector3f MapToUnit (Vector3f kValue)
{
    for (int i = 0; i < 3; i++)
    {
        kValue[i] = 0.5f*kValue[i] + 0.5f;
    }
    return kValue;
}
//----------------------------------------------------------------------------
Vector4f MapToUnit (Vector4f kValue)
{
    for (int i = 0; i < 4; i++)
    {
        kValue[i] = 0.5f*kValue[i] + 0.5f;
    }
    return kValue;
}
//----------------------------------------------------------------------------
float MapFromUnit (float fValue)
{
    fValue = 2.0f*fValue - 1.0f;
    return fValue;
}
//----------------------------------------------------------------------------
Vector2f MapFromUnit (Vector2f kValue)
{
    for (int i = 0; i < 2; i++)
    {
        kValue[i] = 2.0f*kValue[i] - 1.0f;
    }
    return kValue;
}
//----------------------------------------------------------------------------
Vector3f MapFromUnit (Vector3f kValue)
{
    for (int i = 0; i < 3; i++)
    {
        kValue[i] = 2.0f*kValue[i] - 1.0f;
    }
    return kValue;
}
//----------------------------------------------------------------------------
Vector4f MapFromUnit (Vector4f kValue)
{
    for (int i = 0; i < 4; i++)
    {
        kValue[i] = 2.0f*kValue[i] - 1.0f;
    }
    return kValue;
}
//----------------------------------------------------------------------------
float Saturate (float fValue)
{
    if (fValue < 0.0f)
    {
        fValue = 0.0f;
    }
    else if (fValue > 1.0f)
    {
        fValue = 1.0f;
    }

    return fValue;
}
//----------------------------------------------------------------------------
ColorRGB Saturate (ColorRGB kValue)
{
    for (int i = 0; i < 3; i++)
    {
        if (kValue[i] < 0.0f)
        {
            kValue[i] = 0.0f;
        }
        else if (kValue[i] > 1.0f)
        {
            kValue[i] = 1.0f;
        }
    }

    return kValue;
}
//----------------------------------------------------------------------------
ColorRGBA Saturate (ColorRGBA kValue)
{
    for (int i = 0; i < 4; i++)
    {
        if (kValue[i] < 0.0f)
        {
            kValue[i] = 0.0f;
        }
        else if (kValue[i] > 1.0f)
        {
            kValue[i] = 1.0f;
        }
    }

    return kValue;
}
//----------------------------------------------------------------------------
Vector2f Saturate (Vector2f kValue)
{
    for (int i = 0; i < 2; i++)
    {
        if (kValue[i] < 0.0f)
        {
            kValue[i] = 0.0f;
        }
        else if (kValue[i] > 1.0f)
        {
            kValue[i] = 1.0f;
        }
    }

    return kValue;
}
//----------------------------------------------------------------------------
Vector3f Saturate (Vector3f kValue)
{
    for (int i = 0; i < 3; i++)
    {
        if (kValue[i] < 0.0f)
        {
            kValue[i] = 0.0f;
        }
        else if (kValue[i] > 1.0f)
        {
            kValue[i] = 1.0f;
        }
    }

    return kValue;
}
//----------------------------------------------------------------------------
Vector4f Saturate (Vector4f kValue)
{
    for (int i = 0; i < 4; i++)
    {
        if (kValue[i] < 0.0f)
        {
            kValue[i] = 0.0f;
        }
        else if (kValue[i] > 1.0f)
        {
            kValue[i] = 1.0f;
        }
    }

    return kValue;
}
//----------------------------------------------------------------------------
float Lerp (float fV0, float fV1, float fT)
{
    return fV0 + fT*(fV1 - fV0);
}
//----------------------------------------------------------------------------
ColorRGB Lerp (const ColorRGB& rkV0, const ColorRGB& rkV1, float fT)
{
    return rkV0 + fT*(rkV1 - rkV0);
}
//----------------------------------------------------------------------------
ColorRGBA Lerp (const ColorRGBA& rkV0, const ColorRGBA& rkV1, float fT)
{
    return rkV0 + fT*(rkV1 - rkV0);
}
//----------------------------------------------------------------------------
Vector2f Lerp (const Vector2f& rkV0, const Vector2f& rkV1, float fT)
{
    return rkV0 + fT*(rkV1 - rkV0);
}
//----------------------------------------------------------------------------
Vector3f Lerp (const Vector3f& rkV0, const Vector3f& rkV1, float fT)
{
    return rkV0 + fT*(rkV1 - rkV0);
}
//----------------------------------------------------------------------------
Vector4f Lerp (const Vector4f& rkV0, const Vector4f& rkV1, float fT)
{
    return rkV0 + fT*(rkV1 - rkV0);
}
//----------------------------------------------------------------------------
Vector3f Reflect (const Vector3f& rkIncident, const Vector3f& rkNormal)
{
    float fDot = rkIncident.Dot(rkNormal);
    return rkIncident - 2.0f*fDot*rkNormal;
}
//----------------------------------------------------------------------------
Vector3f Refract (const Vector3f& rkIncident, const Vector3f& rkNormal,
    float fEtaRatio)
{
    float fCos1 = -(rkIncident.Dot(rkNormal));
    float fCos2Sqr = 1.0f - fEtaRatio*fEtaRatio*(1.0f - fCos1*fCos1);
    if (fCos2Sqr > 0.0f)
    {
        float fCos2 = Mathf::Sqrt(fCos2Sqr);
        return fEtaRatio*rkIncident + (fEtaRatio*fCos1 - fCos2)*rkNormal;
    }
    return Vector3f::ZERO;
}
//----------------------------------------------------------------------------
void GetDirectionalLightFactors (const Vector3f& rkModelPosition,
    const Vector3f& rkModelNormal, const Vector3f& rkCameraPosition,
    const Vector3f& rkLightDirection, float fSpecularExponent,
    float& rfDiffuseFactor, float& rfSpecularFactor)
{
    rfDiffuseFactor = -rkModelNormal.Dot(rkLightDirection);
    if (rfDiffuseFactor < 0.0f)
    {
        rfDiffuseFactor = 0.0f;
    }

    Vector3f kViewVector = rkCameraPosition - rkModelPosition;
    kViewVector.Normalize();
    Vector3f kHalfVector = kViewVector - rkLightDirection;
    kHalfVector.Normalize();
    rfSpecularFactor = rkModelNormal.Dot(kHalfVector);
    if (rfSpecularFactor > 0.0f)
    {
        rfSpecularFactor = Mathf::Pow(rfSpecularFactor,fSpecularExponent);
    }
    else
    {
        rfSpecularFactor = 0.0f;
    }
}
//----------------------------------------------------------------------------
void GetPointLightFactors (const Vector3f& rkModelPosition,
    const Vector3f& rkModelNormal, const Vector3f& rkCameraPosition,
    const Vector3f& rkLightPosition, float fSpecularExponent,
    float& rfDiffuseFactor, float& rfSpecularFactor)
{
    Vector3f kVertexDirection = rkModelPosition - rkLightPosition;
    kVertexDirection.Normalize();
    rfDiffuseFactor = -rkModelNormal.Dot(kVertexDirection);
    if (rfDiffuseFactor < 0.0f)
    {
        rfDiffuseFactor = 0.0f;
    }

    Vector3f kViewVector = rkCameraPosition - rkModelPosition;
    kViewVector.Normalize();
    Vector3f kHalfVector = kViewVector - kVertexDirection;
    kHalfVector.Normalize();
    rfSpecularFactor = rkModelNormal.Dot(kHalfVector);
    if (rfSpecularFactor > 0.0f)
    {
        rfSpecularFactor = Mathf::Pow(rfSpecularFactor,fSpecularExponent);
    }
    else
    {
        rfSpecularFactor = 0.0f;
    }
}
//----------------------------------------------------------------------------
void GetSpotLightFactors (const Vector3f& rkModelPosition,
    const Vector3f& rkModelNormal, const Vector3f& rkCameraPosition,
    const Vector3f& rkLightPosition, float fSpecularExponent,
    const Vector3f& rkSpotAxis, float fSpotCosAngle, float fSpotExponent,
    float& rfDiffuseFactor, float& rfSpecularFactor, float& rfSpotFactor)
{
    Vector3f kVertexDirection = rkModelPosition - rkLightPosition;
    kVertexDirection.Normalize();
    float fVertexCosAngle = rkSpotAxis.Dot(kVertexDirection);
    if (fVertexCosAngle >= fSpotCosAngle)
    {
        rfDiffuseFactor = -rkModelNormal.Dot(kVertexDirection);
        if (rfDiffuseFactor < 0.0f)
        {
            rfDiffuseFactor = 0.0f;
        }

        Vector3f kViewVector = rkCameraPosition - rkModelPosition;
        kViewVector.Normalize();
        Vector3f kHalfVector = kViewVector - kVertexDirection;
        kHalfVector.Normalize();
        rfSpecularFactor = rkModelNormal.Dot(kHalfVector);
        if (rfSpecularFactor > 0.0f)
        {
            rfSpecularFactor = Mathf::Pow(rfSpecularFactor,
                fSpecularExponent);
        }
        else
        {
            rfSpecularFactor = 0.0f;
        }
        rfSpotFactor = Mathf::Pow(fVertexCosAngle,fSpotExponent);
    }
    else
    {
        rfDiffuseFactor = 0.0f;
        rfSpecularFactor = 0.0f;
        rfSpotFactor = 0.0f;
    }
}
//----------------------------------------------------------------------------
float GetAttenuation (const Matrix4f& rkWMatrix, const Vector3f& rkModelPos,
    const Vector3f& rkLightPos, const float* afAttenuation)
{
    // Attenuate the color (x=constant, y=linear, z=quadratic, w=intensity).
    // Attenuation is not active when the x component is zero.  The distance
    // must be computed in *world* coordinates.  The distance in camera
    // coordinates is not correct when the MVP matrix has nonunit scaling
    // factors.

    Vector4f kVertexModelDir;
    kVertexModelDir[0] = rkModelPos[0] - rkLightPos[0];
    kVertexModelDir[1] = rkModelPos[1] - rkLightPos[1];
    kVertexModelDir[2] = rkModelPos[2] - rkLightPos[2];
    kVertexModelDir[3] = 0.0f;
    Vector3f kVertexWorldDir = (Vector3f)(kVertexModelDir*rkWMatrix);
    float fDistance = Mathf::Sqrt(
        kVertexWorldDir[0]*kVertexWorldDir[0] +
        kVertexWorldDir[1]*kVertexWorldDir[1] +
        kVertexWorldDir[2]*kVertexWorldDir[2]);

    float fAdjust = afAttenuation[3]/(afAttenuation[0] + fDistance*(
        afAttenuation[1] + fDistance*afAttenuation[2]));

    return fAdjust;
}
//----------------------------------------------------------------------------
}
