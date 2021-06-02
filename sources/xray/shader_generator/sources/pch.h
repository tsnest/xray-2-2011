////////////////////////////////////////////////////////////////////////////
//	Created		: 18.09.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#include <string>
#include <set>
#include <map>
#include <vector>
#include <algorithm>

typedef char										string_path[260];
typedef std::vector< std::string >					definitions_type;
typedef std::set< std::string >						unique_definitions_type;
typedef std::set< std::string >						include_ids_type;

struct file_info_type {
	unique_definitions_type							internal_definitions;
	unique_definitions_type							external_definitions;
	definitions_type								personal_unique_definitions;
	definitions_type								unique_definitions;
	include_ids_type								include_ids;
	std::string										body;
}; // file_info_type

typedef std::map< std::string, file_info_type >		total_info_type;

#include <direct.h>

#define	WIN32_LEAN_AND_MEAN
#define	VC_EXTRALEAN				// useful only for MFC projects (http://support.microsoft.com/default.aspx?scid=kb;en-us;166474)

#if 0
#define	NOGDICAPMASKS     			// CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define	NOVIRTUALKEYCODES 			// VK_*
#define	NOWINMESSAGES     			// WM_*, EM_*, LB_*, CB_*
#define	NOWINSTYLES       			// WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define	NOSYSMETRICS      			// SM_*
#define	NOMENUS           			// MF_*
#define	NOICONS           			// IDI_*
#define	NOKEYSTATES       			// MK_*
#define	NOSYSCOMMANDS     			// SC_*
#define	NORASTEROPS       			// Binary and Tertiary raster ops
#define	NOSHOWWINDOW      			// SW_*
#define	OEMRESOURCE       			// OEM Resource values
#define	NOATOM            			// Atom Manager routines
#define	NOCLIPBOARD       			// Clipboard routines
#define	NOCOLOR           			// Screen colors
#define	NOCTLMGR          			// Control and Dialog routines
#define	NODRAWTEXT        			// DrawText() and DT_*
#define	NOGDI             			// All GDI defines and routines
#define	NOKERNEL          			// All KERNEL defines and routines
#define	NOUSER            			// All USER defines and routines
#define	NONLS             			// All NLS defines and routines
#define	NOMB              			// MB_* and MessageBox()
#define	NOMEMMGR          			// GMEM_*, LMEM_*, GHND, LHND, associated routines
#define	NOMETAFILE        			// typedef METAFILEPICT
#define	NOMINMAX          			// Macros min(a,b) and max(a,b)
#define	NOMSG             			// typedef MSG and associated routines
#define	NOOPENFILE        			// OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define	NOSCROLL          			// SB_* and scrolling routines
#define	NOSERVICE         			// All Service Controller routines, SERVICE_ equates, etc.
#define	NOSOUND           			// Sound driver routines
#define	NOTEXTMETRIC      			// typedef TEXTMETRIC and associated routines
#define	NOWH              			// SetWindowsHook and WH_*
#define	NOWINOFFSETS      			// GWL_*, GCL_*, associated routines
#define	NOCOMM            			// COMM driver routines
#define	NOKANJI           			// Kanji support stuff.
#define	NOHELP            			// Help engine interface.
#define	NOPROFILER        			// Profiler interface.
#define	NODEFERWINDOWPOS  			// DeferWindowPos routines
#define	NOMCX             			// Modem Configuration Extensions
#endif // #if 0
#define	STRICT
#include <windows.h>

#undef min
#undef max

#define XRAY_BOOST_NEW		new
#define XRAY_BOOST_DELETE	delete

#endif // #ifndef PCH_H_INCLUDED