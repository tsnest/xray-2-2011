////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WINDOWS_HDD_FILE_SYSTEM_H_INCLUDED
#define WINDOWS_HDD_FILE_SYSTEM_H_INCLUDED

#include <xray/fs/device_file_system_proxy.h>
#include <xray/fs/file_type.h>

#if !XRAY_PLATFORM_PS3

namespace xray {
namespace fs_new {

class windows_hdd_file_system : public device_file_system_interface
{
public:
	virtual signalling_bool	open		(pvoid * const				out_handle,
										 native_path_string const &	absolute_path,
										 open_file_params const &	params);
	virtual void		close			(pvoid handle);
	virtual void		flush			(pvoid handle);

	virtual file_size_type	write		(pvoid handle, pcvoid data, file_size_type size);
	virtual file_size_type	read		(pvoid handle, pvoid data, file_size_type size);

	virtual bool		seek			(pvoid handle, file_size_type offset, seek_file_enum origin);
	virtual file_size_type	tell		(pvoid handle);

	virtual void		setvbuf			(pvoid handle, char * buffer, int mode, file_size_type size);
	virtual bool		get_file_size	(file_size_type * out_size, pvoid handle);

	virtual physical_path_info	get_physical_path_info	(native_path_string const & native_physical_path);
	virtual bool		find_first		(u64 *						out_search_handle,
										 physical_path_info_data *	out_data,
										 pcstr						absolute_native_physical_path_mask);

	virtual bool		find_next		(u64 * in_out_search_handle, physical_path_info_data * out_data);
	virtual void		find_close		(u64 search_handle);
	virtual bool		create_folder	(native_path_string const & absolute_physical_path);
	virtual bool		erase			(native_path_string const & absolute_physical_path);

}; // class windows_hdd_file_system

} // namespace fs_new
} // namespace xray

#endif // #if !XRAY_PLATFORM_PS3

#endif // #ifndef WINDOWS_HDD_FILE_SYSTEM_H_INCLUDED