#include "pch.h"
#include "resources_device_manager.h"
#include "resources_macros.h"
#include <xray/managed_allocator.h>
#include "resources_manager.h"
#include <xray/resources_query_result.h>
#include <xray/os_include.h>

namespace xray {
namespace resources {

using namespace		fs;

// out: false if cannot open file, ready and fail flags are set
bool   device_manager::open_file ()
{
	using namespace							fs;

	ASSERT									(m_query);
	ASSERT									(!m_file_handle);

	if ( !m_query->is_save_type() )
	{
		fs_iterator it					=	m_query->get_fat_it();
		if ( it.is_end() || it.is_folder() )
		{
			m_query->set_error_type			(query_result::error_type_file_not_found);
			return							false;
		}
	}

	fs_new::native_path_string const &	native_path		=	m_query->absolute_physical_path();
	fs_new::virtual_path_string	path_string const portable_path	(native_path);

	u32	notifications_to_skip			=	0;

	if ( m_query->is_load_type() )
	{
 		m_file_handle					=	CreateFile (native_path.c_str(),
 														GENERIC_READ,
 														FILE_SHARE_READ | FILE_SHARE_WRITE,
 														NULL,
 														OPEN_EXISTING,
 														FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED
 																			  /*| FILE_FLAG_NO_BUFFERING*/,
 														NULL);
	}
	else
	{
		notifications_to_skip			=	fs_new::open_file_notifications_count(portable_path.c_str(), open_file_create | open_file_truncate) 
											 + 1; // for write about WriteFile

		skip_deassociation_notifications	(portable_path.c_str(), notifications_to_skip);
		make_dir_r							(portable_path.c_str(), false);

		m_file_handle					=	CreateFile (native_path.c_str(),
 														GENERIC_WRITE,
 														FILE_SHARE_WRITE | FILE_SHARE_READ,
 														NULL,
 														CREATE_ALWAYS,//OPEN_ALWAYS,
 														FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
 														NULL);
	}

	LOGI_DEBUG								("resources:device_manager", "async %s opening %s %s", 
											m_query->is_load_type() ? "read" : "write",
											m_file_handle == INVALID_HANDLE_VALUE ? "failed" : "successfull",
											detail::make_query_logging_string (m_query).c_str());

	if ( m_last_file_name != native_path )
	{
		m_sector_data_last_file_pos		=	(file_size_type)-1;
		m_last_file_name				=	native_path;
	}
	
	if ( m_file_handle == INVALID_HANDLE_VALUE )
	{
		bool const removed				=	try_remove_deassociation_notifications(portable_path.c_str(), notifications_to_skip);
		R_ASSERT_U							(removed);

		m_query->set_error_type				(query_result::error_type_cannot_open_file);
		return								false;
	}

	return									true;
}

bool   device_manager::do_async_operation (mutable_buffer out_data, u64 const file_pos, bool sector_aligned)
{
	COMPILE_ASSERT	(	sizeof(OVERLAPPED) == sizeof(device_manager::m_overlapped), 
						please_change_buffer_size );

	OVERLAPPED & overlapped				=	(OVERLAPPED &)m_overlapped;
	overlapped.hEvent					=	m_async_file_operation_finished_event.get_handle();
	if ( sector_aligned )
	{
		R_ASSERT							(!(file_pos % m_sector_size));
		R_ASSERT							(!(out_data.size() % m_sector_size));
	}
	
	overlapped.Offset					=	file_pos & 0xFFFFFFFF;
	overlapped.OffsetHigh				=	file_pos >> 32;

	BOOL const async_op_result			=	m_query->is_load_type() ? 
											ReadFile(m_file_handle, out_data.c_ptr(), out_data.size(), NULL, & overlapped) :
											WriteFile(m_file_handle, out_data.c_ptr(), out_data.size(), NULL, & overlapped);

	DWORD bytes_done;
	bool result							=	true;
	if ( !async_op_result )
		if ( GetLastError() != ERROR_IO_PENDING )
			result						=	false;

	if ( result )
	{
		m_async_file_operation_finished_event.wait	(threading::wait_time_infinite);
		if ( !GetOverlappedResult(m_file_handle, & overlapped, & bytes_done, FALSE) )
			result						=	false;
	}

	if ( !result )
	{
		R_ASSERT							(false, "async operation returned error code: %d", GetLastError());
		m_query->set_error_type				(m_query->is_load_type() ? 
											 query_result::error_type_cannot_read_file :
											 query_result::error_type_cannot_write_file);
		return								false;
	}

	return									true;
}

void   device_manager::close_file		()
{
	CloseHandle								(m_file_handle);
}


} // namespace resources
} // namespace xray