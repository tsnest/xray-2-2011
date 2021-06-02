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

#ifndef WM4STENCILSTATE_H
#define WM4STENCILSTATE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4GlobalState.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM StencilState : public GlobalState
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;
    WM4_DECLARE_INITIALIZE;
    WM4_DECLARE_TERMINATE;

public:
    virtual StateType GetStateType () const { return STENCIL; }

    StencilState ();
    virtual ~StencilState ();

    enum CompareFunction
    {
        CF_NEVER,
        CF_LESS,
        CF_EQUAL,
        CF_LEQUAL,
        CF_GREATER,
        CF_NOTEQUAL,
        CF_GEQUAL,
        CF_ALWAYS,
        CF_QUANTITY
    };

    enum OperationType
    {
        OT_KEEP,
        OT_ZERO,
        OT_REPLACE,
        OT_INCREMENT,
        OT_DECREMENT,
        OT_INVERT,
        OT_QUANTITY
    };

    bool Enabled;             // default: false
    CompareFunction Compare;  // default: CF_NEVER
    unsigned int Reference;   // default: 0
    unsigned int Mask;        // default: ~0
    unsigned int WriteMask;   // default: ~0
    OperationType OnFail;     // default: OT_KEEP
    OperationType OnZFail;    // default: OT_KEEP
    OperationType OnZPass;    // default: OT_KEEP

private:
    static const char* ms_aacCompare[CF_QUANTITY];
    static const char* ms_aacOperation[OT_QUANTITY];
};

WM4_REGISTER_STREAM(StencilState);
WM4_REGISTER_INITIALIZE(StencilState);
WM4_REGISTER_TERMINATE(StencilState);
typedef Pointer<StencilState> StencilStatePtr;

}

#endif
