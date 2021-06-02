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

#ifndef DELAUNAY2D_H
#define DELAUNAY2D_H

#include "Wm4WindowApplication2.h"
using namespace Wm4;

class Delaunay2D : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    Delaunay2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);

protected:
    Delaunay1f* m_pkDel1;
    Delaunay2f* m_pkDel2;

    int m_iVQuantity;
    Vector2f* m_akVertex;
    int m_iTQuantity;
    int* m_aiTVertex;
    int* m_aiTAdjacent;

    int m_iCurrTriX, m_iCurrTriY;
    int m_iCurrentIndex;
};

WM4_REGISTER_INITIALIZE(Delaunay2D);

#endif
