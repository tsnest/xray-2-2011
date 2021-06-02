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
// Version: 4.0.2 (2006/08/05)

#ifndef CLODMESHES_H
#define CLODMESHES_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

// The sample creates a clodified mesh with a texture.  To see the original
// mesh with the texture, comment out the #define of USE_CLOD_MESH.
#define USE_CLOD_MESH

class ClodMeshes : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    ClodMeshes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

#ifdef USE_CLOD_MESH
    void DrawTriangleQuantity (int iX, int iY, const ColorRGBA& rkColor);
    virtual void MoveForward ();
    virtual void MoveBackward ();
    void UpdateClods ();
    ClodMeshPtr m_aspkClod[2], m_spkActive;
#endif
};

WM4_REGISTER_INITIALIZE(ClodMeshes);

#endif
