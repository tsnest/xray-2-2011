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

#ifndef WM4MATERIALSTATE_H
#define WM4MATERIALSTATE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4GlobalState.h"
#include "Wm4ColorRGB.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM MaterialState : public GlobalState
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;
    WM4_DECLARE_INITIALIZE;
    WM4_DECLARE_TERMINATE;

public:
    virtual StateType GetStateType () const { return MATERIAL; }

    MaterialState ();
    virtual ~MaterialState ();

    ColorRGB Emissive;  // default: ColorRGB(0,0,0)
    ColorRGB Ambient;   // default: ColorRGB(0.2,0.2,0.2)
    ColorRGB Diffuse;   // default: ColorRGB(0.8,0.8,0.8)
    ColorRGB Specular;  // default: ColorRGB(0,0,0)
    float Alpha;        // default: 1
    float Shininess;    // default: 1
};

WM4_REGISTER_STREAM(MaterialState);
WM4_REGISTER_INITIALIZE(MaterialState);
WM4_REGISTER_TERMINATE(MaterialState);
typedef Pointer<MaterialState> MaterialStatePtr;

}

#endif
