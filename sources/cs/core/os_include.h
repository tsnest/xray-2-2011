////////////////////////////////////////////////////////////////////////////
//	Module 		: os_inclue.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : operating system include
////////////////////////////////////////////////////////////////////////////

#ifndef OS_INCLUDE_H_INCLUDED
#define OS_INCLUDE_H_INCLUDED

#undef			INVALID_HANDLE_VALUE
#undef			HMODULE

#undef			HANDLE
#undef			DLL_PROCESS_DETACH
#undef			DLL_PROCESS_ATTACH
#undef			APIENTRY
#undef			BOOL

#define			VC_EXTRALEAN				
#define			WIN32_LEAN_AND_MEAN			
#define			STRICT						
#define			IDIRECTPLAY2_OR_GREATER		
#define			_WIN32_WINNT			0x0500 

#define			NOGDICAPMASKS
//#define			NOSYSMETRICS
#define			NOMENUS
#define			NOICONS
#define			NOKEYSTATES
//#define			NODRAWTEXT
#define			NOMEMMGR
#define			NOMETAFILE
#define			NOSERVICE
#define			NOCOMM
#define			NOHELP
#define			NOPROFILER
#define			NOMCX
#define			NOMINMAX

//#define			WINVER					0x0500 

#if CS_PLATFORM_WINDOWS
#	include		<windows.h>
#	define		IS_DEBUGGER_PRESENT			IsDebuggerPresent
#	define		OUTPUT_DEBUG_STRING			OutputDebugString
#elif CS_PLATFORM_XBOX_360					// #if CS_PLATFORM_WINDOWS
#	include		<xtl.h>
#	include		<xbdm.h>
#	ifdef DEBUG
#		pragma comment( lib, "xbdm.lib" )
#		define	IS_DEBUGGER_PRESENT			DmIsDebuggerPresent
#		define	OUTPUT_DEBUG_STRING			OutputDebugString
#	else // #ifdef DEBUG
		namespace cs {
			inline bool false_predicate( ) { return false; }
		} // namespace cs
#		define	IS_DEBUGGER_PRESENT			cs::false_predicate
#		define	OUTPUT_DEBUG_STRING(string)
#	endif // #ifdef DEBUG
#elif CS_PLATFORM_PS3						// #elif CS_PLATFORM_XBOX_360
	namespace xray {
		inline bool false_predicate( ) { return false; }
		inline bool true_predicate( ) { return true; }
	}
#	ifndef NDEBUG
#		define	IS_DEBUGGER_PRESENT			xray::true_predicate
#	else // #ifndef NDEBUG
#		define	IS_DEBUGGER_PRESENT			xray::false_predicate
#	endif // #ifndef NDEBUG

#	define OUTPUT_DEBUG_STRING( a )			printf("%s",(a));
#else // #elif CS_PLATFORM_PS3
#	error		define IS_DEBUGGER_PRESENT macro here
#	error		define OUTPUT_DEBUG_STRING macro here
#endif // #elif CS_PLATFORM_PS3

#endif // #ifndef OS_INCLUDE_H_INCLUDED