////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OS_PREINCLUDE_H_INCLUDED
#define XRAY_OS_PREINCLUDE_H_INCLUDED

#include <xray/macro_extensions.h>
#include <xray/os_extensions.h>

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
#	include <crtdefs.h>	// for ptrdiff_t

	enum {
		INVALID_HANDLE_VALUE_fake_value = (ptrdiff_t)((unsigned int*)INVALID_HANDLE_VALUE),
	};
	typedef			HMODULE						HMODULE_fake_type;

#	undef			INVALID_HANDLE_VALUE
#	undef			HMODULE
#	undef			HANDLE
#	undef			DLL_PROCESS_DETACH
#	undef			DLL_PROCESS_ATTACH
#	undef			APIENTRY
#	undef			BOOL
#	undef			HWND
#endif // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360

#if XRAY_PLATFORM_WINDOWS
#	define			WIN32_LEAN_AND_MEAN
#	define			VC_EXTRALEAN				// useful only for MFC projects (http://support.microsoft.com/default.aspx?scid=kb;en-us;166474)
#	define			NOGDICAPMASKS     			// CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#	define			NOVIRTUALKEYCODES 			// VK_*
#	define			NOWINMESSAGES     			// WM_*, EM_*, LB_*, CB_*
#	define			NOWINSTYLES       			// WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#	define			NOSYSMETRICS      			// SM_*
#	define			NOMENUS           			// MF_*
#	define			NOICONS           			// IDI_*
#	define			NOKEYSTATES       			// MK_*
#	define			NOSYSCOMMANDS     			// SC_*
#	define			NORASTEROPS       			// Binary and Tertiary raster ops
#	define			NOSHOWWINDOW      			// SW_*
#	define			OEMRESOURCE       			// OEM Resource values
#	define			NOATOM            			// Atom Manager routines
#	define			NOCLIPBOARD       			// Clipboard routines
#	define			NOCOLOR           			// Screen colors
#	define			NOCTLMGR          			// Control and Dialog routines
#	define			NODRAWTEXT        			// DrawText() and DT_*
#	define			NOGDI             			// All GDI defines and routines
#	define			NOKERNEL          			// All KERNEL defines and routines
#	define			NOUSER            			// All USER defines and routines
#	define			NONLS             			// All NLS defines and routines
#	define			NOMB              			// MB_* and MessageBox()
#	define			NOMEMMGR          			// GMEM_*, LMEM_*, GHND, LHND, associated routines
#	define			NOMETAFILE        			// typedef METAFILEPICT
#	define			NOMINMAX          			// Macros min(a,b) and max(a,b)
#	define			NOMSG             			// typedef MSG and associated routines
#	define			NOOPENFILE        			// OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#	define			NOSCROLL          			// SB_* and scrolling routines
#	define			NOSERVICE         			// All Service Controller routines, SERVICE_ equates, etc.
#	define			NOSOUND           			// Sound driver routines
#	define			NOTEXTMETRIC      			// typedef TEXTMETRIC and associated routines
#	define			NOWH              			// SetWindowsHook and WH_*
#	define			NOWINOFFSETS      			// GWL_*, GCL_*, associated routines
#	define			NOCOMM            			// COMM driver routines
#	define			NOKANJI           			// Kanji support stuff.
#	define			NOHELP            			// Help engine interface.
#	define			NOPROFILER        			// Profiler interface.
#	define			NODEFERWINDOWPOS  			// DeferWindowPos routines
#	define			NOMCX             			// Modem Configuration Extensions
#	define			STRICT
//#	ifndef _WIN32_WINNT
//#		define		_WIN32_WINNT				0x0500
//#	endif // ifndef _WIN32_WINNT
#elif XRAY_PLATFORM_XBOX_360					// #if XRAY_PLATFORM_WINDOWS
#	define			NOD3D
#	define			NONET
#	define			NOMINMAX          			// Macros min(a,b) and max(a,b)
#	define			STRICT
#elif XRAY_PLATFORM_PS3							// #ifdef XRAY_PLATFORM_PS3
#else // #elif XRAY_PLATFORM_PS3
#	error please define your platform
#endif // #elif XRAY_PLATFORM_PS3

#endif // #ifndef XRAY_OS_PREINCLUDE_H_INCLUDED