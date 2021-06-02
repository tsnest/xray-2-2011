////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/vfs/virtual_file_system.h>
#include "unmount.h"

namespace xray {
namespace vfs {

using namespace fs_new;

void   on_erased_renamed_node			(mount_result result)
{
	R_ASSERT								(result.result == result_success);
}

struct query_notification_operation : core::noncopyable
{
	query_notification_operation		(virtual_file_system &			file_system,
										 virtual_path_string *			in_out_virtual_path,
										 native_path_string const &		physical_path,
										 native_path_string const &		physical_old_path,
										 native_path_string const &		physical_new_path,
										 query_mount_callback const &	callback,
										 lock_operation_enum			lock_operation,
										 assert_on_fail_bool			assert_on_fail = assert_on_fail_true)	
		:	m_file_system(file_system), 
			m_virtual_path(in_out_virtual_path),
			m_physical_path(physical_path), 
			m_physical_old_path(physical_old_path), 
			m_physical_new_path(physical_new_path), 
			m_callback(callback), 
			m_lock_operation(lock_operation), 
			m_assert_on_fail(assert_on_fail),
			m_mount_root(NULL),
			m_mount_id(NULL),
			m_lock_node(NULL) 
	{
	}

	bool   mounts_filter				(pcstr descriptor, pcstr physical_path, pcstr virtual_path)
	{
		XRAY_UNREFERENCED_PARAMETER			(descriptor);
		if ( !path_starts_with(m_physical_path.c_str(), physical_path) )
			return							false;
		if ( !path_starts_with(m_virtual_path->c_str(), virtual_path) )
			return							false;

		return								true;
	}

	bool   try_convert_to_virtual_path	(native_path_string const &		physical_path)
	{
		if ( m_virtual_path->length() )
		{
			m_mount						=	find_in_mount_history(
				boost::bind(& query_notification_operation::mounts_filter, this, _1, _2, _3),
				m_file_system.mount_history
				);

			R_ASSERT						(m_mount);

			m_mount_root				=	m_mount->get_mount_root();
			m_mount_id					=	m_mount_root->mount_id;
			return							true;
		}

		bool const convertion_result	=	convert_physical_to_virtual_path(m_file_system.mount_history, 
																			 m_virtual_path,
																			 physical_path,
																			 NULL,
																			 & m_mount);
		if ( !convertion_result )
		{
			R_ASSERT						(!m_assert_on_fail, "no corresponding virtual path in file system to: '%s'", physical_path.c_str());
			m_callback						(mount_result(NULL, result_fail));
			return							false;
		}

		m_mount_root					=	m_mount->get_mount_root();
		m_mount_id						=	m_mount_root->mount_id;

		return								true;
	}

	base_node<> *   find_node_on_virtual_path	(overlapped_node_iterator & it, overlapped_node_iterator & it_end)
	{	
		for ( ; it != it_end; ++it )
		{
			base_node<> * const node	=	& * it;
			if ( mount_id_of_node(node) == m_mount_id )
				return						node;
		}

		return								NULL;
	}

	base_node<> *   find_node_on_virtual_path	()
	{
		vfs_hashset::begin_end_pair	begin_end	=	m_file_system.hashset.equal_range(m_virtual_path->c_str(), lock_type_read);
		overlapped_node_iterator	it		=	begin_end.first;
		overlapped_node_iterator	it_end	=	begin_end.second; 
		return									find_node_on_virtual_path(it, it_end);
	}

	bool   try_write_lock			()
	{
		fs_new::virtual_path_string			write_lock_path;
		fs_new::get_path_without_last_item	(& write_lock_path, m_virtual_path->c_str());

		m_lock_node						=	NULL;

		while ( write_lock_path )
		{
			bool const locked			=	m_file_system.hashset.find_and_lock_branch	(m_lock_node, 
											 											 write_lock_path.c_str(), 
																						 lock_type_write, 
																						 m_lock_operation);
			if ( !locked && m_lock_operation == lock_operation_try_lock )
				break;

			if ( m_lock_node )
			{
				R_ASSERT					(locked);
				return						true;
			}			

			fs_new::get_path_without_last_item_inplace	(& write_lock_path);
		}

		m_callback							(mount_result(NULL, result_cannot_lock));
		return								false;
	}

