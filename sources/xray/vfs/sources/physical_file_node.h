////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_PHYSICAL_FILE_NODE_H_INCLUDED
#define VFS_PHYSICAL_FILE_NODE_H_INCLUDED

#include <xray/vfs/base_node.h>
#include <xray/fs/native_path_string.h>
#include "universal_file_node.h"

namespace xray {
namespace vfs {

template <platform_pointer_enum T = platform_pointer_default>
class physical_file_node
{
public:
	typedef			base_node<T>						node_type;
	typedef			typename node_type::node_pointer	node_pointer;

	enum flags_enum	{	flags_checked_is_archive		=	1 << 0, 
						flags_is_archive				=	1 << 1, 
						flags_is_mounted				=	1 << 2, 
						flags_allocated_in_parent_arena	=	1 << 3, };
public:
					physical_file_node	() : m_size(0), m_file_flags(0) {}

	u32				size				() const { return (u32)m_size; }

	bool			checked_is_archive	() const 
	{ 
		return								m_file_flags.has(flags_checked_is_archive); 
	}

	void			set_checked_is_archive	(bool is_archive) 
	{ 
		m_file_flags.set					(flags_checked_is_archive | (is_archive ? flags_is_archive : 0)); 
	}

	void			set_is_mounted		(bool is_mounted) 
	{ 
		if ( is_mounted )
			m_file_flags.set				(flags_is_mounted); 
		else
			m_file_flags.unset				(flags_is_mounted);
	}

	bool			is_archive_file		() const 

	{ 
		ASSERT								(checked_is_archive());
		return								m_file_flags.has(flags_is_archive) ; 
	}

	bool			is_mounted_archive	() const 

	{ 
		ASSERT								(is_archive_file());
		return								m_file_flags.has(flags_is_mounted) ; 
	}

	void			set_is_allocated_in_parent_arena() 
	{ 
		m_file_flags.set					(flags_allocated_in_parent_arena); 
	}

	bool			is_allocated_in_parent_arena () const 
	{ 
		return								m_file_flags.has(flags_allocated_in_parent_arena); 
	}

	inline u32		sizeof_with_name () 
	{ 
		return								sizeof(*this) + strings::length(base.get_name()) + 1;
	}

	static physical_file_node<T> *  create (memory::base_allocator *			allocator,
											mount_root_node_base<T> *			mount_root,
											fs_new::virtual_path_string const &	name, 
									 	    u32 const							file_size);
public:
	threading::atomic32_type			m_size;
	flags_type<flags_enum, threading::multi_threading_policy>	m_file_flags;
	node_type							base;
}; // class fat_disk_node

//--------------------------------------
// implementation:
//--------------------------------------

template <platform_pointer_enum T>
base_node<T> *			cast_node 		(physical_file_node<T> * node)			{ return & node->base; }
template <platform_pointer_enum T>
base_node<T> const *	cast_node 		(physical_file_node<T> const * node)	{ return & node->base; }

template <platform_pointer_enum T>
physical_file_node<T> const *	 cast_physical_file (base_node<T> const * node) { return cast_physical_file(const_cast<base_node<T> *>(node)); }

template <platform_pointer_enum T>
physical_file_node<T> *	 cast_physical_file (base_node<T> * node)
{
	R_ASSERT							(!node->is_link());

	if ( node->is_folder() || !node->is_physical() )
		return								NULL;

	return									cast_base_node_to< physical_file_node<T> >(node);
}

template <template <platform_pointer_enum T> class Node, platform_pointer_enum T>
fs_new::native_path_string   get_node_physical_path (Node<T> * node) 
{
	R_ASSERT								(node);
	base_node<T> * base					=	node_cast<base_node>(node);
	R_ASSERT								(!base->is_mount_helper());
	mount_root_node_base<T> * const mount_root	=	base->get_mount_root();
	R_ASSERT								(mount_root);
	if ( base->is_archive() )
		return								mount_root->physical_path;
	if ( base->is_universal_file() )
	{
		universal_file_node<T> * const uni_node	=	node_cast<universal_file_node>(base);
		return								uni_node->physical_path;
	}
	
	fs_new::virtual_path_string				node_path;
	base->get_full_path						(& node_path);
	fs_new::virtual_path_string				mount_root_path;
	node_cast<base_node>(mount_root)->get_full_path	(& mount_root_path);

	fs_new::native_path_string	relative_to_mount_root_path	=	fs_new::native_path_string::convert(node_path.c_str() + mount_root_path.length());

	fs_new::native_path_string	out_path	=	mount_root->physical_path;
	if ( relative_to_mount_root_path.length() && relative_to_mount_root_path[0] != fs_new::native_path_string::separator )
		out_path						+=	fs_new::native_path_string::separator;

	out_path							+=	relative_to_mount_root_path;
	return									out_path; 
}

template <platform_pointer_enum T>
physical_file_node<T> *   physical_file_node<T>::create (memory::base_allocator *				allocator,
														 mount_root_node_base<T> *				mount_root,
														 fs_new::virtual_path_string const &	name, 
												 		 u32 const								file_size)
{
	ASSERT									(name.length());
	u32 const node_size					=	sizeof(physical_file_node<T>) + name.length() + 1;
	physical_file_node<T> *	new_node	=	(physical_file_node<T>*) XRAY_MALLOC_IMPL(allocator, node_size, "physical_file_node");
	if ( !new_node )
		return								NULL;

	new	(new_node)							physical_file_node<T>;
	new_node->m_size					=	file_size;

	base_node<T> *	base				=	node_cast<base_node>(new_node);
	base->m_flags						=	vfs_node_is_physical;
	typename base_node<T>::mount_root_pointer		mount_root_pointer;	
	mount_root_pointer					=	mount_root;
	base->set_mount_root					(mount_root_pointer);
	strings::copy							(base->m_name, name.length() + 1, name.c_str());
	
	mount_root->mount_size				+=	base->sizeof_with_name();

	return									new_node;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_PHYSICAL_FILE_NODE_H_INCLUDED