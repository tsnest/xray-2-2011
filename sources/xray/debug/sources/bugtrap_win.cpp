////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/debug/extensions.h>
#include <xray/debug/pointer_cast.h>
#include <malloc.h>
#include <Windows.h>
#include <dbghelp.h>								// for MiniDump flags
#include <BugTrap/BugTrap.h>						// for BugTrap functionality
#include "utils.h"
#include "bugtrap.h"

using xray::debug::bugtrap_usage;
using xray::debug::error_mode;

static HMODULE			s_bugtrap_handle			= 0;
#if XRAY_PLATFORM_WINDOWS_64
	static pcstr		s_bugtrap_native_id			= "BugTrap-x64.dll";
	static pcstr		s_bugtrap_dotnet_id			= "BugTrapN-x64.dll";
#else // #if XRAY_PLATFORM_WINDOWS_64
	static pcstr		s_bugtrap_native_id			= "BugTrap.dll";
	static pcstr		s_bugtrap_dotnet_id			= "BugTrapN.dll";
#endif // #if XRAY_PLATFORM_WINDOWS_64
static error_mode		s_error_mode				= xray::debug::error_mode_verbose;
static bugtrap_usage	s_bugtrap_usage				= xray::debug::native_bugtrap;
static bool				s_initialized				= false;

typedef void CDECL BT_CallCppFilter_type			();
static BT_CallCppFilter_type						*s_BT_CallCppFilter = 0;

typedef void APIENTRY BT_SetUserMessage_type		(LPCTSTR pszUserMessage);
static BT_SetUserMessage_type						*s_BT_SetUserMessage = 0;

typedef LPTOP_LEVEL_EXCEPTION_FILTER APIENTRY		BT_InstallSehFilter_type();
static BT_InstallSehFilter_type						*s_BT_InstallSehFilter = 0;

typedef void APIENTRY BT_SetDialogMessage_type		(BUGTRAP_DIALOGMESSAGE eDialogMessage, LPCTSTR pszMessage);
static BT_SetDialogMessage_type						*s_BT_SetDialogMessage = 0;

typedef void APIENTRY BT_SetAppName_type			(LPCTSTR pszAppName);
static BT_SetAppName_type							*s_BT_SetAppName = 0;

typedef void APIENTRY BT_SetReportFormat_type		(BUGTRAP_REPORTFORMAT eReportFormat);
static BT_SetReportFormat_type						*s_BT_SetReportFormat = 0;

typedef void APIENTRY BT_SetFlags_type				(DWORD dwFlags);
static BT_SetFlags_type								*s_BT_SetFlags = 0;

typedef void APIENTRY BT_SetDumpType_type			(DWORD dwDumpType);
static BT_SetDumpType_type							*s_BT_SetDumpType = 0;

typedef void APIENTRY BT_SetSupportEMail_type		(LPCTSTR pszSupportEMail);
static BT_SetSupportEMail_type						*s_BT_SetSupportEMail = 0;

typedef void APIENTRY BT_SetActivityType_type		(BUGTRAP_ACTIVITY eActivityType);
static BT_SetActivityType_type						*s_BT_SetActivityType = 0;

typedef void APIENTRY BT_AddLogFile_type			(LPCTSTR pszLogFile);
static BT_AddLogFile_type							*s_BT_AddLogFile = 0;

template < typename T >
static void load_function							( T*& result, HMODULE const module, pcstr const function_id )
{
	if ( s_bugtrap_usage == xray::debug::no_bugtrap ) {
		result						= 0;
		return;
	}

	result							= ( T* ) GetProcAddress ( module, function_id );
	if ( !result )
		s_bugtrap_usage				= xray::debug::no_bugtrap;
}

static void load_library							( )
{
	if ( s_bugtrap_handle )
		return;

	s_bugtrap_handle				= LoadLibrary( s_bugtrap_usage == xray::debug::native_bugtrap ? s_bugtrap_native_id : s_bugtrap_dotnet_id );
	if ( !s_bugtrap_handle ) {

		xray::debug::log_callback const log_callback	=	xray::debug::get_log_callback();
		if ( log_callback )
			log_callback			( "debug:bugtrap", true, false, "cannot load bugtrap library" );

		s_bugtrap_usage				= xray::debug::no_bugtrap;
		return;
	}

	load_function					( s_BT_CallCppFilter	, s_bugtrap_handle, "BT_CallCppFilter");
	load_function					( s_BT_SetUserMessage	, s_bugtrap_handle, "BT_SetUserMessage");
	load_function					( s_BT_InstallSehFilter	, s_bugtrap_handle, "BT_InstallSehFilter");
	load_function					( s_BT_SetDialogMessage	, s_bugtrap_handle, "BT_SetDialogMessage");
	load_function					( s_BT_SetAppName		, s_bugtrap_handle, "BT_SetAppName");
	load_function					( s_BT_SetReportFormat	, s_bugtrap_handle, "BT_SetReportFormat");
	load_function					( s_BT_SetFlags			, s_bugtrap_handle, "BT_SetFlags");
	load_function					( s_BT_SetDumpType		, s_bugtrap_handle, "BT_SetDumpType");
	load_function					( s_BT_SetSupportEMail	, s_bugtrap_handle, "BT_SetSupportEMail");
	load_function					( s_BT_SetActivityType	, s_bugtrap_handle, "BT_SetActivityType");
	load_function					( s_BT_AddLogFile		, s_bugtrap_handle, "BT_AddLogFile");
}

