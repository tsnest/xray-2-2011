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

#ifndef CONVEXHULL2D_H
#define CONVEXHULL2D_H

#include "Wm4WindowApplication2.h"
using namespace Wm4;

class ConvexHull2D : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    ConvexHull2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual void ScreenOverlay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void GenerateHull0D ();
    void GenerateHull1D ();
    void GenerateHull2D ();
    void GenerateHullManyCollinear ();
    void RegenerateHull ();
    int UnitToScreen (float fValue);

    const float m_fEpsilon;
    Query::Type m_eQueryType;
    ConvexHullf* m_pkHull;
    int m_iVQuantity;
    Vector2f* m_akVertex;
};

WM4_REGISTER_INITIALIZE(ConvexHull2D);

#endif
