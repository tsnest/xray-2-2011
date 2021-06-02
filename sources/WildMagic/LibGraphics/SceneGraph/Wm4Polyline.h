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

#ifndef WM4POLYLINE_H
#define WM4POLYLINE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Geometry.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Polyline : public Geometry
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // construction and destruction
    Polyline (VertexBuffer* pkVBuffer, bool bClosed, bool bContiguous);
    virtual ~Polyline ();

    // member access
    void SetActiveQuantity (int iActiveQuantity);
    int GetActiveQuantity () const;

    void SetClosed (bool bClosed);
    bool GetClosed () const;

    void SetContiguous (bool bContiguous);
    bool GetContiguous () const;

protected:
    Polyline ();
    void SetGeometryType ();

    // Allow application to specify fewer than the maximum number of vertices
    // to draw.
    int m_iActiveQuantity;

    // polyline is open or closed, contiguous or disjoint segments
    bool m_bClosed, m_bContiguous;
};

WM4_REGISTER_STREAM(Polyline);
typedef Pointer<Polyline> PolylinePtr;
#include "Wm4Polyline.inl"

}

#endif
