////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_ROOT_NODE_FUNCTIONS_H_INCLUDED
#define VFS_MOUNT_ROOT_NODE_FUNCTIONS_H_INCLUDED

#include "archive_folder_mount_root_node.h"
#include "physical_file_mount_root_node.h"
#include "physical_folder_mount_root_node.h"
#include <xray/fs/synchronous_device_interface.h>

namespace xray {
namespace vfs {

struct mount_root_node_functions
{

template <platform_pointer_enum T> inline
fs_new::native_path_string	 get_node_physical_path (mount_root_node_base<T> * node)
{
	return								node->physical_path;
}

template <template <platform_pointer_enum T> class Node, platform_pointer_enum T>	inline
static Node<T> *  create				(pcstr										name, 
										 u32 const									name_length,
										 virtual_file_system *						file_system,
										 query_mount_arguments &					args)
{
	u32 const name_allocated_size		=	name_length + 1;
	u32 const virtual_path_offset		=	sizeof(Node<T>) + name_allocated_size;
	u32 const virtual_path_allocated_size	=	args.virtual_path.length() + 1;
	u32 const physical_path_offset		=	virtual_path_offset + virtual_path_allocated_size;
	u32 const physical_path_allocated_size	=	args.physical_path.length() + 1;
	u32 const descriptor_offset			=	physical_path_offset + physical_path_allocated_size;
	u32 const descriptor_allocated_size	=	args.descriptor.length() + 1;
	u32 const node_size					=	physical_path_offset + physical_path_allocated_size + descriptor_allocated_size;
		
	Node<T> * const new_node			=	(Node<T> *)XRAY_MALLOC_IMPL(args.allocator, node_size, "vfs_node");
	if ( !new_node )
		return								NULL;
	pstr virtual_path_ptr				=	(pstr)new_node + virtual_path_offset;
	pstr physical_path_ptr				=	(pstr)new_node + physical_path_offset;
	pstr descriptor_ptr					=	(pstr)new_node + descriptor_offset;
	strings::copy							(virtual_path_ptr, virtual_path_allocated_size, args.virtual_path.c_str());
	strings::copy							(physical_path_ptr, physical_path_allocated_size, args.physical_path.c_str());
	strings::copy							(descriptor_ptr, descriptor_allocated_size, args.descriptor.c_str());

	fs_new::device_file_system_interface * sync_device	=	args.synchronous_device ? args.synchronous_device->get_device()->get_device_file_system_interface() : NULL;
	new	(new_node)							Node<T>(virtual_path_ptr, 
													physical_path_ptr, 
													descriptor_ptr,
													file_system, 
													args.allocator,
													args.type,
													sync_device,
													args.asynchronous_device,
													args.watcher_enabled);

	base_node<T> * const base			=	node_cast<base_node>(new_node);
	base->set_flags							(Node<T>::flags);
	strings::copy							(base->m_name, name_allocated_size, name);

	return									new_node;
}

}; // struct mount_root_node_functions

template <platform_pointer_enum T> inline
mount_root_node_base<T> *		cast_mount_root_node_base	(base_node<T> * node) 
{ 
	if ( physical_folder_mount_root_node<T> * root_node = node_cast<physical_folder_mount_root_node>(& * node) )
		return								root_node;
	else if ( physical_file_mount_root_node<T> * root_node = node_cast<physical_file_mount_root_node>(& * node) )
		return								root_node;
	else if ( archive_folder_mount_root_node<T> * root_node = node_cast<archive_folder_mount_root_node>(& * node) )
		return								root_node;
	return									NULL;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_ROOT_NODE_FUNCTIONS_H_INCLUDED