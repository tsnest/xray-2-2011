////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_EXTENSIONS_H_INCLUDED
#define XRAY_DEBUG_EXTENSIONS_H_INCLUDED

#include <xray/debug/debug.h>
#include <xray/debug/macros.h>
#include <xray/debug/signalling_bool.h>
#include <xray/debug/check_invariants.h>

// Disable specific Code Analysis warning:
// "_alloca indicates failure by raising a stack overflow exception. Consider using _malloca instead"
#pragma warning( disable : 6255 )

#endif // #ifndef XRAY_DEBUG_EXTENSIONS_H_INCLUDED