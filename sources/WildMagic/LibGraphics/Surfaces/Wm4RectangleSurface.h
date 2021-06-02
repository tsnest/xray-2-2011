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

#ifndef WM4RECTANGLESURFACE_H
#define WM4RECTANGLESURFACE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4ParametricSurface.h"
#include "Wm4TriMesh.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM RectangleSurface : public TriMesh
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  The input surface must be rectangular,
    // not triangular.  RectangleSurface accepts responsibility for deleting
    // the input surface.  If bWantColors is 'true', the vertex colors are
    // allocated and set to black.  The application needs to assign colors as
    // needed.  If either of pkUVMin or pkUVMax is not null, both must be not
    // null.  In this case, texture coordinates are generated for the
    // surface.
    RectangleSurface (ParametricSurfacef* pkSurface, int iUSamples,
        int iVSamples, const Attributes& rkAttr, bool bDoubleSided,
        const Vector2f* pkTCoordMin, const Vector2f* pkTCoordMax);

    virtual ~RectangleSurface ();

    // member access
    ParametricSurfacef*& Surface ();
    const ParametricSurfacef* GetSurface () const;
    int GetUSamples () const;
    int GetVSamples () const;

    // If the surface is modified, for example if it is control point
    // based and the control points are modified, then you should call this
    // update function to recompute the rectangle surface geometry.
    void UpdateSurface ();

protected:
    RectangleSurface ();

    ParametricSurfacef* m_pkSurface;
    int m_iUSamples, m_iVSamples;
};

WM4_REGISTER_STREAM(RectangleSurface);
typedef Pointer<RectangleSurface> RectangleSurfacePtr;
#include "Wm4RectangleSurface.inl"

}

#endif
