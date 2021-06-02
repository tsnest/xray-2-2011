////////////////////////////////////////////////////////////////////////////
//	Created		: 04.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_HELPER_NODE_H_INCLUDED
#define VFS_MOUNT_HELPER_NODE_H_INCLUDED

#include "base_folder_node.h"

namespace xray {
namespace vfs {

template <platform_pointer_enum T = platform_pointer_default>
class mount_helper_node
{
public:
	typedef			base_node<T>		node_type;

public:
					mount_helper_node	(memory::base_allocator * allocator, u32 mount_id) 
						: m_allocator(allocator), mount_id(mount_id) {}

	memory::base_allocator * allocator	() const { return m_allocator; }
	
	static void		create_inplace		(mount_helper_node<T> *		in_out_helper_node,
										 memory::base_allocator *	allocator,
										 pcstr						name, 
										 u32 const					name_length,
										 u32 const					mount_id);

private:
	memory::base_allocator *			m_allocator;
public:
	u32									mount_id;
	base_folder_node<T>					folder;
};

//--------------------------------------
// implementation:
//--------------------------------------

template <platform_pointer_enum T>
void   mount_helper_node<T>::create_inplace (mount_helper_node<T> *		in_out_helper_node,
											 memory::base_allocator *	allocator,
											 pcstr						name, 
											 u32 const					name_length,
											 u32 const					mount_id)
{
	new	(in_out_helper_node)				mount_helper_node<T>(allocator, mount_id);

	base_node<T> * const base			=	node_cast<base_node>(in_out_helper_node);
	base->set_flags							(vfs_node_is_folder | vfs_node_is_mount_helper);
	strings::copy							(base->m_name, name_length + 1, name);
}

template <platform_pointer_enum T>
inline base_folder_node<T> *			cast_folder	(mount_helper_node<T> * node) { return & node->folder; }

template <platform_pointer_enum T>
inline base_folder_node<T> const *		cast_folder	(mount_helper_node<T> const * node) { return & node->folder; }

template <platform_pointer_enum T>
inline base_node<T> *					cast_node	(mount_helper_node<T> * node) { return & node->folder.base; }

template <platform_pointer_enum T>
inline base_node<T> const *				cast_node	(mount_helper_node<T> const * node) { return & node->folder.base; }

template <platform_pointer_enum T>
mount_helper_node<T> *	cast_mount_helper_node (base_folder_node<T> * node)
{
	return									node_cast<mount_helper_node>(node_cast<base_node>(node));
}

template <platform_pointer_enum T>
mount_helper_node<T> *	cast_mount_helper_node (base_node<T> * node)
{
	R_ASSERT								(!node->is_link());
	
	if ( !node->is_folder() || !node->is_mount_helper() )
		return								NULL;

	mount_helper_node<T> * out_result	=	NULL;
	size_t const offs					=	(char*)(& out_result->folder.base) - (char *)(out_result);

	out_result							=	(mount_helper_node<T>*)((char*)(node) - offs);
	return									out_result;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_HELPER_NODE_H_INCLUDED