	void   fill_mount_arguments			(query_mount_arguments *		args, 
										 synchronous_device_interface *	sync_device,
										 base_node<> *					node,
										 submount_type_enum				submount_type,
										 native_path_string const &		physical_path)
	{
		* args							=	query_mount_arguments::mount_physical_path
												(m_mount_root->allocator, * m_virtual_path, physical_path, "", NULL, 
												 sync_device, m_callback, recursive_false, m_lock_operation, 
												 m_mount_root->watcher_enabled);
		
		args->mount_ptr					=	m_mount.c_ptr();
		args->submount_node				=	node;
		args->submount_type				=	submount_type;
		args->parent_of_submount_node	=	node ? node_cast<base_node>(node->get_parent()) : NULL;
		R_ASSERT							(m_lock_node);
		args->root_write_lock			=	m_lock_node;
	}

	void   query_hot_mount				(bool only_update_size)
	{
		if ( !try_convert_to_virtual_path(m_physical_path) )
			return;

		vfs_hashset::begin_end_pair	begin_end	=	m_file_system.hashset.equal_range(m_virtual_path->c_str(), lock_type_read);
		overlapped_node_iterator	it		=	begin_end.first;
		overlapped_node_iterator	it_end	=	begin_end.second; 

		base_node<> * node				=	find_node_on_virtual_path(it, it_end);
		
		if ( only_update_size && !node )
		{
			m_callback						(mount_result((vfs_mount *)m_mount_root->mount, result_success));
			return;
		}

		if ( node )
		{
			if ( get_node_physical_path(node) != m_physical_path )
			{
				LOGI_TRACE						("watcher", "wrong physical path passed: '%s'", m_physical_path.c_str());
				m_callback						(mount_result(m_mount, result_fail));
				return;
			}

			if ( node->is_folder() )
			{
				m_callback					(mount_result((vfs_mount *)m_mount_root->mount, result_success));
				return; // folder already created
			}
		}

		synchronous_device_interface	sync_device	(m_mount_root->async_device, m_mount_root->allocator,
													 m_mount_root->device, m_mount_root->watcher_enabled);
		if ( sync_device.out_of_memory() )
		{
			m_callback						(mount_result(NULL, result_out_of_memory));
			return;
		}

		physical_path_info const path_info	=	sync_device->get_physical_path_info(m_physical_path);
		if ( !path_info.exists() ) 
		{
			if ( only_update_size )
				LOGI_INFO					("watcher", "no file to update file size: '%s'", m_physical_path.c_str());
			else
				LOGI_INFO					("watcher", "no file to hot mount: '%s'", m_physical_path.c_str());

			m_callback						(mount_result(m_mount, result_success));
			return;
		}
		
		if ( node )
		{
			physical_file_node<> * file_node	=	cast_physical_file(node);
			R_ASSERT						(file_node);
			u32 const new_size			=	(u32)path_info.get_file_size();
			file_size_type const old_size	=	threading::interlocked_exchange	(file_node->m_size, new_size);

			if ( m_file_system.on_node_hides )
			{
				vfs_iterator	iterator	(node, NULL, & m_file_system.hashset, vfs_iterator::type_non_recursive);
				m_file_system.on_node_hides	(iterator);
			}

			if ( !only_update_size )
				LOGI_INFO					("watcher", "hot_mount '%s' already mounted, only updating size", m_physical_path.c_str(), new_size);

			if ( old_size != new_size )
				LOGI_INFO					("watcher", "updated size of '%s' to (%d)", m_physical_path.c_str(), new_size);
			else
				LOGI_INFO					("watcher", "no need of updating size for '%s'", m_physical_path.c_str());

			m_callback						(mount_result(m_mount, result_success));
			return;
		}

		it.clear							();

		if ( !try_write_lock() )
			return;

		query_mount_arguments				args;
		fill_mount_arguments				(& args, & sync_device, node_cast<base_node>(m_mount_root), 
											 submount_type_hot_mount, m_physical_path);
		m_file_system.query_mount			(args);
	}

	void   query_hot_unmount				()
	{
		if ( !try_convert_to_virtual_path(m_physical_path) )
			return;

		base_node<> * const node		=	find_node_on_virtual_path();
		if ( !node )
		{
			m_callback						(mount_result(NULL, result_success));
			return;						// already gone
		}

		synchronous_device_interface	sync_device	(m_mount_root->async_device, m_mount_root->allocator,
													 m_mount_root->device, m_mount_root->watcher_enabled);
		if ( sync_device.out_of_memory() )
		{
			m_callback						(mount_result(NULL, result_out_of_memory));
			return;
		}

		if ( !try_write_lock() )
			return;

		query_mount_arguments				args;
		fill_mount_arguments				(& args, & sync_device, node_cast<base_node>(m_mount_root), 
											 submount_type_hot_unmount, m_physical_path);
		unmount								(args, m_file_system);
	}

