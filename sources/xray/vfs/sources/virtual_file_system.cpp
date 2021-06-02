////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/virtual_file_system.h>
#include <xray/linkage_helper.h>
#include <xray/fs/device_utils.h>
#include <xray/vfs/mount_history.h>
#include "saving.h"
#include "mount_archive.h"
#include <xray/vfs/watcher.h>
#include "mount_physical_path.h"
#include "branch_locks.h"
#include "find.h"
#include "mount_archive.h"
#include "unmount.h"
#include "mount_physical_path.h"

XRAY_INCLUDE_TO_LINKAGE					(vfs_test);

namespace xray {
namespace vfs {

virtual_file_system::virtual_file_system () 
#if XRAY_FS_NEW_WATCHER_ENABLED
: watcher(NULL), unmount_thread_id(u32(-1))
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED
{
}

struct same_mount_predicate : public core::noncopyable
{
	same_mount_predicate(query_mount_arguments & args) : args(args) {}

	bool operator ()					(mounter * mount) const
	{
		if ( mount->args().type != args.type )
			return							false;

		if ( mount->args().virtual_path != args.virtual_path )
			return							false;

		if ( mount->args().submount_node != args.submount_node )
			return							false;

		if ( mount->args().physical_path != args.physical_path || mount->args().fat_physical_path != args.fat_physical_path )
			return							false;

		return								true;
	}

	query_mount_arguments &				args;
};

bool   virtual_file_system::try_reference_to_pending_mount_unsafe (query_mount_arguments & args, bool * out_of_memory)
{
	* out_of_memory						=	false;
	mounter * mount						=	pending_mounts.find_if(same_mount_predicate(args));
	if ( !mount )
		return								false;
	
	mount_referer * referer				=	XRAY_NEW_WITH_CHECK_IMPL(args.allocator, referer, mount_referer);
	if ( !referer )
	{
		* out_of_memory					=	true;
		return								false;
	}

	referer->allocator					=	args.allocator;
	referer->callback					=	args.callback;
	referer->ready_list					=	get_ready_referers_list(args.allocator);

	mount->add_referer						(referer);
	return									true;
}

void   virtual_file_system::query_mount_impl (query_mount_arguments & args)
{
	// it is essential that whole function executes with holding pending_mounts mutex
	pending_mount_list::mutex_raii	raii	(pending_mounts.policy());

	bool out_of_memory					=	false;
	if ( try_reference_to_pending_mount_unsafe(args, & out_of_memory) )
		return; // added as referer

	if ( out_of_memory )
	{
		args.callback						(mount_result(NULL, result_out_of_memory));
		return;
	}

	if ( !args.root_write_lock && 
		 !hashset.find_and_lock_branch(args.root_write_lock, "", lock_type_write, args.lock_operation) )
	{
		args.callback						(mount_result(NULL, result_cannot_lock));
		return;
	}

	if ( args.type == mount_type_physical_path )
	{
		physical_path_mounter	mounter		(args, * this);
		return;
	}
	
	if ( args.synchronous_device || (args.submount_type != submount_type_subfat) )
	{
		archive_mounter			mounter		(args, * this);
		return;
	}

	archive_mounter	* mounter			=	XRAY_NEW_WITH_CHECK_IMPL(args.allocator, mounter, archive_mounter)(args, * this);
	if ( !mounter )
	{
		unlock_branch						(args.root_write_lock, lock_type_write);
		args.callback						(mount_result(NULL, result_out_of_memory));
		return;
	}

	// mounter will self-destroy when finished
}

bool   virtual_file_system::save_archive	(save_archive_args & args)
{
	archive_saver	saver					(args, hashset);
	return									saver.finished_successfully();
}

bool   virtual_file_system::unpack		(unpack_arguments & args)
{
	return									vfs::unpack(* this, args);
}

void   virtual_file_system::dispatch_callbacks ()
{
	dispatch_ready_referers_list			();
	if ( unmount_thread_id == u32(-1) || unmount_thread_id == threading::current_thread_id() )
		dispatch_scheduled_to_unmount		(scheduled_to_unmount, mount_history);
}

void   virtual_file_system::user_finalize ()
{
	while ( !scheduled_to_unmount.empty() )
		dispatch_scheduled_to_unmount		(scheduled_to_unmount, mount_history);

	free_ready_referers_list				();
}

void   virtual_file_system::query_mount (query_mount_arguments & args)
{
	R_ASSERT								(args.callback);
	bool clear_root_write_lock			=	!args.root_write_lock;

	query_mount_impl						(args);

	if ( clear_root_write_lock )
		args.root_write_lock			=	NULL;
}

struct filter_by_descriptor
{
	filter_by_descriptor(pcstr descriptor) : descriptor(descriptor) {}

