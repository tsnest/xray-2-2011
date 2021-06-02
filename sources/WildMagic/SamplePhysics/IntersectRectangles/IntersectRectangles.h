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

#ifndef INTERSECTRECTANGLES_H
#define INTERSECTRECTANGLES_H

#include "Wm4WindowApplication2.h"
using namespace Wm4;

class IntersectRectangles : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    IntersectRectangles ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void ModifyRectangles ();
    void DrawRectangles ();

    std::vector<AxisAlignedBox2f> m_kRects;
    IntersectingRectanglesf* m_pkIR;
    bool m_bMouseDown;
    float m_fLastIdle;
};

WM4_REGISTER_INITIALIZE(IntersectRectangles);

#endif
