////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MOUNT_ARGS_H_INCLUDED
#define MOUNT_ARGS_H_INCLUDED

#include <xray/vfs/mount_history.h>
#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/device_file_system_proxy_base.h>
#include <xray/vfs/mount_ptr.h>
#include <xray/vfs/types.h>

namespace xray {
namespace vfs {

struct mount_result
{
	vfs_mount_ptr	mount;
	result_enum		result;
	mount_result (vfs_mount_ptr	mount = NULL, result_enum result = result_success) : mount(mount), result(result) {}
};

typedef boost::function< void (mount_result) >	query_mount_callback;

class query_mount_arguments
{
public:
	fs_new::virtual_path_string				virtual_path;
	fs_new::native_path_string	 			physical_path;
	fs_new::native_path_string	 			archive_physical_path;
	fs_new::native_path_string	 			fat_physical_path;
	query_mount_callback					callback;
	fs_new::asynchronous_device_interface *	asynchronous_device;
	fs_new::synchronous_device_interface *	synchronous_device;
	memory::base_allocator *				allocator;
	mount_type_enum							type;
	fs_new::watcher_enabled_bool			watcher_enabled;
	recursive_bool							recursive;
	lock_operation_enum						lock_operation;
	fixed_string<32>						descriptor;
	
	query_mount_arguments	();
	static query_mount_arguments	mount_physical_path
							(memory::base_allocator *					allocator,
							 fs_new::virtual_path_string const &		virtual_path,
							 fs_new::native_path_string const &			physical_path,
							 pcstr										descriptor,
							 fs_new::asynchronous_device_interface *	async_device,
							 fs_new::synchronous_device_interface *		device,
							 query_mount_callback						callback,
							 recursive_bool								recursive,
							 lock_operation_enum						lock_operation	= lock_operation_lock,
							 fs_new::watcher_enabled_bool				watcher_enabled = fs_new::watcher_enabled_true);

	static query_mount_arguments	mount_archive
							(memory::base_allocator *					allocator,
							 fs_new::virtual_path_string const &		virtual_path,
							 fs_new::native_path_string const &			archive_physical_path,
							 fs_new::native_path_string const &			fat_physical_path,
							 pcstr										descriptor,
							 fs_new::asynchronous_device_interface *	async_device,
							 fs_new::synchronous_device_interface *		device,
							 query_mount_callback						callback,
							 lock_operation_enum						lock_operation	= lock_operation_lock);

	void	convert_pathes_to_absolute	();

	fs_new::native_path_string	get_physical_path	();

public: // used internally
	u32										mount_id;
	base_node<> *							root_write_lock;
	base_node<> *							submount_node;
	base_node<> *							parent_of_submount_node;
	vfs_mount *								mount_ptr; // needed for unmount
	submount_type_enum						submount_type;
	bool									unlock_after_mount;

	friend class							archive_mounter;
	friend class							physical_path_mounter;
};

} // namespace vfs
} // namespace xray

#endif // #ifndef MOUNT_ARGS_H_INCLUDED