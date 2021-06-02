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
// Version: 4.6.0 (2007/08/22)

#ifndef VERTEXTEXTURES_H
#define VERTEXTEXTURES_H

// WARNING.  This sample application requires that your graphics card supports
// vertex textures.  For OpenGL, use the following Cg commands to compile the
// shader programs:
//    cgc -profile vp40 -entry v_HeightEffect -o v_HeightEffect.ogl.wmsp HeightEffect.cg
//    cgc -profile fp40 -entry p_HeightEffect -o p_HeightEffect.ogl.wmsp HeightEffect.cg

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class VertexTextures : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    VertexTextures ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    TriMesh* CreateRectangle (int iXSamples, int iYSamples, float fXExtent,
        float fYExtent);

    NodePtr m_spkScene;
    ImagePtr m_spkHeight, m_spkHeight32;
    Culler m_kCuller;
    WireframeStatePtr m_spkWireframe;
};

WM4_REGISTER_INITIALIZE(VertexTextures);

#endif
