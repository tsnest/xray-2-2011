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

#ifndef WRIGGLINGSNAKE_H
#define WRIGGLINGSNAKE_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class WrigglingSnake : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    WrigglingSnake ();
    virtual ~WrigglingSnake ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateSnake ();
    void CreateSnakeBody ();
    void CreateSnakeHead ();
    void UpdateSnake ();
    void UpdateSnakeBody ();
    void UpdateSnakeHead ();
    void ModifyCurve ();
    static float Radial (float fT);

    NodePtr m_spkScene, m_spkTrnNode, m_spkSnakeRoot;
    TubeSurfacePtr m_spkSnakeBody;
    TriMeshPtr m_spkSnakeHead;
    WireframeStatePtr m_spkWireframe;
    Culler m_kCuller;

    int m_iNumCtrl, m_iDegree;
    BSplineCurve3f* m_pkCenter;
    float* m_afAmplitude;
    float* m_afPhase;
    int m_iShellQuantity;
    static float ms_fRadius;
};

WM4_REGISTER_INITIALIZE(WrigglingSnake);

#endif
