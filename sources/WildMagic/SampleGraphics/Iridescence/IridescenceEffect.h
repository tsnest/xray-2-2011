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

#ifndef IRIDESCENCEEFFECT_H
#define IRIDESCENCEEFFECT_H

#include "Wm4ShaderEffect.h"

namespace Wm4
{

class IridescenceEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    IridescenceEffect (const char* acBaseName, const char* acGradName);
    virtual ~IridescenceEffect ();

    void SetInterpolateFactor (float fInterpolateFactor);
    float GetInterpolateFactor () const;

protected:
    // streaming
    IridescenceEffect ();

    // The interpolation factor is stored at index 0.  The other values are
    // unused.
    float m_afInterpolate[4];
};

WM4_REGISTER_STREAM(IridescenceEffect);
typedef Pointer<IridescenceEffect> IridescenceEffectPtr;

#include "IridescenceEffect.inl"

}

#endif
