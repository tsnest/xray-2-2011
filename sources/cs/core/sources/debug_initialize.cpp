////////////////////////////////////////////////////////////////////////////
//	Module 		: debug_initialize.cpp
//	Created 	: 18.03.2007
//  Modified 	: 18.03.2007
//	Author		: Dmitriy Iassenev
//	Description : debug initialization
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/debug.h>

#if !CS_PLATFORM_PS3
#	include <cs/core/os_include.h>	// for BugTrap.h and dbghelp.h
#	include <new.h>					// for _set_new_mode
#	include <signal.h>				// for signals
#endif // #if !CS_PLATFORM_PS3

#if CS_PLATFORM_WINDOWS
#	define CS_USE_BUG_TRAP				1
#	include <BugTrap/BugTrap.h>		// for BugTrap functionality
#else // #if CS_PLATFORM_WINDOWS
#	define CS_USE_BUG_TRAP				0
#endif // #if CS_PLATFORM_WINDOWS

#if CS_USE_BUG_TRAP
#	include <dbghelp.h>				// MiniDump flags
#endif // #if CS_USE_BUG_TRAP

//#define BUGTRAP_LINK_STATIC		1
#define BUGTRAP_LINK_STATIC			0

#if CS_USE_BUG_TRAP
#	if BUGTRAP_LINK_STATIC
#		if 0// CS_DEBUG_LIBRARIS
#			pragma comment(lib,"BugTrapD.lib")
#		else // #if CS_DEBUG_LIBRARIES
#			pragma comment(lib,"BugTrap.lib")
#		endif // #if CS_DEBUG_LIBRARIES
#	else // #if BUGTRAP_LINK_STATIC
		static	HMODULE	s_bugtrap_handle		= 0;
#if defined( WIN64 )
		static	pcstr	s_bugtrap_native_id		= "BugTrap-x64.dll";
		static	pcstr	s_bugtrap_dotnet_id		= "BugTrapN-x64.dll";
#elif defined( WIN32 )
		static	pcstr	s_bugtrap_native_id		= "BugTrap.dll";
		static	pcstr	s_bugtrap_dotnet_id		= "BugTrapN.dll";
#else
#	error please define your platform
#endif // #if defined( WIN32 )
		static	bool	s_use_dot_net			= false;
		static	bool	s_initialized			= false;
#	endif // #if BUGTRAP_LINK_STATIC

typedef void CDECL BT_CallCppFilter_type		();
static BT_CallCppFilter_type*					s_BT_CallCppFilter = 0;

typedef void APIENTRY BT_SetUserMessage_type	(LPCTSTR pszUserMessage);
static BT_SetUserMessage_type*					s_BT_SetUserMessage = 0;

typedef LPTOP_LEVEL_EXCEPTION_FILTER APIENTRY	BT_InstallSehFilter_type();
static BT_InstallSehFilter_type*					s_BT_InstallSehFilter = 0;

typedef void APIENTRY BT_SetDialogMessage_type	(BUGTRAP_DIALOGMESSAGE eDialogMessage, LPCTSTR pszMessage);
static BT_SetDialogMessage_type*					s_BT_SetDialogMessage = 0;

typedef void APIENTRY BT_SetAppName_type		(LPCTSTR pszAppName);
static BT_SetAppName_type*						s_BT_SetAppName = 0;

typedef void APIENTRY BT_SetReportFormat_type	(BUGTRAP_REPORTFORMAT eReportFormat);
static BT_SetReportFormat_type*					s_BT_SetReportFormat = 0;

typedef void APIENTRY BT_SetFlags_type			(DWORD dwFlags);
static BT_SetFlags_type*							s_BT_SetFlags = 0;

typedef void APIENTRY BT_SetDumpType_type		(DWORD dwDumpType);
static BT_SetDumpType_type*						s_BT_SetDumpType = 0;

typedef void APIENTRY BT_SetSupportEMail_type	(LPCTSTR pszSupportEMail);
static BT_SetSupportEMail_type*					s_BT_SetSupportEMail = 0;
#endif // #if CS_USE_BUG_TRAP

