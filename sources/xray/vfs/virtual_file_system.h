////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VIRTUAL_FILE_SYSTEM_H_INCLUDED
#define VIRTUAL_FILE_SYSTEM_H_INCLUDED

#include <xray/vfs/platform_configuration.h>
#include <xray/vfs/watcher_notification.h>
#include <xray/vfs/mount_args.h>
#include <xray/vfs/saving_args.h>
#include <xray/vfs/scheduled_to_unmount.h>
#include <xray/vfs/find_results.h>
#include <xray/vfs/unpack.h>
#include <xray/vfs/mounter_base.h>
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

class vfs_watcher;

// called just before freeing node. a check for resource leak that is associated with node
// can be done here
typedef boost::function < void (vfs::vfs_iterator & it) >	on_node_unmount_callback;

// is called when node is being erased, its size updated or it is being overlapped
// its good time to deassociate node's resource
typedef boost::function < void (vfs::vfs_iterator & it) >	on_node_hides_callback;
typedef boost::function < void () >							on_unmount_started_callback;
typedef boost::function < void () >							dispatch_callbacks_function;
typedef boost::function < void (base_node<> * ) >			on_mounted_callback;

class virtual_file_system 
{
public:
			virtual_file_system			();

 	void	query_mount					(query_mount_arguments & args);
	bool	save_archive				(save_archive_args & args);
	bool	unpack						(unpack_arguments & args);

	bool	convert_physical_to_virtual_path	(fs_new::virtual_path_string *			out_path, 
												 fs_new::native_path_string const &		path,
												 pcstr mount_descriptor					= NULL);
	bool	convert_virtual_to_physical_path	(fs_new::native_path_string *			out_path, 
												 fs_new::virtual_path_string const &	path,
												 pcstr mount_descriptor					= NULL);
	bool	convert_virtual_to_physical_path	(fs_new::native_path_string *			out_path, 
												 fs_new::virtual_path_string const &	path,
												 u32 const								index,
												 vfs_mount_ptr *						out_mount_ptr = NULL);

	void			try_find_async		(fs_new::virtual_path_string const &	path_to_find,
										 find_callback							callback,
										 find_enum								find_flags,
										 memory::base_allocator *				allocator);

	result_enum		try_find_sync		(fs_new::virtual_path_string const &	path_to_find,
										 vfs_locked_iterator *					out_iterator,
 										 find_enum								find_flags,
 										 memory::base_allocator *				allocator);

	result_enum		find_async			(fs_new::virtual_path_string const &	path_to_find,
										 vfs_locked_iterator *					out_iterator,
										 find_enum								find_flags,
										 memory::base_allocator *				allocator,
										 dispatch_callbacks_function const &	dispatch_callback);


	void	dispatch_callbacks			();
	void	user_finalize				();

	void	assert_equal				(virtual_file_system & other, bool exact_equality_test = true);

	void	query_hot_mount				(fs_new::native_path_string const &	physical_path,
										 fs_new::virtual_path_string *		in_out_virtual_path, // can be ""
										 query_mount_callback const &		callback,
										 lock_operation_enum				lock_operation,
										 assert_on_fail_bool				assert_on_fail = assert_on_fail_true);

	void	query_hot_unmount			(fs_new::native_path_string const &	physical_path,
										 query_mount_callback const &		callback,
										 lock_operation_enum				lock_operation,
										 assert_on_fail_bool				assert_on_fail = assert_on_fail_true);

#if XRAY_FS_NEW_WATCHER_ENABLED
	void	on_watcher_notification		(vfs_notification const &			notification,
										 query_mount_callback const &		callback,
										 lock_operation_enum				lock_operation);
	void	set_watcher					(vfs_watcher * watcher);
	void	wakeup_watcher_thread		();
	u32		subscribe					(fs_new::virtual_path_string const & virtual_path, vfs_notification_callback const &);
	void	unsubscribe					(u32 subscriber_id);
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

private: 

 	void	query_mount_impl			(query_mount_arguments & args);

	bool	try_reference_to_pending_mount_unsafe	(query_mount_arguments & args, bool * out_of_memory);

private:
	friend	class						vfs_intrusive_mount_base;

public:
	typedef intrusive_double_linked_list<mounter_base, mounter *, & mounter_base::prev, & mounter_base::next, threading::mutex>	
		pending_mount_list;

#if XRAY_FS_NEW_WATCHER_ENABLED
	vfs_watcher *						watcher;
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

	mount_history_container				mount_history;
	vfs_hashset							hashset;
	pending_mount_list					pending_mounts;
	scheduled_to_unmount_container		scheduled_to_unmount;
	u32									unmount_thread_id; // default is -1 which means any
	on_node_unmount_callback			on_node_unmount;
	on_node_hides_callback				on_node_hides;
	on_unmount_started_callback			on_unmount_started;
	on_mounted_callback					on_mounted;

}; // class virtual_file_system


mount_result	query_mount_and_wait		(virtual_file_system &					vfs, 
											 query_mount_arguments &				args,
											 dispatch_callbacks_function			dispatch_callback = NULL);

void			destroy_temp_physical_node	(base_node<> * node);
base_node<> *   create_temp_physical_node	(fs_new::synchronous_device_interface &	device,
											 fs_new::native_path_string const &		physical_path,
											 memory::base_allocator * const			allocator);

void			query_hot_mount_and_wait	(virtual_file_system &					file_system, 
										  	 fs_new::virtual_path_string const &	virtual_path,
										 	 vfs_locked_iterator *					out_iterator,
										 	 memory::base_allocator *				allocator,
											 dispatch_callbacks_function const &	dispatch_callback = NULL);

void			query_hot_mount_and_wait	(virtual_file_system &					file_system, 
										  	 fs_new::native_path_string const &		physical_path,
										 	 vfs_locked_iterator *					out_iterator,
										 	 memory::base_allocator *				allocator,
											 dispatch_callbacks_function const &	dispatch_callback = NULL);

void			query_hot_mount_and_wait	(virtual_file_system &					file_system, 
											 fs_new::native_path_string const &		physical_path,
											 fs_new::virtual_path_string *			in_out_virtual_path,
											 vfs_locked_iterator *					out_iterator,
											 memory::base_allocator *				allocator,
											 dispatch_callbacks_function const &	dispatch_callback = NULL);

// waits while result will not be result_cannot_lock
result_enum		find_async_and_wait			(virtual_file_system &					file_system, 
											 fs_new::virtual_path_string const &	path_to_find,
											 vfs_locked_iterator *					out_iterator,
											 find_enum								find_flags,
											 memory::base_allocator *				allocator,
											 dispatch_callbacks_function const &	dispatch_callback);

} // namespace vfs
} // namespace xray

#endif // #ifndef VIRTUAL_FILE_SYSTEM_H_INCLUDED