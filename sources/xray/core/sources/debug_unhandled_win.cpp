////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_platform.h"
#include "debug_internal.h"
#include "debug_bugtrap.h"
#include "logging.h"
#include "core_entry_point.h"
#include <xray/fs_utils.h>
#include <xray/fs_path.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMB
#include <xray/os_include.h>

#include <dbghelp.h>

static void save_minidump						( pcstr const output_file_name, _EXCEPTION_POINTERS* const exception_information )
{
	static const pcstr			library_id = "dbghelp.dll";

	HMODULE library_handle		= 0;

	string_path					file_name;
	xray::strings::join			( file_name, xray::core::current_directory(), library_id );
	library_handle				= LoadLibrary ( file_name );

	if ( !library_handle ) {
		library_handle			= LoadLibrary ( library_id );
		if ( !library_handle )
			return;
	}

	typedef BOOL (WINAPI *MiniDumpWriteDumpType) (
		HANDLE hProcess,
		DWORD dwPid,
		HANDLE hFile,
		MINIDUMP_TYPE DumpType,
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
		CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

	MiniDumpWriteDumpType const	MiniDumpWriteDump = ( MiniDumpWriteDumpType ) GetProcAddress( library_handle, "MiniDumpWriteDump" );
	if ( !MiniDumpWriteDump )
		return;

	xray::fs::make_dir_r		( output_file_name, false );
	// create the file
	HANDLE minidump_handle		= CreateFile ( output_file_name, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( minidump_handle == INVALID_HANDLE_VALUE )
		return;

	_MINIDUMP_EXCEPTION_INFORMATION info;

	info.ThreadId				= ::GetCurrentThreadId();
	info.ExceptionPointers		= exception_information;
	info.ClientPointers			= 0;

	MiniDumpWriteDump			(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		minidump_handle,
		MINIDUMP_TYPE (
			MiniDumpWithDataSegs |
//			MiniDumpWithFullMemory |
//			MiniDumpWithHandleData |
//			MiniDumpFilterMemory |
//			MiniDumpScanMemory |
//			MiniDumpWithUnloadedModules |
			MiniDumpWithIndirectlyReferencedMemory |
//			MiniDumpFilterModulePaths |
//			MiniDumpWithProcessThreadData |
//			MiniDumpWithPrivateReadWriteMemory |
//			MiniDumpWithoutOptionalData |
//			MiniDumpWithFullMemoryInfo |
//			MiniDumpWithThreadInfo |
//			MiniDumpWithCodeSegs |
			0
		),
		&info,
		0,
		0
	);

	CloseHandle					( minidump_handle );
}

void xray::core::debug::platform::format_message	( )
{
    DWORD const error_code		= GetLastError( ); 

	if ( !error_code )
		return;

    pstr						message = 0;
    FormatMessage				(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        0,
        error_code,
        MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        (pstr)&message,
        0,
		0
	);

	LOGI_ERROR					( "debug", "%d: %s", error_code, message );
    LocalFree					( message );
}

void xray::core::debug::platform::save_minidump		( SYSTEMTIME const& date_time, _EXCEPTION_POINTERS* exception_information )
{
	string_path					temp;
	detail::generate_file_name	( temp, date_time, application_name(), "error_report", ".dmp");

	string_path					minidump_file_name;
	strings::copy				( minidump_file_name, temp );
	save_minidump				( fs::convert_to_portable_in_place(minidump_file_name), exception_information );
	bugtrap::add_file			( minidump_file_name );
}

void xray::core::debug::platform::prologue_dump_call_stack	( _EXCEPTION_POINTERS* exception_information )
{
	__try {
		if ( !xray::core::debug::platform::error_after_dialog( ) )
			xray::debug::dump_call_stack	( "debug", xray::logging::error, 0, 0, exception_information );
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}
}

void xray::core::debug::set_thread_stack_guarantee			( )
{
	unsigned long				stack_size = 32*1024;

	typedef BOOL ( __stdcall * SetThreadStackGuarantee_type ) (PULONG);
	static SetThreadStackGuarantee_type set_thread_stack_guarantee = 0;

	static bool initialized			= false;
	if ( !initialized ) {
		initialized					= true;
		HMODULE const kernel32		= LoadLibrary("kernel32.dll");
		R_ASSERT					(kernel32);
		set_thread_stack_guarantee	= ( SetThreadStackGuarantee_type )GetProcAddress( kernel32, "SetThreadStackGuarantee" );
	}

	if ( !set_thread_stack_guarantee )
		return;

	BOOL const result				= set_thread_stack_guarantee ( &stack_size );
	XRAY_UNREFERENCED_PARAMETER		( result );
	R_ASSERT						( result );
}
