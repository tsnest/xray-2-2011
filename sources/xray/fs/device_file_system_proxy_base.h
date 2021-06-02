////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_DEVICE_FILE_SYSTEM_PROXY_BASE_H_INCLUDED
#define FS_DEVICE_FILE_SYSTEM_PROXY_BASE_H_INCLUDED

#include <xray/fs/api.h>
#include <xray/fs/device_file_system_interface.h>

namespace xray {
namespace fs_new {

enum watcher_enabled_bool				{	watcher_enabled_false, watcher_enabled_true	};

class XRAY_FS_API device_file_system_proxy_base
{
public:
	device_file_system_proxy_base (device_file_system_interface * device) 
										: m_device_file_system(device) {}

	void			close				(file_type * file) const;
	void			flush				(file_type * file) const;

	file_size_type	write				(file_type * file, pcvoid data, file_size_type size) const;
	file_size_type	read				(file_type * file, pvoid data, file_size_type size) const;

	bool			seek				(file_type * file, file_size_type offset, seek_file_enum origin) const;
	file_size_type	tell				(file_type * file) const;
	
	void			setvbuf				(file_type * stream, char * buffer, int mode, file_size_type size) const;
	bool			get_file_size		(file_size_type * out_size, file_type * file) const;

	bool			file_exists			(native_path_string const & physical_path) const;
	physical_path_info	get_physical_path_info	(native_path_string const &	native_physical_path) const;
	bool			find_first			(u64 *						out_search_handle,
										 physical_path_info_data *	out_data,
										 pcstr						absolute_native_physical_path_mask) const;

	bool			find_next			(u64 *						in_out_search_handle, 
										 physical_path_info_data *	out_data) const;

	void			find_close			(u64 search_handle) const;

	bool			create_folder		(native_path_string const & absolute_physical_path) const;
	bool			erase				(native_path_string const & absolute_physical_path) const;

	device_file_system_interface *		get_device_file_system_interface () const { return m_device_file_system; }
	
protected:
	device_file_system_interface *		m_device_file_system;
};

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_DEVICE_FILE_SYSTEM_PROXY_BASE_H_INCLUDED