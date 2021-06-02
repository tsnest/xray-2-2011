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
// Version: 4.0.2 (2008/01/20)

#ifndef WM4TRISTRIP_H
#define WM4TRISTRIP_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Triangles.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM TriStrip : public Triangles
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.
    TriStrip (VertexBuffer* pkVBuffer,
        bool bUpdateModelNormals = true,
        int iTangentSpaceType = GU_TANGENT_SPACE_NONE);

    TriStrip (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer,
        bool bUpdateModelNormals = true,
        int iTangentSpaceType = GU_TANGENT_SPACE_NONE);

    virtual ~TriStrip ();

    // Interpretation of the index buffer data.
    virtual int GetTriangleQuantity () const;
    virtual bool GetTriangle (int i, int& riV0, int& riV1, int& riV2) const;

protected:
    TriStrip ();
};

WM4_REGISTER_STREAM(TriStrip);
typedef Pointer<TriStrip> TriStripPtr;
#include "Wm4TriStrip.inl"

}

#endif
