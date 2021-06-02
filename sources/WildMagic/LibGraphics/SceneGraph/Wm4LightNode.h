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

#ifndef WM4LIGHTNODE_H
#define WM4LIGHTNODE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Light.h"
#include "Wm4Node.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM LightNode : public Node
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  The node's world translation is used
    // as the light's location.  The node's world rotation matrix is used
    // for the light's coordinate axes.  Column 0 of the world rotation
    // matrix is the light's direction vector, column 1 of the world rotation
    // matrix is the light's up vector, and column 2 of the world rotation
    // matrix is the light's right vector.
    //
    // On construction, the node's local transformation is set to the
    // light's current coordinate system.
    //   local translation       = light location
    //   local rotation column 0 = light direction
    //   local rotation column 1 = light up
    //   local rotation column 2 = light right
    LightNode (Light* pkLight = 0);
    virtual ~LightNode ();

    // When you set the light, the node's local transformation is set to the
    // light's current current coordinate system.  The node's world
    // transformation is computed, and the light's coordinate system is set
    // to use the node's world transformation.
    void SetLight (Light* pkLight);

    Light* GetLight ();
    const Light* GetLight () const;

protected:
    // geometric updates
    virtual void UpdateWorldData (double dAppTime);

    LightPtr m_spkLight;
};

WM4_REGISTER_STREAM(LightNode);
typedef Pointer<LightNode> LightNodePtr;
#include "Wm4LightNode.inl"

}

#endif
