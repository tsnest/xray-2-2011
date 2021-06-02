////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_ERASED_NODE_H_INCLUDED
#define VFS_ERASED_NODE_H_INCLUDED

namespace xray {
namespace vfs {

#pragma pack(push, 8)

template <platform_pointer_enum T = platform_pointer_default>
class erased_node
{
public:
	enum { flags = vfs_node_is_erased };

	typedef		base_node<T>	node_type;
	typedef		typename node_type::node_pointer	node_pointer;
public:
	erased_node() : base(flags) {}

	u32   sizeof_with_name				()
	{
		return								sizeof(* this) + strings::length(base.get_name()) + 1;
	}

	void	reverse_bytes ()
	{
		base.reverse_bytes					();
	}

	static erased_node<T> *   create	(memory::base_allocator *				allocator,
										 mount_root_node_base<T> *				mount_root,
 										 fs_new::virtual_path_string const &	name);

public:
	node_type							base;

}; // class erased_node

#pragma pack(pop)

template <platform_pointer_enum T>
base_node<T> const *		cast_node	(erased_node<T> const * node) { return & node->base; }

template <platform_pointer_enum T>
base_node<T> *				cast_node	(erased_node<T> * node) { return & node->base; }

template <platform_pointer_enum T>
erased_node<T> *   cast_erased_node		(base_node<T> * node)
{
	R_ASSERT								(node->is_erased());
	return									cast_base_node_to< erased_node<T> >(node);
}

template <platform_pointer_enum T>
erased_node<T> const *   cast_erased_node (base_node<T> const * node)
{
	return									node_cast<erased_node>(const_cast<base_node<T> *>(node));
}

template <platform_pointer_enum T>
erased_node<T> *   erased_node<T>::create (memory::base_allocator *				allocator,
										   mount_root_node_base<T> *			mount_root,
 										   fs_new::virtual_path_string const &	name)
{
	u32 const node_size					=	sizeof(erased_node<T>) + name.length() + 1;
	erased_node<T> *	new_node		=	(erased_node<T>*) XRAY_MALLOC_IMPL(allocator, node_size, "erased_node");
	if ( !new_node )
		return								NULL;

	new	(new_node)							erased_node<T>;
	base_node<T> *	base				=	node_cast<base_node>(new_node);
	base->add_flags							(vfs_node_is_physical);
	typename base_node<T>::mount_root_pointer		mount_root_pointer;	
	mount_root_pointer					=	mount_root;
	base->set_mount_root					(mount_root_pointer);
	strings::copy							(base->m_name, name.length() + 1, name.c_str());

	return									new_node;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_ERASED_NODE_H_INCLUDED