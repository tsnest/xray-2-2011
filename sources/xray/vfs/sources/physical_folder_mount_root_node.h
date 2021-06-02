////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_PHYSICAL_FOLDER_MOUNT_ROOT_NODE_H_INCLUDED
#define VFS_PHYSICAL_FOLDER_MOUNT_ROOT_NODE_H_INCLUDED

#include "mount_root_node_base.h"
#include "physical_folder_node.h"

namespace xray {
namespace vfs {

template <platform_pointer_enum T = platform_pointer_default>
class physical_folder_mount_root_node : public mount_root_node_base<T>
{
public:
	typedef	mount_root_node_base<T>		super;
public:
	enum { flags = vfs_node_is_folder | vfs_node_is_physical | vfs_node_is_mount_root };

	physical_folder_mount_root_node(pcstr							virtual_path, 
						 		    pcstr							physical_path, 
									pcstr							descriptor,
						 		    virtual_file_system *			file_system, 
									memory::base_allocator *		allocator,
						 		    mount_type_enum					mount_type,
						 		    fs_new::device_file_system_interface *	device,
									fs_new::asynchronous_device_interface *	async_device,
						 		    fs_new::watcher_enabled_bool	watcher_enabled) 
		:	super(node_cast<base_node>(& folder), virtual_path, physical_path, descriptor, file_system, allocator, mount_type,
								 device, async_device, watcher_enabled) {}
	
	inline u32 sizeof_with_name ()
	{
		return	sizeof(*this) + 
				strings::length(super::virtual_path) + 1 +
				strings::length(super::physical_path) + 1 +
				strings::length(folder.folder.base.get_name()) + 1;
	}

	physical_folder_node<T>				folder;
}; // class physical_folder_mount_root_node

template <platform_pointer_enum T>
base_node<T> *				cast_node	(physical_folder_mount_root_node<T> * node) { return & node->folder.folder.base; }

template <platform_pointer_enum T>
base_node<T> const *		cast_node	(physical_folder_mount_root_node<T> const * node) { return & node->folder.folder.base; }

template <platform_pointer_enum T>
base_folder_node<T> *		cast_folder	(physical_folder_mount_root_node<T> * node) { return & node->folder.folder; }

template <platform_pointer_enum T>
base_folder_node<T> const *	cast_folder	(physical_folder_mount_root_node<T> const * node) { return & node->folder.folder; }

template <platform_pointer_enum T>
physical_folder_node<T> *		cast_physical_folder	(physical_folder_mount_root_node<T> * node) { return & node->folder; }
 
template <platform_pointer_enum T>
physical_folder_node<T> const *	cast_physical_folder	(physical_folder_mount_root_node<T> const * node) { return & node->folder; }

template <platform_pointer_enum T>
physical_folder_mount_root_node<T> *	cast_physical_folder_mount_root (base_node<T> * node) 
{
	R_ASSERT								(!node->is_link());

	if ( !node->is_folder() || !node->is_mount_root() || !node->is_physical() )
		return								NULL;
	
	physical_folder_mount_root_node<T> *	out_result =	NULL;
	size_t const offs					=	(char*)(& out_result->folder.folder.base) - (char *)(out_result);

	out_result							=	(physical_folder_mount_root_node<T>*)((char*)(node) - offs);
	return									out_result;
}

template <platform_pointer_enum T> inline
physical_folder_mount_root_node<T> *	cast_physical_folder_mount_root (mount_root_node_base<T> * node) 
{
	return									node_cast<physical_folder_mount_root_node> (& * node->node);
}

inline
mount_root_node_base<> *   cast_mount_root_node_base (physical_folder_mount_root_node<> * const mount_root)
{
	return									mount_root;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_PHYSICAL_FOLDER_MOUNT_ROOT_NODE_H_INCLUDED