static pcstr convert_to_unicode_if_needed			( pcstr const message, WCHAR* const output, u32 const max_count )
{
	if ( s_bugtrap_usage == xray::debug::native_bugtrap )
		return						message;

	output[0]						= 0;
	MultiByteToWideChar				(
		CP_ACP,
		MB_PRECOMPOSED,
		message,
		(u32)strlen(message) + 1,
		&output[0],
		max_count
	);

	return							xray::pointer_cast<pstr>(&output[0]);
}

template <int count>
static pcstr convert_to_unicode_if_needed			( pcstr const message, WCHAR (&output)[count] )
{
	return							convert_to_unicode_if_needed( message, &output[0], count );
}

static void install									( )
{
	s_BT_InstallSehFilter			( );
	xray::debug::bugtrap::setup_unhandled_exception_handler( );

	char const message[]			=
		"This is " XRAY_ENGINE_ID " crash reporting client. "
		"To help the development process, "
		"please Submit Bug or save report and email it manually (button More...)."
		"\r\nMany thanks in advance and sorry for the inconvenience.";
	WCHAR unicode_message[sizeof(message)/sizeof(message[0])];

	s_BT_SetDialogMessage			( BTDM_INTRO2, convert_to_unicode_if_needed(message,unicode_message) );

	pcstr bugtrap_application_name	=	xray::debug::debug_engine()->bugtrap_application_name();
	s_BT_SetAppName					( convert_to_unicode_if_needed(bugtrap_application_name, unicode_message) );
	s_BT_SetSupportEMail			( convert_to_unicode_if_needed("iassenev@gsc-game.kiev.ua",unicode_message) );

	s_BT_SetReportFormat			( BTRF_TEXT );
	s_BT_SetFlags					( BTF_DETAILEDMODE | /**BTF_EDIETMAIL | /**/BTF_ATTACHREPORT /**| BTF_LISTPROCESSES /**| BTF_SHOWADVANCEDUI /**| BTF_SCREENCAPTURE/**/ );
	s_BT_SetDumpType				( MiniDumpNoDump );
}

bool xray::debug::bugtrap::initialized		( )
{
	return							( s_initialized );
}

void xray::debug::bugtrap::change_usage		( error_mode error_mode, bugtrap_usage bugtrap_usage )
{
	R_ASSERT						( !s_initialized, "you are trying to initialize bugtrap twice" );

	s_bugtrap_usage					= bugtrap_usage;
	s_error_mode					= error_mode;
}

void xray::debug::bugtrap::initialize		( )
{
	R_ASSERT						( !s_initialized, "you are trying to initialize bugtrap twice" );

	if ( s_bugtrap_usage != no_bugtrap ) {
		load_library				( );

		if ( s_bugtrap_usage != no_bugtrap ) {
			install					( );
			s_BT_SetActivityType	( s_error_mode == error_mode_silent ? BTA_SAVEREPORT : BTA_SHOWUI );
		}
	}

	s_initialized					= true;
}

void xray::debug::bugtrap::finalize			( )
{
	if ( s_bugtrap_usage == no_bugtrap )
		return;

	if ( !s_initialized ) {
		xray::debug::log_callback const log_callback	=	xray::debug::get_log_callback();
		if ( log_callback )
			log_callback				( "debug::bugtrap", true, false, "you cannot finalize bugtrap library usage until you initialize it" );
		return;
	}

	s_BT_CallCppFilter				= 0;
	s_BT_SetUserMessage				= 0;
	s_BT_InstallSehFilter			= 0;
	s_BT_SetDialogMessage			= 0;
	s_BT_SetAppName					= 0;
	s_BT_SetReportFormat			= 0;
	s_BT_SetFlags					= 0;
	s_BT_SetDumpType				= 0;
	s_BT_SetSupportEMail			= 0;
	
	FreeLibrary						(s_bugtrap_handle);

	s_initialized					= false;
}

bool xray::debug::bugtrap::on_thread_spawn	( )
{
	return							( true );
}

void xray::debug::bugtrap::add_file			( pcstr full_path_file_name )
{
	if ( s_bugtrap_usage == no_bugtrap )
		return;

	WCHAR unicode_path[260];
	s_BT_AddLogFile					( convert_to_unicode_if_needed(full_path_file_name,unicode_path) );
}

void set_show_dialog_for_unhandled_exceptions	( bool show );

void xray::debug::on_error					( pcstr message )
{
// seems this check is obsolete, but for a case its left commented
// 	if ( !debug_engine() || !debug_engine()->core_initialized( ) )
// 		xray::debug::terminate	( "" );

	if ( !bugtrap::initialized( ) )
		bugtrap::initialize			( );

	if ( s_bugtrap_usage == native_bugtrap ) {
		u32 const count				= (u32)strlen(message) + 1;
		WCHAR* unicode_message		= (WCHAR*)_alloca( count*sizeof(WCHAR) );
		s_BT_SetUserMessage			( convert_to_unicode_if_needed(message,unicode_message,count) );
	}

	set_show_dialog_for_unhandled_exceptions	( false );
}