	bool operator () (pcstr in_descriptor, pcstr physical_path, pcstr virtual_path) const
	{
		XRAY_UNREFERENCED_PARAMETERS		(physical_path, virtual_path);
		return								!descriptor || 
											!* descriptor || 
											strings::equal(in_descriptor, descriptor);
	}

	pcstr descriptor;
};

bool   virtual_file_system::convert_physical_to_virtual_path (fs_new::virtual_path_string *			out_path, 
															  fs_new::native_path_string const &	path,
															  pcstr									mount_descriptor)
{
	filter_by_descriptor const				filter(mount_descriptor);
	if ( vfs::convert_physical_to_virtual_path (mount_history, out_path, path, filter) )
		return								true;

	return	vfs::convert_physical_to_virtual_path (mount_history, out_path, path, NULL);
}

bool   virtual_file_system::convert_virtual_to_physical_path (fs_new::native_path_string *			out_path, 
															  fs_new::virtual_path_string const &	path,
															  pcstr									mount_descriptor)
{
	filter_by_descriptor const				filter(mount_descriptor);
	if ( vfs::convert_virtual_to_physical_path (mount_history, out_path, path, filter) )
		return								true;

	
	return	vfs::convert_virtual_to_physical_path (mount_history, out_path, path, NULL);
}

bool   virtual_file_system::convert_virtual_to_physical_path (fs_new::native_path_string *			out_path, 
															  fs_new::virtual_path_string const &	path,
															  u32 const								index,
															  vfs_mount_ptr *						out_mount_ptr)
{
	return	vfs::convert_virtual_to_physical_path (mount_history, out_path, path, index, out_mount_ptr);
}

void   virtual_file_system::try_find_async	(fs_new::virtual_path_string const &	path,
											 find_callback							callback,
											 find_enum								find_flags,
											 memory::base_allocator *				allocator)
{
	vfs::try_find_async						(path.c_str(), callback, find_flags, this, allocator);
}

result_enum	   virtual_file_system::try_find_sync (fs_new::virtual_path_string const &	path,
												   vfs_locked_iterator *				out_iterator,
												   find_enum							find_flags,
												   memory::base_allocator *				allocator)
{
	return vfs::try_find_sync				(path.c_str(), out_iterator, find_flags, this, allocator);
}

result_enum		virtual_file_system::find_async		(fs_new::virtual_path_string const &	path,
													 vfs_locked_iterator *					out_iterator,
													 find_enum								find_flags,
													 memory::base_allocator *				allocator,
													 dispatch_callbacks_function const &	dispatch_callback)
{
	return									vfs::find_async(path.c_str(), out_iterator, find_flags, this, allocator, dispatch_callback);
}

#if XRAY_FS_NEW_WATCHER_ENABLED

void   quit_watching_history			(vfs_watcher * watcher, mount_history_container & history);
void   watch_history					(vfs_watcher * watcher, mount_history_container & history);

void   virtual_file_system::set_watcher	(vfs_watcher * const in_watcher)
{
	quit_watching_history					(watcher, mount_history);
	watcher								=	in_watcher;
	watch_history							(in_watcher, mount_history);
}

void   virtual_file_system::wakeup_watcher_thread	()
{
	if ( watcher )
		watcher->wakeup						();
}

u32	  virtual_file_system::subscribe	(fs_new::virtual_path_string const & virtual_path, vfs_notification_callback const & callback)
{
	if ( watcher )
		return								watcher->subscribe(virtual_path, callback);

	return									0;
}

void   virtual_file_system::unsubscribe	(u32 subscriber_id)
{
	if ( watcher )
		watcher->unsubscribe				(subscriber_id);
}

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

void   on_mounted						(vfs::mount_result		result, 
										 vfs::mount_result *	out_result, 
										 bool * 				finished)
{
	* out_result						=	result;
	* finished							=	true;
}

vfs::mount_result   query_mount_and_wait	(vfs::virtual_file_system &		vfs, 
											 vfs::query_mount_arguments &	args,
											 dispatch_callbacks_function	dispatch_callback)
{
	vfs::mount_result						result;
	bool				finished		=	false;
	args.callback						=	boost::bind(& on_mounted, _1, & result, & finished);
	
	do {

		finished						=	false;

		while ( !finished )
		{
			vfs.query_mount					(args);

			if ( args.asynchronous_device )
				args.asynchronous_device ->dispatch_callbacks	();

			if ( dispatch_callback )
				dispatch_callback			();

  			vfs.dispatch_callbacks			();
		}

	} while ( result.result == result_cannot_lock );

	return									result;
}

base_node<> *   create_temp_physical_node (fs_new::synchronous_device_interface &	device,
										   fs_new::native_path_string const &		physical_path,
										   memory::base_allocator * const			allocator)
{
	if ( !physical_path )
		return								NULL;
	
	file_size_type file_size			=	0;
	if ( !fs_new::calculate_file_size(device, & file_size, physical_path) )
		return								NULL;

	query_mount_arguments args			=	query_mount_arguments::mount_physical_path
												(allocator, "", physical_path, "", NULL, & device, NULL, recursive_false);

	pcstr const file_name				=	fs_new::file_name_from_path(physical_path);

	physical_file_mount_root_node<> * const out_node	=	
			mount_root_node_functions::create<physical_file_mount_root_node, platform_pointer_default>
											(file_name, strings::length(file_name), NULL, args);

	return									node_cast<base_node>(out_node);
}

void   destroy_temp_physical_node		(base_node<> * node)
{
	physical_file_mount_root_node<> * mount_root	=	node_cast<physical_file_mount_root_node>(node);
	R_ASSERT								(mount_root);
	memory::base_allocator * const allocator	=	mount_root->allocator;
	XRAY_FREE_IMPL							(allocator, mount_root);
}

void	query_hot_mount_and_wait		(virtual_file_system &					file_system, 
										 fs_new::virtual_path_string const &	in_virtual_path,
										 vfs_locked_iterator *					out_iterator,
										 memory::base_allocator *				allocator,
										 dispatch_callbacks_function const &	dispatch_callback)
{
	fs_new::native_path_string				physical_path;
	if ( out_iterator )
		out_iterator->clear					();

	fs_new::virtual_path_string	virtual_path	=	in_virtual_path;
	u32 index							=	0;
	while ( file_system.convert_virtual_to_physical_path(& physical_path, virtual_path, index) )
	{
		query_hot_mount_and_wait			(file_system, physical_path, & virtual_path, out_iterator, allocator, dispatch_callback);
		
		//file_system.query_hot_mount			(physical_path, 
		//									 & virtual_path,
		//									 boost::bind(& on_hot_mounted, 
		//												 out_iterator,  
		//												 & file_system, 
		//												 & virtual_path, 
		//												 allocator, 														 
		//												 _1),
		//									 lock_operation_lock);

		++index;
	}
}

void	query_hot_mount_and_wait		(virtual_file_system &					file_system, 
										 fs_new::native_path_string const &		physical_path,
										 vfs_locked_iterator *					out_iterator,
										 memory::base_allocator *				allocator,
										 dispatch_callbacks_function const &	dispatch_callback)
{
	fs_new::virtual_path_string				virtual_path;
	bool const convertion_result		=	file_system.convert_physical_to_virtual_path(& virtual_path, physical_path);
	R_ASSERT								(convertion_result);

	query_hot_mount_and_wait				(file_system, physical_path, & virtual_path, out_iterator, allocator, dispatch_callback);
}

static void hot_mount_helper			(bool *									out_got_lock,
										 vfs_locked_iterator *					out_iterator, 
										 virtual_file_system *					file_system,
										 fs_new::virtual_path_string const *	virtual_path,
										 memory::base_allocator *				allocator,
										 mount_result							result)
{
	if ( result.result == result_cannot_lock )
		return;

	* out_got_lock						=	true;
	if ( !out_iterator || * out_iterator )
		return;

	file_system->try_find_sync				(virtual_path->c_str(), 
											 out_iterator, 
											 (find_enum)0, 
											 allocator);
}

void	query_hot_mount_and_wait		(virtual_file_system &					file_system, 
										 fs_new::native_path_string const &		physical_path,
										 fs_new::virtual_path_string *			in_out_virtual_path,
										 vfs_locked_iterator *					out_iterator,
										 memory::base_allocator *				allocator,
										 dispatch_callbacks_function const &	dispatch_callback)
{
	bool got_lock						=	false;
	while ( !got_lock )
	{
		file_system.query_hot_mount			(physical_path, 
											in_out_virtual_path,
											 boost::bind(& hot_mount_helper, 
														 & got_lock,
														 out_iterator,  
														 & file_system, 
														 in_out_virtual_path, 
														 allocator,
														 _1),
											 lock_operation_try_lock);
		if ( got_lock )
			break;

		if ( dispatch_callback )
			dispatch_callback				();
	}	
}

result_enum		find_async_and_wait			(virtual_file_system &					file_system, 
											 fs_new::virtual_path_string const &	path_to_find,
											 vfs_locked_iterator *					out_iterator,
											 find_enum								find_flags,
											 memory::base_allocator *				allocator,
											 dispatch_callbacks_function const &	dispatch_callback)
{
	result_enum result					=	result_fail;

	do 
	{
		result							=	file_system.find_async(path_to_find, out_iterator, 
																   find_flags, allocator, dispatch_callback);
	} while ( result == result_cannot_lock );

	return									result;
}

} // namespace vfs
} // namespace xray