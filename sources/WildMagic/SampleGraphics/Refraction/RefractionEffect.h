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
// Version: 4.0.1 (2006/08/11)

#ifndef REFRACTIONEFFECT_H
#define REFRACTIONEFFECT_H

#include "Wm4ShaderEffect.h"

namespace Wm4
{

class RefractionEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    RefractionEffect (const char* acEnvironmentName,
        const char* acReflectionName);
    virtual ~RefractionEffect ();

    void SetFresnelConstant (int i, float fValue);
    float GetFresnelConstant (int i) const;
    void SetEtaRatio (float fEtaRatio);
    float GetEtaRatio () const;

protected:
    // streaming
    RefractionEffect ();

    // The first three values are the Fresnel constants.  The fourth value
    // is the eta ratio.
    float m_afVConstant[4];
};

WM4_REGISTER_STREAM(RefractionEffect);
typedef Pointer<RefractionEffect> RefractionEffectPtr;

#include "RefractionEffect.inl"

}

#endif
