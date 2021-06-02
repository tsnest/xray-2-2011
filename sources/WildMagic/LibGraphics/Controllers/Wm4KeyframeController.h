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

#ifndef WM4KEYFRAMECONTROLLER_H
#define WM4KEYFRAMECONTROLLER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Controller.h"
#include "Wm4FloatArray.h"
#include "Wm4QuaternionArray.h"
#include "Wm4Vector3Array.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM KeyframeController : public Controller
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    KeyframeController ();
    virtual ~KeyframeController ();

    FloatArrayPtr TranslationTimes;
    Vector3fArrayPtr TranslationData;

    FloatArrayPtr RotationTimes;
    QuaternionfArrayPtr RotationData;

    FloatArrayPtr ScaleTimes;
    FloatArrayPtr ScaleData;

    // animation update
    virtual bool Update (double dAppTime);

protected:
    static void GetKeyInfo (float fCtrlTime, int iQuantity, float* afTime,
        int& riLastIndex, float& rfNormTime, int& ri0, int& ri1);

    Vector3f GetTranslate (float fNormTime, int i0, int i1);
    Matrix3f GetRotate (float fNormTime, int i0, int i1);
    float GetScale (float fNormTime, int i0, int i1);

    // Cached indices for the last found pair of keys used for interpolation.
    // For a sequence of times, this guarantees an O(1) lookup.
    int m_iTLastIndex;
    int m_iRLastIndex;
    int m_iSLastIndex;
};

WM4_REGISTER_STREAM(KeyframeController);
typedef Pointer<KeyframeController> KeyframeControllerPtr;

}

#endif
