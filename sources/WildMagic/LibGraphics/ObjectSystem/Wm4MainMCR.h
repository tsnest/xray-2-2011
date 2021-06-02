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

#ifndef WM4MAINMCR_H
#define WM4MAINMCR_H

//----------------------------------------------------------------------------
#define WM4_DECLARE_INITIALIZE \
public: \
    static bool RegisterInitialize (); \
    static void Initialize (); \
private: \
    static bool ms_bInitializeRegistered
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_INITIALIZE(classname) \
bool classname::ms_bInitializeRegistered = false; \
bool classname::RegisterInitialize () \
{ \
    if (!ms_bInitializeRegistered) \
    { \
        Main::AddInitializer(classname::Initialize); \
        ms_bInitializeRegistered = true; \
    } \
    return ms_bInitializeRegistered; \
}
//----------------------------------------------------------------------------
#define WM4_REGISTER_INITIALIZE(classname) \
static bool gs_bInitializeRegistered_##classname = \
    classname::RegisterInitialize ()
//----------------------------------------------------------------------------
#define WM4_DECLARE_TERMINATE \
public: \
    static bool RegisterTerminate (); \
    static void Terminate (); \
private: \
    static bool ms_bTerminateRegistered
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_TERMINATE(classname) \
bool classname::ms_bTerminateRegistered = false; \
bool classname::RegisterTerminate () \
{ \
    if (!ms_bTerminateRegistered) \
    { \
        Main::AddTerminator(classname::Terminate); \
        ms_bTerminateRegistered = true; \
    } \
    return ms_bTerminateRegistered; \
}
//----------------------------------------------------------------------------
#define WM4_REGISTER_TERMINATE(classname) \
static bool gs_bTerminateRegistered_##classname = \
    classname::RegisterTerminate ()
//----------------------------------------------------------------------------

#endif
