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
// Version: 4.0.1 (2006/08/23)

#ifndef SIMPLEBUMPMAPEFFECT_H
#define SIMPLEBUMPMAPEFFECT_H

#include "Wm4ShaderEffect.h"
#include "Wm4Light.h"
#include "Wm4Triangles.h"

namespace Wm4
{

class SimpleBumpMapEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    SimpleBumpMapEffect (const char* acBaseName, const char* acNormalName,
        const Vector3f& rkLightDirection);
    virtual ~SimpleBumpMapEffect ();

    void SetLightDirection (const Vector3f& rkLightDirection);
    const Vector3f& GetLightDirection () const;
    void ComputeLightVectors (Triangles* pkMesh);

protected:
    SimpleBumpMapEffect ();

    // Compute a tangent at the vertex P0.  The triangle is counterclockwise
    // ordered, <P0,P1,P2>.
    bool ComputeTangent (
        const Vector3f& rkPos0, const Vector2f& rkTCoord0,
        const Vector3f& rkPos1, const Vector2f& rkTCoord1,
        const Vector3f& rkPos2, const Vector2f& rkTCoord2,
        Vector3f& rkTangent);

    Vector3f m_kLightDirection;
};

WM4_REGISTER_STREAM(SimpleBumpMapEffect);
typedef Pointer<SimpleBumpMapEffect> SimpleBumpMapEffectPtr;

}

#endif
