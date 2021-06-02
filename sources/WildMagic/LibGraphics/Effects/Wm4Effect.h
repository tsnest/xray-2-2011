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
// Version: 4.0.1 (2007/01/19)

#ifndef WM4EFFECT_H
#define WM4EFFECT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"

namespace Wm4
{

class Geometry;
class Renderer;
class Spatial;
class VisibleObject;

class WM4_GRAPHICS_ITEM Effect : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~Effect ();

    // Override this function to obtain whatever drawing behavior your effect
    // requires.  If this is not overridden, the default behavior is to
    // draw all the Geometry objects in akVisible.
    virtual void Draw (Renderer* pkRenderer, Spatial* pkGlobalObject,
        int iMin, int iMax, VisibleObject* akVisible);

    // Override these to allow loading and releasing of any resources your
    // effect requires.  The defaults are to do nothing.  The functions are
    // called by Renderer::LoadResources and Renderer::ReleaseResources for
    // Geometry and Effect objects.
    virtual void LoadResources (Renderer* pkRenderer, Geometry* pkGeometry);
    virtual void ReleaseResources (Renderer* pkRenderer,
        Geometry* pkGeometry);

protected:
    Effect ();
};

WM4_REGISTER_STREAM(Effect);
typedef Pointer<Effect> EffectPtr;

}

#endif
