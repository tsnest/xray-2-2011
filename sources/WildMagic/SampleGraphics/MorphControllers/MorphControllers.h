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

#ifndef MORPHCONTROLLERS_H
#define MORPHCONTROLLERS_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class MorphControllers : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    MorphControllers ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);

protected:
    void CreateScene ();
    void LoadTargets ();
    void CreateFace ();
    void CreateController ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkFace;
    Culler m_kCuller;

    // shared data
    IndexBufferPtr m_spkIBuffer;
    MaterialStatePtr m_spkMaterial;
    LightPtr m_spkLight;

    enum
    {
        TARGET_QUANTITY = 5,
        KEY_QUANTITY = 6
    };
    TriMeshPtr m_aspkTarget[TARGET_QUANTITY];

    // animation time
    double m_dBaseTime, m_dCurrTime;

    // picking
    enum { PICK_MESSAGE_SIZE = 128 };
    int m_iXPick, m_iYPick;
    char m_acPickMessage[PICK_MESSAGE_SIZE];
    bool m_bPickPending;
    Picker m_kPicker;
};

WM4_REGISTER_INITIALIZE(MorphControllers);

#endif
