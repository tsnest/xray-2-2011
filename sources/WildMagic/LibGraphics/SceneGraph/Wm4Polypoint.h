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

#ifndef WM4POLYPOINT_H
#define WM4POLYPOINT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Geometry.h"
#include "Wm4IntArray.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Polypoint : public Geometry
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // construction and destruction
    Polypoint (VertexBuffer* pkVBuffer);
    virtual ~Polypoint ();

    // member access
    void SetActiveQuantity (int iActiveQuantity);
    int GetActiveQuantity () const;

protected:
    Polypoint ();

    // Allow application to specify fewer than the maximum number of vertices
    // to draw.
    int m_iActiveQuantity;
};

WM4_REGISTER_STREAM(Polypoint);
typedef Pointer<Polypoint> PolypointPtr;
#include "Wm4Polypoint.inl"

}

#endif
