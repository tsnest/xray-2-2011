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

#ifndef MAPTEXTURETOQUAD_H
#define MAPTEXTURETOQUAD_H

#include "Wm4WindowApplication2.h"
using namespace Wm4;

// If this is defined, use a HmQuadToSqrf mapping.  If this is not defined,
// use a BiQuadToSqrf mapping.
#define USE_HM_QUAD_TO_SQR

class MapTextureToQuad : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    MapTextureToQuad ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();

    // allows user to drag vertices of convex quadrilateral
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnMotion (int iButton, int iX, int iY,
        unsigned int uiModifiers);

protected:
    // the image to perspectively draw onto the convex quadrilateral.
    Image* m_pkImage;

    // The four vertices of the convex quadrilateral in counterclockwise
    // order:  Q00 = V[0], Q10 = V[1], Q11 = V[2], Q01 = V[3].
    void CreateMapping ();
    Vector2f m_akVertex[4];
#ifdef USE_HM_QUAD_TO_SQR
    HmQuadToSqrf* m_pkMap;
#else
    BiQuadToSqrf* m_pkMap;
#endif

    // for dragging the quadrilateral vertices
    void SelectVertex (const Vector2f& rkPos);
    void UpdateQuadrilateral (const Vector2f& rkPos);
    bool m_bMouseDown;
    int m_iSelected;
};

WM4_REGISTER_INITIALIZE(MapTextureToQuad);

#endif
