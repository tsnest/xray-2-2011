////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_DEVICE_FILE_SYSTEM_INTERFACE_H_INCLUDED
#define FS_DEVICE_FILE_SYSTEM_INTERFACE_H_INCLUDED

#include <xray/fs/native_path_string.h>
#include <xray/fs/portable_path_string.h>
#include <xray/fs/file_type.h>
#include <xray/fs/platform_configuration.h>

namespace xray {
namespace fs_new {

class physical_path_info;
struct physical_path_info_data;

class XRAY_NOVTABLE device_file_system_interface
{
public:
	virtual ~device_file_system_interface	() {}
	virtual signalling_bool	open		(pvoid * const					out_handle,
										 native_path_string const &		absolute_path,
										 open_file_params const &		params) = 0;
	virtual void		close			(pvoid handle) = 0;
	virtual void		flush			(pvoid handle) = 0;

	virtual file_size_type	write		(pvoid handle, pcvoid data, file_size_type size) = 0;
	virtual file_size_type	read		(pvoid handle, pvoid data, file_size_type size) = 0;

	virtual bool		seek			(pvoid handle, file_size_type offset, seek_file_enum origin) = 0;
	virtual file_size_type	tell		(pvoid handle) = 0;

	virtual void		setvbuf			(pvoid handle, char * buffer, int mode, file_size_type size) = 0;
	virtual bool		get_file_size	(file_size_type * out_size, pvoid handle) = 0;

	virtual physical_path_info	get_physical_path_info	(native_path_string const & native_physical_path) = 0;
	virtual bool		find_first		(u64 *						out_search_handle,
										 physical_path_info_data *	out_data,
										 pcstr						absolute_native_physical_path_mask) = 0;

	virtual bool		find_next		(u64 * in_out_search_handle, physical_path_info_data * out_data) = 0;
	virtual void		find_close		(u64 search_handle) = 0;
	virtual bool		create_folder	(native_path_string const & absolute_physical_path) = 0;
	virtual bool		erase			(native_path_string const & absolute_physical_path) = 0;
};

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_DEVICE_FILE_SYSTEM_INTERFACE_H_INCLUDED