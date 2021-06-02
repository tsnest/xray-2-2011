////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/windows_hdd_file_system.h>
//#include "debug_platform.h"
#include <xray/os_include.h>
#include <direct.h>					// for _mkdir

namespace xray {
namespace fs_new {

#pragma message(XRAY_TODO("fix it"))
void   log_last_error (pcstr initiator, pcstr header)
{
	XRAY_UNREFERENCED_PARAMETERS(initiator, header);
	/*pstr message					=	core::debug::platform::fill_format_message	((int)GetLastError());
	fixed_string<128>	full_initiator	=	XRAY_LOG_MODULE_INITIATOR;
	if ( initiator && *initiator )
	{
		full_initiator				+=	":";
		full_initiator				+=	initiator;
	}

	logging::helper	( __FILE__, __FUNCSIG__, __LINE__,  full_initiator.c_str(), logging::error)
										("%s: %s", header, message);

	core::debug::platform::free_format_message	(message);
	if ( debug::is_debugger_present() )
		DEBUG_BREAK();*/
}

signalling_bool   windows_hdd_file_system::open (pvoid * const				out_handle,
												 native_path_string const &	absolute_path,
												 open_file_params const &	params)
{
	DWORD flags							=	FILE_ATTRIBUTE_NORMAL;
	if ( params.use_buffering == !use_buffering_true )
		flags							|=	FILE_FLAG_NO_BUFFERING;
	DWORD desired_access				=	0;
	if ( params.access == file_access::read )
		desired_access					|=	GENERIC_READ;
	else if ( params.access == file_access::write )
		desired_access					|=	GENERIC_WRITE;
	else if ( params.access == file_access::read_write )
		desired_access					|=	GENERIC_READ | GENERIC_WRITE;

	DWORD share_mode					=	FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD creation_disposition			=	0;
	if ( params.mode == file_mode::create_always )
		creation_disposition			=	CREATE_ALWAYS;
	else if ( params.mode == file_mode::open_existing )
		creation_disposition			=	OPEN_EXISTING;
	else if ( params.mode == file_mode::append_or_create )
		creation_disposition			=	OPEN_ALWAYS;

	HANDLE const handle					=	CreateFile (absolute_path.c_str(),
 														desired_access,
 														share_mode,
 														NULL,
 														creation_disposition,
 														flags,
 														NULL);
	if ( handle == INVALID_HANDLE_VALUE && params.assert_on_fail )
	{
		log_last_error						("windows_hdd_file_system", "open_file_impl");
		FATAL								("failed opening file:%s with open flags:%s",
											 absolute_path.c_str(),
											 file_open_flags_to_string(params.mode, params.access).c_str());
	}

	
	if ( handle != INVALID_HANDLE_VALUE && params.mode == file_mode::append_or_create )
		SetFilePointer						(handle, 0, 0, FILE_END);

	* out_handle						=	handle;

	return									(handle != INVALID_HANDLE_VALUE);
}

bool   windows_hdd_file_system::get_file_size (file_size_type * out_size, pvoid const handle)
{
	LARGE_INTEGER							out;
	if ( !GetFileSizeEx((HANDLE)handle, & out) )
	{
		log_last_error						("windows_hdd_file_system", "get_file_size");
		return								false;
	}
	* out_size							=	file_size_type(out.QuadPart);
	return									true;
}

bool   windows_hdd_file_system::seek (pvoid const handle, file_size_type in_offset, seek_file_enum origin)
{
	LARGE_INTEGER							offset;
	offset.LowPart						=	in_offset & u32(-1);
	offset.HighPart						=	(in_offset >> 32);
	DWORD									move_method;
	if ( origin == seek_file_begin )
		move_method						=	FILE_BEGIN;
	else if ( origin == seek_file_current )
		move_method						=	FILE_CURRENT;
	else if ( origin == seek_file_end )
		move_method						=	FILE_END;
	else
		NODEFAULT							(move_method = FILE_BEGIN);

	if ( !SetFilePointerEx((HANDLE)handle, offset, NULL, move_method) )
	{
		log_last_error						("windows_hdd_file_system", "seek_file");
		return								false;
	}

	return									true;
}

file_size_type   windows_hdd_file_system::tell (pvoid const handle)
{
	LARGE_INTEGER	offset, out;
	offset.QuadPart						=	0;
	if ( !SetFilePointerEx(handle, offset, & out, FILE_CURRENT) )
	{
		log_last_error						("windows_hdd_file_system", "tell_file");
		return								0;
	}
	
	return									file_size_type(out.QuadPart);
}

void   windows_hdd_file_system::setvbuf (pvoid const handle, char * buffer, int mode, file_size_type size)
{
	int const os_handle					=	_open_osfhandle ( reinterpret_cast<intptr_t>(handle), 0);
	if ( os_handle == -1 )
	{
		LOGI_ERROR							("windows_hdd_file_system", "_open_osfhandle: failed");
		return;
	}

	FILE * fp							=	_fdopen(os_handle, "r+b");
	if ( !fp )
	{
		LOGI_ERROR							("windows_hdd_file_system", "_fdopen: failed");
		return;
	}
	::setvbuf								(fp, buffer, mode, (size_t)size);
}

void   windows_hdd_file_system::flush	(pvoid const handle)
{
	if ( !FlushFileBuffers((HANDLE)handle) )
		log_last_error						("windows_hdd_file_system", "flush_file");
}

bool   windows_hdd_file_system::create_folder (native_path_string const & absolute_physical_path)
{
	if ( _unlink(absolute_physical_path.c_str()) != 0 ) 
	{
	}

	if ( _mkdir (absolute_physical_path.c_str()) == -1 )
	{
		errno_t err;
		_get_errno							(&err);
		return								err == EEXIST ? true : false;
	}

	return									true;
}

bool   windows_hdd_file_system::erase	(native_path_string const & absolute_physical_path)
{
	if ( _unlink(absolute_physical_path.c_str()) != 0 ) 
	{
		if ( _rmdir(absolute_physical_path.c_str()) != 0 )
			return							false;
	}

	return									true;
}

file_size_type	  windows_hdd_file_system::read	(pvoid const handle, pvoid data, file_size_type size)
{
	DWORD	bytes_read					=	0;
	if ( !ReadFile((HANDLE)handle, data, (DWORD)size, & bytes_read, NULL) )
		log_last_error						("windows_hdd_file_system", "read_file");

	return									bytes_read;
}

file_size_type   windows_hdd_file_system::write (pvoid const handle, pcvoid data, file_size_type size)
{
	R_ASSERT								(handle);
	DWORD	bytes_written				=	0;
	if ( !WriteFile((HANDLE)handle, data, (DWORD)size, & bytes_written, NULL) )
		log_last_error						("windows_hdd_file_system", "write_file");

	return									bytes_written;
}

void   windows_hdd_file_system::close	(pvoid const handle)
{
	if ( !CloseHandle((HANDLE)handle) ) 
		log_last_error						("windows_hdd_file_system", "close_file");
}

} // namespace fs
} // namespace xray
