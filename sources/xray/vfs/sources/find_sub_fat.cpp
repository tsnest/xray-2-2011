////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "find.h"

namespace xray {
namespace vfs {

void   find_async_expand_sub_fat			(base_node<> * const			node, 
											 base_node<> * const			node_parent, 
											 async_callbacks_data * const	async_data,
											 u32 const						increment)
{
	R_ASSERT								(node->is_sub_fat());

	mount_root_node_base<> * mount_root	=	node->get_mount_root();
	fs_new::virtual_path_string				virtual_path;
	node->get_full_path						(& virtual_path);
	fs_new::native_path_string	physical_path	=	mount_root->physical_path;

	fs_new::device_file_system_proxy		device_proxy(mount_root->device, mount_root->watcher_enabled);
	fs_new::synchronous_device_interface	sync_device(device_proxy);

	query_mount_arguments	args		=	query_mount_arguments::mount_archive
												(mount_root->allocator, // async_data->env.allocator
												 virtual_path, 
												 physical_path, 
												 physical_path,
												 "",
												 mount_root->async_device,
												 mount_root->device ? & sync_device : NULL,
												 boost::bind(& async_callbacks_data::on_automatic_archive_or_subfat_mounted, 
															 async_data, _1, increment),
												 lock_operation_lock
												);

	args.submount_type					=	submount_type_subfat;
	args.submount_node					=	node;
	args.parent_of_submount_node		=	node_parent;
	args.root_write_lock				=	async_data->env.node;
	args.unlock_after_mount				=	false;

	async_data->env.file_system->query_mount	(args);
}

} // namespace vfs
} // namespace xray 