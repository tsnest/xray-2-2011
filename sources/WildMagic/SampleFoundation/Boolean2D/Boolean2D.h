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
// Version: 4.0.1 (2006/09/25)

#ifndef BOOLEAN2D_H
#define BOOLEAN2D_H

#include "Wm4WindowApplication2.h"
#include "Polygon2.h"

class Boolean2D : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    Boolean2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    Polygon2* ConstructInvertedEll ();
    Polygon2* ConstructPentagon ();
    Polygon2* ConstructSquare ();
    Polygon2* ConstructSShape ();
    Polygon2* ConstructPolyWithHoles ();

    void DoBoolean ();
    void DrawPolySolid (Polygon2& rkP, Color kColor);

    Polygon2 m_kIntersection, m_kUnion, m_kDiff01, m_kDiff10, m_kXor;
    Polygon2* m_pkPoly0;
    Polygon2* m_pkPoly1;
    Polygon2* m_pkActive;
    int m_iChoice;
};

WM4_REGISTER_INITIALIZE(Boolean2D);

#endif
