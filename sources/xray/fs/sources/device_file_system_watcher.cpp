////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/device_file_system_watcher.h>
#include <xray/fs/device_file_system_proxy.h>
#include <xray/fs/physical_path_info.h>
#include <xray/fs/native_path_string.h>
#include <xray/fs/device_utils.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/watcher_utils.h>

#if XRAY_FS_NEW_WATCHER_ENABLED

namespace xray {
namespace fs_new {

void   skip_deassociation_notifications (file_type * file, u32 const notifications_to_skip);

signalling_bool	  device_file_system_watcher_proxy::open (file_type * *						out, 
														  native_path_string const &		physical_path,
														  file_mode::mode_enum const		mode, 
														  file_access::access_enum const	access,
														  assert_on_fail_bool const			assert_on_fail,
														  notify_watcher_bool const			notify_watcher,
														  use_buffering_bool const			use_buffering) const
{
	return									open(out, physical_path, 
												 open_file_params(mode, 
																  access,
																  assert_on_fail, 
																  notify_watcher, 
																  use_buffering));
}

static
u32   open_file_notifications_count (device_file_system_interface * const	device,
									 native_path_string const &		absolute_physical_path, 
									 file_mode::mode_enum const &	mode)
{
	physical_path_info const path_type	=	device->get_physical_path_info(absolute_physical_path);
	if ( !path_type.exists() )
	{
		if ( mode != file_mode::append_or_create && mode != file_mode::create_always )
			return							0;
		return								1;
	}

	if ( path_type.is_folder() )
		return								0;

	if ( mode == file_mode::create_always )
		return								1;

	return									0;
}

signalling_bool	  device_file_system_watcher_proxy::open	(file_type * *					out_file, 
												 			 native_path_string const &		physical_path,
												 			 open_file_params const &		params) const
{
	if ( params.mode == file_mode::create_always )
		create_folder_r						(synchronous_device_interface(*this), physical_path, false);

	native_path_string						absolute_native_path;
	if ( !convert_to_absolute_path(& absolute_native_path, physical_path, params.assert_on_fail) )
		return								false;

	u32 const notifications_to_skip		=	!params.notify_watcher && m_watcher_enabled ? 
												open_file_notifications_count(m_device_file_system, absolute_native_path, params.mode) 
												: 0;
	skip_deassociation_notifications		(absolute_native_path, notifications_to_skip);

	pvoid out_handle					=	NULL;
	bool const out_result				=	m_device_file_system->open(& out_handle, absolute_native_path, params);
	if ( !out_result )
	{
		bool const removed				=	try_remove_deassociation_notifications(physical_path, notifications_to_skip);
		R_ASSERT							(removed);
	}
	else
	{
		u32 const path_length			=	physical_path.length();
		file_type * const file			=	params.file_type_allocated_by_user ?
												* out_file 
													:			
												(file_type *)DEBUG_ALLOC(char, sizeof(file_type) + path_length + 1);

		new (file)							file_type;
		file->set_skip_notifications		(!params.notify_watcher && m_watcher_enabled);
		file->set_allocated_by_user			(params.file_type_allocated_by_user);
		strings::copy						(file->file_name, path_length + 1, physical_path.c_str());
		file->handle					=	out_handle;

		* out_file						=	file;
	}

	return									out_result;
}

file_size_type   device_file_system_watcher_proxy::write (file_type * file, pcvoid data, file_size_type size) const
{
	R_ASSERT								(file->handle);

	if ( !file->done_write() )
	{
		if ( file->skip_notifications() )
			skip_deassociation_notifications(file, 1);

		file->set_done_write				(true);
	}

	return									m_device_file_system->write(file_type_to_handle(file), data, size);
}

void   device_file_system_watcher_proxy::close_file (file_type * file) const
{
	R_ASSERT								(!file->owned_by_pointer());
	m_device_file_system->close				(file_type_to_handle(file));
	file->~file_type						();
	if ( !file->allocated_by_user() )
		DEBUG_FREE							(file);
}

void   device_file_system_watcher_proxy::set_file_size_and_close (file_type * file, file_size_type size) const
{
	if ( file->skip_notifications() ) 
	{
		file_size_type						previous_size;
		bool const got_file_size		=	get_file_size(& previous_size, file);
		R_ASSERT_U							(got_file_size);

		u32 notifications_to_skip		=	0;
		if ( previous_size < size )
			notifications_to_skip		=	1;
		else if ( previous_size > size )
			notifications_to_skip		=	2;

		skip_deassociation_notifications	(file, notifications_to_skip);
	}

	seek 									(file, size, seek_file_begin);
	close									(file);
}

void   device_file_system_watcher_proxy::set_end_of_file_and_close (file_type * file) const
{
	s64 const file_pos					=	tell(file);
	set_file_size_and_close					(file, file_pos);
}

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

