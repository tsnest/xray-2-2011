////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_FILE_TYPE_POINTER_H_INCLUDED
#define XRAY_FS_FILE_TYPE_POINTER_H_INCLUDED

#include <xray/fs/file_type.h>
#include <xray/fs/synchronous_device_interface.h>

namespace xray {
namespace fs_new {

class file_type_pointer : core::noncopyable
{
public:
	file_type_pointer					(native_path_string const &				physical_path,
										 synchronous_device_interface const &	device,
										 file_mode::mode_enum const				mode, 
										 file_access::access_enum const			access,
										 assert_on_fail_bool const				assert_on_fail = assert_on_fail_true,
										 notify_watcher_bool const				notify_watcher = notify_watcher_true,
										 use_buffering_bool const				use_buffering  = use_buffering_true)
		: device(device)
	{
		bool const open_result			=	device->open(& file, physical_path, mode, access, assert_on_fail, notify_watcher, use_buffering);
		if ( open_result )
			set_owned_by_pointer			(file, true);
	}

	file_type_pointer					(native_path_string const &				physical_path, 
										 synchronous_device_interface const &	device,
										 open_file_params const &				params)
		: device(device)
	{
		bool const open_result			=	device->open(& file, physical_path, params);
		XRAY_UNREFERENCED_PARAMETER			(open_result);
	}

	~file_type_pointer					()	{ close(); }

	operator bool						() const	{ return !!file; }
	operator file_type *				() const	{ return file; }

	void	close						()
	{ 
		if ( file )							
		{
			set_owned_by_pointer			(file, false);
			device->close					(file); 
		}
		file							=	NULL;
	}

private:
	synchronous_device_interface const &	device;
	file_type *								file;

}; // class close_file_guard

} // namespace fs_new
} // namespace xray

#endif // #ifndef XRAY_FS_FILE_TYPE_POINTER_H_INCLUDED