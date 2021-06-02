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
// Version: 4.7.0 (2008/09/13)

#ifndef BSPLINESURFACEFITTER_H
#define BSPLINESURFACEFITTER_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class BSplineSurfaceFitter : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    BSplineSurfaceFitter ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void UpdateFog ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkHeightField;
    TriMeshPtr m_spkFittedField;
    ImagePtr m_spkHeightImage;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(BSplineSurfaceFitter);

#endif
