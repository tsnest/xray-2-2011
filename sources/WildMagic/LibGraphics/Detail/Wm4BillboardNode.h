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

#ifndef WM4BILLBOARDNODE_H
#define WM4BILLBOARDNODE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Node.h"
#include "Wm4Camera.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM BillboardNode : public Node
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // The model space of the billboard has an up vector of (0,1,0) that is
    // chosen to be the billboard's axis of rotation.

    // Construction and destruction.
    BillboardNode (Camera* pkCamera = 0);
    virtual ~BillboardNode ();

    // the camera to which the billboard is aligned
    void AlignTo (Camera* pkCamera);

protected:
    // geometric updates
    virtual void UpdateWorldData (double dAppTime);

    Pointer<Camera> m_spkCamera;
};

WM4_REGISTER_STREAM(BillboardNode);
typedef Pointer<BillboardNode> BillboardNodePtr;
#include "Wm4BillboardNode.inl"

}

#endif
