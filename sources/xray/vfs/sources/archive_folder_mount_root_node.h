////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_ARCHIVE_FOLDER_MOUNT_ROOT_NODE_H_INCLUDED
#define VFS_ARCHIVE_FOLDER_MOUNT_ROOT_NODE_H_INCLUDED

#include "mount_root_node_base.h"
#include "base_folder_node.h"

namespace xray {
namespace vfs {

#pragma pack(push, 8)

template <platform_pointer_enum T>
class archive_folder_mount_root_node : public mount_root_node_base<T>
{
public:
	typedef	mount_root_node_base<T>	super;
public:
	enum { flags = vfs_node_is_folder | vfs_node_is_archive | vfs_node_is_mount_root };

	archive_folder_mount_root_node		() 
		:	super				(& folder.base, 
								 virtual_path_holder, 
								 fat_path_holder, 
								 descriptor,
								 NULL,
								 NULL,
								 mount_type_archive, 
								 NULL, 
								 NULL,
								 fs_new::watcher_enabled_false),
			 nodes_buffer(NULL),
			 attach_node(NULL)
	{
		memory::zero						(virtual_path_holder);
		memory::zero						(fat_path_holder);
		memory::zero						(archive_path_holder);
		memory::zero						(descriptor);
		super::node->set_flags				(flags);
	}

	inline u32 sizeof_with_name			()
	{
		return								sizeof(*this) + strings::length(folder.base.get_name()) + 1;
	}

	typedef typename	platform_pointer<char const,	T>::type	char_pointer;
	typedef	typename 	platform_pointer<base_node<T>,	T>::type	node_pointer;
	
	string_path							virtual_path_holder;
	string_path							fat_path_holder;
	string_path							archive_path_holder;
	char								descriptor[32];

	char_pointer						nodes_buffer;
	node_pointer						attach_node;
	base_folder_node<T>					folder;

}; // class archive_folder_mount_root_node

#pragma pack(pop)

template <platform_pointer_enum T> inline
base_node<T> *				cast_node	(archive_folder_mount_root_node<T> * node) { return & node->folder.base; }

template <platform_pointer_enum T> inline
base_node<T> const *		cast_node	(archive_folder_mount_root_node<T> const * node) { return & node->folder.base; }

template <platform_pointer_enum T> inline
base_folder_node<T> *		cast_folder (archive_folder_mount_root_node<T> * node) { return & node->folder; }

template <platform_pointer_enum T> inline
base_folder_node<T> const *	cast_folder (archive_folder_mount_root_node<T> const * node) { return & node->folder; }

template <platform_pointer_enum T> inline
archive_folder_mount_root_node<T> *	 cast_archive_folder_mount_root (base_node<T> * node) 
{
	R_ASSERT								(!node->is_link());

	if ( !node->is_folder() || !node->is_mount_root() || !node->is_archive() )
		return								NULL;
	
	archive_folder_mount_root_node<T> *	out_result =	NULL;
	size_t const offs					=	(char*)(& out_result->folder.base) - (char *)(out_result);

	out_result							=	(archive_folder_mount_root_node<T>*)((char*)(node) - offs);
	return									out_result;
}

template <platform_pointer_enum T> inline
archive_folder_mount_root_node<T> *	 cast_archive_folder_mount_root (mount_root_node_base<T> * node) 
{
	return									cast_archive_folder_mount_root(& * node->node);
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_ARCHIVE_FOLDER_MOUNT_ROOT_NODE_H_INCLUDED