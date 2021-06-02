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

#ifndef WM4ALPHASTATE_H
#define WM4ALPHASTATE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4GlobalState.h"
#include "Wm4ColorRGBA.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM AlphaState : public GlobalState
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;
    WM4_DECLARE_INITIALIZE;
    WM4_DECLARE_TERMINATE;

public:
    virtual StateType GetStateType () const { return ALPHA; }

    AlphaState ();
    virtual ~AlphaState ();

    enum SrcBlendMode
    {
        SBF_ZERO,
        SBF_ONE,
        SBF_DST_COLOR,
        SBF_ONE_MINUS_DST_COLOR,
        SBF_SRC_ALPHA,
        SBF_ONE_MINUS_SRC_ALPHA,
        SBF_DST_ALPHA,
        SBF_ONE_MINUS_DST_ALPHA,
        SBF_SRC_ALPHA_SATURATE,
        SBF_CONSTANT_COLOR,
        SBF_ONE_MINUS_CONSTANT_COLOR,
        SBF_CONSTANT_ALPHA,
        SBF_ONE_MINUS_CONSTANT_ALPHA,
        SBF_QUANTITY
    };

    enum DstBlendMode
    {
        DBF_ZERO,
        DBF_ONE,
        DBF_SRC_COLOR,
        DBF_ONE_MINUS_SRC_COLOR,
        DBF_SRC_ALPHA,
        DBF_ONE_MINUS_SRC_ALPHA,
        DBF_DST_ALPHA,
        DBF_ONE_MINUS_DST_ALPHA,
        DBF_CONSTANT_COLOR,
        DBF_ONE_MINUS_CONSTANT_COLOR,
        DBF_CONSTANT_ALPHA,
        DBF_ONE_MINUS_CONSTANT_ALPHA,
        DBF_QUANTITY
    };

    enum TestMode
    {
        TF_NEVER,
        TF_LESS,
        TF_EQUAL,
        TF_LEQUAL,
        TF_GREATER,
        TF_NOTEQUAL,
        TF_GEQUAL,
        TF_ALWAYS,
        TF_QUANTITY
    };

    bool BlendEnabled;      // default: false
    SrcBlendMode SrcBlend;  // default: SBF_SRC_ALPHA
    DstBlendMode DstBlend;  // default: DBF_ONE_MINUS_SRC_ALPHA
    bool TestEnabled;       // default: false;
    TestMode Test;          // default: TF_ALWAYS
    float Reference;        // default: 0, always in [0,1]

    // TO DO.  New member, needs to be streamed.
    ColorRGBA ConstantColor;   // default: (0,0,0,0)

private:
    static const char* ms_aacSrcBlend[SBF_QUANTITY];
    static const char* ms_aacDstBlend[DBF_QUANTITY];
    static const char* ms_aacTestMode[TF_QUANTITY];
};

WM4_REGISTER_STREAM(AlphaState);
WM4_REGISTER_INITIALIZE(AlphaState);
WM4_REGISTER_TERMINATE(AlphaState);
typedef Pointer<AlphaState> AlphaStatePtr;

}

#endif
