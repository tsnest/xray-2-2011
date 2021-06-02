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

#ifndef WM4SOFTSHADERLIBRARYMCR_H
#define WM4SOFTSHADERLIBRARYMCR_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4Main.h"

#ifdef WM4_HAS_SOFT_SHADER

//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_VPROGRAM(name) \
static bool ms_bVInitializeRegistered_##name = false; \
static void v_##name##Initialize () \
{ \
    SoftRenderer::RegisterVProgram(#name,&v_##name); \
} \
\
bool v_##name##Register () \
{ \
    if (!ms_bVInitializeRegistered_##name) \
    { \
        Main::AddInitializer(&v_##name##Initialize); \
        ms_bVInitializeRegistered_##name = true; \
    } \
    return ms_bVInitializeRegistered_##name; \
}
//----------------------------------------------------------------------------
#define WM4_REGISTER_VPROGRAM(name) \
extern bool v_##name##Register (); \
static bool gs_bVInitializeRegistered_##name = v_##name##Register ()
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_PPROGRAM(name) \
static bool ms_bPInitializeRegistered_##name = false; \
static void p_##name##Initialize () \
{ \
    SoftRenderer::RegisterPProgram(#name,&p_##name); \
} \
\
bool p_##name##Register () \
{ \
    if (!ms_bPInitializeRegistered_##name) \
    { \
        Main::AddInitializer(&p_##name##Initialize); \
        ms_bPInitializeRegistered_##name = true; \
    } \
    return ms_bPInitializeRegistered_##name; \
}
//----------------------------------------------------------------------------
#define WM4_REGISTER_PPROGRAM(name) \
extern bool p_##name##Register (); \
static bool gs_bPInitializeRegistered_##name = p_##name##Register ()
//----------------------------------------------------------------------------

#else

//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_VPROGRAM(name) \
static bool ms_bVInitializeRegistered_##name = false; \
static void v_##name##Initialize () \
{ \
    SoftRenderer::RegisterVProgram(#name,&v_##name); \
} \
\
WM4_RENDERER_ITEM bool v_##name##Register () \
{ \
    if (!ms_bVInitializeRegistered_##name) \
    { \
        Main::AddInitializer(&v_##name##Initialize); \
        ms_bVInitializeRegistered_##name = true; \
    } \
    return ms_bVInitializeRegistered_##name; \
}
//----------------------------------------------------------------------------
#define WM4_REGISTER_VPROGRAM(name) \
extern WM4_RENDERER_ITEM bool v_##name##Register (); \
static bool gs_bVInitializeRegistered_##name = v_##name##Register ()
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_PPROGRAM(name) \
static bool ms_bPInitializeRegistered_##name = false; \
static void p_##name##Initialize () \
{ \
    SoftRenderer::RegisterPProgram(#name,&p_##name); \
} \
\
WM4_RENDERER_ITEM bool p_##name##Register () \
{ \
    if (!ms_bPInitializeRegistered_##name) \
    { \
        Main::AddInitializer(&p_##name##Initialize); \
        ms_bPInitializeRegistered_##name = true; \
    } \
    return ms_bPInitializeRegistered_##name; \
}
//----------------------------------------------------------------------------
#define WM4_REGISTER_PPROGRAM(name) \
extern WM4_RENDERER_ITEM bool p_##name##Register (); \
static bool gs_bPInitializeRegistered_##name = p_##name##Register ()
//----------------------------------------------------------------------------

#endif

#endif
