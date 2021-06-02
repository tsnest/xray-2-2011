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
// Version: 4.0.1 (2006/08/07)

#ifndef CASTLE_H
#define CASTLE_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class Castle : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    Castle ();
    virtual ~Castle ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);

protected:
    // arrange for camera to be fixed distance above the nearest object
    void AdjustVerticalDistance ();
    virtual void MoveForward ();
    virtual void MoveBackward ();
    float m_fVerticalDistance;

    void SetAllWireframe (Spatial* pkSpatial);
    void ToggleAllWireframe (Spatial* pkSpatial);

    NodePtr m_spkScene;
    NodePtr m_spkModel;
    Culler m_kCuller;

    // name of picked object
    enum { PICKED_SIZE = 1024 };
    char m_acPicked[PICKED_SIZE];

    // pick ray parameters
    bool AllowMotion (float fSign);
    int m_iNumRays;
    float m_fHalfAngle;
    float* m_afCos;
    float* m_afSin;
    float* m_afTolerance;
    Picker m_kPicker;
};

WM4_REGISTER_INITIALIZE(Castle);

#endif
