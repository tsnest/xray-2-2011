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
// Version: 4.0.1 (2006/08/07)

#ifndef WM4SWITCHNODE_H
#define WM4SWITCHNODE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Node.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM SwitchNode : public Node
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    SwitchNode ();
    virtual ~SwitchNode ();

    enum { SN_INVALID_CHILD = -1 };

    void SetActiveChild (int iActiveChild);
    int GetActiveChild () const;
    void DisableAllChildren ();

protected:
    // culling
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull);

    int m_iActiveChild;
};

WM4_REGISTER_STREAM(SwitchNode);
typedef Pointer<SwitchNode> SwitchNodePtr;
#include "Wm4SwitchNode.inl"

}

#endif