namespace debug {

void on_thread_spawn					()
{
#if CS_USE_BUG_TRAP
	set_terminate					(s_BT_CallCppFilter);
#endif // #if CS_USE_BUG_TRAP
}

void on_error							(pcstr message)
{
#if CS_USE_BUG_TRAP
	if (!s_use_dot_net)
		s_BT_SetUserMessage			(message);
#endif // #if CS_USE_BUG_TRAP
}

#if CS_USE_BUG_TRAP
static void setup_bug_trap				()
{
	s_BT_InstallSehFilter			();

	if (!s_use_dot_net) {
		s_BT_SetDialogMessage		(
			BTDM_INTRO2,
			"This is CS AI Library crash reporting client."
			"To help the development process,"
			"please Submit Bug or save report and email it manually (button More...)."
			"\r\nMany thanks in advance and sorry for the inconvenience."
		);
		s_BT_SetAppName				("CS AI Library");
		s_BT_SetSupportEMail		("iassenev@gmail.com");
	}

	s_BT_SetReportFormat			(BTRF_TEXT);
	s_BT_SetFlags					(
		BTF_DETAILEDMODE |
		//BTF_EDIETMAIL |
		BTF_ATTACHREPORT
		//| BTF_LISTPROCESSES | BTF_SHOWADVANCEDUI | BTF_SCREENCAPTURE
	);
	s_BT_SetDumpType				(
		MiniDumpWithDataSegs |
//		MiniDumpWithFullMemory |
//		MiniDumpWithHandleData |
//		MiniDumpFilterMemory |
//		MiniDumpScanMemory |
//		MiniDumpWithUnloadedModules |
		MiniDumpWithIndirectlyReferencedMemory |
//		MiniDumpFilterModulePaths |
//		MiniDumpWithProcessThreadData |
//		MiniDumpWithPrivateReadWriteMemory |
//		MiniDumpWithoutOptionalData |
//		MiniDumpWithFullMemoryInfo |
//		MiniDumpWithThreadInfo |
//		MiniDumpWithCodeSegs |
		0
	);
}
#endif // #if CS_USE_BUG_TRAP

} // namespace debug

#if !CS_PLATFORM_PS3
static void handler_base				(pcstr reason_string)
{
	debug::show_error				("error handler is invoked!",reason_string,__FILE__, __FUNCTION__, __LINE__);\
	DEBUG_BREAK						();
}

static void invalid_parameter_handler	(wchar_t const* , wchar_t const* , wchar_t const* , unsigned int, uintptr_t)
{
	handler_base					("invalid parameter passed");
}

static int out_of_memory_handler		(size_t size)
{
	CS_UNREFERENCED_PARAMETER		( size );
	handler_base					("out of memory");
	return							0;
}

static void pure_call_handler			()
{
	handler_base					("pure virtual function call");
}

static void abort_handler				(int signal)
{
	CS_UNREFERENCED_PARAMETER		( signal );
	handler_base					("application is aborting");
}

static void floating_point_handler		(int signal)
{
	CS_UNREFERENCED_PARAMETER		( signal );
	handler_base					("floating point error");
}

static void illegal_instruction_handler	(int signal)
{
	CS_UNREFERENCED_PARAMETER		( signal );
	handler_base					("illegal instruction");
}

//static void storage_access_handler		(int signal)
//{
//	CS_UNREFERENCED_PARAMETER		( signal );
//	handler_base					("illegal storage access");
//}

static void termination_handler			(int signal)
{
	CS_UNREFERENCED_PARAMETER		( signal );
	handler_base					("termination with exit code 3");
}
#endif // #if !CS_PLATFORM_PS3