	void	query_rename				()
	{
		if ( !try_convert_to_virtual_path(m_physical_old_path) )
			return;

		synchronous_device_interface	sync_device	(m_mount_root->async_device, m_mount_root->allocator,
													 m_mount_root->device, m_mount_root->watcher_enabled);
		if ( sync_device.out_of_memory() )
		{
			m_callback						(mount_result(NULL, result_out_of_memory));
			return;
		}

		if ( !try_write_lock() )
			return;

		LOG_INFO							("hot renaming '%s' to '%s'", 
											 m_physical_old_path.c_str(), m_physical_new_path.c_str());

		base_node<> * const node		=	find_node_on_virtual_path();
		if ( node )
		{
			query_mount_arguments			args;
			fill_mount_arguments			(& args, & sync_device, node_cast<base_node>(m_mount_root), 
											 submount_type_hot_unmount, m_physical_old_path);
			args.callback				=	& on_erased_renamed_node;
			args.unlock_after_mount		=	false;
			unmount							(args, m_file_system);
		}

		* m_virtual_path				=	"";
		bool const convertion_result	=	try_convert_to_virtual_path(m_physical_new_path);
		R_ASSERT							(convertion_result);
		query_mount_arguments				args;
		fill_mount_arguments				(& args, & sync_device, node_cast<base_node>(m_mount_root), 
											 submount_type_hot_mount, m_physical_new_path);
		args.unlock_after_mount			=	true;
		m_file_system.query_mount			(args);
	}

private:
	virtual_file_system &					m_file_system;
	native_path_string const &				m_physical_path;
	native_path_string const &				m_physical_old_path;
	native_path_string const &				m_physical_new_path;
	query_mount_callback const &			m_callback;
	lock_operation_enum						m_lock_operation;
	assert_on_fail_bool						m_assert_on_fail;
	vfs_mount_ptr							m_mount;
	mount_root_node_base<> *				m_mount_root;
	u32										m_mount_id;
	base_node<> *							m_lock_node;

	virtual_path_string *					m_virtual_path;
};

void   virtual_file_system::query_hot_mount		(fs_new::native_path_string const &		physical_path,
												 fs_new::virtual_path_string *			in_out_virtual_path,
												 query_mount_callback const &			callback,
												 lock_operation_enum					lock_operation,
												 assert_on_fail_bool					assert_on_fail)
{
	query_notification_operation	operation	(* this, 
												 in_out_virtual_path,
												 physical_path, 												 
												 "", 
												 "", 
												 callback, 
												 lock_operation,
												 assert_on_fail);

	operation.query_hot_mount				(false);
}

void   virtual_file_system::query_hot_unmount	(fs_new::native_path_string const &	physical_path,
												 query_mount_callback const &		callback,
												 lock_operation_enum				lock_operation,
												 assert_on_fail_bool				assert_on_fail)
{
	fs_new::virtual_path_string	virtual_path	=	"";
	query_notification_operation	operation	(* this, 
												 & virtual_path,
												 physical_path, 
												 "", 
												 "", 
												 callback, 
												 lock_operation,
												 assert_on_fail);

	operation.query_hot_unmount				();
}


#if XRAY_FS_NEW_WATCHER_ENABLED

void   virtual_file_system::on_watcher_notification	(vfs_notification const &			notification,
													 query_mount_callback const &		callback,
													 lock_operation_enum				lock_operation)
{
	fs_new::virtual_path_string	virtual_path	=	"";
	query_notification_operation			operation(* this, 
													  & virtual_path,
													  notification.physical_path, 
													  notification.physical_old_path, 
													  notification.physical_new_path, 
													  callback, 
													  lock_operation);

	if ( notification.type == vfs_notification::type_added )
		operation.query_hot_mount			(false);
	else if ( notification.type == vfs_notification::type_modified )
 		operation.query_hot_mount			(true);
	else if ( notification.type == vfs_notification::type_removed )
		operation.query_hot_unmount			();
	else if ( notification.type == vfs_notification::type_renamed )
 		operation.query_rename				();
 	else
 		UNREACHABLE_CODE					();
}

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

} // namespace vfs 
} // namespace xray 

