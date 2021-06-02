////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/device_file_system_no_watcher.h>
#include <xray/fs/device_utils.h>
#include <xray/fs/path_string_utils.h>

#if !XRAY_FS_NEW_WATCHER_ENABLED

namespace xray {
namespace fs_new {

signalling_bool	  device_file_system_no_watcher_proxy::open (file_type * *					out, 
													  		 native_path_string const &		physical_path,
 													  		 file_mode::mode_enum const		mode, 
													  		 file_access::access_enum const	access,
													  		 assert_on_fail_bool const		assert_on_fail,
													  		 notify_watcher_bool const		notify_watcher,
													  		 use_buffering_bool const		use_buffering) const
{
	return									open	(out, physical_path, 
													 open_file_params(mode, 
																	  access,
   																	  assert_on_fail, 
																	  notify_watcher, 
																	  use_buffering));
}

signalling_bool	  device_file_system_no_watcher_proxy::open(file_type * *					out_file, 
															native_path_string const &		physical_path,
															open_file_params const &		params) const

{
	if ( params.mode == file_mode::create_always )
		create_folder_r						(synchronous_device_interface(*this), physical_path, false);

	native_path_string						absolute_native_path;
	if ( !convert_to_absolute_path(& absolute_native_path, physical_path, params.assert_on_fail) )
		return								false;
	
	pvoid	handle						=	NULL;
	bool const open_result				=	m_device_file_system->open	(& handle, absolute_native_path, params);
	* out_file							=	(file_type *)handle;
	return									open_result;
}

file_size_type	device_file_system_no_watcher_proxy::write	(file_type * file, pcvoid data, file_size_type size) const
{
	return									m_device_file_system->write( file_type_to_handle(file), data, size );
}

void	device_file_system_no_watcher_proxy::close	(file_type * file) const
{
	m_device_file_system->close				(file_type_to_handle(file));
}

void   device_file_system_no_watcher_proxy::set_file_size_and_close (file_type * file, file_size_type size) const
{
	m_device_file_system->seek 				(file_type_to_handle(file), size, seek_file_begin);
	m_device_file_system->close				(file_type_to_handle(file));
}

void   device_file_system_no_watcher_proxy::set_end_of_file_and_close (file_type * file) const
{
	file_size_type const file_pos		=	m_device_file_system->tell(file_type_to_handle(file));
	set_file_size_and_close					((file_type*)file_type_to_handle(file), file_pos);
}

} // namespace fs_new
} // namespace xray

#endif // #if !XRAY_FS_NEW_WATCHER_ENABLED