#if CS_USE_BUG_TRAP
static void init_bugtrap				()
{
#if BUGTRAP_LINK_STATIC
	s_BT_CallCppFilter				= &BT_CallCppFilter		;
	s_BT_SetUserMessage				= &BT_SetUserMessage	;
	s_BT_InstallSehFilter			= &BT_InstallSehFilter	;
	s_BT_SetDialogMessage			= &BT_SetDialogMessage	;
	s_BT_SetAppName					= &BT_SetAppName		;
	s_BT_SetReportFormat			= &BT_SetReportFormat	;
	s_BT_SetFlags					= &BT_SetFlags			;
	s_BT_SetDumpType				= &BT_SetDumpType		;
	s_BT_SetSupportEMail			= &BT_SetSupportEMail	;
#else // #if BUGTRAP_LINK_STATIC
	ASSERT							(!s_bugtrap_handle);

	s_bugtrap_handle				= LoadLibrary(s_use_dot_net ? s_bugtrap_dotnet_id : s_bugtrap_native_id);
	if (!s_bugtrap_handle) {
		OUTPUT_DEBUG_STRING			("cannot load bugtrap library");
		abort						();
	}
	s_BT_CallCppFilter				= (BT_CallCppFilter_type*)		GetProcAddress(s_bugtrap_handle, "BT_CallCppFilter");
	s_BT_SetUserMessage				= (BT_SetUserMessage_type*)		GetProcAddress(s_bugtrap_handle, "BT_SetUserMessage");
	s_BT_InstallSehFilter			= (BT_InstallSehFilter_type*)	GetProcAddress(s_bugtrap_handle, "BT_InstallSehFilter");
	s_BT_SetDialogMessage			= (BT_SetDialogMessage_type*)	GetProcAddress(s_bugtrap_handle, "BT_SetDialogMessage");
	s_BT_SetAppName					= (BT_SetAppName_type*)			GetProcAddress(s_bugtrap_handle, "BT_SetAppName");
	s_BT_SetReportFormat			= (BT_SetReportFormat_type*)	GetProcAddress(s_bugtrap_handle, "BT_SetReportFormat");
	s_BT_SetFlags					= (BT_SetFlags_type*)			GetProcAddress(s_bugtrap_handle, "BT_SetFlags");
	s_BT_SetDumpType				= (BT_SetDumpType_type*)		GetProcAddress(s_bugtrap_handle, "BT_SetDumpType");
	s_BT_SetSupportEMail			= (BT_SetSupportEMail_type*)	GetProcAddress(s_bugtrap_handle, "BT_SetSupportEMail");
	s_initialized					= true;
#endif // #if BUGTRAP_LINK_STATIC
}
#endif // #if CS_USE_BUG_TRAP

void debug::initialize					()
{
#if CS_USE_BUG_TRAP
	init_bugtrap					();
	debug::on_thread_spawn			();
	setup_bug_trap					();
#endif // #if CS_USE_BUG_TRAP

#if !CS_PLATFORM_PS3
	_set_abort_behavior				(0,_WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	signal							(SIGABRT,		abort_handler);
	signal							(SIGABRT_COMPAT,abort_handler);
	signal							(SIGFPE,		floating_point_handler);
	signal							(SIGILL,		illegal_instruction_handler);
	signal							(SIGINT,		0);
//	signal							(SIGSEGV,		storage_access_handler);
	signal							(SIGTERM,		termination_handler);

	_set_invalid_parameter_handler	(&invalid_parameter_handler);

	_set_new_mode					(1);
	_set_new_handler				(&out_of_memory_handler);

	_set_purecall_handler			(&pure_call_handler);
#endif // #if !CS_PLATFORM_PS3
}

void debug::uninitialize				()
{
#if CS_USE_BUG_TRAP
	s_BT_CallCppFilter				= 0;
	s_BT_SetUserMessage				= 0;
	s_BT_InstallSehFilter			= 0;
	s_BT_SetDialogMessage			= 0;
	s_BT_SetAppName					= 0;
	s_BT_SetReportFormat			= 0;
	s_BT_SetFlags					= 0;
	s_BT_SetDumpType				= 0;
	s_BT_SetSupportEMail			= 0;

#	if !BUGTRAP_LINK_STATIC
		FreeLibrary					(s_bugtrap_handle);
		s_bugtrap_handle			= 0;
#	endif // BUGTRAP_LINK_STATIC
#endif // #if CS_USE_BUG_TRAP
}

void debug::use_dot_net					(bool const value)
{
	CS_UNREFERENCED_PARAMETER		( value );
#if CS_USE_BUG_TRAP
#	if BUGTRAP_LINK_STATIC
		NODEFAULT;
#	else // BUGTRAP_LINK_STATIC
		R_ASSERT					(!s_initialized, "you cannot change bug trap library after debug engine is initialized");
		s_use_dot_net				= value;
#	endif // BUGTRAP_LINK_STATIC
#endif // #if CS_USE_BUG_TRAP
}
