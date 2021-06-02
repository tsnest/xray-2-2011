////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_PHYSICAL_FOLDER_NODE_H_INCLUDED
#define VFS_PHYSICAL_FOLDER_NODE_H_INCLUDED

#include "base_folder_node.h"
#include "mount_helper_node.h"

namespace xray {
namespace vfs {

template <platform_pointer_enum T = platform_pointer_default>
class physical_folder_node
{
public:
	typedef			base_node<T>						node_type;
	typedef			typename node_type::node_pointer	node_pointer;

	enum	flags_enum 					{	flags_scanned					=	1 << 0, 
											flags_scanned_recursively		=	1 << 1,	
											flags_allocated_in_parent_arena	=	1 << 2,	};
public:
					physical_folder_node	() : m_folder_flags(0) {}
	bool			is_scanned			(bool recursively) const { return (m_folder_flags & (flags_scanned | (recursively ? flags_scanned_recursively : 0))) != 0; }
	bool			set_is_scanned		(bool recursively) 
	{ 
		u32 flags_to_set				=	flags_scanned | (recursively ? flags_scanned_recursively : 0);
		return								!(m_folder_flags.set(flags_to_set) & flags_to_set);
	}

	void			set_is_allocated_in_parent_arena() { m_folder_flags |= flags_allocated_in_parent_arena; }
	bool			is_allocated_in_parent_arena () const { return (m_folder_flags & flags_allocated_in_parent_arena) != NULL; }
	
	static physical_folder_node<> *		create(memory::base_allocator *				allocator,
											   mount_root_node_base<T> *			mount_root,
											   fs_new::virtual_path_string const &	name);
	inline u32		sizeof_with_name () 
	{ 
		return								sizeof(*this) + strings::length(folder.base.get_name()) + 1; 
	}

public:
	const_buffer						children_arena;
private:
	flags_type<flags_enum, threading::multi_threading_policy>	m_folder_flags;
public:
	base_folder_node<T>					folder;

	template <platform_pointer_enum T2>
	friend class						physical_folder_mount_root_node;
};

//--------------------------------------
// implementation:
//--------------------------------------

template <platform_pointer_enum T>
physical_folder_node<> *  physical_folder_node<T>::create ( memory::base_allocator *	allocator,
														    mount_root_node_base<T> *	mount_root,
															fs_new::virtual_path_string const &	name)
{
	ASSERT									(name.length());
	u32 const node_size					=	sizeof(physical_folder_node<T>) + name.length() + 1;
	physical_folder_node<T> * const new_node	=	(physical_folder_node<T> *)XRAY_MALLOC_IMPL(allocator, node_size, "physical_folder_node");
	new	(new_node)							physical_folder_node<T>;
	if ( !new_node )
		return								NULL;

	base_node<T> * const base			=	node_cast<base_node>(new_node);
	typename base_node<T>::mount_root_pointer		mount_root_pointer;	
	mount_root_pointer					=	mount_root;
	base->set_mount_root					(mount_root_pointer);
	base->set_flags							(vfs_node_is_folder | vfs_node_is_physical);
	strings::copy							(base->m_name, name.length() + 1, name.c_str());

	mount_root->mount_size				+=	base->sizeof_with_name();

	return									new_node;
}

template <platform_pointer_enum T>
inline base_folder_node<T> *			cast_folder	(physical_folder_node<T> * node) { return & node->folder; }

template <platform_pointer_enum T>
inline base_folder_node<T> const *		cast_folder	(physical_folder_node<T> const * node) { return & node->folder; }

template <platform_pointer_enum T>
inline base_node<T> *					cast_node	(physical_folder_node<T> * node) { return & node->folder.base; }

template <platform_pointer_enum T>
inline base_node<T> const *				cast_node	(physical_folder_node<T> const * node) { return & node->folder.base; }

template <platform_pointer_enum T>
physical_folder_node<T> *	cast_physical_folder (base_node<T> * node)
{
	R_ASSERT							(!node->is_link());
	
	if ( !node->is_folder() || !node->is_physical() )
		return								NULL;

	if ( node->is_mount_root() )
	{
		physical_folder_mount_root_node<T> * out_result	=	node_cast<physical_folder_mount_root_node>(node);
		return							& out_result->folder;
	}

	physical_folder_node<T> * out_result	=	NULL;
	size_t const offs					=	(char*)(& out_result->folder.base) - (char *)(out_result);

	out_result							=	(physical_folder_node<T>*)((char*)(node) - offs);
	return									out_result;
}

template <platform_pointer_enum T>
inline base_folder_node<T> *   cast_folder (base_node<T> * node)
{
	R_ASSERT								(!node->is_link());

	if ( !node->is_folder() )
		return								NULL;

	if ( node->is_mount_helper() )
	{
		mount_helper_node<T> * mount_helper	=	node_cast<mount_helper_node>(node);
		return								node_cast<base_folder_node>(mount_helper);
	}

	if ( node->is_mount_root() )
	{
		if ( node->is_archive() )
		{
			archive_folder_mount_root_node<T> * archive_folder_mount_root	=	node_cast<archive_folder_mount_root_node>(node);
			base_folder_node<T> * out_folder	=	node_cast<base_folder_node>(archive_folder_mount_root);
			return							out_folder;
		}
		else
		{
			physical_folder_mount_root_node<T> * physical_folder_mount_root	=	node_cast<physical_folder_mount_root_node>(node);
			base_folder_node<T> * out_folder	=	node_cast<base_folder_node>(physical_folder_mount_root);
			return							out_folder;
		}
	}

	if ( !node->is_physical() )
		return								cast_base_node_to< base_folder_node<T> >(node);

	physical_folder_node<T> * const disk_folder	=	node_cast<physical_folder_node>(node);
	base_folder_node<T> * const out_folder	=	node_cast<base_folder_node>(disk_folder);
	return									out_folder;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_PHYSICAL_FOLDER_NODE_H_INCLUDED