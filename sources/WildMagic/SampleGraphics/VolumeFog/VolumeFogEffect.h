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

#ifndef WM4VOLUMEFOGEFFECT_H
#define WM4VOLUMEFOGEFFECT_H

#include "Wm4ShaderEffect.h"

namespace Wm4
{

class VolumeFogEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    VolumeFogEffect (const std::string& rkBaseName);
    virtual ~VolumeFogEffect ();

protected:
    // streaming
    VolumeFogEffect ();
};

WM4_REGISTER_STREAM(VolumeFogEffect);
typedef Pointer<VolumeFogEffect> VolumeFogEffectPtr;

}

#endif
