////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_ROOT_NODE_H_INCLUDED
#define VFS_MOUNT_ROOT_NODE_H_INCLUDED

#include <xray/vfs/base_node.h>
#include <xray/vfs/device_file_system.h>
#include <xray/fs/device_file_system_proxy_base.h>
#include <xray/fs/asynchronous_device_interface.h>
#include "erased_node.h"
#include <xray/vfs/types.h>

namespace xray {
namespace vfs {

class	sub_fat_resource;
class	virtual_file_system;
class	query_mount_arguments;
class	vfs_mount;

#pragma pack(push, 8)

template <platform_pointer_enum T>
class mount_root_node_base
{
public:
	typedef			base_node<T>		node_type;
public:
	mount_root_node_base (node_type *								node,
						  pcstr										virtual_path, 
						  pcstr										physical_path, 
						  pcstr										descriptor,
						  virtual_file_system *						file_system, 
						  memory::base_allocator *					allocator,
						  mount_type_enum							mount_type,
						  fs_new::device_file_system_interface *	device,
						  fs_new::asynchronous_device_interface *	async_device,
						  fs_new::watcher_enabled_bool				watcher_enabled)
		:	file_system			(file_system), 
			allocator			(allocator), 
			node				(node), 
			physical_path		(physical_path), 
			virtual_path		(virtual_path), 
			descriptor			(descriptor),
			mount_type			(mount_type), 
			device				(device), 
			async_device		(async_device),
			mount				(NULL),
			watcher_enabled		(watcher_enabled),
			first_erased		(NULL),
			mount_id			(0),
			mount_operation_id	(0),
			erased				(false),
			mount_size			(0)

	{
	}

	void		prepend_erased_node		(base_node<> * node)
	{
		if ( first_erased )
			node->set_next					(first_erased);
		else
			node->set_next					(NULL);

		first_erased					=	node;
	}
	
public:
	typename	platform_pointer<virtual_file_system,	T>::type	file_system;
	typename	platform_pointer<memory::base_allocator,T>::type	allocator;

	typename	platform_pointer<fs_new::device_file_system_interface,	T>::type	device;
	typename	platform_pointer<fs_new::asynchronous_device_interface,	T>::type	async_device;
	
	typename	platform_pointer<char const,	T>::type	virtual_path;
	typename	platform_pointer<char const,	T>::type	physical_path;
	typename	platform_pointer<char const,	T>::type	descriptor;
	typename	platform_pointer<vfs_mount, T>::type		mount;

	typename	node_type::node_pointer		node;
	typename	node_type::node_pointer		first_erased;
	u32										mount_id;
	u32										mount_operation_id;
	u32										mount_size; // in bytes
	mount_type_enum							mount_type;
	fs_new::watcher_enabled_bool			watcher_enabled;
	bool									erased;

}; // class mount_root_node_base

#pragma pack(pop)

template <platform_pointer_enum T> inline
base_node<T> *				cast_node	(mount_root_node_base<T> * node) { return node->node; }

template <platform_pointer_enum T> inline
base_node<T> const *		cast_node	(mount_root_node_base<T> const * node) { return node->node; }

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_ROOT_NODE_H_INCLUDED