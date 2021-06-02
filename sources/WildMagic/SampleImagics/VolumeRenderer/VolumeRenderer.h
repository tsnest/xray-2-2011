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

#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include "Wm4WindowApplication2.h"
#include "RayTrace.h"
using namespace Wm4;

class VolumeRenderer : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    VolumeRenderer ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnMotion (int iButton, int iX, int iY,
        unsigned int uiModifiers);

protected:
    int m_iBound;
    RayTrace* m_pkRT;
    float m_fX0, m_fY0, m_fX1, m_fY1, m_fHBound, m_fGamma;
    bool m_bButtonDown;
};

WM4_REGISTER_INITIALIZE(VolumeRenderer);

#endif
