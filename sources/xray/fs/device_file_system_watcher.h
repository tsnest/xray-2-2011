////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_DEVICE_FILE_SYSTEM_WATCHER_H_INCLUDED
#define FS_DEVICE_FILE_SYSTEM_WATCHER_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/fs/device_file_system_proxy_base.h>

namespace xray {
namespace fs_new {

class XRAY_FS_API device_file_system_watcher_proxy : public device_file_system_proxy_base
{
public:
	device_file_system_watcher_proxy	() : device_file_system_proxy_base(NULL), m_watcher_enabled(watcher_enabled_false) {}
	
	device_file_system_watcher_proxy	(device_file_system_interface * device, watcher_enabled_bool watcher_enabled)
		: device_file_system_proxy_base(device), m_watcher_enabled(watcher_enabled) {}

	device_file_system_watcher_proxy const *	operator -> () const { return this; }

	signalling_bool		open			(file_type * *					out, 
										 native_path_string const &		physical_path,
										 file_mode::mode_enum const		mode, 
										 file_access::access_enum const	access,
										 assert_on_fail_bool const		assert_on_fail = assert_on_fail_true,
										 notify_watcher_bool const		notify_watcher = notify_watcher_true,
										 use_buffering_bool const		use_buffering  = use_buffering_true) const;

	signalling_bool		open			(file_type * *					out, 
										 native_path_string const &		physical_path,
										 open_file_params const &		params) const;

	file_size_type	write						(file_type * file, pcvoid data, file_size_type size) const;
	void			close_file					(file_type * file) const;
	void			set_end_of_file_and_close	(file_type * file) const;
	void			set_file_size_and_close		(file_type * file, file_size_type size) const;

	bool								watcher_enabled () const { return m_watcher_enabled == watcher_enabled_true; }
private:
	watcher_enabled_bool				m_watcher_enabled;
};

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef FS_DEVICE_FILE_SYSTEM_WATCHER_H_INCLUDED