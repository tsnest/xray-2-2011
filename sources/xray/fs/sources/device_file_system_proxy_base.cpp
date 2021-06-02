////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/device_file_system_proxy_base.h>
#include <xray/fs/physical_path_info.h>

namespace xray {
namespace fs_new {

void   device_file_system_proxy_base::close (file_type * file) const
{
	m_device_file_system->close(file_type_to_handle(file));
}

file_size_type	 device_file_system_proxy_base::write (file_type * file, pcvoid data, file_size_type size) const
{
	return	m_device_file_system->write(file_type_to_handle(file), data, size);
}

file_size_type   device_file_system_proxy_base::read (file_type * file, pvoid data, file_size_type size) const
{
	return	m_device_file_system->read(file_type_to_handle(file), data, size);
}

bool   device_file_system_proxy_base::seek (file_type * file, file_size_type offset, seek_file_enum origin) const
{
	return	m_device_file_system->seek(file_type_to_handle(file), offset, origin);
}

file_size_type   device_file_system_proxy_base::tell (file_type * file) const
{
	return	m_device_file_system->tell(file_type_to_handle(file));
}

void   device_file_system_proxy_base::setvbuf (file_type * stream, char * buffer, int mode, file_size_type size) const
{ 
	m_device_file_system->setvbuf			(file_type_to_handle(stream), buffer, mode, size);
}

void   device_file_system_proxy_base::flush (file_type * file) const
{
	return	m_device_file_system->flush		(file_type_to_handle(file));
}

bool   device_file_system_proxy_base::get_file_size (file_size_type * out_size, file_type * file) const
{
	return	m_device_file_system->get_file_size	(out_size, file_type_to_handle(file));
}

physical_path_info	 device_file_system_proxy_base::get_physical_path_info (native_path_string const &	native_physical_path) const
{
	return	m_device_file_system->get_physical_path_info(native_physical_path);
}

bool   device_file_system_proxy_base::find_first (u64 *						out_search_handle,
												 physical_path_info_data *	out_data,
												 pcstr						absolute_native_physical_path_mask) const
{
	return	m_device_file_system->find_first(out_search_handle, out_data, absolute_native_physical_path_mask);
}

bool   device_file_system_proxy_base::find_next (u64 * in_out_search_handle, physical_path_info_data * out_data) const
{
	return	m_device_file_system->find_next(in_out_search_handle, out_data);
}

void   device_file_system_proxy_base::find_close (u64 search_handle) const
{
	return	m_device_file_system->find_close(search_handle);
}

bool   device_file_system_proxy_base::create_folder (native_path_string const & absolute_physical_path) const
{
	return	m_device_file_system->create_folder(absolute_physical_path);
}

bool   device_file_system_proxy_base::erase (native_path_string const & absolute_physical_path) const
{
	return	m_device_file_system->erase(absolute_physical_path);
}

bool   device_file_system_proxy_base::file_exists (native_path_string const & physical_path) const
{
	pvoid	handle;
	open_file_params	params				(file_mode::open_existing, file_access::read, assert_on_fail_false, notify_watcher_false);
	bool const open_result				=	m_device_file_system->open	(& handle, physical_path, params);
	if ( !open_result )
		return								false;
	m_device_file_system->close				(handle);
	return									true;
}

} // namespace fs
} // namespace xray
